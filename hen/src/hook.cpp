#include <errno.h>
#include <stdint.h>
#include <sys/types.h>

#include "hook.h"
#include "kdlsym.h"

#include "hooks/2_50.h"

struct hook *find_hook(hook_id id)
{
    uint64_t fw_ver;
    struct hook *hooks;
    struct hook *cur_hook;
    int num_hooks;

    fw_ver = get_fw_version();
    auto printf = (void (*)(const char *fmt, ...)) kdlsym(KERNEL_SYM_PRINTF);

    switch (fw_ver) {
    // case 0x1050000:
    //     hooks = (struct hook *) &g_kernel_hooks_105;
    //     num_hooks = sizeof(g_kernel_hooks_105) / sizeof(struct hook);
    //     break;
    case 0x2500000:
        hooks = (struct hook *) &g_kernel_hooks_250;
        num_hooks = sizeof(g_kernel_hooks_250) / sizeof(struct hook);
        break;
    default:
        return 0;
    }

    printf("find_hook: num_hooks = %d\n", num_hooks);

    for (int i = 0; i < num_hooks; i++) {
        cur_hook = &hooks[i];
        printf("hook_func_call: hook->id = %d\n", cur_hook->id);
        if (cur_hook->id == id) {
            return cur_hook;
        }
    }

    return 0;
}

int install_hook(hook_id id, void *func)
{
    struct hook *hook_info;
    uint64_t call_addr;
    uint64_t call_install;
    int32_t call_rel32;

    auto printf = (void (*)(const char *fmt, ...)) kdlsym(KERNEL_SYM_PRINTF);
    printf("hook_func_call: hook id = %d\n", id);

    // Find info for this hook
    hook_info = find_hook(id);
    if (hook_info == 0)
        return -ENOENT;

    printf("hook_func_call: found hook\n");

    // Calculate rel32
    call_addr   = ktext(hook_info->call_offset);
    call_rel32  = (int32_t) ((uint64_t) (func) - call_addr) - 5; // Subtract 5 for call opcodes

    printf("hook_func_call: call_addr=0x%llx (call_rel32=0x%x)\n", call_addr, call_rel32);

    // Install hook
    printf("hook_func_call: installing hook to 0x%lx (rel32=0x%x)\n", call_addr, call_rel32);

    call_install = call_addr + 1;
    *(uint32_t *) (call_install) = call_rel32;
    return 0;
}

int hook_is_development_mode()
{
    return 0xc001;
}

int apply_test_hook()
{
    auto printf = (void (*)(const char *fmt, ...)) kdlsym(KERNEL_SYM_PRINTF);

    printf("sys_is_development_mode() -> isDevelopmentMode()\n");
    return install_hook(HOOK_TEST_SYS_IS_DEVELOPMENT_MODE, (void *) &hook_is_development_mode);
}