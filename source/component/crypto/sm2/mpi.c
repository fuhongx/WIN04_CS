
#include <stdio.h>
#include <string.h>
#include "mpi.h"

 mpi_t* mpi_copy_d(mpi_t *n, uint32_t iInitial)
{
    n->sign = POSITIVE;
	
    if (iInitial==0)
        n->len = 0;
    else
        n->len = 1;
    n->v[0]= iInitial;

    n->carry = 0;
    return n;
}

// b = a
 mpi_t* mpi_copy(mpi_t *a, mpi_t *b) 
{
    int32_t i;
    b->sign = a->sign;
    b->len =  a->len;
    b->carry = 0;
    for (i = b->len - 1; i >= 0; --i) 
    {
        b->v[i] = a->v[i];
    }
    return b;
}

 mpi_t* mpi_add(mpi_t *a, mpi_t *b)
{
    int32_t i,j;
    register DOUBLE_INT direg;
    mpi_t temp;
    a->carry = 0;

    if (a->sign==POSITIVE)
    {
        if (b->sign!=POSITIVE)
        {
            return mpi_sub(a, mpi_neg(b, &temp));
        }
    }
    else
    {
        if (b->sign==POSITIVE)
        {
            mpi_neg(a, &temp);
            return mpi_sub2(b, &temp, a);
        }
    }

    j = __min(a->len, b->len);
    direg = 0;
    for (i = 0; i < j; i++)
    {	
        direg += a->v[i];
        direg += b->v[i];
        a->v[i] = (uint32_t)direg;
        direg >>= 32;
    }

    if (a->len > b->len)
    {
        j = a->len;
        for (; i < j; i++)
        {
            direg += a->v[i];
            a->v[i] = (uint32_t)direg;
            direg >>= 32;
        }
	}
	else
	{
        j = b->len;
        for (; i < j; i++)
        {
            direg += b->v[i];
            a->v[i] = (uint32_t)direg;
            direg >>= 32;
        }
	}

    if (j == MPI_LENGTH)
    {
        a->carry = (uint32_t)direg;
        a->len = j;
    }
    else
    {
        a->v[j] = (uint32_t)direg;
        if (a->v[j])
            a->len=j+1;
        else
            a->len=j;
    }
		
    return a;
}

 mpi_t* mpi_add2(mpi_t *a, mpi_t *b, mpi_t *c)
{
    return mpi_add(mpi_copy(a, c), b);
}

 mpi_t* mpi_neg(mpi_t *a, mpi_t *b)
{
    mpi_copy(a, b);
    if (a->sign == POSITIVE)
        b->sign = NEGTIVE;
    else
        b->sign = POSITIVE;

    return b;
}

 mpi_t* mpi_sub2(mpi_t *a, mpi_t *b, mpi_t *c)
{
    return mpi_sub(mpi_copy(a, c), b);
}

 mpi_t* mpi_sub(mpi_t *a, mpi_t *b)
{
    mpi_t temp;
    int32_t i,j;
    register DOUBLE_INT direg;
    const uint32_t *pm1, *pm2;
    int32_t len2;

    a->carry = 0;
    if (a->sign != b->sign)
    {
        mpi_neg(b, &temp);
        return mpi_add(a, &temp);
    }

    if (mpi_lt_abs(b, a))
    {
        pm2= a->v;
        len2 = a->len;
        pm1= b->v;

        if (b->sign == POSITIVE)
            a->sign = NEGTIVE;
        else
            a->sign = POSITIVE;
    }
    else
    {
        pm1 = a->v;
        pm2 = b->v;
        len2 = b->len;
    }
    j = __max(a->len,b->len);

#ifdef _MSC_VER
    direg = 0x100000000L + pm1[0];
#else
    direg = 0x100000000ll+ pm1[0];
#endif
    for (i = 0; i < len2; i++)
    {
        direg -= pm2[i];
        a->v[i] = (uint32_t)direg;
        direg >>= 32;
        direg += pm1[i+1];
        direg += 0xffffffffL;
    }

    for (; i < j; i++)
    {
        a->v[i] = (uint32_t)direg;
        direg >>= 32;
        direg += pm1[i+1];
        direg += 0xffffffffL;
    }

    a->len = j;
    for (i = j-1; i >= 0; i--)
    {
        if (a->v[i])
            break;
        a->len=i;
    }
    return a;
}

 int32_t mpi_lt_abs(mpi_t *a, mpi_t *b)
{
    int32_t i, j;
	
    j = __max(a->len,b->len);
	
    for (i = j-1; i >= 0; i--)
    {
        if (a->len>i && b->len>i)
        {
            if (a->v[i]>b->v[i])
                return 1;
            if (a->v[i]<b->v[i])
                return 0;
        }
        else
        {
            if (a->len<=i)
            {
                if (b->v[i])
                    return 0;
                else
                    continue;
            }
            if (b->len<=i)
            {
                if (a->v[i])
                    return 1;
                else
                    continue;
            }
        }
    }
		
    return 0;
}

 int32_t mpi_equ(mpi_t *a, mpi_t *b)
{
    int32_t i, j;
	
    j = __max(a->len, b->len);
	
    for (i = j-1; i >= 0; i--)
    {
        if (a->len > i && b->len > i)
        {
            if (a->v[i] != b->v[i])
                return 0;
            else
                continue;
        }
        if (a->len <= i)
        {
            if (b->v[i])
                return 0;
            else
                continue;
        }
        if (b->len <= i)
        {
            if (a->v[i])
                return 0;
            else
                continue;
        }
    }
		
    return 1;
}

 mpi_t* mpi_muld(mpi_t *a, uint32_t n)
{
    register DOUBLE_INT direg;
    uint32_t icarry = 0;
    int32_t i;
    a->carry = 0;
    if (n == 0)
    {
        a->sign = POSITIVE;
        a->len=0;
        a->v[0]= 0;
        return a;
    }

    for (i = 0; i < a->len; i++)
    {
        direg = a->v[i];
        direg *= n;
        direg += icarry;
        a->v[i] = (uint32_t) direg;
        icarry = (uint32_t)(direg>>32);
    }
		
    if (i == MPI_LENGTH)
    {
        a->carry = icarry;
    }
    else
    {
        a->v[i] = icarry;
        if (a->v[i])
        {
            a->len = i+1;
        }
    }
		
    return a;
}

mpl_t* mpi_mul(mpi_t *a, mpi_t *b, mpl_t *c)
{
    DOUBLE_INT direg = 0,sum;
    int32_t i,j,k,len, n;
    uint32_t r[MPI_LENGTH*2] = {0};
    int32_t rlen = 0;
		
    len = a->len + b->len;

    for (i = 0; i < MPI_LENGTH*2; i++)
        r[i]=0;

    for (k = 0; k < len; k++)
    {
        for (i = 0, n = k-i; i <= k; i++, n--)
        {
            if ((a->len>i) && b->len>n)
            {
                direg = a->v[i];
                direg *= b->v[n];

                sum = direg+r[k];
                r[k] = (uint32_t)sum;
                sum >>= BITS_OF_INT;

                sum += r[k+1];
                r[k+1] = (uint32_t)sum;
                sum >>= BITS_OF_INT;

                j = k+2;
                while (sum)
                {
                    sum += r[j];
                    r[j++] = (uint32_t)sum;
                    sum >>= BITS_OF_INT;
                }
            }			
        }
    }

    for (i=MPI_LENGTH*2-1;i>=0;i--)
    {
        if (r[i])
            break;
    }
    rlen = i+1;

    if (rlen>MPI_LENGTH)
    {
        c->l.len = MPI_LENGTH;
        c->h.len = rlen - MPI_LENGTH;

        for (i=0;i<c->h.len;i++)
            c->h.v[i]=r[i+MPI_LENGTH];
    }
    else
    {
        c->l.len = rlen;
        c->h.len = 0;
    }
    for (i=0;i<c->l.len;i++)
        c->l.v[i]=r[i];

    if (a->sign == b->sign)
        c->h.sign = c->l.sign = POSITIVE;
    else
        c->h.sign = c->l.sign = NEGTIVE;
    c->l.carry = c->h.carry = 0;

    return c;
}

 mpi_t* mpi_truncate(mpl_t *a, mpi_t *b)
{
    mpi_copy(&a->l, b);
    b->carry = 0;
    return b;
}

 int32_t mpi_equ_d(mpi_t *a, uint32_t n)
{
    int32_t i;
    if ((a->len==1 ) && (n==a->v[0]))
        return 1;
    if ((a->len==0 ) && n==0)
        return 1;
    for (i=1;i<a->len;i++)
    {
        if (a->v[i])
            return 0;
    }
    if (n==a->v[0])
        return 1;
    return 0;
}

 mpl_t* mpl_FastReduction(mpl_t *n, mpi_t *m)
{
	int32_t i, tflag;
	mpl_t tempH;

	tflag = n->l.sign;
	n->h.sign = n->l.sign = POSITIVE;

	i = n->h.len;
	while (--i >= 0)
	{
		while (n->h.v[i])
		{
			mpi_copy_d(&tempH.h, 0);
			mpi_copy_d(&tempH.l, 0);
			tempH.l.v[7] = tempH.l.v[0] = n->h.v[i];
			tempH.l.v[6] = tempH.l.v[5] = tempH.l.v[4] = tempH.l.v[2] = tempH.l.v[1] = 0;
			tempH.l.v[2] = 0 - n->h.v[i];
			tempH.l.v[3] = n->h.v[i] - 1;
			tempH.l.len = 8;

			mpl_wsl(&tempH, i);

			n->h.v[i] = 0;

			// add
			mpl_add(n, &tempH);
		}
	}

	n->h.len = 0;
	n->h.sign = tflag;

	// here, m must > 0. The field.
	if (mpi_cmp(&n->l, m) == 1)
		mpi_sub(&n->l, m);

	if (mpi_cmp(&n->l, m) == 0)
		mpi_copy_d(&n->l, 0);
	else
		n->l.sign = tflag;
	return n;
}

 mpl_t* mpl_Reduction(mpl_t *n, mpi_t *m)
{
	if (m->v[0] == 0xFFFFFFFF
		&& m->v[1] == 0xFFFFFFFF
		&& m->v[2] == 0x00000000
		&& m->v[3] == 0xFFFFFFFF
		&& m->v[4] == 0xFFFFFFFF
		&& m->v[5] == 0xFFFFFFFF
		&& m->v[6] == 0xFFFFFFFF
		&& m->v[7] == 0xFFFFFFFE
		&& m->len == MPI_LENGTH)
	{
		return mpl_FastReduction(n, m);
	}

	mpi_t odiv;
	mpi_t mt;
	mpl_t temp,tdiv;
	int32_t i,j,mflag,len;
	uint32_t ui,uj,flag;
	DOUBLE_INT t64;

	mpi_copy(m, &mt);
	mflag = n->l.sign;

	n->l.sign = n->h.sign = POSITIVE;

	for (i=mt.len-1;i>=0;i--)
	{
		if (mt.v[i])
			break;
	}
	mt.len = i+1;

	if (mpi_equ_d(&mt, 0))
	{
		return n;
	}

	mt.sign = POSITIVE;

	mpi_copy(&mt, &odiv);
	odiv.sign = POSITIVE;
	flag=0;
	ui=odiv.v[odiv.len-1];
	
	while (!(0x80000000 & (ui << flag)))
		flag++;
	mpi_bsl(&odiv, flag);

	len = n->l.len + n->h.len -odiv.len+1;
	if (len <= 0)
	{
		n->l.sign = n->h.sign = mflag;
		return n;
	}

	if (n->h.len<MPI_LENGTH)
		n->h.v[n->h.len]=0;

	ui = odiv.v[odiv.len-1]+1;

	for (i = n->h.len-1, j = n->h.len + MPI_LENGTH - odiv.len; i>=0; i--, j--)
	{
		if ((i+1)>=MPI_LENGTH)
			t64 = 0;
		else
			t64 = n->h.v[i+1];
		t64 <<=32 ;
		t64 += n->h.v[i];
		if (ui)
			uj = (uint32_t)(t64/ui);
		else
			uj = (uint32_t)(t64>>32);
		mpi_copy(&odiv, &temp.l);
		mpi_copy_d(&temp.h, 0);
		mpi_muld(&temp.l, uj);
		if (temp.l.carry)
		{
			temp.h.len = 1;
			temp.h.v[0]=temp.l.carry;
		}
		else
		{
			temp.h.len = 0;
		}
		mpl_wsl(&temp, j);
		mpi_copy(&odiv, &tdiv.l);
		mpi_copy_d(&tdiv.h, 0);
		mpl_wsl(&tdiv, j);
		mpl_sub(n, &temp);

		while (!mpi_lt_abs(&tdiv.h, &n->h))
		{
			mpl_sub(n, &tdiv);
		}
	}
	if (n->l.len < MPI_LENGTH)
		n->l.v[n->l.len]=0;

	ui=odiv.v[odiv.len-1]+1;

	for (i=n->l.len-1, j=n->l.len-odiv.len; i>=odiv.len-1; i--, j--)
	{
		if (i == MPI_LENGTH-1)
		{
			if (n->h.len == 0)
				t64 = 0;
			else
				t64 = n->h.v[0];
		}
		else
			t64=n->l.v[i+1];
		t64<<=32;
		t64+=n->l.v[i];

		if (ui)
			uj = (uint32_t)(t64/ui);
		else
			uj = (uint32_t)(t64>>32);
		mpi_copy_d(&temp.h, 0);
		mpi_copy(&odiv, &temp.l);
		mpi_muld(&temp.l, uj);
		if (temp.l.carry)
		{
			temp.h.len = 1;
			temp.h.v[0]=temp.l.carry;
		}
		else
		{
			temp.h.len = 0;
		}
		mpl_wsl(&temp, j);
		mpi_copy_d(&tdiv.h, 0);
		mpi_copy(&odiv, &tdiv.l);
		mpl_wsl(&tdiv, j);
		mpl_sub(n, &temp);

		while ((!mpi_lt_abs(&tdiv.l, &n->l))||(n->h.v[0] && n->h.len))
		{
			mpl_sub(n, &tdiv);
		}
	}

	while (!mpi_lt_abs(&mt, &n->l))
	{
		uj=n->l.v[n->l.len-1]/(mt.v[mt.len-1]+1);
		if (0==uj)
			uj=1;
		mpi_copy(&mt, &temp.l);
		mpi_muld(&temp.l, uj);
		mpi_sub(&n->l, &temp.l);
	}
	n->l.sign = n->h.sign = mflag;
	return n;
}

 mpl_t* mpl_add(mpl_t *a, mpl_t *b)
{
	mpi_t o;
	int32_t i;

	mpi_add(&a->l, &b->l);
	mpi_add(&a->h, &b->h);

	if (mpi_equ_d(&a->h, 0))
		a->h.sign = a->l.sign;

	if (a->l.sign == a->h.sign)
	{
		if (a->l.carry)
		{
			mpi_copy_d(&o, a->l.carry);
			o.sign = a->l.sign;
			mpi_add(&a->h, &o);
			a->l.carry = 0;
		}
	}
	else
	{
		for (i=0;i<MPI_LENGTH;i++)
			o.v[i]=HEX32BITS;
		o.len = MPI_LENGTH;
		o.sign = a->h.sign;
		mpi_add(&a->l, &o);
		
		o.len = 1;
		o.v[1] = 0;
		o.v[0] = 1;
		o.sign = a->h.sign;
		mpi_add(&a->l, &o);

		mpi_sub(&a->h, &o);
	}
	if (a->l.len < MPI_LENGTH && !mpi_equ_d(&a->h, 0))
		a->l.len = MPI_LENGTH;
	return a;
}

 mpl_t* mpl_addi(mpl_t *a, mpi_t *b)
{
	mpi_t o;
	int32_t i;

	mpi_add(&a->l, b);

	if (mpi_equ_d(&a->h, 0))
		a->h.sign = a->l.sign;

	if (a->l.sign == a->h.sign)
	{
		if (a->l.carry)
		{
			mpi_copy_d(&o, a->l.carry);
			o.sign = a->l.sign;
			mpi_add(&a->h, &o);
			a->l.carry = 0;
		}
	}
	else
	{
		for (i=0;i<MPI_LENGTH;i++)
			o.v[i]=HEX32BITS;
		o.len = MPI_LENGTH;
		o.sign = a->h.sign;
		mpi_add(&a->l, &o);
		
		o.len = 1;
		o.v[1] = 0;
		o.v[0] = 1;
		o.sign = a->h.sign;
		mpi_add(&a->l, &o);

		mpi_sub(&a->h, &o);
	}
	if (a->l.len < MPI_LENGTH && !mpi_equ_d(&a->h, 0))
		a->l.len = MPI_LENGTH;
	return a;
}

 mpl_t* mpl_sub(mpl_t *a, mpl_t *b)
{
    mpl_t temp;
    mpi_copy(&b->h, &temp.h);
    mpi_copy(&b->l, &temp.l);
    if (b->l.sign == POSITIVE)
    {
        temp.l.sign = temp.h.sign = NEGTIVE;
    }
    else
    {
        temp.l.sign = temp.h.sign = POSITIVE;
    }
    return mpl_add(a, &temp);
}

 mpl_t* mpl_subi(mpl_t *a, mpi_t *b)
{
    mpi_t temp;
  
    mpi_copy(b, &temp);

    if (b->sign == POSITIVE)
    {
        temp.sign = NEGTIVE;
    }
    else
    {
        temp.sign = POSITIVE;
    }
		
    return mpl_addi(a, &temp);
}

 mpi_t* mpi_wsr(mpi_t *a, int32_t n)
{
    mpl_t temp;
	
    mpi_copy_d(&temp.h, 0);
    mpi_copy(a, &temp.l);
    mpl_wsr(&temp, n);
    mpi_truncate(&temp, a);
    a->carry = 0;
    return a;
}

 mpl_t* mpl_wsr(mpl_t *a, int32_t n)
{
	int32_t i,j;
	if (n<0)
		return mpl_wsl(a, -n);

	for (i=0;i<a->l.len;i++) {
		if ((i+n) < a->l.len) {
			a->l.v[i] = a->l.v[i+n];
		} else {
			if (a->l.len == MPI_LENGTH) {
				j = i+n-MPI_LENGTH;
				if (j<a->h.len) {
                    if(j >=0)
					    a->l.v[i] = a->h.v[j];
					continue;
				} else {
					a->l.v[i] = 0;
					a->l.len = i;
					a->h.len = 0;
					return a;
				}
			} else {
				a->l.v[i]=0;
				a->l.len = i;
				a->h.len = 0;
				return a;
			}
		}
	}

	for (i=0;i<a->h.len;i++)
	{
		if ((i+n)<a->h.len)
		{
			a->h.v[i] = a->h.v[n+i];
		}
		else
		{
			a->h.v[i]=0;
			a->h.len = i;
			return a;
		}
	}
	return a;
}

 mpl_t* mpl_wsl(mpl_t *a, int32_t n)
{
	int32_t i,j,k,p;
	if (n<0)
		return mpl_wsr(a, -n);
	k = a->h.len + a->l.len;
	if (k==0)
		return a;
	if (k==1 && a->l.v[0]==0)
		return a;

	k += n;
	if (k >= MPI_LENGTH*2)
		k = MPI_LENGTH*2;
	j = k-MPI_LENGTH;

	for (i=j-1, p=i-n; i>=0; i--, p--)
	{
		if (p>=0)
		{
			a->h.v[i] = a->h.v[p];
		}
		else
		{
			if ((p+MPI_LENGTH)>=0)
			{
				a->h.v[i] = a->l.v[MPI_LENGTH+p];
				continue;
			}
			else
			{
				a->h.v[i]=0;
				continue;
			}
		}
	}

	if (k>=MPI_LENGTH)
		j = MPI_LENGTH;
	else
		j = k;
	for (i=j-1, p=i-n;i>=0;i--, p--)
	{
		if (p>=0)
		{
			a->l.v[i]=a->l.v[p];
		}
		else
		{
			a->l.v[i]=0;
		}
	}
	a->l.len = j;
	if (j>=MPI_LENGTH)
		a->h.len = k-MPI_LENGTH;
	else
		a->h.len = 0;
	return a;
}

 mpi_t* mpi_wsl(mpi_t *a, int32_t n)
{
    mpl_t temp;
    mpi_copy_d(&temp.h, 0);
    mpi_copy(a, &temp.l);
    mpl_wsl(&temp, n);
    mpi_truncate(&temp, a);
    a->carry = 0;
    return a;
}

 mpl_t* mpl_mod(mpl_t *a, mpi_t *m)
{
    return mpl_Reduction(a, m);
}

 mpi_t* mpi_bsl(mpi_t *a, int32_t iShiftBits)
{
	int32_t i,j,k;
	j = iShiftBits/BITS_OF_INT;
	if (j)
	{
		mpi_wsl(a, j);
	}

	j = iShiftBits%BITS_OF_INT;

	if (j==0)
		return a;

	k = a->len;
	if (k<MPI_LENGTH)
	{
		a->v[k] = a->v[k-1]>>(BITS_OF_INT-j);
		if (a->v[k])
			a->len++;
	}
	else
		a->carry = a->v[MPI_LENGTH-1]>>(BITS_OF_INT-j);

	for (i=k-1;i>0;i--)
	{
		a->v[i]<<=j;
		a->v[i] += a->v[i-1]>>(BITS_OF_INT-j);
	}

	a->v[0]<<=j;

	return a;
}

 mpi_t* mpi_bsr(mpi_t *a, int32_t iShiftBits)
{
	int32_t i,j;
	j = iShiftBits/BITS_OF_INT;
	if (j)
	{
		mpi_wsr(a, j);
	}

	j = iShiftBits%BITS_OF_INT;

	if (j==0)
		return a;

	for (i=0; i<a->len-1;i++)
	{
		a->v[i] >>= j;
		a->v[i] += (a->v[i+1]<<(BITS_OF_INT-j));
	}

	a->v[a->len-1] >>= j;

	if (a->v[a->len-1]==0)
		a->len--;

	return a;
}

 mpl_t* mpl_bsl(mpl_t *n, int32_t iShiftBits)
{
	int32_t i,j,k;
	j = iShiftBits/BITS_OF_INT;
	if (j)
	{
		mpl_wsl(n, j);
	}
	j = iShiftBits%BITS_OF_INT;

	if (j==0)
		return n;

	k = n->h.len ;
	if (k>0)
	{
		if (k<MPI_LENGTH)
		{
			n->h.v[k] = n->h.v[k-1]>>(BITS_OF_INT-j);
			if (n->h.v[k])
				n->h.len ++;
		}
		for (i = k-1; i>0;i--)
		{
			n->h.v[i] <<= j;
			n->h.v[i] += n->h.v[i-1]>>(BITS_OF_INT-j);
		}
		n->h.v[0] <<= j;
		n->h.v[0] += n->l.v[MPI_LENGTH-1]>>(BITS_OF_INT-j);
	}
	else
	{
		if (n->l.len==MPI_LENGTH)
		{
			n->h.v[0] = n->l.v[MPI_LENGTH-1]>>(BITS_OF_INT-j);
			if (n->h.v[0])
				n->h.len = 1;
		}
	}

	k = n->l.len;
	if (k<MPI_LENGTH)
	{
			n->l.v[k] = n->l.v[k-1]>>(BITS_OF_INT-j);
			if (n->l.v[k])
				n->l.len ++;
	}
	for (i=k-1;i>0;i--)
	{
		n->l.v[i]<<=j;
		n->l.v[i] += n->l.v[i-1]>>(BITS_OF_INT-j);
	}
	n->l.v[0]<<=j;

	return n;
}

mpl_t* mpi_square(mpi_t *n, mpl_t *n2)
{
	uint32_t c[MPI_LENGTH*2] = {
		0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0
	};
	int32_t k;
	int32_t len = n->len*2;

	int32_t i, j;
	DOUBLE_INT uv, sum;
	uint32_t R0 = 0, R1 = 0, R2 = 0;
	int32_t imax;

	for (k = 0; k <= len-2; k++)
	{
		imax = __min(k/2, n->len-1);
		for (i = imax, j = k-i; i >= 0; i--, j++)
		{
			if (j >= n->len)
				break;

			uv = n->v[i];
			uv *= n->v[j];
			if (i < j)
			{
				if (0x8000000000000000ull & uv)
					R2++;
				uv <<= 1;
			}
			sum = R0;
			sum += (uint32_t)uv;
			R0 = (uint32_t)sum;
			sum >>= BITS_OF_INT;

			sum += R1;
			sum += (uv >> BITS_OF_INT);
			R1 = (uint32_t)sum;
			sum >>= BITS_OF_INT;

			R2 += (uint32_t)sum;
		}
		c[k] = R0;
		R0 = R1;
		R1 = R2;
		R2 = 0;
	}

	c[len-1] = R0;

	mpi_copy_d(&n2->l, 0);
	mpi_copy_d(&n2->h, 0);
	if (len>MPI_LENGTH)
	{
		n2->l.len = MPI_LENGTH;
		n2->h.len = len - MPI_LENGTH;

		for (i=0;i<n2->h.len;i++)
			n2->h.v[i]=c[i+MPI_LENGTH];
	}
	else
	{
		n2->l.len = len;
		n2->h.len = 0;
	}

	for (i=0;i<n2->l.len;i++)
		n2->l.v[i]=c[i];

	return n2;
}

 mpl_t* mpl_bsr(mpl_t *n, int32_t iShiftBits)
{
	int32_t i,j,k;
	j = iShiftBits/(BITS_OF_INT);
	if (j)
	{
		mpl_wsr(n, j);
	}
	j = iShiftBits%(BITS_OF_INT);

	if (j==0)
		return n;

	k = n->l.len;

	for (i=0; i<n->l.len-1;i++)
	{
		n->l.v[i]>>=j;
		n->l.v[i] += n->l.v[i+1]<<(BITS_OF_INT-j);
	}
	if (k<MPI_LENGTH)
	{
		n->l.v[n->l.len-1]>>=j;
		if (n->l.v[n->l.len-1]==0)
			n->l.len --;
	}
	else
	{
		n->l.v[n->l.len-1]>>=j;
		if (n->h.len>0)
			n->l.v[n->l.len-1]+= n->h.v[0]<<(BITS_OF_INT-j);
	}
	
	k = n->h.len ;
	if (k>0)
	{
		for (i =0;i< k-1; i++)
		{
			n->h.v[i] >>= j;
			n->h.v[i] += n->h.v[i+1]<<(BITS_OF_INT-j);
		}

		n->h.v[k-1] >>= j;
		if (n->h.v[k-1]==0)
			n->h.len --;
	}
	return n;
}

 mpi_t* mpi_binary_inverse(mpi_t *m, mpi_t *x, mpi_t *xi)
{
	mpi_t u, v;
	mpl_t x1, x2, mod;

	mpi_copy(x, &u);
	mpi_copy(m, &v);
	mpi_copy_d(&x1.h, 0);
	mpi_copy_d(&x1.l, 1);
	mpi_copy_d(&x2.h, 0);
	mpi_copy_d(&x2.l, 0);
	mpi_regularize(m);
	mpi_copy_d(&mod.h, 0);
	mpi_copy(m, &mod.l);

	while (!mpi_equ_d(&u, 1) && !mpi_equ_d(&v, 1))
	{
		while (!(u.v[0] & 0x01))
		{
			mpi_bsr(&u, 1);
			if (!(x1.l.v[0] & 0x01))
				mpl_bsr(&x1, 1);
			else
			{
				mpl_bsr(mpl_add(&x1, &mod), 1);
			}
		}

		while (!(v.v[0] & 0x01))
		{
			mpi_bsr(&v, 1);
			if (!(x2.l.v[0] & 0x01))
				mpl_bsr(&x2, 1);
			else
			{
				mpl_bsr(mpl_add(&x2, &mod), 1);
			}
		}

		if (mpi_cmp(&u, &v) == -1)
		{
			mpi_sub(&v, &u);
			mpl_sub(&x2, &x1);
		}
		else
		{
			mpi_sub(&u, &v);
			mpl_sub(&x1, &x2);
		}
	}

	if (mpi_equ_d(&u, 1) == 1)
	{
		mpl_mod(&x1, m);
		if (mpi_is_neg(&x1.l))
			mpl_add(&x1, &mod);
		mpi_copy(&x1.l, xi);
	}
	else
	{
		mpl_mod(&x2, m);
		if (mpi_is_neg(&x2.l))
			mpl_add(&x2, &mod);
		mpi_copy(&x2.l, xi);
	}
	return xi;
}

 mpi_t* mpi_regularize(mpi_t *n)
{
    int32_t i;

    for (i=n->len-1;i>=0;i--)
    {
        if (n->v[i])
            break;
    }
    n->len = i+1;

    return n;
}

 int32_t mpi_LargerThan(mpi_t *a, mpi_t *b)
{
    if (a->sign!=NEGTIVE)
    {
        if (b->sign==NEGTIVE)
            return 1;
        if (a->sign==POSITIVE)
        {
            if (b->sign == POSITIVE)
            {
                if (mpi_lt_abs(a, b))
                    return 1;
                else
                    return 0;
            }
            else
            {
                return 0;
            }
        }
        else
            return 0;
    }
    if (a->sign==NEGTIVE)
    {
        if (b->sign!=NEGTIVE)
            return 0;
        else
        {
            if (mpi_lt_abs(a, b))
                return 1;
            else
				        return 0;
        }
    }
		
    return 0;
}

 int32_t mpi_cmp(mpi_t *a, mpi_t *b)
{
    if (mpi_equ(a, b))
        return 0;
		
    if (mpi_LargerThan(a, b))
        return 1;
		
    return -1;
}

 int32_t mpi_bytes(mpi_t *n)
{
    int32_t i;
    uint32_t j, k;
	
    mpi_regularize(n);
    if (n->len == 0)
        return 0;
    j = n->v[n->len-1];
    k = 0xff;
    for (i = sizeof(uint32_t)-1; i >= 0; i--)
    {
        if (j &(k << (i * 8)))
            break;
    }
    return i+1+(n->len-1)*sizeof(uint32_t);
}

 int32_t mpi_import(mpi_t *n, uint8_t *abContent, int32_t iLength)
{
    int32_t i, j;
	
    n->carry = 0;
    n->len = 0;
    n->sign = POSITIVE;

    if (iLength > (int32_t)(MPI_LENGTH*sizeof(uint32_t)))
    {
        return 0;
    }

    j = 0;
		
    n->v[n->len] = 0;
		
    for (i = iLength - 1; i >= 0 ; i--)
    {
        n->v[n->len] += abContent[i]<<(8*j);
        j++;
        if (j >= (int32_t)sizeof(uint32_t))
        {
            j = 0;
            n->len++;
            if (n->len != MPI_LENGTH)
                n->v[n->len] = 0;
        }
    }
		
    if (j)
        n->len++;
		
    return n->len;
}

 int32_t mpi_export(mpi_t *n, uint8_t *abOutBytes, int32_t iMinLength)
{
    int32_t j,k;
    int32_t iLengthOfExport;
    uint32_t u;
    iLengthOfExport = 0;
    j = n->len-1;
    if (j<0)
		    return -1;

    u = n->v[j];
    while (u==0)
    {
        j--;
        if (j < 0)
            return -1;
        u = n->v[j];
    }

    k = sizeof(uint32_t)-1;
    while (0==(u>>(k*8)))
        k--;

    int32_t iOut = j*sizeof(uint32_t) + k + 1;
    while (iOut < iMinLength)
    {
        abOutBytes[iLengthOfExport++] = 0x00;
        iMinLength--;
    }

    while (k >= 0)
    {
        abOutBytes[iLengthOfExport] = (uint8_t )(u>>(k*8));
        iLengthOfExport++;
        k--;
    }

    j--;
    u = n->v[j];
    while (j >= 0)
    {
        u = n->v[j];
        for (k=sizeof(uint32_t)-1;k>=0;k--)
        {
            abOutBytes[iLengthOfExport] = (uint8_t )(u>>(k*8));
            iLengthOfExport++;
        }
        j--;
    }

    return iLengthOfExport;
}


 int32_t mpi_bits(mpi_t *n)
{
    int32_t i;
    uint32_t k;
    int32_t iLength;
    for (i=n->len-1;i>=0;i--)
    {
        if (n->v[i])
				{
            break;
				}
    }
    iLength = (i+1)*8*sizeof(uint32_t);
    k = n->v[i];
    for (i = 0; i < (int32_t)(8*sizeof(uint32_t)); i++)
    {
        if (UNSIGNEDLEFTBIT & (k<<i))
				{
            break;
				}
    }
    return iLength - i;
}

 int32_t mpi_is_neg(mpi_t *n)
{
    return n->sign == NEGTIVE;
}


 mpi_t* mpi_not(mpi_t *n)
{
    n->sign *= -1;
    return n;
}
