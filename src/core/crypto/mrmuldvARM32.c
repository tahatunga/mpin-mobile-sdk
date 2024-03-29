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

#include <stdio.h>
#include "miracl.h"

mr_small muldiv(mr_small a,mr_small b,mr_small c,mr_small m,mr_small *rp)
{
    mr_small q;
    mr_large dble=(mr_large)a*b+c;
    q=(mr_small)MR_LROUND(dble/m);
    *rp=(mr_small)(dble-(mr_large)q*m);
    return q;
}

#ifdef MR_FP_ROUNDING

mr_small imuldiv(mr_small a,mr_small b,mr_small c,mr_small m,mr_large im,mr_small *rp)
{
    mr_small q;
    mr_large dble=(mr_large)a*b+c;
    q=(mr_small)MR_LROUND(dble*im);
    *rp=(mr_small)(dble-(mr_large)q*m);
    return q;
}

#endif


#ifndef MR_NOFULLWIDTH

mr_small muldvm(mr_small a,mr_small c,mr_small m,mr_small *rp)
{
    mr_small q;
    union doubleword dble;
    dble.h[MR_BOT]=c;
    dble.h[MR_TOP]=a;
    q=(mr_small)(dble.d/m);
    *rp=(mr_small)(dble.d-(mr_large)q*m);
    return q;
}

mr_small muldvd(mr_small a,mr_small b,mr_small c,mr_small *rp)
{
    union doubleword dble; 
    dble.d=(mr_large)a*b+c;
    *rp=dble.h[MR_BOT];
    return dble.h[MR_TOP];
}

void muldvd2(mr_small a,mr_small b,mr_small *c,mr_small *rp)
{
    union doubleword dble; 
    dble.d=(mr_large)a*b+*c+*rp;
    *rp=dble.h[MR_BOT];
    *c=dble.h[MR_TOP];
}

#endif

