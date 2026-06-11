#include "y86sim.h"

int cond(ybyte fun)
{
    // three condition code bits
    // zero
    ybyte zf = getCC(CCZF);   
    // sign
    ybyte sf = getCC(CCSF);   
    // overflow
    ybyte of = getCC(CCOF);   

    // cases
    switch (fun)
    {
        case 0:
            return 1;

        // true if (SF ^ OF) or ZF
        case 1:
            return ((sf ^ of) || zf);

        // rue if SF ^ OF
        case 2:
            return (sf ^ of);

        // true if ZF is set
        case 3:
            return zf;

        // true if ZF is not set
        case 4:
            return !zf;

        // ttrue if SF and OF have same value 
        case 5:
            return !(sf ^ of);

        // true if >= and not equal
        case 6:
            return (!(sf ^ of) && !zf);

        default:
            return 0;
    }
}
