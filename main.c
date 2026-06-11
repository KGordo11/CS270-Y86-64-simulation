/*
 * Y86-64 simulator.
 * Stages update parameters passed by pointers, which represent wires
 * passing values to next stage.
 * fetch() - reads first byte of instruction from memory, then additional
 *    bytes as needed, parses out steering values for next stage.
 * decode() - selects from fetched values based on opcode and function
 * execute() - ALU carries out the required computation, updates result vars
 * memory() - write word to/read word from memory, updates vars
 * writeback() - write results of other stages back to register file
 * updatePC() - compute next address based on condition codes
 */
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include "y86sim.h"
/* the following are (probably) only for xCode/mac OS */
#include <stdlib.h>
#include <fcntl.h>


/********* static variables - only accessible from here *********/

static ybyte mem[MEMSIZE];  // data memory
static yword regfile[NREGS];  // initially zeros

static ybyte icode, ifun, rA, rB, srcA, srcB;
static yword valP, valA, valB, valC, valE, valM, valP;
static yword PC;  // program counter
static yword newPC; // next value, changes on clock tick;
static ybyte CCs[3];
static ybyte status;


static char *status_names[] = {
  "???????",                // 0 is not used
  "AOK",
  "Invalid Address",
  "Invalid Instruction",
  "Halted"
};

static char *reg_names[] = {     /* for dump() */
  "%rax", "%rcx", "%rdx", "%rbx",
  "%rsp", "%rbp", "%rsi", "%rdi",
  "%r8", "%r9", "%r10", "%r11",
  "%r12", "%r13", "%r14" };


/**************** aux functions *********************/

/* halt and catch fire - never returns */
void hcf(char *error, int perrflag) {
  if (perrflag) {
    perror(error);
    fprintf(stderr,"exiting\n.");
  } else
    fprintf(stderr,"%s - exiting\n",error);
  exit(1);
}

int alliswell(void) {
  return (status == SAOK);
}

void setstatus(ybyte newstatus) {
  status = newstatus;
}

yword alu(ybyte aluop) {
  switch (aluop) {
  case OPADD:
    return valB + valA;
    break;
  case OPSUB:
    return valB - valA;
    break;
  case OPAND:
    return valB & valA;
    break;
  case OPXOR:
    return valB ^ valA;;
    break;
  default:
    setstatus(SINS);
    return 0;
  }
  /* NOTREACHED */
  return 0;
}

/* CC-setting functions - argument is a single-bit mask */
void setCC(int which) {
  CCs[which] = 1;
}

void clearCC(int which) {
  CCs[which] = 0;
}

ybyte getCC(int which) {
  return CCs[which];
}


/* Load program into memory from named file. */
/* Bail on any error. */
int loadprog(char *fname) {
  int fd;
  off_t fsize;
  int rv;
  fd = open(fname,O_RDONLY);
  if (fd < 0)
    hcf("couldn't open prog file",1);
  fsize = lseek(fd,0,SEEK_END);
  if (fsize < 0)
    hcf("lseek() failed",1);
  if (fsize > (MEMSIZE-MEMSTART))
    hcf("program file is too big",0);
  if (lseek(fd,0,SEEK_SET) < 0) // "this cannot happen"
    hcf("rewind lseek failed",1);
  rv = read(fd, &mem[MEMSTART], fsize);
  if(rv < fsize)
    hcf("short read loading program - should not happen",0);
  close(fd);
  return fsize;
}

ybyte mreadb(yword addr) {
  if (addr < MEMSTART || addr >= MEMSIZE) {
    status = SADR;
    return 0;
  }
  return mem[addr];
}

yword mreadw(yword addr) {
  yword v;
  if (addr < MEMSTART || addr+sizeof(yword)  > MEMSIZE) {
    status = SADR;
    return 0;
  }
  return *(yword *)(mem+addr);
}

void mwritew(yword value, yword addr) {
  if (addr < MEMSTART || (addr+sizeof(yword)) > MEMSIZE) {
    status = SADR;
  }
  *(yword *)(mem+addr) = value;
}

void regget1(ybyte srcreg, yword *dest) {
  if (srcreg > NREGS) {
    fprintf(stderr,"regget1: srcreg was %d. PC=0x%lx (%ld).\n",
	    srcreg,PC,PC);
    //    hcf("regget1: invalid srcA/srcB",0);
    return;
  }
  *dest = regfile[srcreg];
}

/* XXXX handle erroneous programs better! */
void regget(ybyte srcA, ybyte srcB, yword *valA, yword *valB) {
  regget1(srcA,valA);
  regget1(srcB,valB);
}

void regput1(ybyte dstreg, yword val) {
  if (dstreg >= NREGS)
    hcf("regget: invalid dstE/dstM",0);
  regfile[dstreg] = val;
}

void regput(ybyte dstX, ybyte dstY, yword valX, yword valY) {
  regput1(dstX,valX);
  regput1(dstY,valY);
}
  
void tick(void) {
  PC = newPC;
}

void dump(void) {
  int i;
  printf("PC=0x%lx (%ld)\n",PC,PC);
  printf("newPC=0x%lx (%ld)\n",newPC,newPC);
  printf("registers:\n");
  for (i=0; i<NREGS; i++)
    printf("%s: 0x%lx (%ld)\n",reg_names[i],regfile[i],regfile[i]);
}

int main(int argc, char *argv[]) {
  int steps = 0;
  int bytes;
  if (argc != 2) 
    hcf("Usage: y86sim <program file>",0);

  bytes = loadprog(argv[1]);
  printf("loaded program file %s of %d bytes.\n",argv[1],bytes);
  status = SAOK;
  PC = PROGENTRY;

  /* run until halting or encountering an error */
  while (status == SAOK) {
    fetch(PC, &icode, &ifun, &rA, &rB, &valC, &valP);
    decode(icode, rA, rB, &valA, &valB);
    execute(icode, ifun, valA, valB, valC, &valE);
    memory(icode, valA, valP, valE, &valM);
    writeback(icode, ifun, rA, rB, valE, valM);
    pcupdate(icode, ifun, valP, valC, valM, &newPC);
    steps++;
    tick();     // PC <- newPC
  }

  /* status indicates state */
  printf("Computation ended after %d instructions with status %s.\n",
	 steps,status_names[status]);
  dump(); // 
  
} // main
