#include "y86sim.h"

// loads stores pushes pop call returns with memory
void memory(ybyte icode,
            yword valA, yword valB, yword valE,
            yword *valM)
{
    // if error before 
    if (!alliswell())
        return;

    // output  0
    *valM = 0;

    // cases
    switch (icode)
    {
        // store register value into memory
        case IRMMOV:
            // valA value from rA valE is effective address
            mwritew(valA, valE);
            break;

        // load value from memory into register
        case IMRMOV:
            // read 8 bytes from memory at address valE
            *valM = mreadw(valE);
            break;

        // store a value on the stack
        case IPUSH:
            // valE is new stack  %rsp - 8
            // valA is value we want to push
            mwritew(valA, valE);
            break;

        // take value from  stack
        case IPOP:
            // valA holds old %rsp 
            // Read word stored at that stack address 
            *valM = mreadw(valA);
            break;

        // push return address on stack
        case ICALL:
            // main.c calls memory
            //   valA = valA from decode
            //   valB = valP 
            //   valE = new %rsp / old %rsp - 8
            // store valB at valE
            mwritew(valB, valE);
            break;

        //  pop return address from stack
        case IRET:
            // valA holds old %rsp
            *valM = mreadw(valA);
            break;

        default:
            break;
    }
}
