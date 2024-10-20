#include <sys/types.h>
#include <sys/param.h>

#include "fkeys.h"
#include "fpkg.h"
#include "hook.h"
#include "kdlsym.h"
#include "util.h"

#define IDX_TO_HANDLE(x) (0x13374100 | ((uint8_t)((x)+1)))
#define HANDLE_TO_IDX(x) ((((x) & 0xffffff00) == 0x13374100 ? ((int)(uint8_t)(x)) : (int)0) - 1)

constexpr uint8_t rif_debug_key[] = {
    0x96, 0xC2, 0x26, 0x8D, 0x69, 0x26, 0x1C, 0x8B, 0x1E, 0x3B, 0x6B, 0xFF, 0x2F, 0xE0, 0x4E, 0x12
};

const uint8_t g_ypkg_p[] =
{
    0x2D, 0xE8, 0xB4, 0x65, 0xBE, 0x05, 0x78, 0x6A, 0x89, 0x31, 0xC9, 0x5A, 0x44, 0xDE, 0x50, 0xC1,
    0xC7, 0xFD, 0x9D, 0x3E, 0x21, 0x42, 0x17, 0x40, 0x79, 0xF9, 0xC9, 0x41, 0xC1, 0xFC, 0xD7, 0x0F,
    0x34, 0x76, 0xA3, 0xE2, 0xC0, 0x1B, 0x5A, 0x20, 0x0F, 0xAF, 0x2F, 0x52, 0xCD, 0x83, 0x34, 0x72,
    0xAF, 0xB3, 0x12, 0x33, 0x21, 0x2C, 0x20, 0xB0, 0xC6, 0xA0, 0x2D, 0xB1, 0x59, 0xE3, 0xA7, 0xB0,
    0x4E, 0x1C, 0x4C, 0x5B, 0x5F, 0x10, 0x9A, 0x50, 0x18, 0xCC, 0x86, 0x79, 0x25, 0xFF, 0x10, 0x02,
    0x8F, 0x90, 0x03, 0xA9, 0x37, 0xBA, 0xF2, 0x1C, 0x13, 0xCC, 0x09, 0x45, 0x15, 0xB8, 0x55, 0x74,
    0x0A, 0x28, 0x24, 0x04, 0xD1, 0x19, 0xAB, 0xB3, 0xCA, 0x44, 0xB6, 0xF8, 0x3D, 0xB1, 0x2A, 0x72,
    0x88, 0x35, 0xE4, 0x86, 0x6B, 0x55, 0x47, 0x08, 0x25, 0x16, 0xAB, 0x69, 0x1D, 0xBF, 0xF6, 0xFE,
};

const uint8_t g_ypkg_q[] =
{
    0x23, 0x80, 0x77, 0x84, 0x4D, 0x6F, 0x9B, 0x24, 0x51, 0xFE, 0x2A, 0x6B, 0x28, 0x80, 0xA1, 0x9E,
    0xBD, 0x6D, 0x18, 0xCA, 0x8D, 0x7D, 0x9E, 0x79, 0x5A, 0xE0, 0xB8, 0xEB, 0xD1, 0x3D, 0xF3, 0xD9,
    0x02, 0x90, 0x2A, 0xA7, 0xB5, 0x7E, 0x9A, 0xA2, 0xD7, 0x2F, 0x21, 0xA8, 0x50, 0x7D, 0x8C, 0xA1,
    0x91, 0x2F, 0xBF, 0x97, 0xBE, 0x92, 0xC2, 0xC1, 0x0D, 0x8C, 0x0C, 0x1F, 0xDE, 0x31, 0x35, 0x15,
    0x39, 0x90, 0xCC, 0x97, 0x47, 0x2E, 0x7F, 0x09, 0xE9, 0xC3, 0x9C, 0xCE, 0x91, 0xB2, 0xC8, 0x58,
    0x76, 0xE8, 0x70, 0x1D, 0x72, 0x5F, 0x4A, 0xE6, 0xAA, 0x36, 0x22, 0x94, 0xC6, 0x52, 0x90, 0xB3,
    0x9F, 0x9B, 0xF0, 0xEF, 0x57, 0x8E, 0x53, 0xC3, 0xE3, 0x30, 0xC9, 0xD7, 0xB0, 0x3A, 0x0C, 0x79,
    0x1B, 0x97, 0xA8, 0xD4, 0x81, 0x22, 0xD2, 0xB0, 0x82, 0x62, 0x7D, 0x00, 0x58, 0x47, 0x9E, 0xC7,
};

const uint8_t g_ypkg_dmp1[] =
{
    0x25, 0x54, 0xDB, 0xFD, 0x86, 0x45, 0x97, 0x9A, 0x1E, 0x17, 0xF0, 0xE3, 0xA5, 0x92, 0x0F, 0x12,
    0x2A, 0x5C, 0x4C, 0xA6, 0xA5, 0xCF, 0x7F, 0xE8, 0x5B, 0xF3, 0x65, 0x1A, 0xC8, 0xCF, 0x9B, 0xB9,
    0x2A, 0xC9, 0x90, 0x5D, 0xD4, 0x08, 0xCF, 0xF6, 0x03, 0x5A, 0x5A, 0xFC, 0x9E, 0xB6, 0xDB, 0x11,
    0xED, 0xE2, 0x3D, 0x62, 0xC1, 0xFC, 0x88, 0x5D, 0x97, 0xAC, 0x31, 0x2D, 0xC3, 0x15, 0xAD, 0x70,
    0x05, 0xBE, 0xA0, 0x5A, 0xE6, 0x34, 0x9C, 0x44, 0x78, 0x2B, 0xE5, 0xFE, 0x38, 0x56, 0xD4, 0x68,
    0x83, 0x13, 0xA4, 0xE6, 0xFA, 0xD2, 0x9C, 0xAB, 0xAC, 0x89, 0x5F, 0x10, 0x8F, 0x75, 0x6F, 0x04,
    0xBC, 0xAE, 0xB9, 0xBC, 0xB7, 0x1D, 0x42, 0xFA, 0x4E, 0x94, 0x1F, 0xB4, 0x0A, 0x27, 0x9C, 0x6B,
    0xAB, 0xC7, 0xD2, 0xEB, 0x27, 0x42, 0x52, 0x29, 0x41, 0xC8, 0x25, 0x40, 0x54, 0xE0, 0x48, 0x6D,
};

const uint8_t g_ypkg_dmq1[] =
{
    0x4D, 0x35, 0x67, 0x38, 0xBC, 0x90, 0x3E, 0x3B, 0xAA, 0x6C, 0xBC, 0xF2, 0xEB, 0x9E, 0x45, 0xD2,
    0x09, 0x2F, 0xCA, 0x3A, 0x9C, 0x02, 0x36, 0xAD, 0x2E, 0xC1, 0xB1, 0xB2, 0x6D, 0x7C, 0x1F, 0x6B,
    0xA1, 0x8F, 0x62, 0x20, 0x8C, 0xD6, 0x6C, 0x36, 0xD6, 0x5A, 0x54, 0x9E, 0x30, 0xA9, 0xA8, 0x25,
    0x3D, 0x94, 0x12, 0x3E, 0x0D, 0x16, 0x1B, 0xF0, 0x86, 0x42, 0x72, 0xE0, 0xD6, 0x9C, 0x39, 0x68,
    0xDB, 0x11, 0x80, 0x96, 0x18, 0x2B, 0x71, 0x41, 0x48, 0x78, 0xE8, 0x17, 0x8B, 0x7D, 0x00, 0x1F,
    0x16, 0x68, 0xD2, 0x75, 0x97, 0xB5, 0xE0, 0xF2, 0x6D, 0x0C, 0x75, 0xAC, 0x16, 0xD9, 0xD5, 0xB1,
    0xB5, 0x8B, 0xE8, 0xD0, 0xBF, 0xA7, 0x1F, 0x61, 0x5B, 0x08, 0xF8, 0x68, 0xE7, 0xF0, 0xD1, 0xBC,
    0x39, 0x60, 0xBF, 0x55, 0x9C, 0x7C, 0x20, 0x30, 0xE8, 0x50, 0x28, 0x44, 0x02, 0xCE, 0x51, 0x2A,
};

const uint8_t g_ypkg_iqmp[] =
{
    0xF5, 0x73, 0xB8, 0x7E, 0x5C, 0x98, 0x7C, 0x87, 0x67, 0xF1, 0xDA, 0xAE, 0xA0, 0xF9, 0x4B, 0xAB,
    0x77, 0xD8, 0xCE, 0x64, 0x6A, 0xC1, 0x4F, 0xA6, 0x9B, 0xB9, 0xAA, 0xCC, 0x76, 0x09, 0xA4, 0x3F,
    0xB9, 0xFA, 0xF5, 0x62, 0x84, 0x0A, 0xB8, 0x49, 0x02, 0xDF, 0x9E, 0xC4, 0x1A, 0x37, 0xD3, 0x56,
    0x0D, 0xA4, 0x6E, 0x15, 0x07, 0x15, 0xA0, 0x8D, 0x97, 0x9D, 0x92, 0x20, 0x43, 0x52, 0xC3, 0xB2,
    0xFD, 0xF7, 0xD3, 0xF3, 0x69, 0xA2, 0x28, 0x4F, 0x62, 0x6F, 0x80, 0x40, 0x5F, 0x3B, 0x80, 0x1E,
    0x5E, 0x38, 0x0D, 0x8B, 0x56, 0xA8, 0x56, 0x58, 0xD8, 0xD9, 0x6F, 0xEA, 0x12, 0x2A, 0x40, 0x16,
    0xC1, 0xED, 0x3D, 0x27, 0x16, 0xA0, 0x63, 0x97, 0x61, 0x39, 0x55, 0xCC, 0x8A, 0x05, 0xFA, 0x08,
    0x28, 0xFD, 0x55, 0x56, 0x31, 0x94, 0x65, 0x05, 0xE7, 0xD3, 0x57, 0x6C, 0x0D, 0x1C, 0x67, 0x0B,
};

const uint8_t g_FakeKeySeed[] =
{
    0x46, 0x41, 0x4B, 0x45, 0x46, 0x41, 0x4B, 0x45, 0x46, 0x41, 0x4B, 0x45, 0x46, 0x41, 0x4B, 0x45,
};

int npdrm_cmd_5_sceSblServiceMailbox(uint64_t handle, const NpDrmCmd5* input, NpDrmCmd5* output) {
    auto printf                 = (void (*)(const char *fmt, ...)) kdlsym(KERNEL_SYM_PRINTF);
    auto sceSblServiceMailbox   = (int (*)(uint64_t handle, void *in, void *out)) kdlsym(KERNEL_SYM_SCESBLSERVICEMAILBOX);

    printf("npdrm_cmd_5_sceSblServiceMailbox pre call\n");
    //hexdump(input, 0x80, nullptr, 0);
    if(input->rif_pa) {
        //auto va = get_dmap_addr(input->rif_pa);
        //printf("rif_dump_pre\n");
        //hexdump(va, 0x400, nullptr, 0);
    }

    int res = sceSblServiceMailbox(handle, (void *) input, output);
    if(output->res == 0x800F0A01) {
        printf("fixup npdrm cmd 5\n");
        auto layout = reinterpret_cast<RifCmd5MemoryLayout*>(get_dmap_addr(input->rif_pa));
        if(layout->rif.type == 2) {

            layout->output.version = __builtin_bswap16(layout->rif.version);
            layout->output.unk04 = __builtin_bswap16(layout->rif.unk06);
            layout->output.psnid = __builtin_bswap64(layout->rif.psnid);
            layout->output.startTimestamp = __builtin_bswap64(layout->rif.startTimestamp);
            layout->output.endTimestamp = __builtin_bswap64(layout->rif.endTimestamp);
            layout->output.extraFlags = __builtin_bswap64(layout->rif.extraFlags);
            layout->output.type = __builtin_bswap16(layout->rif.type);
            layout->output.contentType = __builtin_bswap16(layout->rif.contentType);
            layout->output.skuFlag = __builtin_bswap16(layout->rif.skuFlag);
            layout->output.unk34 = __builtin_bswap32(layout->rif.unk60);
            layout->output.unk38 = __builtin_bswap32(layout->rif.unk64);
            layout->output.unk3C = 0;
            layout->output.unk40 = 0;
            layout->output.unk44 = 0;
            memcpy(layout->output.contentId, layout->rif.contentId, 0x30);
            memcpy(layout->output.rifIv, layout->rif.rifIv, 0x10);
            layout->output.unk88 = __builtin_bswap32(layout->rif.unk70);
            layout->output.unk8C = __builtin_bswap32(layout->rif.unk74);
            layout->output.unk90 = __builtin_bswap32(layout->rif.unk78);
            layout->output.unk94 = __builtin_bswap32(layout->rif.unk7C);
            memcpy(layout->output.unk98, layout->rif.unk80, 0x10);
            if (layout->output.skuFlag == 2) {
                layout->output.skuFlag = 1;
            }

            output->res = 0;
            res = 0;
        }
    }

    //printf("npdrm_cmd_5_sceSblServiceMailbox post call (%i)\n", res);
    //hexdump(output, 0x80, nullptr, 0);

    if(output->rif_pa) {
        //auto va = get_dmap_addr(output->rif_pa);
        //printf("rif_dump_post\n");
        //hexdump(va, 0x4A8, nullptr, 0);
    }

    return res;
}

int npdrm_cmd_6_sceSblServiceMailbox(uint64_t handle, const NpDrmCmd6* input, NpDrmCmd6* output) {
    auto printf                 = (void (*)(const char *fmt, ...)) kdlsym(KERNEL_SYM_PRINTF);
    auto sceSblServiceMailbox   = (int (*)(uint64_t handle, void *in, void *out)) kdlsym(KERNEL_SYM_SCESBLSERVICEMAILBOX);
    auto bnet_crypto_aes_cbc_cfb128_decrypt = (void (*)(void *, void *, size_t, void *, size_t, void *)) kdlsym(KERNEL_SYM_BNET_CRYPTO_AES_CBC_CFB128_DECRYPT);

    printf("npdrm_cmd_6_sceSblServiceMailbox pre call\n");
    //hexdump(input, 0x80, nullptr, 0);
    if(input->rif_pa) {
        //auto va = get_dmap_addr(input->rif_pa);
        //printf("rif_dump_pre\n");
        //hexdump(va, 0x400, nullptr, 0);
    }

    int res = sceSblServiceMailbox(handle, (void *) input, output);
    if(output->res == 0x800F0A01) {
        printf("fixup npdrm cmd\n");
        auto va = reinterpret_cast<Rif*>(get_dmap_addr(input->rif_pa));
        if(va->type == 0x2) {
            bnet_crypto_aes_cbc_cfb128_decrypt(va->rifSecret, va->rifSecret, sizeof(va->rifSecret), (void *) rif_debug_key, 128, va->rifIv);
            memcpy(output->unk10, &va->rifSecret[0x70], 0x10);
            memcpy(output->unk20, &va->rifSecret[0x80], 0x10);
            output->res = 0;
        }

    }

    //printf("npdrm_cmd_6_sceSblServiceMailbox post call (%i)\n", res);
    //hexdump(output, 0x80, nullptr, 0);
    if(output->rif_pa) {
        //auto va = get_dmap_addr(output->rif_pa);
        //printf("rif_dump_post\n");
        //hexdump(va, 0x4A8, nullptr, 0);
    }

    return res;
}

int RsaesPkcs1v15Dec2048CRT(RsaBuffer *output, RsaBuffer *input, RsaKey *key) {
    //auto printf = (void (*)(const char *fmt, ...)) kdlsym(KERNEL_SYM_PRINTF);
    auto M_TEMP = (void *) kdlsym(KERNEL_SYM_M_TEMP);
    auto malloc = (void*(*)(unsigned long size, void* type, int flags)) kdlsym(KERNEL_SYM_MALLOC);
    auto free   = (void(*)(void* addr, void* type)) kdlsym(KERNEL_SYM_FREE);
    auto fpu_kern_enter = (int (*)(void *td, void *ctx, int)) kdlsym(KERNEL_SYM_FPU_KERN_ENTER);
    auto fpu_kern_leave = (int (*)(void *td, void *ctx)) kdlsym(KERNEL_SYM_FPU_KERN_LEAVE);
    auto LoCACRYPTO_rsadpCRT_core = (int (*)(void *, void *, void *, size_t)) kdlsym(KERNEL_SYM_LACACRYPTO_RSADPCRT_CORE);

    auto fpu_ctx = (void *) malloc(0x1000, M_TEMP, 0x102);
    auto thr = curthread;
    fpu_kern_enter(thr, fpu_ctx, 0);

    uint8_t buffer[0x200];
    int res = -1;
    if(input->size != 0x100) {
        fpu_kern_leave(thr, fpu_ctx);
        free(fpu_ctx, M_TEMP);
        return res;
    }

    for(int i = 0; i < 0x100; i++) {
        buffer[0x80 + i] = input->ptr[0xFF - i];
    }

    if(LoCACRYPTO_rsadpCRT_core(&buffer[0x80], &buffer[0x80], key, 0x40) == -1) {
        fpu_kern_leave(thr, fpu_ctx);
        free(fpu_ctx, M_TEMP);
        return res;
    }

    for(int i = 0; i < 0x20; i++) {
        output->ptr[i] = buffer[0x9F - i];
    }

    fpu_kern_leave(thr, fpu_ctx);
    free(fpu_ctx, M_TEMP);
    return 0;
}

int verifySuperBlock_sceSblServiceMailbox(uint64_t handle, const PfsmgrCmd11* input, PfsmgrCmd11 *output)
{
    int ret;

    auto printf = (void (*)(const char *fmt, ...)) kdlsym(KERNEL_SYM_PRINTF);
    auto sceSblServiceMailbox = (int (*)(uint64_t handle, void *in, void *out)) kdlsym(KERNEL_SYM_SCESBLSERVICEMAILBOX);
    auto Sha256Hmac = (void (*)(void *hash, void *data, size_t data_sz, void *key, size_t key_size)) kdlsym(KERNEL_SYM_SHA256_HMAC);

    printf("sceSblPfsSetKeys verify superblock\n");

    ret = sceSblServiceMailbox(handle, (void *) input, (void *) output);
    if (ret != 0 || output->res != 0) {
        printf("verifySuperBlock_sceSblServiceMailbox: register fake keys\n");
        auto tablePA    = input->tablePa;
        auto headerPA   = input->headerPa;
        auto header     = (uint8_t *) get_dmap_addr(headerPA);

        if (!tablePA || !headerPA) {
            printf("verifySuperBlock_sceSblServiceMailbox: no tablePA or headerPA (0x%lx, 0x%lx)\n", tablePA, headerPA);
            return ret;
        }

        auto table = (struct sbl_chunk_table_header *) get_dmap_addr(tablePA);
        printf("first pa:       0x%lx\n", table->first_pa);
        printf("data_size:      0x%lx\n", table->data_size);
        printf("used_entries:   0x%lx\n", table->used_entries);
        printf("unk18:          0x%lx\n", table->unk18);
        printf("entry[0] pa:    0x%lx\n", table->entries[0].pa);
        printf("entry[0] sz:    0x%lx\n", table->entries[0].size);

        auto keyPA      = table->entries[0].pa;
        auto key        = (uint8_t *) get_dmap_addr(keyPA);
        if (!keyPA) {
            printf("verifySuperBlock_sceSblServiceMailbox: no keyPA (0x%lx)\n", keyPA);
            return ret;
        }

        if (table->data_size == 0x100) {
            RsaBuffer rsaInput{};
            rsaInput.ptr    = key;
            rsaInput.size   = 0x100;

            uint8_t ekpfs[0x20]{0};
            RsaBuffer rsaOutput{};
            rsaOutput.ptr = ekpfs;
            rsaOutput.size = 0x20;

            RsaKey rsaKey{};
            rsaKey.p    = g_ypkg_p;
            rsaKey.q    = g_ypkg_q;
            rsaKey.dmp1 = g_ypkg_dmp1;
            rsaKey.dmq1 = g_ypkg_dmq1;
            rsaKey.iqmp = g_ypkg_iqmp;

            RsaesPkcs1v15Dec2048CRT(&rsaOutput, &rsaInput, &rsaKey);

            auto pfsSeed = &header[0x370];
            uint8_t pfs_seed[0x14]{};
            memcpy(&pfs_seed[0x4], pfsSeed, 0x10);
            *(uint32_t*)(pfs_seed) = 0x1;
            uint8_t xts_key[0x20]{};
            Sha256Hmac(xts_key, pfs_seed, 0x14, ekpfs, 0x20);
            *(uint32_t*)(pfs_seed) = 0x2;
            uint8_t hmac_key[0x20]{};
            Sha256Hmac(hmac_key, pfs_seed, 0x14, ekpfs, 0x20);

            printf("xts_key:\n");
            printf("%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n", 
                xts_key[0x00], xts_key[0x01], xts_key[0x02], xts_key[0x03],
                xts_key[0x04], xts_key[0x05], xts_key[0x06], xts_key[0x07], 
                xts_key[0x08], xts_key[0x09], xts_key[0x0A], xts_key[0x0B], 
                xts_key[0x0C], xts_key[0x0D], xts_key[0x0E], xts_key[0x0F]);
            printf("%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n", 
                xts_key[0x10], xts_key[0x11], xts_key[0x12], xts_key[0x13],
                xts_key[0x14], xts_key[0x15], xts_key[0x16], xts_key[0x17], 
                xts_key[0x18], xts_key[0x19], xts_key[0x1A], xts_key[0x1B], 
                xts_key[0x1C], xts_key[0x1D], xts_key[0x1E], xts_key[0x1F]);

            printf("hmac_key:\n");
            printf("%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n", 
                hmac_key[0x00], hmac_key[0x01], hmac_key[0x02], hmac_key[0x03],
                hmac_key[0x04], hmac_key[0x05], hmac_key[0x06], hmac_key[0x07], 
                hmac_key[0x08], hmac_key[0x09], hmac_key[0x0A], hmac_key[0x0B], 
                hmac_key[0x0C], hmac_key[0x0D], hmac_key[0x0E], hmac_key[0x0F]);
            printf("%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n", 
                hmac_key[0x10], hmac_key[0x11], hmac_key[0x12], hmac_key[0x13],
                hmac_key[0x14], hmac_key[0x15], hmac_key[0x16], hmac_key[0x17], 
                hmac_key[0x18], hmac_key[0x19], hmac_key[0x1A], hmac_key[0x1B], 
                hmac_key[0x1C], hmac_key[0x1D], hmac_key[0x1E], hmac_key[0x1F]);

            int key0 = register_fake_key((const char *) &xts_key);
            int key1 = register_fake_key((const char *) &hmac_key);
            output->keyHandle0 = IDX_TO_HANDLE(key0);
            output->keyHandle1 = IDX_TO_HANDLE(key1);

            printf("verifySuperBlock_sceSblServiceMailbox: key0 = 0x%x (handle = 0x%x), key1 = 0x%x (handle = 0x%x)\n", key0, output->keyHandle0, key1, output->keyHandle1);
            output->res = 0;
            ret = 0;
        }
    }

    return ret;
}

struct ccp_msg
{
    uint64_t unk_00h;
    uint64_t unk_08h;
    uint64_t unk_10h;
    uint64_t unk_18h;
    uint64_t unk_20h;
    uint64_t unk_28h;
    uint64_t unk_30h;
    uint64_t unk_38h;
    uint64_t unk_40h;
    uint64_t unk_48h;
    uint64_t unk_50h;
    uint64_t unk_58h;
    uint64_t unk_60h;
    uint64_t unk_68h;
    uint64_t unk_70h;
    uint64_t unk_78h;
    uint64_t unk_80h;
    uint64_t unk_88h;
    uint64_t unk_90h;
    uint64_t unk_98h;
    uint64_t unk_A0h;
    uint64_t unk_A8h;
    uint64_t unk_B0h;
    uint64_t unk_C0h;
    uint64_t unk_C8h;
    uint64_t unk_D0h;
    uint64_t unk_D8h;
    uint64_t unk_E0h;
    uint64_t unk_E8h;
    uint64_t unk_F0h;
    uint64_t unk_F8h;
    uint64_t unk_100h;
    uint64_t unk_108h;
    uint64_t unk_110h;
    uint64_t unk_118h;
    uint64_t unk_120h;
    uint64_t unk_128h;
    uint64_t unk_130h;
    uint64_t unk_138h;
    struct ccp_msg *next;
    uint64_t unk_148h;
};

struct ccp_common
{
    uint32_t cmd;               // 0x00
    uint32_t status;            // 0x10
};

struct ccp_hmac
{
    struct ccp_common common;   // 0x00
    uint64_t data_size;         // 0x08
    void *data;                 // 0x10
    uint64_t data_size_bits;    // 0x18
    union {                     // 0x20
        uint16_t keygen_index;
        uint8_t hash[0x20];
    };
    char unk_40h[0x60];         // 0x40
    union {                     // 0xA0
        uint32_t key_index;     
        uint8_t key[0x40];
    };
    char unk_E0h[0x50];         // 0xE0
    uint64_t key_size;          // 0x130
};

struct ccp_xts
{
    struct ccp_common common;   // 0x00
    uint32_t num_sectors;       // 0x08
    void *in_data;              // 0x10
    void *out_data;             // 0x18
    uint64_t start_sector;      // 0x20
    union {                     // 0x28
        uint32_t key_index;
        uint8_t key[0x20];
    };
};

struct ccp_req {
    struct ccp_msg* tqh_first;
    struct ccp_msg** tqh_last;
    void(*cb)(void*, int);
    void* args;
    uint64_t mid;
    void* le_next;
    void** le_prev;
};


int sceSblServiceCryptAsync_hook(void *async_req)
{
    struct ccp_common *msg;

    auto printf = (void (*)(const char *fmt, ...)) kdlsym(KERNEL_SYM_PRINTF);
    auto sceSblServiceCryptAsync = (int (*)(void *req)) kdlsym(KERNEL_SYM_SCE_SBL_SERVICE_CRYPT_ASYNC);
    auto Sha256Hmac = (void (*)(void *hash, void *data, size_t data_sz, void *key, size_t key_size)) kdlsym(KERNEL_SYM_SHA256_HMAC);
    auto sceSblFinalizeCryptAsync = (void (*)(void *msg, int status)) kdlsym(KERNEL_SYM_SCE_SBL_FINALIZE_CRYPT_ASYNC);

    msg = (struct ccp_common *) (*(uint64_t *) (async_req));
    //printf("sceSblServiceCryptAsync_hook: msg = %p, before (msg->cmd = 0x%x)\n", msg, msg->cmd);

    if ((msg->cmd & 0x7FFFFFFF) == 0x9132000) { // SHA256 HMAC with key handle
        struct ccp_hmac *hmac_msg = (struct ccp_hmac *) msg;
        int idx = HANDLE_TO_IDX(hmac_msg->key_index);
        //printf("sceSblServiceCryptAsync_hook: SHA256 hmac key idx = 0x%x\n", idx);

        if (idx < 0)
            return sceSblServiceCryptAsync(async_req);

        if (idx >= 0) {
            get_fake_key(idx, (char *) &hmac_msg->key);
            hmac_msg->common.cmd &= ~(1 << 20);
            hmac_msg->key_size = 0x20;
        }

        // printf("sceSblServiceCryptAsync_hook: cmd = 0x%x\n", msg->cmd);
        // printf("%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n", 
        //     hmac_msg->key[0x00], hmac_msg->key[0x01], hmac_msg->key[0x02], hmac_msg->key[0x03],
        //     hmac_msg->key[0x04], hmac_msg->key[0x05], hmac_msg->key[0x06], hmac_msg->key[0x07], 
        //     hmac_msg->key[0x08], hmac_msg->key[0x09], hmac_msg->key[0x0A], hmac_msg->key[0x0B], 
        //     hmac_msg->key[0x0C], hmac_msg->key[0x0D], hmac_msg->key[0x0E], hmac_msg->key[0x0F]);
        // printf("%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n", 
        //     hmac_msg->key[0x10], hmac_msg->key[0x11], hmac_msg->key[0x12], hmac_msg->key[0x13],
        //     hmac_msg->key[0x14], hmac_msg->key[0x15], hmac_msg->key[0x16], hmac_msg->key[0x17], 
        //     hmac_msg->key[0x18], hmac_msg->key[0x19], hmac_msg->key[0x1A], hmac_msg->key[0x1B], 
        //     hmac_msg->key[0x1C], hmac_msg->key[0x1D], hmac_msg->key[0x1E], hmac_msg->key[0x1F]);
        // printf("%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n", 
        //     hmac_msg->key[0x20], hmac_msg->key[0x21], hmac_msg->key[0x22], hmac_msg->key[0x23],
        //     hmac_msg->key[0x24], hmac_msg->key[0x25], hmac_msg->key[0x26], hmac_msg->key[0x27], 
        //     hmac_msg->key[0x28], hmac_msg->key[0x29], hmac_msg->key[0x2A], hmac_msg->key[0x2B], 
        //     hmac_msg->key[0x2C], hmac_msg->key[0x2D], hmac_msg->key[0x2E], hmac_msg->key[0x2F]);
        // printf("%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n", 
        //     hmac_msg->key[0x30], hmac_msg->key[0x31], hmac_msg->key[0x32], hmac_msg->key[0x33],
        //     hmac_msg->key[0x34], hmac_msg->key[0x35], hmac_msg->key[0x36], hmac_msg->key[0x37], 
        //     hmac_msg->key[0x38], hmac_msg->key[0x39], hmac_msg->key[0x3A], hmac_msg->key[0x3B], 
        //     hmac_msg->key[0x3C], hmac_msg->key[0x3D], hmac_msg->key[0x3E], hmac_msg->key[0x3F]);

        Sha256Hmac(hmac_msg->hash, hmac_msg->data, hmac_msg->data_size, hmac_msg->key, 0x20);
        struct ccp_req* req = (struct ccp_req*)async_req;
        //printf("ccp_req->args: (%lx)\n", req->args);
        req->cb(req->args, 0);
        sceSblFinalizeCryptAsync((void *) hmac_msg, 0);
        return 0;
    } else if ((msg->cmd & 0x7FFFF7FF) == 0x2108000) { // AES-XTS with key handle
        struct ccp_xts *xts_msg = (struct ccp_xts *) msg;
        int idx = HANDLE_TO_IDX(xts_msg->key_index);
        //printf("sceSblServiceCryptAsync_hook: AES-XTS key idx = 0x%x\n", idx);

        if (idx >= 0) {
            get_fake_key(idx, (char *) &xts_msg->key);
            xts_msg->common.cmd &= ~(1 << 20);
        }

        // printf("sceSblServiceCryptAsync_hook: cmd = 0x%x\n", msg->cmd);
        // printf("%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n", 
        //     xts_msg->key[0x00], xts_msg->key[0x01], xts_msg->key[0x02], xts_msg->key[0x03],
        //     xts_msg->key[0x04], xts_msg->key[0x05], xts_msg->key[0x06], xts_msg->key[0x07], 
        //     xts_msg->key[0x08], xts_msg->key[0x09], xts_msg->key[0x0A], xts_msg->key[0x0B], 
        //     xts_msg->key[0x0C], xts_msg->key[0x0D], xts_msg->key[0x0E], xts_msg->key[0x0F]);
        // printf("%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n", 
        //     xts_msg->key[0x10], xts_msg->key[0x11], xts_msg->key[0x12], xts_msg->key[0x13],
        //     xts_msg->key[0x14], xts_msg->key[0x15], xts_msg->key[0x16], xts_msg->key[0x17], 
        //     xts_msg->key[0x18], xts_msg->key[0x19], xts_msg->key[0x1A], xts_msg->key[0x1B], 
        //     xts_msg->key[0x1C], xts_msg->key[0x1D], xts_msg->key[0x1E], xts_msg->key[0x1F]);
    }

    return sceSblServiceCryptAsync(async_req);
}

void apply_fpkg_hooks()
{
    auto printf = (void (*)(const char *fmt, ...)) kdlsym(KERNEL_SYM_PRINTF);

    printf("[HEN] [FPKG] npdrm_ioctl(cmd=5) -> sceSblServiceMailbox()\n");
    install_hook(HOOK_FPKG_NPDRM_IOCTL_CMD_5_CALL_SCE_SBL_SERVICE_MAILBOX, (void *) &npdrm_cmd_5_sceSblServiceMailbox);

    printf("[HEN] [FPKG] npdrm_ioctl(cmd=6) -> sceSblServiceMailbox()\n");
    install_hook(HOOK_FPKG_NPDRM_IOCTL_CMD_6_CALL_SCE_SBL_SERVICE_MAILBOX, (void *) &npdrm_cmd_6_sceSblServiceMailbox);

    printf("[HEN] [FPKG] sceSblPfsVerifySuperBlock() -> sceSblServiceMailbox()\n");
    install_hook(HOOK_FPKG_PFS_VERIFY_SUPER_BLOCK_CALL_SCE_SBL_SERVICE_MAILBOX, (void *) &verifySuperBlock_sceSblServiceMailbox);

    //printf("[HEN] [FPKG] sceSblServiceCryptAsync() -> ccpMsgEnqueue()\n");
    //install_hook(HOOK_FPKG_SCE_SBL_SERVICE_CRYPT_ASYNC_CALL_CCP_MSG_ENQUEUE, (void *) &sceSblServiceCryptAsync_ccpMsgEnqueue);

    // Install hook on all calls to sceSblServiceCryptAsync()
    printf("[HEN] [FPKG] installing hooks to sceSblServiceCryptAsync() [0x%lx, 0x%lx]\n", ktext(0), kdlsym(KERNEL_SYM_TEXT_END));
    for (uint64_t scan_ptr = ktext(0); scan_ptr < kdlsym(KERNEL_SYM_TEXT_END); scan_ptr++) {
        uint8_t *scan = (uint8_t *) scan_ptr;
        int32_t target_rel32;
        int32_t rel32;

        if (scan[0] == 0xE8) {
            target_rel32 = (int32_t) ((uint64_t) (kdlsym(KERNEL_SYM_SCE_SBL_SERVICE_CRYPT_ASYNC)) - scan_ptr) - 5;
            rel32 = *(int32_t *) (scan + 1);

            if (rel32 == target_rel32) {
                install_raw_hook(scan_ptr, (void *) &sceSblServiceCryptAsync_hook);
            }
        }
    }

    printf("[HEN] [FPKG] done\n");
}