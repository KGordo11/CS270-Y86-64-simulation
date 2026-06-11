#include "y86sim.h"
#define NIBMASK 0xf

void fetch(yword PC, ybyte *icode, ybyte *ifun,
      ybyte *rA, ybyte *rB,
      yword *valC, yword *valP) {
  ybyte instr;
  ybyte lonib;
  ybyte hinib;
  ybyte funcb;
  yword imval;

  if (!alliswell())
    return;
  instr = mreadb(PC++);
  hinib = (instr >> 4) & NIBMASK;
  lonib = instr & NIBMASK;
  if (hinib > IPOP) {
    setstatus(SINS);
    return;
  }
  *icode = hinib;
  *ifun = lonib;
  if ((hinib == IRRMOV) || (hinib == IIRMOV)  ||
      (hinib == IRMMOV) || (hinib == IMRMOV) ||
      (hinib == IPUSH) || (hinib == IPOP) ||
      (hinib == IOP)) {
    funcb = mreadb(PC++);
    *rA = (funcb >> 4) & NIBMASK;
    *rB = funcb & NIBMASK;
  }
  switch (hinib) {
  default:
    break;
  case IIRMOV:
  case IRMMOV:
  case IMRMOV:
  case IJXX:
  case ICALL:
    *valC = mreadw(PC);
    PC += 8;
  }
  *valP = PC; // after incrementing;
}
  
