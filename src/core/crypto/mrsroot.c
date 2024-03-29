/* 
Copyright 2014 CertiVox UK Ltd, All Rights Reserved.

The CertiVox M-Pin Client and Server Libraries are free software: you can
redistribute it and/or modify it under the terms of the BSD 3-Clause
License - http://opensource.org/licenses/BSD-3-Clause

For full details regarding our CertiVox terms of service please refer to
the following links:

  * Our Terms and Conditions -
    http://www.certivox.com/about-certivox/terms-and-conditions/
  
  * Our Security and Privacy -
    http://www.certivox.com/about-certivox/security-privacy/

  * Our Statement of Position and Our Promise on Software Patents -
    http://www.certivox.com/about-certivox/patents/
*/
/*
 *   MIRACL method for modular square root
 *   mrsroot.c 
 *
 *   Siguna Mueller's O(lg(p)^3) algorithm, Designs Codes and Cryptography, 2004 
 * 
 *   This is a little slower for p=1 mod 4 primes, but its not time critical, and
 *   more importantly it doesn't pull in the large powmod code into elliptic curve programs
 *   It does require code from mrjack.c and mrlucas.c
 *
 *   If p=3 mod 4, then  sqrt(a)=a^[(p+1)/4] mod p. Note that for many elliptic curves
 *   (p+1)/4 has very low hamming weight.
 *
 *   (was sqrt(a) = V_{(p+1)/4}(a+1/a,1)/(1+1/a))
 *
 *   Mueller's method is also very simple, uses very little memory, and it works just fine for p=1 mod 8 primes
 *   (for example the "annoying" NIST modulus 2^224-2^96+1)
 *   Also doesn't waste time on non-squares, as a jacobi test is done first
 *
 *   If you know that the prime is 3 mod 4, and you know that x is almost certainly a QR
 *   then the jacobi-dependent code can be deleted with some space savings.
 * 
 *   NOTE - IF p IS NOT PRIME, THIS CODE WILL FAIL SILENTLY!
 *
 */

#include <stdlib.h>
#include "miracl.h"

BOOL nres_sqroot(_MIPD_ big x,big w)
{ /* w=sqrt(x) mod p. This depends on p being prime! */
    int t,js;
   
#ifdef MR_OS_THREADS
    miracl *mr_mip=get_mip();
#endif
    if (mr_mip->ERNUM) return FALSE;

    copy(x,w);
    if (size(w)==0) return TRUE; 

    MR_IN(100)

    redc(_MIPP_ w,w);   /* get it back into normal form */

    if (size(w)==1) /* square root of 1 is 1 */
    {
        nres(_MIPP_ w,w);
        MR_OUT
        return TRUE;
    }

    if (size(w)==4) /* square root of 4 is 2 */
    {
        convert(_MIPP_ 2,w);
        nres(_MIPP_ w,w);
        MR_OUT
        return TRUE;
    }

    if (jack(_MIPP_ w,mr_mip->modulus)!=1) 
    { /* Jacobi test */ 
        zero(w);
        MR_OUT
        return FALSE;
    }

    js=mr_mip->pmod8%4-2;     /* 1 mod 4 or 3 mod 4 prime? */

    incr(_MIPP_ mr_mip->modulus,js,mr_mip->w10);
    subdiv(_MIPP_ mr_mip->w10,4,mr_mip->w10);    /* (p+/-1)/4 */

    if (js==1)
    { /* 3 mod 4 primes - do a quick and dirty sqrt(x)=x^(p+1)/4 mod p */
        nres(_MIPP_ w,mr_mip->w2);
        copy(mr_mip->one,w);
        forever
        { /* Simple Right-to-Left exponentiation */

            if (mr_mip->user!=NULL) (*mr_mip->user)();
            if (subdiv(_MIPP_ mr_mip->w10,2,mr_mip->w10)!=0)
                nres_modmult(_MIPP_ w,mr_mip->w2,w);
            if (mr_mip->ERNUM || size(mr_mip->w10)==0) break;
            nres_modmult(_MIPP_ mr_mip->w2,mr_mip->w2,mr_mip->w2);
        }
 
 /*     nres_moddiv(_MIPP_ mr_mip->one,w,mr_mip->w11); 
        nres_modadd(_MIPP_ mr_mip->w11,w,mr_mip->w3);  
        nres_lucas(_MIPP_ mr_mip->w3,mr_mip->w10,w,w);
        nres_modadd(_MIPP_ mr_mip->w11,mr_mip->one,mr_mip->w11); 
        nres_moddiv(_MIPP_ w,mr_mip->w11,w); */
    } 
    else
    { /* 1 mod 4 primes */
        for (t=1; ;t++)
        { /* t=1.5 on average */
            if (t==1) copy(w,mr_mip->w4);
            else
            {
                premult(_MIPP_ w,t,mr_mip->w4);
                divide(_MIPP_ mr_mip->w4,mr_mip->modulus,mr_mip->modulus);
                premult(_MIPP_ mr_mip->w4,t,mr_mip->w4);
                divide(_MIPP_ mr_mip->w4,mr_mip->modulus,mr_mip->modulus);
            }

            decr(_MIPP_ mr_mip->w4,4,mr_mip->w1);
            if (jack(_MIPP_ mr_mip->w1,mr_mip->modulus)==js) break;
            if (mr_mip->ERNUM) break;
        }
    
        decr(_MIPP_ mr_mip->w4,2,mr_mip->w3);
        nres(_MIPP_ mr_mip->w3,mr_mip->w3);
        nres_lucas(_MIPP_ mr_mip->w3,mr_mip->w10,w,w); /* heavy lifting done here */
        if (t!=1)
        {
            convert(_MIPP_ t,mr_mip->w11);
            nres(_MIPP_ mr_mip->w11,mr_mip->w11);
            nres_moddiv(_MIPP_ w,mr_mip->w11,w);
        }
    }
    
    MR_OUT
    return TRUE;
}

BOOL sqroot(_MIPD_ big x,big p,big w)
{ /* w = sqrt(x) mod p */
#ifdef MR_OS_THREADS
    miracl *mr_mip=get_mip();
#endif
    if (mr_mip->ERNUM) return FALSE;

    MR_IN(101)

    if (subdivisible(_MIPP_ p,2))
    { /* p must be odd */
        zero(w);
        MR_OUT
        return FALSE;
    }

    prepare_monty(_MIPP_ p);
    nres(_MIPP_ x,w);
    if (nres_sqroot(_MIPP_ w,w))
    {
        redc(_MIPP_ w,w);
        MR_OUT
        return TRUE;
    }

    zero(w);
    MR_OUT
    return FALSE;
}
