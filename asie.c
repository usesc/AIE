#include <stdint.h>
#include <stdio.h>
#include <stdio.h>

#define u8  uint8_t
#define u16 uint16_t
#define u32 uint32_t
#define u64 uint64_t

#define asmcpy(dest, src, n) \
__builtin_memcpy(dest, src, n)

#define asclz(ll) \
__builtin_clzll(ll)

#define NOHDR 0
#define HDR8  1
#define HDR16 2

#define REG 0
#define MEM 1
#define IMM 2

#define RBITS 5

#define SZ8  0
#define SZ16 1
#define SZ32 2
#define SZ64 3

#define SCL1 0
#define SCL2 1
#define SCL4 2
#define SCL8 3

#define mdisp(m)  (m & (u32)-1)
#define mmod(m)   ((m >> 32) & 7)
#define mscale(m) ((m >> 35) & 3)
#define mbase(m)  ((m >> 37) & 31)
#define midx(m)   ((m >> 42) & 31)
#define mpack(mod, base, index, scale, disp) ( \
	((u64)(disp)   & (u32) -1 ) | \
	((u64)((mod)   & 7)  << 32) | \
	((u64)((scale) & 3)  << 35) | \
	((u64)((base)  & 31) << 37) | \
	((u64)((index) & 31) << 42)   \
)

#define dtype(d) (d & 7)
#define dsize(d) ((d >> 3) & 3)
#define dsign(d) ((d >> 5) & 1)
#define dpack(type, size, sign) (  \
	((u64)(type)  & 7)        | \
	((u64)((size) & 3) << 3) | \
	((u64)((sign) & 1) << 5)   \
)

#define inopc(i) ((i) & 65535)
#define intype(i) ((i >> 17) & 255)
#define inops(i) ((i >> 28) & 255)
#define inpack(opc, type, nopers) ( \
	((u64)opc & 65535)        | \
	((u64)(type & 255) << 17) | \
	((u64)(nopers & 255) << 28) \
)

#define AS_M(mod, base) (128 | mod << 5 | base)
#define AS_R(sz, rcd) (sz << 5 | rcd)
#define AS_N(scl, idx) (scl << 5 | idx)

typedef struct op {
	int desc;
	union {
		u64 reg;
		u64 mem;
		u64 imm;
	};
} op;

typedef struct insn {
	struct op *o;
	u32 meta;
} insn;

int eis(int edi, u64 rsi) {
	int eax;

	eax = 63 - asclz(rsi | 1);
	eax = (eax + 8) >> 3;

	return rsi < 2 ? edi ^ rsi : eax;
}

int emsc(u8*p, u64 m) {
	u8 *s = p;
	
	int mod = mmod(m);

	*p++ = AS_M(mod, mbase(m));
	if (mod & 4) 
		*p++ = AS_N(mscale(m), midx(m));
	
	mod &= 3;
	int len = mod ? (1 << (mod - 1)) : 0;

	*(u32*)p = mdisp(m);
	p += len;
	
	return p - s;
}

int aie_encode(u8 *b, insn i) {
	u8 *st = b;
	
	u32 meta = i.meta;
	int itype = intype(meta);
	u32 opc = inopc(meta);
	int ops = inops(meta);
	
	if (itype == NOHDR) goto nohdr;
	
	u8 *h_ptr = b++; 
	if (itype == HDR16) {
        	*(u16 *)b = (u16)opc;
         	b += 2;
	} else {
        	*b++ = (u8)opc;
      	}
	
	u8 *ps = b;
	
	int sign = 0;
	int mod_bit = 0;
	
	for (int j = 0; j < ops; j++) {
		op o = i.o[j];
		
		u64 d = o.desc;
		int type = dtype(d);
		if (type == REG) {
			*b++ = AS_R(dsize(d), o.reg);
		} 
		else if (type == MEM) {
			b += emsc(b, o.mem);
			mod_bit = (mmod(o.mem) & 4);
		} 
		else if (type == IMM) {
			sign = dsign(d);
			u8 immsz = eis(sign, o.imm);
			*(u64*)b = o.imm;
			b += immsz;
			break;
		}
	}

	int s = b - ps;

	if (s > 15) return -1;

	u8 h = 0x80 | s;
	if (sign) h |= 0x10;
	if (mod_bit) h |= 0x20;
	if (itype == HDR16) h |= 0x40;

	*h_ptr = h;

	return b-st;
nohdr:;
	*b++ = (u8)opc & 0x7F;

	if (ops > 1) return -1;

	if (ops == 1) {
		op o = i.o[0];
		u8 val;
		
		int d = o.desc;
		int t = dtype(d);
		if (t == REG) 
			val = AS_R(dsize(d), o.reg);
		else if (t == IMM)
			val = (u8)o.imm;
		else if (t == MEM) {
			u64 m = o.mem;
			val = AS_M(mmod(m), mbase(m));
		}

		*b++ = val;
	}

	return b-st;
}

int main() {
	u64 m = mpack(0b101, 1, 2, 8, 0xFF);
	
	struct op o[] = {
		{.desc = dpack(REG, SZ64, 0), .reg = 0},
		{.desc = dpack(MEM, SZ64, 0), .mem = m},
		{.desc = dpack(IMM, SZ64, 0), .imm = 0x1234}
	};

	u64 p = inpack(0x1F, HDR8, sizeof(o)/sizeof(o[0]));
	insn i = {.o = o, .meta = p};

	u8 buf[24];

	printf("eis: %d\n", eis(1,1));

	int sz = aie_encode(buf, i);
	if (sz == -1) printf("failure");

	for (int i = 0; i < sz; i++) {
		printf("%.2Xh ", buf[i]);
	}

	printf("\n");

	return 0;
}

// 1001 0010 = 92h
// 1010 1010 = AAh
// 0110 0000 = 60h
// 0000 0000 = 00h


/*
clang asie.c -S -O3 \
-fno-unwind-tables \
-fno-asynchronous-unwind-tables \
-fno-verbose-asm \
-masm=intel
 */