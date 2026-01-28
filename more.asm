; ideas for instructions

; headered, 8 bit
MOV r/m, r/m
MOV r/m, imm

ADD r/m, imm
ADD r/m, r/m

SUB r/m, imm
SUB r/m, r/m

MUL r/m, imm
MUL r/m, r/m

DIV r/m, imm
DIV r/m, r/m

SHL r/m, imm
SHL r/m, r/m

SHR r/m, imm
SHR r/m, r/m

ROL r/m, imm
ROL r/m, r/m

ROR r/m, imm
ROR r/m, r/m

AND r/m, imm
AND r/m, r/m

OR r/m, imm
OR r/m, r/m

XOR r/m, imm
XOR r/m, r/m

XCHG r/m, r/m

LEA r/m, r/m

CALL imm
CALL r/m

CMP r/m, r/m
CMP r/m, imm

JFR imm8, r/m
JFR imm8, imm

JFA imm8, r/m 
JFA imm8, imm 

JMP imm
JMP r/m

; headered, 16 bit
ADD r/m, r/m, imm
ADD r/m, r/m, r/m

SUB r/m, r/m, imm
SUB r/m, r/m, r/m

MUL r/m, r/m, imm
MUL r/m, r/m, r/m

DIV r/m, r/m, imm
DIV r/m, r/m, r/m

SHL r/m, r/m, r/m
SHL r/m, r/m, imm

SHR r/m, r/m, r/m
SHR r/m, r/m, imm

ROL r/m, r/m, r/m
ROL r/m, r/m, imm

ROR r/m, r/m, r/m
ROR r/m, r/m, imm

AND r/m, r/m, r/m
AND r/m, r/m, imm

OR r/m, r/m, r/m
OR r/m, r/m, imm

XOR r/m, r/m, r/m
XOR r/m, r/m, imm

COMB imm8, r/m, r/m, r/m
COMB imm8, r/m, imm, r/m
COMB imm8, r/m, r/m, imm

; non-headered
PUSH r/m
POP r/m

RET imm8

PUSHALL 
POPALL

NOP
RET
HLT

/*
documentation:
	// pushall pushes everything except IP and SP

	// NOT reg
	// can be performed with
	XOR reg, -1
	// same size

JFR (jafar)
Jump if Flag Relative
imm = relative address
imm8 = flag

reg = relative address (runtime)
imm8 = flag

Example:
#define ZERO_FLAG 0
#define OVERFLOW_FLAG 1
// ETC ETC

	MVI r0, 5
start:
	CMP r0, 0
	JFR ZERO_FLAG, end
	DEC r0
	JMP start
end:

*/

/*
Vector and scalar float instructions
would look like this... probably:

// float add
FADD r0, r1

FDIV, FSUB, FMUL, etc

// vector add 
VADD r0_256, r1_256

// vector add float
VADDF  r0_256, r1_256

// vector add mask
MOV r0, 0b1001
VADDM r1_256, r2_256, r0
	(the _64 suffix for r0, r0_64 is 
	implied because 64 is word size)

// vector broadcast
VBCST r0_256, r1

// vector broadcast mask
MOV r0, 0b0110
MOV r1_8, 255
VBCSTM r2_256, r1_8, r0

*/