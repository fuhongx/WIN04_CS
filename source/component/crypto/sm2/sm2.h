#ifndef __SM2_H__
#define __SM2_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "mpi.h"

#define SM2_PRIVATE_KEY_SIZE 32

typedef struct sm2_pubkey
{
    mpi_t x;
    mpi_t y;
} sm2_pubkey_t;

typedef struct sm2_prvkey
{
    mpi_t d;
    mpi_t i;
} sm2_prvkey_t;

typedef struct sm2_ctx
{
    mpi_t p;
    mpi_t a;
    mpi_t b;
    mpi_t x;
    mpi_t y;
    mpi_t n;
    mpi_t inv;
    sm2_pubkey_t pubkey;
    sm2_prvkey_t prikey;
} sm2_ctx_t;

int32_t sm2_check_point(struct sm2_ctx *ctx, mpi_t *x, mpi_t *y);
int32_t sm2_import_point(struct sm2_ctx *ctx, uint8_t *pbIn, int32_t iInLen, mpi_t *x, mpi_t *y);
int32_t sm2_export_point(struct sm2_ctx *ctx, uint8_t *pbOut, mpi_t *x, mpi_t *y, int32_t fCompression);
int32_t sm2_generate_key(struct sm2_ctx *ctx, uint8_t *ramdom, uint32_t size);
int32_t sm2_set_private_key(struct sm2_ctx *ctx, uint8_t *data, uint32_t size);
int32_t sm2_set_public_key(struct sm2_ctx *ctx, uint8_t *data, uint32_t size);
int32_t sm2_sign_message(struct sm2_ctx *ctx, uint8_t *out, uint8_t *in, int32_t inlen, char *uname, int32_t unamelen, uint8_t *random, uint32_t randlen);								
int32_t sm2_verify_message(struct sm2_ctx *ctx, uint8_t *in,  int32_t inlen, uint8_t *pSig, char *uname, int32_t unamelen);
int32_t sm2_encrypt_message(struct sm2_ctx *ctx, uint8_t *out, uint8_t *in, int32_t inlen, uint8_t *random, int32_t rlen);
int32_t sm2_decrypt_message(struct sm2_ctx *ctx, uint8_t *out, uint8_t *in, int32_t inlen);
int32_t sm2_sign(struct sm2_ctx *ctx, uint8_t *in, int32_t inlen, uint8_t *out, uint8_t *random, int32_t rlen);
int32_t sm2_verify(struct sm2_ctx *ctx, uint8_t *hash, uint8_t *signature);								
int32_t sm2_encrypt(struct sm2_ctx *ctx, uint8_t *out, uint8_t *px, uint8_t *random, uint32_t rlen);
int32_t sm2_decrypt(struct sm2_ctx *ctx, uint8_t *in, int32_t inlen, uint8_t *out);
int32_t sm2_init(struct sm2_ctx *ctx);
int32_t sm2_kp_sw(uint8_t prikey[64], uint8_t pubkey[64], uint8_t out[64]);
int sm2_key_exchange_rnd_msg(struct sm2_ctx *ctx, uint8_t *pbRndMsg, uint8_t *pbRnd, int iLenOfRnd);

int sm2_key_exchange_result(struct sm2_ctx *ctx, uint8_t *pOut, int iLenOut, 
                            uint8_t *pbMyRnd, int iLenOfRnd, char *pMyUserName, int iLenOfMyUserName,
                            uint8_t *pbOtherRndMsg, int iLenOfRndMsg, sm2_pubkey_t *pOtherPublicKey, 
                            char *pOtherUserName, int iLenOfOtherUserName, int fInit);
                            
#ifdef __cplusplus
}
#endif

#endif /* __SM2_H__ */
