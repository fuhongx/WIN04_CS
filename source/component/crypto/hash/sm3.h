#ifndef __SM3_H__
#define __SM3_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
    
#define SM3_HASH_256 32

typedef struct sm3_ctx
{
    uint32_t h[8];
    uint8_t bb[64];
    uint64_t len;
} sm3_ctx_t;

uint32_t sm3_kdf(unsigned char* pKeyOut, uint32_t iLenOfOut, const unsigned char* pSecret, uint32_t iLenOfSecret, uint32_t ct);

int32_t sm3(const uint8_t *in, uint32_t inlen, uint8_t *hash);
int32_t sm3_init(struct sm3_ctx *ctx, const uint8_t *in, uint32_t inlen);
int32_t sm3_update(struct sm3_ctx *ctx, const uint8_t *in, uint32_t inlen);
int32_t sm3_final(struct sm3_ctx *ctx, uint8_t *hash);

void hmac_sm3(uint8_t *key, uint32_t keylen, uint8_t *input, uint32_t ilen, uint8_t *output);

#ifdef __cplusplus
}
#endif

#endif /* __SM3_H__ */
