#include "y86sim.h"


 // valP = next sequential PC 
 // valC = constant / jump target
 // valM = value from memory
 
void pcupdate(ybyte icode, ybyte ifun,
              yword valP, yword valC, yword valM,
              yword *newPC)
{
    if (!alliswell())
        return;

    // cases
    switch (icode)
    {
        // halt 
        case IHALT:
            setstatus(SHLT);

            // halt's address is valP - 1.
            if (valP > 0)
                *newPC = valP - 1;
            else
                *newPC = valP;   
            break;

        // conditional jumps
        case IJXX:
            //check if jump is true
            if (cond(ifun))
                // If truevgo to target address valC
                *newPC = valC;
            else
                // go to next instruction valP
                *newPC = valP;
            break;

        case ICALL:
            // PC becomes the target (valC)
            *newPC = valC;
            break;

        case IRET:
            // new PC is the value popped from the stack (valM)
            *newPC = valM;
            break;

        default:
            // go to next sequential instruction
            *newPC = valP;
            break;
    }
}
