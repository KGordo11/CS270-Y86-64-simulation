# Y86-64 Sequential Processor Simulator
**CS 270: Systems Programming | University of Kentucky | Fall 2025**

> A C implementation of a sequential Y86-64 processor simulator, executing the full six-stage pipeline — Fetch, Decode, Execute, Memory, Write-back, and PC Update — for a 64-bit toy instruction set architecture.

---

## Overview

This project implements a functional simulator of the Y86-64 ISA (a simplified subset of x86-64) as described in *Computer Systems: A Programmer's Perspective*. The simulator processes instructions through six sequential pipeline stages, maintaining processor state including a 15-register file, 32KB memory, condition codes (ZF, SF, OF), and a program counter.

The simulator correctly executes real Y86-64 programs including recursive functions, array summation routines, and Fibonacci computation — verified against expected register and memory state after execution.

---

## Pipeline Stages Implemented

| Stage | File | Responsibility |
|---|---|---|
| Fetch | `fetch.c` *(provided)* | Reads instruction bytes, sets icode/ifun/rA/rB/valC/valP |
| Decode | `decode.c` | Reads source registers into valA and valB |
| Execute | `execute.c` | ALU computation, condition code updates (ZF/SF/OF) |
| Memory | `memory.c` | Memory reads and writes (mrmov, rmmov, push, pop, call, ret) |
| Write-back | `writeback.c` | Writes results back to register file |
| PC Update | `pcupdate.c` | Computes next PC (sequential, jump, call, ret) |
| Condition | `cond.c` | Evaluates condition codes for jxx and cmovxx instructions |

---

## Instructions Supported

| Instruction | Description |
|---|---|
| `halt` | Stops execution with status Halted |
| `nop` | No operation |
| `irmovq` | Immediate → register |
| `rrmovq` / `cmovxx` | Register → register (conditional move) |
| `rmmovq` | Register → memory |
| `mrmovq` | Memory → register |
| `addq`, `subq`, `andq`, `xorq` | ALU operations, update condition codes |
| `jmp`, `jle`, `jl`, `je`, `jne`, `jge`, `jg` | Unconditional and conditional jumps |
| `pushq`, `popq` | Stack operations |
| `call`, `ret` | Subroutine call and return |

---

## Key Implementation Details

### Decode (`decode.c`)
Routes register reads based on `icode`. Instructions like `IPUSH` and `ICALL` read both `rA` (data value) and `%rsp` (stack pointer) simultaneously using `regget` and `regget1`.

### Execute (`execute.c`)
Calls the provided `alu()` function for arithmetic/logical ops and manually computes condition codes:
- **ZF**: set if result == 0
- **SF**: set if result < 0 (signed)
- **OF**: set on signed overflow — with special handling for `subq` (treated as adding the negation of the source, per x86-64 semantics)

### Condition Codes (`cond.c`)
Implements the 7 condition functions from Figure 3.15 of the textbook:
```
fun=0  unconditional      → always 1
fun=1  jle / cmovle       → (SF ^ OF) || ZF
fun=2  jl  / cmovl        → SF ^ OF
fun=3  je  / cmove        → ZF
fun=4  jne / cmovne       → !ZF
fun=5  jge / cmovge       → !(SF ^ OF)
fun=6  jg  / cmovg        → !(SF ^ OF) && !ZF
```

### Memory (`memory.c`)
Handles all memory-touching instructions using `mreadw`/`mwritew`. `ICALL` stores the return address (valB = valP) at the new stack pointer (valE). `IRET` reads the return address from the old stack pointer (valA).

### PC Update (`pcupdate.c`)
- `IJXX`: branches to `valC` if `cond(ifun)` is true, otherwise falls through to `valP`
- `ICALL`: jumps to target `valC`
- `IRET`: jumps to address popped from stack (`valM`)

---

## Build & Run

```bash
# Unpack and build
tar xvf p3.tar
cd project3
make

# Run a test program
./ysim asum.yo
./ysim fibo.yo
./ysim foorec.yo
```

---

## Test Programs & Expected Results

| Test | Instructions | Key Result |
|---|---|---|
| `irmovtest.yo` | 5 | `%rcx=10`, `%rbp=0x123456789`, `%r11=0xdeadbeeffeedface` |
| `jmptest.yo` | 7 | All registers 0, PC=0x333 |
| `optest.yo` | 14 | `%rax=0x12345678a`, `%rcx=0xdb`, `%rsi=-37` |
| `jxxtest.yo` | 23 | `%rcx=1`, `%rdx=%r12=0xff00000000000000` |
| `rmmr.yo` | 11 | Tests rmmovq/mrmovq round-trip |
| `pushpop.yo` | 8 | `%rax=67890`, `%rcx=12345`, `%rsp=512` |
| `fibo.yo` | 167 | `%rax=6765` (Fibonacci F20) |
| `asum.yo` | 34 | `%rax=0xabcdabcdabcd` (array sum) |
| `foorec.yo` | 461 | `%rax=1275` (recursive sum 0..50) |

---

## Concepts Demonstrated

- CPU pipeline stage design (Fetch → Decode → Execute → Memory → Write-back → PC Update)
- Register file and memory abstraction in C
- Condition code computation and signed overflow detection
- Stack frame management (`call`/`ret`, `push`/`pop`)
- Instruction set simulation and binary decoding
- Pointer-based output parameters and C `switch` dispatch

---

## Environment
- Language: C
- OS: Ubuntu 22.04 (x86)
- Compiler: GCC via provided Makefile
- Course: CS 270 Systems Programming, University of Kentucky
