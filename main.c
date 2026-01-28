#include <stdint.h>
#include <unistd.h>

#define u8  uint8_t
#define u16 uint16_t
#define u32 uint32_t
#define u64 uint64_t

#define Xupper(n) ((u32)( \
("0123456789ABCDEF"[((n) >> 4) & 0xF]) | \
("0123456789ABCDEF"[(n) & 0xF] << 8)))

/* RNG means range */
#define XRNG002(b, t) [(b)]=t((b)), [(b)+1]=t((b)+1)
#define XRNG004(b, t) XRNG002(b, t), XRNG002((b)+0x02, t)
#define XRNG008(b, t) XRNG004(b, t), XRNG004((b)+0x04, t)
#define XRNG016(b, t) XRNG008(b, t), XRNG008((b)+0x08, t)
#define XRNG032(b, t) XRNG016(b, t), XRNG016((b)+0x10, t)
#define XRNG064(b, t) XRNG032(b, t), XRNG032((b)+0x20, t)
#define XRNG128(b, t) XRNG064(b, t), XRNG064((b)+0x40, t)
#define XRNG256(b, t) XRNG128(b, t), XRNG128((b)+0x80, t)

static const u16 hex[256] = {
XRNG256(0x00, Xupper) };

/* VLAs were meant to be used */
void prinsnh(u8 *buf, u8 len) {
	u8 rl = len*4, arr[rl], *p = arr;
	for (int i = 0; i < len; i++) {
		*((u16*)p) = hex[buf[i]];
		p += 2;
		*p++ = 'h';
		*p++ = ' ';
	}
	write(STDOUT_FILENO, arr, rl);
}

void prinsn(u8 *buf, u8 len) {
	u8 rl = len*3, arr[rl], *p = arr;
	for (int i = 0; i < len; i++) {
		*((u16*)p) = hex[buf[i]];
		p += 2;
		*p++ = ' ';
	}
	write(STDOUT_FILENO, arr, rl);
}

int size(register u8 byte) {
	register u8 b2 = byte >> 6;

	if (b2 == 0) return 1;
	if (b2 == 1) return 2;
	
	return (byte & 15) + 2 + ((byte >> 6) & 1);
}

int main() {
	u8 arr[] = {0x00, 0x01, 0xFF, 0x67};

	prinsn(arr, 4);

	return 0;
}