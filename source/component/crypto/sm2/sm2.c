// EllipticCurve.cpp: implementation of the CEllipticCurve class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "sm2.h"
#include "sm3.h"

#define KDF sm3_kdf
#define HASH_256 SM3_HASH_256
#define HASH_STATE sm3_ctx_t
#define HashInit sm3_init
#define HashPending sm3_update
#define HashFinal sm3_final

void sm2_jacobian_2_stand(struct sm2_ctx *ctx, mpi_t *x, mpi_t *y, mpi_t *z);
void sm2_multiply_g_by_table_z(struct sm2_ctx *ctx, mpi_t *x, mpi_t *y, mpi_t *z, mpi_t *m);
// (x, y) = m*(Gx, GY)
void sm2_multiply_2(struct sm2_ctx *ctx, mpi_t *x, mpi_t *y, mpi_t *z, mpi_t *m, mpi_t *Gx, mpi_t *Gy);
// (x, y, z) = (x, y, z) + (x, y, z)
void sm2_double_mpl_jacobian(struct sm2_ctx *ctx, mpi_t *x, mpi_t *y, mpi_t *z);
// (x2, y2, z2) = (x, y, z) + (x, y, z)
void sm2_double_mpl_jacobian_2(struct sm2_ctx *ctx, mpi_t *x2, mpi_t *y2, mpi_t *z2, mpi_t *x, mpi_t *y, mpi_t *z);
// (x, y, z) = (x, y, z) + (mx, my, mz)
void sm2_add_mpl_jacobian_z(struct sm2_ctx *ctx, mpi_t *x, mpi_t *y, mpi_t *z, mpi_t *mx, mpi_t *my, mpi_t *mz);
// only for mz == 1
void sm2_add_mpl_jacobian(struct sm2_ctx *ctx, mpi_t *x, mpi_t *y, mpi_t *z, mpi_t *mx, mpi_t *my);
void sm2_multiply_g_by_table(struct sm2_ctx *ctx, mpi_t *x, mpi_t *y, mpi_t *m);

extern void dump_buf(char* info, uint8_t* buf, uint32_t len);
void sm2_double_mpl_jacobian_2(struct sm2_ctx *ctx, mpi_t *x2, mpi_t *y2, mpi_t *z2, mpi_t *x, mpi_t *y, mpi_t *z)
{
    mpi_copy(x, x2);
    mpi_copy(y, y2);
    mpi_copy(z, z2);
    sm2_double_mpl_jacobian(ctx, x2, y2, z2);
}

void sm2_multiply_2(struct sm2_ctx *ctx, mpi_t *x, mpi_t *y, mpi_t *z, mpi_t *m, mpi_t *Gx, mpi_t *Gy)
{
#define WINDOWS_BITS		(4)
    static mpi_t pre_table[1<<WINDOWS_BITS][3];	// Jacobian
    int32_t i, k, pos;

    mpi_copy(Gx, &pre_table[1][0]);
    mpi_copy(Gy, &pre_table[1][1]);
    mpi_copy_d(&pre_table[1][2], 1);
	
    for (i = 2; i < (1<<WINDOWS_BITS); i += 2)
    {
        sm2_double_mpl_jacobian_2(ctx, &pre_table[i][0], &pre_table[i][1], &pre_table[i][2], &pre_table[i>>1][0], &pre_table[i>>1][1], &pre_table[i>>1][2]);
        mpi_copy(&pre_table[i][0], &pre_table[i+1][0]);
        mpi_copy(&pre_table[i][1], &pre_table[i+1][1]);
        mpi_copy(&pre_table[i][2], &pre_table[i+1][2]);

        sm2_add_mpl_jacobian(ctx, &pre_table[i+1][0], &pre_table[i+1][1], &pre_table[i+1][2], Gx, Gy);
    }

    mpi_copy_d(x, 1);
    mpi_copy_d(y, 1);
    mpi_copy_d(z, 0);

    for (i = m->len-1; i >= 0; i--)
    {
        for (k = sizeof(uint32_t)*8 - WINDOWS_BITS; k >= 0; k -= WINDOWS_BITS)
        {
            // WINDOWS_BITS * DoubleMplJacobian
            sm2_double_mpl_jacobian(ctx, x, y, z);
            sm2_double_mpl_jacobian(ctx, x, y, z);
            sm2_double_mpl_jacobian(ctx, x, y, z);
            sm2_double_mpl_jacobian(ctx, x, y, z);

            pos = ((m->v[i] >> k) & ((1<<WINDOWS_BITS)-1));
			
            if (pos)
            {
                sm2_add_mpl_jacobian_z(ctx, x, y, z, &pre_table[pos][0], &pre_table[pos][1], &pre_table[pos][2]);
            }
        }
    }
}

void sm2_multiply(struct sm2_ctx *ctx, mpi_t *x, mpi_t *y, mpi_t *m, mpi_t *Gx, mpi_t *Gy)
{
    if (mpi_equ_d(m, 0))
    {
        mpi_copy_d(x, 0);
        mpi_copy_d(y, 0);
        return;
    }

    mpi_t z;
    mpi_copy_d(&z, 1);
    sm2_multiply_2(ctx, x, y, &z, m, Gx, Gy);

    sm2_jacobian_2_stand(ctx, x, y, &z);
}

void sm2_multiply_g_by_table(struct sm2_ctx *ctx, mpi_t *x, mpi_t *y, mpi_t *m)
{
    mpi_t z;
    mpi_copy_d(&z, 0);
    sm2_multiply_g_by_table_z(ctx, x, y, &z, m);
    sm2_jacobian_2_stand(ctx, x, y, &z);
}

void sm2_multiply_g_by_table_z(struct sm2_ctx *ctx, mpi_t *x, mpi_t *y, mpi_t *z, mpi_t *m)
{
    mpi_t gx, gy, gz;
    int32_t i, n;

    mpi_copy(&ctx->x, &gx);
    mpi_copy(&ctx->y, &gy);
    mpi_copy_d(&gz, 1);

    if (m->v[0] & 0x01)
    {
        mpi_copy(&ctx->x, x);
        mpi_copy(&ctx->y, y);
        mpi_copy_d(z, 1);
    }
    else
    {
        mpi_copy_d(x, 1);
        mpi_copy_d(y, 1);
        mpi_copy_d(z, 0);
    }

    for (i = 1, n = mpi_bits(m); i < n; i++)
    {
        sm2_double_mpl_jacobian(ctx, &gx, &gy, &gz);

        if (0x01 & (m->v[i >> 5] >> (i & 0x1f)))
        {
            sm2_add_mpl_jacobian_z(ctx, x, y, z, &gx, &gy, &gz);
        }
    }
}

void sm2_jacobian_2_stand(struct sm2_ctx *ctx, mpi_t *x, mpi_t *y, mpi_t *z)
{
    mpi_t t, t2;
    mpl_t lltmp;

    mpi_binary_inverse(&ctx->p, z, &t);

    mpi_square(&t, &lltmp);

    mpi_truncate(mpl_mod(&lltmp, &ctx->p), &t2);

    mpi_mul(x, &t2, &lltmp);
    mpi_truncate(mpl_mod(&lltmp, &ctx->p), x);

    mpi_mul(&t, &t2, &lltmp);
    mpi_truncate(mpl_mod(&lltmp, &ctx->p), &t2);
    mpi_mul(&t2, y, &lltmp);
    mpi_truncate(mpl_mod(&lltmp, &ctx->p), y);
}

void sm2_double_mpl_jacobian(struct sm2_ctx *ctx, mpi_t *x, mpi_t *y, mpi_t *z)
{
    mpl_t lltmp;
    mpl_t lda1, lda2;
    mpi_t lda3;

    if (mpi_equ_d(x, 1) && mpi_equ_d(y, 1) && mpi_equ_d(z, 0))
    {
        return;
    }

    mpi_square(x, &lltmp);
    mpl_mod(&lltmp, &ctx->p);
    mpi_copy_d(&lda1.h, 0);
    mpi_copy(&lltmp.l, &lda1.l);

    mpl_bsl(&lda1, 1);
    mpl_addi(&lda1, &lltmp.l);

    while (!mpi_equ_d(&lda1.h, 0))
		{
        mpl_subi(&lda1, &ctx->p);
		}
    while (!mpi_lt_abs(&ctx->p, &lda1.l))
		{
        mpi_sub(&lda1.l, &ctx->p);
		}

    mpl_mod(mpi_square(z, &lltmp), &ctx->p);
    mpl_mod(mpi_square(&lltmp.l, &lltmp), &ctx->p);

    mpl_mod(mpi_mul(&lltmp.l, &ctx->a, &lltmp), &ctx->p);
    mpl_addi(&lda1, &lltmp.l);
    while (!mpi_equ_d(&lda1.h, 0))
		{
        mpl_subi(&lda1, &ctx->p);
		}
    while (!mpi_lt_abs(&ctx->p, &lda1.l))
		{
        mpi_sub(&lda1.l, &ctx->p);
		}

    mpl_mod(mpi_square(y, &lda2), &ctx->p);
    mpl_mod(mpi_square(&lda2.l, &lltmp), &ctx->p);

    mpl_bsl(&lltmp, 3);

    while (!mpi_equ_d(&lltmp.h, 0))
		{
        mpl_subi(&lltmp, &ctx->p);
		}
    while (!mpi_lt_abs(&ctx->p, &lltmp.l))
		{
        mpi_sub(&lltmp.l, &ctx->p);
		}

    mpi_copy(&lltmp.l, &lda3);

    mpl_mod(mpi_mul(x, &lda2.l, &lda2), &ctx->p);
    mpl_bsl(&lda2, 2);
    while (!mpi_equ_d(&lda2.h, 0))
		{
        mpl_subi(&lda2, &ctx->p);
		}
    while (!mpi_lt_abs(&ctx->p, &lda2.l))
		{
        mpi_sub(&lda2.l, &ctx->p);
		}

    mpl_mod(mpi_square(&lda1.l, &lltmp), &ctx->p);
    mpi_copy(&lltmp.l, x);

    mpi_copy(&lda2.h, &lltmp.h);
    mpi_copy(&lda2.l, &lltmp.l);
    mpl_bsl(&lltmp, 1);
    while (!mpi_equ_d(&lltmp.h, 0))
		{
        mpl_subi(&lltmp, &ctx->p);
		}
    while (!mpi_lt_abs(&ctx->p, &lltmp.l))
		{
        mpi_sub(&lltmp.l, &ctx->p);
		}
    mpi_sub(x, &lltmp.l);
    if (mpi_is_neg(x))
		{
        mpi_add(x, &ctx->p);
		}

    // y
    mpl_subi(&lda2, x);
    mpi_mul(&lda1.l, &lda2.l, &lltmp);
    mpl_mod(&lltmp, &ctx->p);
    mpl_subi(&lltmp, &lda3);
    mpi_copy(y, &lda1.l);
    mpl_mod(&lltmp, &ctx->p);
    mpi_copy(&lltmp.l, y);
    if (mpi_is_neg(y))
		{
        mpi_add(y, &ctx->p);
		}

    // z
    mpl_mod(mpi_mul(&lda1.l, z, &lltmp), &ctx->p);
    mpl_bsl(&lltmp, 1);
    while (!mpi_equ_d(&lltmp.h, 0))
		{
        mpl_subi(&lltmp, &ctx->p);
		}
    while (!mpi_lt_abs(&ctx->p, &lltmp.l))
		{
        mpi_sub(&lltmp.l, &ctx->p);
		}
    mpi_copy(&lltmp.l, z);
}

// only for mz == 1
void sm2_add_mpl_jacobian(struct sm2_ctx *ctx, mpi_t *x, mpi_t *y, mpi_t *z, mpi_t *mx, mpi_t *my)
{
    mpl_t lltmp, lda1, lda2;
    mpi_t lda3, lda4, lda5, lda6;

    if (mpi_equ_d(x, 1) && mpi_equ_d(y, 1) && mpi_equ_d(z, 0))
    {
        mpi_copy(mx, x);
        mpi_copy(my, y);
        mpi_copy_d(z, 1);
        return;
    }

    if (mpi_equ(x, mx) && mpi_equ(y, my) && mpi_equ_d(z, 1))	
		{
		    // mz == 1
        sm2_double_mpl_jacobian(ctx, x, y, z);
		    return;
		}

    mpi_copy_d(&lda1.h, 0);
    mpi_copy(x, &lda1.l);
    mpi_copy(y, &lda4);

    // lltmp = z*z;
    mpl_mod(mpi_square(z, &lltmp), &ctx->p);

    mpl_mod(mpi_mul(&lltmp.l, mx, &lda2), &ctx->p);

    mpl_mod(mpi_mul(&lltmp.l, z, &lltmp), &ctx->p);
    mpl_mod(mpi_mul(&lltmp.l, my, &lltmp), &ctx->p);
    mpi_copy(&lltmp.l, &lda5);

    mpi_sub2(&lda1.l, &lda2.l, &lda3);
    if (mpi_is_neg(&lda3))
		{
        mpi_add(&lda3, &ctx->p);
		}

    mpi_sub2(&lda4, &lda5, &lda6);
    if (mpi_is_neg(&lda6))
		{
        mpi_add(&lda6, &ctx->p);
		}

    mpl_addi(&lda1, &lda2.l);
    while (!mpi_equ_d(&lda1.h, 0))
		{
        mpl_subi(&lda1, &ctx->p);
		}
    while (!mpi_lt_abs(&ctx->p, &lda1.l))
		{
        mpi_sub(&lda1.l, &ctx->p);
		}

    mpi_copy(&lda4, &lda2.l);
    mpl_addi(&lda2, &lda5);
    while (!mpi_equ_d(&lda2.h, 0))
		{
        mpl_subi(&lda2, &ctx->p);
		}
    while (!mpi_lt_abs(&ctx->p, &lda2.l))
		{
        mpi_sub(&lda2.l, &ctx->p);
		}

    mpl_mod(mpi_mul(z, &lda3, &lltmp), &ctx->p);
    mpi_copy(&lltmp.l, z);

    // lda4 = 3*3
    // lltmp = lda3*lda3;
    mpl_mod(mpi_square(&lda3, &lltmp), &ctx->p);
    mpi_copy(&lltmp.l, &lda4);
    // lda5 = 3*3*7
    mpl_mod(mpi_mul(&lda4, &lda1.l, &lltmp), &ctx->p);
    mpi_copy(&lltmp.l, &lda5);

    // x
    // lltmp = lda6*lda6;
    mpl_mod(mpi_square(&lda6, &lltmp), &ctx->p);
    mpi_sub(mpi_copy(&lltmp.l, x), &lda5);
    if (mpi_is_neg(x))
		{
        mpi_add(x, &ctx->p);
		}

    mpi_copy(x, &lltmp.l);
    mpl_bsl(&lltmp, 1);
    while (!mpi_equ_d(&lltmp.h, 0))
		{
        mpl_subi(&lltmp, &ctx->p);
		}
    while (!mpi_lt_abs(&ctx->p, &lltmp.l))
		{
        mpi_sub(&lltmp.l, &ctx->p);
		}
    mpi_sub2(&lda5, &lltmp.l, &lda1.l);
    if (mpi_is_neg(&lda1.l))
		{
        mpi_add(&lda1.l, &ctx->p);
		}

    // y
    mpl_mod(mpi_mul(&lda4, &lda3, &lltmp), &ctx->p);
    mpl_mod(mpi_mul(&lltmp.l, &lda2.l, &lltmp), &ctx->p);

    mpl_mod(mpi_mul(&lda1.l, &lda6, &lda1), &ctx->p);
    mpi_sub(&lda1.l, &lltmp.l);
    if (mpi_is_neg(&lda1.l))
		{
        mpi_add(&lda1.l, &ctx->p);
		}

    // lda1 = lda1.l * g_Inv2;
    if (lda1.l.v[0] & 0x01)
    {
        mpi_copy(mpi_bsr(&lda1.l, 1), y);
        mpi_add(y, &ctx->inv);
        while (!mpi_lt_abs(&ctx->p, y))
				{
            mpi_sub(y, &ctx->p);
				}
    }
    else
    {
        mpi_copy(mpi_bsr(&lda1.l, 1), y);
    }
}

void sm2_add_mpl_jacobian_z(struct sm2_ctx *ctx, mpi_t *x, mpi_t *y, mpi_t *z, mpi_t *mx, mpi_t *my, mpi_t *mz)
{
    mpl_t lltmp, lda1, lda2;
    mpi_t lda3, lda4, lda5, lda6;

    if (mpi_equ_d(x, 1) && mpi_equ_d(y, 1) && mpi_equ_d(z, 0))
    {
        mpi_copy(mx, x);
        mpi_copy(my, y);
        mpi_copy(mz, z);
        return;
    }

    if (mpi_equ_d(mx, 1) && mpi_equ_d(my, 1) && mpi_equ_d(mz, 0))
		{
        return;
		}

    if (mpi_equ(x, mx) && mpi_equ(y, my) && mpi_equ(z, mz))
		{
         sm2_double_mpl_jacobian(ctx, x, y, z);
         return;
		}

    // mpl_t lltmp = mz*mz;
    mpl_mod(mpi_square(mz, &lltmp), &ctx->p);

    mpl_mod(mpi_mul(&lltmp.l, x, &lda1), &ctx->p);

    mpl_mod(mpi_mul(&lltmp.l, mz, &lltmp), &ctx->p);
    mpl_mod(mpi_mul(&lltmp.l, y, &lltmp), &ctx->p);
    mpi_copy(&lltmp.l, &lda4);

    // lltmp = z*z;
    mpl_mod(mpi_square(z, &lltmp), &ctx->p);

    mpl_mod(mpi_mul(&lltmp.l, mx, &lda2), &ctx->p);

    mpl_mod(mpi_mul(&lltmp.l, z, &lltmp), &ctx->p);
    mpl_mod(mpi_mul(&lltmp.l, my, &lltmp), &ctx->p);
    mpi_copy(&lltmp.l, &lda5);

    mpi_sub2(&lda1.l, &lda2.l, &lda3);
    if (mpi_is_neg(&lda3))
		{
        mpi_add(&lda3, &ctx->p);
		}

    mpi_sub2(&lda4, &lda5, &lda6);
    if (mpi_is_neg(&lda6))
		{
        mpi_add(&lda6, &ctx->p);
		}
    
    mpl_addi(&lda1, &lda2.l);
    while (!mpi_equ_d(&lda1.h, 0))
		{
        mpl_subi(&lda1, &ctx->p);
		}
    while (!mpi_lt_abs(&ctx->p, &lda1.l))
		{
        mpi_sub(&lda1.l, &ctx->p);
		}

    mpi_copy(&lda4, &lda2.l);
    mpl_addi(&lda2, &lda5);
    while (!mpi_equ_d(&lda2.h, 0))
		{
        mpl_subi(&lda2, &ctx->p);
		}
    while (!mpi_lt_abs(&ctx->p, &lda2.l))
		{
        mpi_sub(&lda2.l, &ctx->p);
		}

    // z
    mpl_mod(mpi_mul(z, mz, &lltmp), &ctx->p);
    mpl_mod(mpi_mul(&lda3, &lltmp.l, &lltmp), &ctx->p);
    mpi_copy(&lltmp.l, z);

    // lda4 = 3*3
    // lltmp = lda3*lda3;
    mpl_mod(mpi_square(&lda3, &lltmp), &ctx->p);
    mpi_copy(&lltmp.l, &lda4);
    // lda5 = 3*3*7
    mpl_mod(mpi_mul(&lda1.l, &lda4, &lltmp), &ctx->p);
    mpi_copy(&lltmp.l, &lda5);

    // x
    // lltmp = lda6*lda6;
    mpl_mod(mpi_square(&lda6, &lltmp), &ctx->p);
    mpi_sub(mpi_copy(&lltmp.l, x), &lda5);
    if (mpi_is_neg(x))
		{
        mpi_add(x, &ctx->p);
		}

    mpi_copy(x, &lltmp.l);
    mpl_bsl(&lltmp, 1);
    while (!mpi_equ_d(&lltmp.h, 0))
		{
        mpl_subi(&lltmp, &ctx->p);
		}
    while (!mpi_lt_abs(&ctx->p, &lltmp.l))
		{
        mpi_sub(&lltmp.l, &ctx->p);
		}
    mpi_copy(&lda5, &lda1.l);
    mpl_subi(&lda1, &lltmp.l);
    if (mpi_is_neg(&lda1.l))
		{
        mpi_add(&lda1.l, &ctx->p);
		}

    // y
    mpl_mod(mpi_mul(&lda4, &lda3, &lltmp), &ctx->p);
    mpl_mod(mpi_mul(&lltmp.l, &lda2.l, &lltmp), &ctx->p);

    mpl_mod(mpi_mul(&lda1.l, &lda6, &lda1), &ctx->p);
    mpi_sub(&lda1.l, &lltmp.l);
    if (mpi_is_neg(&lda1.l))
		{
        mpi_add(&lda1.l, &ctx->p);
		}
    // lda1.h = 0;
    // lda1 = lda1.l * g_Inv2;
    if (lda1.l.v[0] & 0x01)
    {
        mpi_copy(mpi_bsr(&lda1.l, 1), y);
        mpi_add(y, &ctx->inv);
        while (!mpi_lt_abs(&ctx->p, y))
				{
            mpi_sub(y, &ctx->p);
				}
    }
    else
    {
        mpi_copy(mpi_bsr(&lda1.l, 1), y);
    }
}

static int32_t sm2_authenticate_message(uint8_t *pDigest, uint8_t *pSecret, uint8_t *pMsg, int32_t iLenOfMsg)
{
    HASH_STATE hashState;
    HashInit(&hashState, pSecret, 32);
    HashPending(&hashState, pMsg, iLenOfMsg);
    HashPending(&hashState, pSecret+32, 32);
    HashFinal(&hashState, pDigest);

    return HASH_256;
}

int32_t sm2_hash_user_id(struct sm2_ctx *ctx, sm2_pubkey_t *pubKey, uint8_t *pbOut, char *pUserName, int32_t iLenOfName)
{
    if (!pUserName)
		{
        iLenOfName = 0;
		}

    uint8_t ENTL[2];
    ENTL[0] = (uint8_t)(iLenOfName >> 5);
    ENTL[1] = (uint8_t)(iLenOfName << 3);

    HASH_STATE hashState;
    HashInit(&hashState, ENTL, 2);
    if (pUserName)
		{
        HashPending(&hashState, (uint8_t *)pUserName, iLenOfName);
		}
    uint8_t bufPnt[MPI_LENGTH*4];
    mpi_export(&ctx->a, bufPnt, SM2_PRIVATE_KEY_SIZE);
    HashPending(&hashState, bufPnt, SM2_PRIVATE_KEY_SIZE);
    mpi_export(&ctx->b, bufPnt, SM2_PRIVATE_KEY_SIZE);
    HashPending(&hashState, bufPnt, SM2_PRIVATE_KEY_SIZE);
    mpi_export(&ctx->x, bufPnt, SM2_PRIVATE_KEY_SIZE);
    HashPending(&hashState, bufPnt, SM2_PRIVATE_KEY_SIZE);
    mpi_export(&ctx->y, bufPnt, SM2_PRIVATE_KEY_SIZE);
    HashPending(&hashState, bufPnt, SM2_PRIVATE_KEY_SIZE);
    mpi_export(&pubKey->x, bufPnt, SM2_PRIVATE_KEY_SIZE);
    HashPending(&hashState, bufPnt, SM2_PRIVATE_KEY_SIZE);
    mpi_export(&pubKey->y, bufPnt, SM2_PRIVATE_KEY_SIZE);
    HashPending(&hashState, bufPnt, SM2_PRIVATE_KEY_SIZE);

    HashFinal(&hashState, pbOut);

    return HASH_256;
}

static int32_t sm2_message_hash(struct sm2_ctx *ctx, sm2_pubkey_t *pubKey, uint8_t *pbDigest, uint8_t *pMsg, int32_t iLenOfMsg, char *pUserName, int32_t iLenOfUserName)
{
    uint8_t hashResult[HASH_256];
    sm2_hash_user_id(ctx, pubKey, hashResult, pUserName, iLenOfUserName);
    // dump_buf("Za", hashResult, HASH_256);
    // dump_buf("pMsg", pMsg, iLenOfMsg);

    // the 2nd hash
    HASH_STATE hashState;

    HashInit(&hashState, hashResult, HASH_256);
    HashPending(&hashState, pMsg, iLenOfMsg);
    return HashFinal(&hashState, pbDigest);
}

int32_t sm2_check_point(struct sm2_ctx *ctx, mpi_t *x, mpi_t *y)
{
    mpl_t left, right;
    if (mpi_is_neg(x) || mpi_is_neg(y) || !mpi_lt_abs(&ctx->p, x) || !mpi_lt_abs(&ctx->p, y))
    {
        return -1;
    }

    mpi_square(x, &left);
    mpl_mod(mpl_addi(&left, &ctx->a), &ctx->p);

    mpi_mul(&left.l, x, &left);
    mpl_mod(mpl_addi(&left, &ctx->b), &ctx->p);

    mpl_mod(mpi_square(y, &right), &ctx->p);

    if (mpi_equ(&right.l, &left.l))
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

int32_t sm2_import_point(struct sm2_ctx *ctx, uint8_t *pbIn, int32_t iInLen, mpi_t *x, mpi_t *y)
{
		switch (pbIn[0])
		{
		case 0x04:
				if (iInLen < (1+2*mpi_bytes(&ctx->p)))
				{
						return -1;
				}
				mpi_import(x, pbIn+1, mpi_bytes(&ctx->p));
				mpi_import(y, pbIn+1+mpi_bytes(&ctx->p), mpi_bytes(&ctx->p));
				break;

		case 0x02:
		case 0x03:
				return -1;

		case 0x00:
				mpi_copy_d(x, 0);
				mpi_copy_d(y, 0);
				break;

		default:
				return -1;
		}

		return 0;	
}

int32_t sm2_export_point(struct sm2_ctx *ctx, uint8_t *pbOut, mpi_t *x, mpi_t *y, int32_t compress)
{
    if ((y == 0) && (x == 0))
    {
        pbOut[0] = 0x00;
        return 1;
    }

    int32_t iRetLen = 0;
    if (compress)
    {
        if (y->v[0] & 0x01)
            pbOut[iRetLen++] = 0x03;
        else
            pbOut[iRetLen++] = 0x02;

        iRetLen += mpi_export(x, pbOut+iRetLen, mpi_bytes(&ctx->p));
    }
    else
    {
        pbOut[iRetLen++] = 0x04;
        iRetLen += mpi_export(x, pbOut+iRetLen, mpi_bytes(&ctx->p));
        iRetLen += mpi_export(y, pbOut+iRetLen, mpi_bytes(&ctx->p));
    }

	return iRetLen;
}

int32_t sm2_encrypt_message(struct sm2_ctx *ctx, uint8_t *out, uint8_t *in, int32_t inlen, uint8_t *random, int32_t rlen)
{
    uint8_t secret[MPI_LENGTH*4*2];
    int32_t iLenOfSecret;
    int32_t iLenOfOut;
    int32_t kkk, i;

    while (1)
    {
        iLenOfOut = sm2_encrypt(ctx, out, secret, random, rlen);
        // dump_buf("k", random, rlen);
        // dump_buf("C1", out, iLenOfOut);
        // dump_buf("Pb-x2", secret, 32);
        // dump_buf("Pb-y2", secret+32, 32);
        if (!iLenOfOut)
				{
            return -1;
				}
        iLenOfSecret = 2*32;
        KDF(out+iLenOfOut, inlen, secret, iLenOfSecret, 1);
        // dump_buf("t", out + iLenOfOut, inlen);

        kkk = 0;
        while (kkk < inlen)
        {
            if (0 != out[iLenOfOut+kkk])
                break;
            kkk++;
        }

        if (kkk >= inlen)	// all are zero
        {
            if (random)
						{
                return -1;
						}
            continue;
        }
        break;
    }

    for (i = 0; i < inlen; i++)
    {
        out[iLenOfOut+i] ^= in[i];
	}
    // dump_buf("C2", out + iLenOfOut, inlen);
    iLenOfOut += inlen;

    iLenOfOut += sm2_authenticate_message(out+iLenOfOut, secret, in, inlen);
    // dump_buf("C3", out + 65+inlen, 32);

    return iLenOfOut;
}

int32_t sm2_decrypt_message(struct sm2_ctx *ctx, uint8_t *out, uint8_t *in, int32_t inlen)
{
    int32_t i;
    if (inlen <= (1+2*32+HASH_256))
		{
		    return -1;
		}

    uint8_t secret[32*2];
    if (!sm2_decrypt(ctx, in, 2*32, secret))
		{
		    return -1;
		}
    // dump_buf("Pb-x2", secret, 32);
    // dump_buf("Pb-y2", secret + 32, 32);

    int32_t iLenOfOut = inlen - (1+2*32+HASH_256);
    int32_t iLenOfSecret = 2*32;

    KDF(out, iLenOfOut, secret, iLenOfSecret, 1);
    // dump_buf("t", out, iLenOfOut);
    for (i = 0; i < iLenOfOut; i++)
		{
        out[i] ^= in[1+iLenOfSecret+i];
		}
    // dump_buf("M'", out, iLenOfOut);

    // C3
    uint8_t digest[HASH_256];
    sm2_authenticate_message(digest, secret, out, iLenOfOut);
    // dump_buf("u", digest, 32);

    i = 0;
    while (i < HASH_256)
    {
		    if (digest[i] != in[inlen+i-HASH_256])
				{
            return 0;
				}
		    i++;
    }

    return iLenOfOut;
}

int32_t sm2_sign_message(struct sm2_ctx *ctx, uint8_t *out, uint8_t *in, int32_t inlen, char *usrname, int32_t usrnamelen, uint8_t *random, uint32_t randlen)
{
    uint8_t digest[HASH_256];

    sm2_message_hash(ctx, &ctx->pubkey, digest, in, inlen, usrname, usrnamelen);
    // dump_buf("e", digest, HASH_256);

    return sm2_sign(ctx, digest, HASH_256, out, random, randlen);
}

int32_t sm2_verify_message(struct sm2_ctx *ctx, uint8_t *in,  int32_t inlen, uint8_t *pSig, char *uname, int32_t unamelen)
{
    uint8_t hashResult[HASH_256];
    uint8_t digest[HASH_256];
    HASH_STATE hashState;
	
    sm2_hash_user_id(ctx, &ctx->pubkey, hashResult, uname, unamelen);

    // the 2nd hash
    HashInit(&hashState, hashResult, HASH_256);
    HashPending(&hashState, in, inlen);
    HashFinal(&hashState, digest);

    return sm2_verify(ctx, digest, pSig);
}

int32_t sm2_sign(struct sm2_ctx *ctx, uint8_t *in, int32_t inlen, uint8_t *out, uint8_t *random, int32_t rlen)
{
    int32_t outlen = 0;
    mpi_t r, s, x, t;
    mpl_t lltmp;

    if (!random || !mpi_import(&r, random, rlen))
		{
        return -1;
		}

    while (mpi_lt_abs(&r, &ctx->n))
		{
        mpi_sub(&r, &ctx->n);
		}

    if (mpi_equ_d(&r, 0) || mpi_equ(&r, &ctx->n))
		{
        return -1;
		}

    sm2_multiply_g_by_table(ctx, &x, &t, &r);
	
    if (!mpi_import(&t, in, inlen))
		{
        return -1;
		}

    mpi_copy_d(&lltmp.h, 0);
    mpi_copy(&x, &lltmp.l);
    mpl_mod(mpl_addi(&lltmp, &t), &ctx->n);
    mpi_copy(&lltmp.l, &x);

    if (mpi_equ_d(&x, 0) || mpi_equ(&ctx->n, mpi_add2(&x, &r, &t)))
		{
        return -1;
		}

    mpl_mod(mpi_mul(&x, &ctx->prikey.d, &lltmp), &ctx->n);
    mpi_copy(&lltmp.l, &s);
    mpi_sub(&r, &s);
    while (mpi_is_neg(&r))
		{
        mpi_add(&r, &ctx->n);
		}

    if (mpi_equ_d(&ctx->prikey.i, 0))
    {
        mpi_copy_d(&t, 1);
        mpi_add(&t, &ctx->prikey.d);
        mpi_binary_inverse(&ctx->n, &t, &ctx->prikey.i);
    }

    mpl_mod(mpi_mul(&ctx->prikey.i, &r, &lltmp), &ctx->n);
    mpi_copy(&lltmp.l, &s);

    if (mpi_equ_d(&s, 0))
		{
        return -1;
		}

    outlen = mpi_export(&x, out, mpi_bytes(&ctx->p));
    outlen += mpi_export(&s, out+outlen, mpi_bytes(&ctx->p));

    return outlen;
}

int32_t sm2_verify(struct sm2_ctx *ctx, uint8_t *hash, uint8_t *signature)
{
    mpi_t e, r, s;
    mpl_t t;

    if (!mpi_import(&e, hash, HASH_256))
    {
        return -1;
    }

    mpi_import(&r, signature, mpi_bytes(&ctx->p));
    mpi_import(&s, signature + mpi_bytes(&ctx->p), mpi_bytes(&ctx->p));

    if (mpi_equ_d(&r, 0) || !mpi_lt_abs(&ctx->n, &r) || mpi_equ_d(&s, 0) || !mpi_lt_abs(&ctx->n, &s))
    {
		    return 0;
    }

    mpi_copy_d(&t.h, 0);
    mpi_copy(&r, &t.l);
    mpl_mod(mpl_addi(&t, &s), &ctx->n);
    if (mpi_equ_d(&t.l, 0))
    {
		    return 0;
    }
  
    mpi_t x1, y1, z1, x2, y2, z2;
    sm2_multiply_g_by_table_z(ctx, &x1, &y1, &z1, &s);
    sm2_multiply_2(ctx, &x2, &y2, &z2, &t.l, &ctx->pubkey.x, &ctx->pubkey.y);
    sm2_add_mpl_jacobian_z(ctx, &x1, &y1, &z1, &x2, &y2, &z2);
    sm2_jacobian_2_stand(ctx, &x1, &y1, &z1);

    mpi_copy(&e, &t.l);
    mpl_mod(mpl_addi(&t, &x1), &ctx->n);

    if (mpi_equ(&r, &t.l))
		{
		    return 0;
		}
    else
		{
		    return -1;
		}
}

int32_t sm2_encrypt(struct sm2_ctx *ctx, uint8_t *in, uint8_t *out, uint8_t *random, uint32_t rlen)
{
    int32_t len = 0;
    mpi_t r, x, y;

    if (!random || !mpi_import(&r, random, rlen))
		{
        return -1;
		}

    while (mpi_lt_abs(&r, &ctx->n))
		{
        mpi_sub(&r, &ctx->n);
		}

    if (mpi_equ_d(&r, 0) || mpi_equ(&r, &ctx->n))
		{
        return -1;
		}

    sm2_multiply_g_by_table(ctx, &x, &y, &r);
    len = sm2_export_point(ctx, in, &x, &y, 0);

    sm2_multiply(ctx, &x, &y, &r, &ctx->pubkey.x, &ctx->pubkey.y);

    if (mpi_equ_d(&y, 0))
		{
        return -1;
		}

    mpi_export(&x, out, mpi_bytes(&ctx->p));
    mpi_export(&y, out+32, mpi_bytes(&ctx->p));

    return len;
}

int32_t sm2_decrypt(struct sm2_ctx *ctx, uint8_t *in, int32_t inlen, uint8_t *out)
{
    mpi_t x, y, x2, y2;

    if (sm2_import_point(ctx, in, 2 * 32 + 1, &x, &y) || sm2_check_point(ctx, &x, &y))
    {
        return -1;
    }

    sm2_multiply(ctx, &x2, &y2, &ctx->prikey.d, &x, &y);
    if (mpi_equ_d(&y2, 0))
    {
        return -1;
    }

    mpi_export(&x2, out, mpi_bytes(&ctx->p));
    mpi_export(&y2, out + 32, mpi_bytes(&ctx->p));

    return mpi_bytes(&ctx->p);
}

int32_t sm2_generate_key(struct sm2_ctx *ctx, uint8_t *ramdom, uint32_t size)
{
    if (ramdom)
    {
        if (size > mpi_bytes(&ctx->p))
				{
            return -1;
				}

        mpi_import(&ctx->prikey.d, ramdom, size);
				
        while (!mpi_lt_abs(&ctx->n, &ctx->prikey.d))
		    {
            mpi_sub(&ctx->prikey.d, &ctx->n);
		    }

		    if (mpi_equ_d(&ctx->prikey.d, 0))
		    {
			      return -1;
		    }
    }

    sm2_multiply_g_by_table(ctx, &ctx->pubkey.x, &ctx->pubkey.y, &ctx->prikey.d);

    if (mpi_equ_d(&ctx->pubkey.y,0))
		{
		    return 0;
		}

    mpi_copy_d(&ctx->prikey.i, 0);
		
    return 0; 
}

int32_t sm2_set_public_key(struct sm2_ctx *ctx, uint8_t *data, uint32_t size)
{
    if (size < (2 * mpi_bytes(&ctx->p)))
		{
		    return -1;
		}

    mpi_import(&ctx->pubkey.x, data, mpi_bytes(&ctx->p));
    mpi_import(&ctx->pubkey.y, data+mpi_bytes(&ctx->p), mpi_bytes(&ctx->p));

    return sm2_check_point(ctx, &ctx->pubkey.x, &ctx->pubkey.y);
}

int32_t sm2_set_private_key(struct sm2_ctx *ctx, uint8_t *data, uint32_t size)
{
    int32_t ret;

    if (size < mpi_bytes(&ctx->p))
		{
		    return -1;
		}

    ret = mpi_import(&ctx->prikey.d, data, mpi_bytes(&ctx->p));

    mpi_copy_d(&ctx->prikey.i, 0);

    return ret;
}

int32_t sm2_init(struct sm2_ctx *ctx)
{
    uint8_t inv[] = 
    { 
        0x7f, 0xff, 0xff, 0xff, 0x7f, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00,
        0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    };

    memset(ctx, 0, sizeof(struct sm2_ctx));

    ctx->p.v[0] = 0xFFFFFFFF;
    ctx->p.v[1] = 0xFFFFFFFF;
    ctx->p.v[2] = 0x00000000;
    ctx->p.v[3] = 0xFFFFFFFF;
    ctx->p.v[4] = 0xFFFFFFFF;
    ctx->p.v[5] = 0xFFFFFFFF;
    ctx->p.v[6] = 0xFFFFFFFF;
    ctx->p.v[7] = 0xFFFFFFFE;
    ctx->p.carry = 0;
    ctx->p.len = 8;
    ctx->p.sign = POSITIVE;

    ctx->a.v[0] = 0xFFFFFFFC;
    ctx->a.v[1] = 0xFFFFFFFF;
    ctx->a.v[2] = 0x00000000;
    ctx->a.v[3] = 0xFFFFFFFF;
    ctx->a.v[4] = 0xFFFFFFFF;
    ctx->a.v[5] = 0xFFFFFFFF;
    ctx->a.v[6] = 0xFFFFFFFF;
    ctx->a.v[7] = 0xFFFFFFFE;
    ctx->a.carry = 0;
    ctx->a.len = 8;
    ctx->a.sign = POSITIVE;

    ctx->b.v[0] = 0x4D940E93;
    ctx->b.v[1] = 0xDDBCBD41;
    ctx->b.v[2] = 0x15AB8F92;
    ctx->b.v[3] = 0xF39789F5;
    ctx->b.v[4] = 0xCF6509A7;
    ctx->b.v[5] = 0x4D5A9E4B;
    ctx->b.v[6] = 0x9D9F5E34;
    ctx->b.v[7] = 0x28E9FA9E;
    ctx->b.carry = 0;
    ctx->b.len = 8;
    ctx->b.sign = POSITIVE;

    ctx->x.v[0] = 0x334C74C7;
    ctx->x.v[1] = 0x715A4589;
    ctx->x.v[2] = 0xF2660BE1;
    ctx->x.v[3] = 0x8FE30BBF;
    ctx->x.v[4] = 0x6A39C994;
    ctx->x.v[5] = 0x5F990446;
    ctx->x.v[6] = 0x1F198119;
    ctx->x.v[7] = 0x32C4AE2C;
    ctx->x.carry = 0;
    ctx->x.len = 8;
    ctx->x.sign = POSITIVE;

    ctx->y.v[0] = 0x2139F0A0;
    ctx->y.v[1] = 0x02DF32E5;
    ctx->y.v[2] = 0xC62A4740;
    ctx->y.v[3] = 0xD0A9877C;
    ctx->y.v[4] = 0x6B692153;
    ctx->y.v[5] = 0x59BDCEE3;
    ctx->y.v[6] = 0xF4F6779C;
    ctx->y.v[7] = 0xBC3736A2;
    ctx->y.carry = 0;
    ctx->y.len = 8;
    ctx->y.sign = POSITIVE;

    ctx->n.v[0] = 0x39D54123;
    ctx->n.v[1] = 0x53BBF409;
    ctx->n.v[2] = 0x21C6052B;
    ctx->n.v[3] = 0x7203DF6B;
    ctx->n.v[4] = 0xFFFFFFFF;
    ctx->n.v[5] = 0xFFFFFFFF;
    ctx->n.v[6] = 0xFFFFFFFF;
    ctx->n.v[7] = 0xFFFFFFFE;
    ctx->n.carry = 0;
    ctx->n.len = 8;
    ctx->n.sign = POSITIVE;

    mpi_import(&ctx->inv, inv, SM2_PRIVATE_KEY_SIZE);

    return 0;
}

int sm2_key_exchange_rnd_msg(struct sm2_ctx *ctx, uint8_t *pbRndMsg, uint8_t *pbRnd, int iLenOfRnd)
{
    mpi_t rnd, x, y;

    mpi_copy_d(&x, 0);
    mpi_copy_d(&y, 0);

    if (!mpi_import(&rnd, pbRnd, iLenOfRnd))
        return 0;   // error

    if (mpi_equ_d(&rnd, 0) || mpi_equ(&rnd, &ctx->n))
        return 0;   // error

    if (mpi_lt_abs(&rnd, &ctx->n))
        mpi_sub(&rnd, &ctx->n);

    sm2_multiply_g_by_table(ctx, &x, &y, &rnd);
    if (mpi_equ_d(&y, 0))
        return 0;

    int iRet = mpi_export(&x, pbRndMsg, mpi_bytes(&ctx->p));
    return iRet = mpi_export(&y, pbRndMsg+iRet, mpi_bytes(&ctx->p));
}

int sm2_key_exchange_result(struct sm2_ctx *ctx, uint8_t *pOut, int iLenOut, 
                            uint8_t *pbMyRnd, int iLenOfRnd, char *pMyUserName, int iLenOfMyUserName,
                            uint8_t *pbOtherRndMsg, int iLenOfRndMsg, sm2_pubkey_t *pOtherPublicKey, 
                            char *pOtherUserName, int iLenOfOtherUserName, int fInit)
{
    mpi_t rnd;

    if (!mpi_import(&rnd, pbMyRnd, iLenOfRnd))
        return 0;   // error

    if (mpi_equ_d(&rnd, 0) || mpi_equ(&rnd, &ctx->n))
        return 0;   // error

    if (mpi_lt_abs(&rnd, &ctx->n))
        mpi_sub(&rnd, &ctx->n);

    mpi_t x1, y1;
    if (!mpi_import(&x1, pbOtherRndMsg, iLenOfRndMsg/2)
        || !mpi_import(&y1, pbOtherRndMsg+iLenOfRndMsg/2, iLenOfRndMsg/2)
        || (sm2_check_point(ctx, &x1, &y1) == -1))
        return 0;

    mpi_t mpiX, mpiY;
    sm2_multiply_g_by_table(ctx, &mpiX, &mpiY, &rnd); // RA
    if (mpi_equ_d(&mpiY, 0))
        return 0;

    uint8_t secret[32*2 + 2*HASH_256];
    int iLenAll = mpi_export(&mpiX, secret, 32); // 暂时存放在secret中
    
    // 取其半bits, 同时将最高bit置1
    secret[iLenAll/2] |= 0x80;
    mpi_import(&mpiX, secret+(iLenAll/2), iLenAll/2);  // x1u

    mpl_t ta;
    mpi_mul(&mpiX, &rnd, &ta);

    mpl_mod(mpl_addi(&ta, &ctx->prikey.d), &ctx->n);
    mpi_copy(&ta.l, &rnd);

    iLenAll = mpi_export(&x1, secret, 32);
   
    // 取其半bits, 同时将最高bit置1
    secret[iLenAll/2] |= 0x80;
    mpi_t x2u;
    mpi_import(&x2u, secret+(iLenAll/2), iLenAll/2);   // x2u

    sm2_multiply(ctx, &mpiX, &mpiY, &x2u, &x1, &y1);

    mpi_t mZ;
    mpi_copy_d(&mZ, 1);
    sm2_add_mpl_jacobian(ctx, &mpiX, &mpiY, &mZ, &pOtherPublicKey->x, &pOtherPublicKey->y);
    sm2_jacobian_2_stand(ctx, &mpiX, &mpiY, &mZ);
    sm2_multiply(ctx, &x1, &y1, &rnd, &mpiX, &mpiY);

    if (mpi_equ_d(&y1, 0))
        return 0;

    int iLenOfSecret = mpi_export(&x1, secret, 32);
    iLenOfSecret += mpi_export(&y1, secret+iLenOfSecret, 32);

    // Za||Zb
    if (fInit)
    {
        iLenOfSecret += sm2_hash_user_id(ctx, &ctx->pubkey, secret+iLenOfSecret, pMyUserName, iLenOfMyUserName);
        iLenOfSecret += sm2_hash_user_id(ctx, pOtherPublicKey, secret+iLenOfSecret, pOtherUserName, iLenOfOtherUserName);
    }
    else
    {
        iLenOfSecret += sm2_hash_user_id(ctx, pOtherPublicKey, secret+iLenOfSecret, pOtherUserName, iLenOfOtherUserName);
        iLenOfSecret += sm2_hash_user_id(ctx, &ctx->pubkey, secret+iLenOfSecret, pMyUserName, iLenOfMyUserName);
    }

    KDF(pOut, iLenOut, secret, iLenOfSecret, 1);
    
    return iLenOut;
}

int32_t sm2_kp_sw(uint8_t prikey[64], uint8_t pubkey[64], uint8_t out[64])
{
    mpi_t x, y, m, gx, gy;
    struct sm2_ctx sw_sm2_temp;
    mpi_import(&m, prikey, 32);
    mpi_import(&x, pubkey, 32);
    mpi_import(&y, pubkey + 32, 32);
    sm2_init(&sw_sm2_temp);
    sm2_multiply(&sw_sm2_temp, &gx, &gy, &m, &x, &y);
    mpi_export(&gx, out, 32);
    mpi_export(&gy, out + 32, 32);

    return 0;
}
