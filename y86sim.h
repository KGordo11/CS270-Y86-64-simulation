/* 32K memory */
#define MEMSIZE 0x4000 // 16K bytes of memory
#define MEMSTART 0
#define PROGENTRY 0x00  // examples start at first byte
#define NREGS 15

/* constants that designate registers */
#define R_RSP 4
#define R_NONE  15

/* status values */
#define SAOK  1   // normal ops
#define SADR  2   // invalid memory address
#define SINS  3   // invalid instruction
#define SHLT  4   // halt executed

/* Condition codes - index into CCs array */
#define CCZF  0
#define CCSF  1
#define CCOF  2

#define IHALT  0
#define INOP   1
#define IRRMOV 2
#define IIRMOV 3
#define IRMMOV 4
#define IMRMOV 5
#define IOP    6
#define IJXX   7
#define ICALL  8
#define IRET   9
#define IPUSH  0xa
#define IPOP   0xb

#define FNONE  0

#define OPADD 0
#define OPSUB 1
#define OPAND 2
#define OPXOR 3

#define CNDLE  1
#define CNDLS  2
#define CNDEQ  3
#define CNDNE  4
#define CNDGE  5
#define CNDGR  6

typedef unsigned long yword;
typedef unsigned char ybyte;

/******** aux functions, defined in main.c ************/
int alliswell(void);
void hcf(char *msg, int callperror);
void setstatus(ybyte newstat);
ybyte getCC(int which);
void setCC(int which);
void clearCC(int which);
int cond(ybyte fun);
yword alu(ybyte fun);
void dump(void);

ybyte mreadb(yword addr);  // read a byte from memory
yword mreadw(yword addr);  // read a word from memory
void mwritew(yword value, yword addr); // write a word to memory

void regget1(ybyte rX, yword *valX);
void regget(ybyte rX, ybyte rB, yword *valX, yword *valY);
void regput1(ybyte rX, yword valX);
void regput(ybyte rX, ybyte rY, yword valX, yword valY);

/******** stage functions ************/
/* Each returns true as long as everything is OK.
 * postcondition: status indicates what happened.
 */
void fetch(yword PC, ybyte *icode, ybyte *ifun,
	  ybyte *rA, ybyte *rB,
	  yword *valC, yword *valP);

void decode(ybyte icode, ybyte rA, ybyte rB,
	     yword *valA, yword *valB);

void execute(ybyte icode, ybyte ifun,
	    yword valA, yword valB, yword valC,
	    yword *valE);

void memory(ybyte icode,
	   yword valA, yword valB, yword valE,
	   yword *valM);

void writeback(ybyte icode, ybyte ifun,
		ybyte rA, ybyte rB,
		yword valE, yword valM);

void pcupdate(ybyte icode, ybyte ifun,
	     yword valC, yword valP, yword valM,
	     yword *newPC);
