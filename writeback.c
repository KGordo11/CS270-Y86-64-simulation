#include "y86sim.h"

// write calculated value into registers
void writeback(ybyte icode, ybyte ifun,
               ybyte rA, ybyte rB,
               yword valE, yword valM)
{

    if (!alliswell())
        return;

    // Choose behavior based on instruction code
    switch (icode)
    {
        // write valE into rB
        case IIRMOV:
            regput1(rB, valE);
            break;

        // conditional
        case IRRMOV:
            // we only when condition is true
            //checks the condition 
            if (cond(ifun))
            {
                // Write valE into destination rB
                regput1(rB, valE);
            }
            break;

        // add sub and xor
        case IOP:
            //  valE goes into rB
            regput1(rB, valE);
            break;

        // load from memory into rA
        case IMRMOV:
            // valM has value we read from memory
            regput1(rA, valM);
            break;

        // rmmov push and call all change %rsp 
        case IRMMOV:
        case IPUSH:
        case ICALL:
            // valE holds new %rsp val
            regput1(R_RSP, valE);
            break;

        // write pop value into rA and update %rsp
        case IPOP:
            // valM has value popped from stack write into rA
            regput1(rA, valM);
            // valE has the new %rsp 
            regput1(R_RSP, valE);
            break;

        // only update %rsp 
        // new PC set in pcupdate using valM
        case IRET:
            // valE holds  new %rsp 
            regput1(R_RSP, valE);
            break;

        default:
            break;
    }
}
