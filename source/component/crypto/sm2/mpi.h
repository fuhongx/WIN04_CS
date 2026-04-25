#ifndef __MPI_H__
#define __MPI_H__

#ifdef __cplusplus
extern "C" {
#endif
	
#include <stdint.h>
#define MPI_LENGTH 8

#define POSITIVE 1
#define NEGTIVE (-1)

#define HEX28BITS 0xFFFFFFF
#define HEX32BITS 0xFFFFFFFF
#define UNSIGNEDLEFTBIT 0x80000000
#define BITS_OF_INT 32
#define DOUBLE_INT uint64_t

#ifndef __max
#define __max(x,y) (x>y?x:y)
#endif
#ifndef __min
#define __min(x,y) (x<y?x:y)
#endif

typedef struct mpi
{
    uint32_t v[MPI_LENGTH];
    uint32_t carry;
    int32_t sign;
    int32_t len;
} mpi_t;

typedef struct mpl
{
    mpi_t h;
    mpi_t l;
} mpl_t;

 int32_t mpi_is_neg(mpi_t *n);
 int32_t mpi_bits(mpi_t *n);
 int32_t mpi_import(mpi_t *n, uint8_t *abContent, int32_t iLength);
 int32_t mpi_export(mpi_t *n, uint8_t *abOutBytes, int32_t iMinLength);
 int32_t mpi_bytes(mpi_t *n);
 int32_t mpi_equ_d(mpi_t *n, uint32_t d);
 int32_t mpi_equ(mpi_t *a, mpi_t *b);
 int32_t mpi_cmp(mpi_t *a, mpi_t *b);
 int32_t mpi_lt_abs(mpi_t *a, mpi_t *b);
 int32_t mpl_equ(mpl_t *a, mpl_t *b); // a == b

 mpi_t* mpi_copy(mpi_t *m, mpi_t *x);
 mpi_t* mpi_copy_d(mpi_t *n, uint32_t d);
 mpi_t* mpi_sub2(mpi_t *a, mpi_t *b, mpi_t *c);
 mpi_t* mpi_sub(mpi_t *a, mpi_t *b); // a -= b
 mpi_t* mpi_neg(mpi_t *a, mpi_t *b); //b = -a
 mpi_t* mpi_add2(mpi_t *a, mpi_t *b, mpi_t *c);
 mpi_t* mpi_add(mpi_t *a, mpi_t *b); // a += b
 mpi_t* mpi_bsl(mpi_t *n, int32_t bits);
 mpi_t* mpi_bsr(mpi_t *n, int32_t bits);
 mpi_t* mpi_wsl(mpi_t *a, int32_t n);
 mpi_t* mpi_wsr(mpi_t *a, int32_t n);
 mpl_t* mpi_mul(mpi_t *a, mpi_t *b, mpl_t *c);
 mpi_t* mpi_muld(mpi_t *n, uint32_t d);
 mpi_t* mpi_regularize(mpi_t *n);
 mpi_t* mpi_not(mpi_t *n);
 mpl_t* mpi_square(mpi_t *n, mpl_t* n2);
 mpi_t* mpi_truncate(mpl_t *n, mpi_t *m);
 mpi_t* mpi_binary_inverse(mpi_t *m, mpi_t *x, mpi_t *x1);

 mpl_t* mpl_bsr(mpl_t *n, int32_t bits);
 mpl_t* mpl_bsl(mpl_t *n, int32_t bits);
 mpl_t* mpl_wsr(mpl_t *a, int32_t n);
 mpl_t* mpl_wsl(mpl_t *a, int32_t n);
 mpl_t* mpl_init(mpl_t *n);
 mpl_t* mpl_copy(mpl_t *n, mpi_t *m);
 mpl_t* mpl_mod(mpl_t *n, mpi_t *m); //n %= m
 mpl_t* mpl_sub(mpl_t *a, mpl_t *b); // a -= b
 mpl_t* mpl_subi(mpl_t *a, mpi_t *b); // a -= b
 mpl_t* mpl_add(mpl_t *a, mpl_t *b); //a += b
 mpl_t* mpl_addi(mpl_t *a, mpi_t *b); //a += b

#ifdef __cplusplus
}
#endif

#endif /* __MPI_H__ */
