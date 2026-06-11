#include "y86sim.h"

// calculations and condition 
void execute(ybyte icode, ybyte ifun,
             yword valA, yword valB, yword valC,
             yword *valE)
{
    // if error happend before then stop 
    if (!alliswell())
        return;

    // cases
    switch (icode)
    {
        // move an constant into a reg
        case IIRMOV:
            // result is constant valueC
            *valE = valC;
            break;

        // move value from source register to destination
        case IRRMOV:
            // Result is just valA (value we read from rA)
            *valE = valA;
            break;

        // add subtract and xor
        case IOP:
        {
            // compute the result based on ifun
            yword result = alu(ifun);

            // store result into *valE
            *valE = result;

            // clear condition code ZF SF OF
            clearCC(CCZF);
            clearCC(CCSF);
            clearCC(CCOF);

            // result is a signed long
            long sres = (long)result;

            // if result is zero set the ZF
            if (sres == 0)
                setCC(CCZF);

            // if result is negative set the SF 
            if (sres < 0)
                setCC(CCSF);

            // calc OF 
            long sa = (long)valA;  // signed version of valA
            long sb = (long)valB;  // signed version of valB
            int of = 0;            // start with no overflow

            // cases ifun
            switch (ifun)
            {
                // signed addition
                case OPADD:
                    // overflow if same sign but sign is different
                    of = ((sa < 0) == (sb < 0)) && ((sres < 0) != (sa < 0));
                    break;

                // signed subtract sb - sa
                case OPSUB:
                    // overflow if operands have different sign and result sign is diffrent than sb
                    of = ((sb < 0) != (sa < 0)) && ((sres < 0) != (sb < 0));
                    break;

                // cannot overflow
                default:
                    of = 0;
                    break;
            }

            // detected overflow set the OF flag
            if (of)
                setCC(CCOF);

            // Break IOP case
            break;
        }

        // calc address as base an displacement
        case IRMMOV:
        case IMRMOV:
        case IJXX:
            // address is valB + valC
            *valE = valB + valC;
            break;

        // move the stack down
        case IPUSH:
        case ICALL:
            // New %rsp is old %rsp -8 byte
            *valE = valB - 8;
            break;

        // we move the stack up 
        case IPOP:
        case IRET:
            // New %rsp is old %rsp +8 byte 
            *valE = valA + 8;
            break;

        // if something else move on
        default:
            // result is 0 
            *valE = 0;
            break;
    }
}
