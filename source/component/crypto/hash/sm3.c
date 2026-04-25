#include <string.h>
#include "sm3.h"

#define ROTL(x,y)	((((x)<<(y&(32-1))) | ((x)>>(32-(y&(32-1))))) & 0xffffffff)

void sm3_ms(uint32_t *pW, const uint32_t *pMsg)
{
    int32_t j;
	
    for (j = 0; j < 16; j++)
    {
        pW[j] = pMsg[j];
		}

    for (; j < 68; j++)
    {
        pW[j] = pW[j-16] ^ pW[j-9] ^ ROTL(pW[j-3], 15);
        pW[j] ^= (ROTL(pW[j], 15) ^ ROTL(pW[j], 23));
        pW[j] ^= ((ROTL(pW[j-13], 7) ^ pW[j-6]));
    }

    for (j = 0; j < 64; j++)
		{
        pW[68+j] = pW[j] ^ pW[j+4];
		}
}

void sm3_cf(uint32_t *pH, const uint8_t *pMsg)
{
    uint32_t A = pH[0], B = pH[1], C = pH[2], D = pH[3], E = pH[4], F = pH[5], G = pH[6], H = pH[7];
    uint32_t w[68+64];

    // w[0..67] for W
    // w[68+0 .. 68+63] for W'

    int32_t j;
    // unsigned char --> uint32_t
    uint32_t BB[16];
    for (j = 0; j < 16; j++)
    {
        BB[j] = (pMsg[4*j] << 24) | (pMsg[4*j+1] << 16) | (pMsg[4*j+2] << 8) | pMsg[4*j+3];
		}

    sm3_ms(w, BB);

    uint32_t SS1, SS2, TT1, TT2;
    uint32_t T = 0x79CC4519;
    for (j = 0; j < 16; j++)
    {
        SS1 = (ROTL(A, 12) + E + ROTL(T, j))%0x100000000;
        SS1 = ROTL(SS1, 7);
        SS2 = SS1 ^ ROTL(A, 12);
        TT1 = (((A^B^C) + D + SS2) + w[j+68])%0x100000000;
        TT2 = (((E^F^G) + H + SS1) + w[j])%0x100000000;
        D = C;
        C = ROTL(B, 9);
        B = A;
        A = TT1;
        H = G;
        G = ROTL(F, 19);
        F = E;
        E = TT2 ^ ROTL(TT2, 9) ^ ROTL(TT2, 17);
    }

    T = 0x7A879D8A;
    for (; j < 64; j++)
    {
        SS1 = (ROTL(A, 12) + E + ROTL(T, j))%0x100000000;
        SS1 = ROTL(SS1, 7);
        SS2 = SS1 ^ ROTL(A, 12);
        TT1 = ((((A&B) | (A&C) | (B&C)) + D + SS2) + w[j+68])%0x100000000;
        TT2 = ((((E&F) | ((~E)&G)) + H + SS1) + w[j])%0x100000000;
        D = C;
        C = ROTL(B, 9);
        B = A;
        A = TT1;
        H = G;
        G = ROTL(F, 19);
        F = E;
        E = TT2 ^ ROTL(TT2, 9) ^ ROTL(TT2, 17);
    }

    pH[0] ^= A;
    pH[1] ^= B;
    pH[2] ^= C;
    pH[3] ^= D;
    pH[4] ^= E;
    pH[5] ^= F;
    pH[6] ^= G;
    pH[7] ^= H;
}

uint32_t sm3_kdf(unsigned char *pKeyOut, uint32_t iLenOfOut, const unsigned char *pSecret, uint32_t iLenOfSecret, uint32_t ct)
{
    unsigned char acHashResult[SM3_HASH_256];
    struct sm3_ctx state;
    uint32_t tmp;
    uint32_t i, iRound = iLenOfOut/SM3_HASH_256;
    for (i = 0; i < iRound; i++)
    {
        sm3_init(&state, pSecret, iLenOfSecret);
        tmp = (ct << 24) | ((ct&0x0000ff00) << 8) | ((ct&0x00ff0000) >> 8) | (ct >> 24);
        sm3_update(&state, (unsigned char *)&tmp, sizeof(tmp));
        sm3_final(&state, pKeyOut + i*SM3_HASH_256);
        ct++;
    }

    if (0 != (iLenOfOut % SM3_HASH_256))
    {
        sm3_init(&state, pSecret, iLenOfSecret);
        tmp = (ct << 24) | ((ct&0x0000ff00) << 8) | ((ct&0x00ff0000) >> 8) | (ct >> 24);
        sm3_update(&state, (unsigned char *)&tmp, sizeof(tmp));
        sm3_final(&state, acHashResult);

        for (i = 0; i < iLenOfOut % SM3_HASH_256; i++)
        {
            pKeyOut[iLenOfOut - (iLenOfOut % SM3_HASH_256) + i] = acHashResult[i];
				}
        ct++;
    }

    return iLenOfOut;
}

int32_t sm3_init(struct sm3_ctx *ctx, const uint8_t *in, uint32_t inlen)
{
    uint32_t i = 0;

    ctx->h[0] = 0x7380166F;
    ctx->h[1] = 0x4914B2B9;
    ctx->h[2] = 0x172442D7;
    ctx->h[3] = 0xDA8A0600;
    ctx->h[4] = 0xA96F30BC;
    ctx->h[5] = 0x163138AA;
    ctx->h[6] = 0xE38DEE4D;
    ctx->h[7] = 0xB0FB0E4E;

    ctx->len = inlen;

    for (i = 0; i < inlen/64; i++)
    {
        sm3_cf(ctx->h, in+i*64);
    }

    for (i = 0; i < (inlen & 63); i++)
    {
        ctx->bb[i] = in[(inlen&0xffffffc0)+i];
    }

    return 0;
}

int32_t sm3_update(struct sm3_ctx *ctx, const uint8_t *in, uint32_t inlen)
{
    uint32_t i, iCurPos = (uint32_t)(ctx->len % 64);
    uint32_t iMin = (64-iCurPos < inlen) ? (64-iCurPos) : (inlen);

    for (i = 0; i < iMin; i++)
    {
        ctx->bb[i+iCurPos] = in[i];
    }
	
    ctx->len += inlen;

    if (inlen >= 64-iCurPos)
    {
        sm3_cf(ctx->h, ctx->bb);

        inlen -= (64-iCurPos);
        in += (64-iCurPos);
        for (i = 0; i < inlen/64; i++)
        {
            sm3_cf(ctx->h, in+i*64);
        }

        for (i = 0; i < (inlen & 63); i++)
        {
            ctx->bb[i] = in[(inlen&0xffffffc0)+i];
        }
    }

    return 0;
}

int32_t sm3_final(struct sm3_ctx *ctx, uint8_t *hash)
{
    uint32_t i;
    uint8_t bb[128];

    for (i = 0; i < sizeof(bb); i++)
    {
        bb[i] = 0x00;
    }

    for (i = 0; i < (ctx->len & 63); i++)
    {
        bb[i] = ctx->bb[i];
    }

    uint32_t low = 0, high = 0;
    high = (uint32_t)(ctx->len >> 29);
    high = (high >> 24) | ((high&0x00ff0000) >> 8) | ((high&0x0000ff00) << 8) | (high << 24);
    low = (uint32_t)(ctx->len << 3);
    low = (low >> 24) | ((low&0x00ff0000) >> 8) | ((low&0x0000ff00) << 8) | (low << 24);

    if ((ctx->len & 63) < 56)
    {
        // 1 block
        bb[i] = 0x80;

        // BB[56 .. 63]
        *(uint32_t *)(bb+56) = high;
        *(uint32_t *)(bb+60) = low;

        sm3_cf(ctx->h, bb);
    }
    else
    {
        // 2 block
        bb[i] = 0x80;
        sm3_cf(ctx->h, bb);

        // BB[120 .. 127]
        *(uint32_t *)(bb+120) = high;
        *(uint32_t *)(bb+124) = low;

        sm3_cf(ctx->h, bb+64);
    }

    for (i = 0; i < 8; i++)
    {
		    hash[i*4] = (unsigned char)(ctx->h[i] >> 24);
		    hash[i*4+1] = (unsigned char)(ctx->h[i] >> 16);
		    hash[i*4+2] = (unsigned char)(ctx->h[i] >> 8);
		    hash[i*4+3] = (unsigned char)(ctx->h[i]);
    }

    return 0;
}

int32_t sm3(const uint8_t *in, uint32_t inlen, uint8_t *hash)
{
    uint32_t i;

    uint32_t H[8] =
    {
        0x7380166F,
        0x4914B2B9,
        0x172442D7,
        0xDA8A0600,
        0xA96F30BC,
        0x163138AA,
        0xE38DEE4D,
        0xB0FB0E4E,
    };

    for (i = 0; i < inlen/64; i++)
    {
        sm3_cf(H, in+i*64);
    }

    unsigned char BB2[128];
    for (i = 0; i < sizeof(BB2); i++)
		{
        BB2[i] = 0x00;
		}

    for (i = 0; i < (inlen & 63); i++)
    {
        BB2[i] = in[(inlen&0xffffffc0)+i];
    }

    // in bytes ==> in bits
    uint32_t low = 0, high = 0;
    high = inlen >> 29;
    high = (high >> 24) | ((high&0x00ff0000) >> 8) | ((high&0x0000ff00) << 8) | (high << 24);
    low = inlen << 3;
    low = (low >> 24) | ((low&0x00ff0000) >> 8) | ((low&0x0000ff00) << 8) | (low << 24);

    if ((inlen & 63) < 56)
    {
        // 1 block
        BB2[i] = 0x80;

        // BB[56 .. 63]
        *(uint32_t *)(BB2+56) = high;
        *(uint32_t *)(BB2+60) = low;

        sm3_cf(H, BB2);
    }
    else
    {
        // 2 block
        BB2[i] = 0x80;
        sm3_cf(H, BB2);

        // BB[120 .. 127]
        *(uint32_t *)(BB2+120) = high;
        *(uint32_t *)(BB2+124) = low;

        sm3_cf(H, BB2+64);
    }

    for (i = 0; i < 8; i++)
    {
        hash[i*4] = (unsigned char)(H[i] >> 24);
        hash[i*4+1] = (unsigned char)(H[i] >> 16);
        hash[i*4+2] = (unsigned char)(H[i] >> 8);
        hash[i*4+3] = (unsigned char)H[i];
    }

    return 0;
}

void hmac_sm3(uint8_t *key, uint32_t keylen, uint8_t *input, uint32_t ilen, uint8_t *output)
{
    uint8_t sum[64];
    uint8_t ipad[64];
    uint8_t opad[64];

    if(keylen > 64)
    {
        sm3(key, keylen, sum);
        keylen = 32;
        key = sum;
    }

    memset(ipad, 0x36, 64);
    memset(opad, 0x5C, 64);

    for(uint32_t i = 0; i < keylen; i++)
    {
        ipad[i] = (uint8_t)(ipad[i] ^ key[i]);
        opad[i] = (uint8_t)(opad[i] ^ key[i]);
    }

    struct sm3_ctx ctx;
    sm3_init(&ctx, ipad, 64);
    sm3_update(&ctx, input, ilen);
    sm3_final(&ctx, sum);

    sm3_init(&ctx, opad, 64);
    sm3_update(&ctx, sum, 32);
    sm3_final(&ctx, output);
}

#if 0
#include <assert.h>
void dump_buf(char *info, uint8_t *buf, uint32_t len)
{
    printf("%s[%d]", info, len);
    for (int i = 0; i < len; i++) {
        printf("%s%02X%s", i % 16 == 0 ? "\n     ":" ", 
                        buf[i], i == len - 1 ? "\n":"");
    }
}

int main(int argc, char const *argv[])
{
    uint8_t key[32];
    memset(key, 0x11, 32);

    uint8_t src[32];
    memset(src, 0x33, 32);

    uint8_t out[32];
    memset(out, 0x00, 32);

    hmac_sm3(key, 32, src, 32, out);
    dump_buf("hmac sm3 out", out, 32);

    return 0;
}
#endif

