#include "y86sim.h"

// which registers to read
void decode(ybyte icode, ybyte rA, ybyte rB,
            yword *valA, yword *valB)
{
    // if invalid then stop
    if (!alliswell())
        return;

    // set both output values to 0
    *valA = 0;
    *valB = 0;

    // cases based on the instruction icode
    switch (icode)
    {
        // do not need to read any register values 
        case IHALT:
        case INOP:
        case IIRMOV:
            break;

        //  use rA and rB registers as inputs
        case IRRMOV:
        case IOP:
            // read registers and store values
            regget(rA, rB, valA, valB);
            break;

        // rA is data value rB is  base address
        case IRMMOV:
            // read rA and rB value
            regget(rA, rB, valA, valB);
            break;

        // only need base addres
        case IMRMOV:
            // Read rB and store
            regget1(rB, valB);
            break;
            
        // use %rsp
        case IPOP:
        case IRET:
            // read %rsp and store
            regget1(R_RSP, valA);
            break;

        // need value in rA and current %rsp
        case IPUSH:
        case ICALL:
            // read rA
            regget1(rA, valA);
            // read %rsp 
            regget1(R_RSP, valB);
            break;

        // just the PC and constant values
        case IJXX:
            break;

        // if unknown instruction then invalid instruction
        default:
            setstatus(SINS);
            break;
    }
}
