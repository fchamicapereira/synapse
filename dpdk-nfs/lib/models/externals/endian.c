#include <endian.h>

// We need __ctype_b_loc, but klee-uclibc doesn't give it to us unless we also
// enable other settings (such as enabling stdio) From the musl libc, Copyright
// © 2005-2014 Rich Felker, et al. File src/ctype/__ctype_b_loc.c
#if __BYTE_ORDER == __BIG_ENDIAN
#define X(x) x
#else
#define X(x) (((x) / 256 | (x)*256) % 65536)
#endif

static const unsigned short table[] = {
    0,        0,        0,        0,        0,        0,        0,
    0,        0,        0,        0,        0,        0,        0,
    0,        0,        0,        0,        0,        0,        0,
    0,        0,        0,        0,        0,        0,        0,
    0,        0,        0,        0,        0,        0,        0,
    0,        0,        0,        0,        0,        0,        0,
    0,        0,        0,        0,        0,        0,        0,
    0,        0,        0,        0,        0,        0,        0,
    0,        0,        0,        0,        0,        0,        0,
    0,        0,        0,        0,        0,        0,        0,
    0,        0,        0,        0,        0,        0,        0,
    0,        0,        0,        0,        0,        0,        0,
    0,        0,        0,        0,        0,        0,        0,
    0,        0,        0,        0,        0,        0,        0,
    0,        0,        0,        0,        0,        0,        0,
    0,        0,        0,        0,        0,        0,        0,
    0,        0,        0,        0,        0,        0,        0,
    0,        0,        0,        0,        0,        0,        0,
    0,        0,        X(0x200), X(0x200), X(0x200), X(0x200), X(0x200),
    X(0x200), X(0x200), X(0x200), X(0x200), X(0x320), X(0x220), X(0x220),
    X(0x220), X(0x220), X(0x200), X(0x200), X(0x200), X(0x200), X(0x200),
    X(0x200), X(0x200), X(0x200), X(0x200), X(0x200), X(0x200), X(0x200),
    X(0x200), X(0x200), X(0x200), X(0x200), X(0x200), X(0x200), X(0x160),
    X(0x4c0), X(0x4c0), X(0x4c0), X(0x4c0), X(0x4c0), X(0x4c0), X(0x4c0),
    X(0x4c0), X(0x4c0), X(0x4c0), X(0x4c0), X(0x4c0), X(0x4c0), X(0x4c0),
    X(0x4c0), X(0x8d8), X(0x8d8), X(0x8d8), X(0x8d8), X(0x8d8), X(0x8d8),
    X(0x8d8), X(0x8d8), X(0x8d8), X(0x8d8), X(0x4c0), X(0x4c0), X(0x4c0),
    X(0x4c0), X(0x4c0), X(0x4c0), X(0x4c0), X(0x8d5), X(0x8d5), X(0x8d5),
    X(0x8d5), X(0x8d5), X(0x8d5), X(0x8c5), X(0x8c5), X(0x8c5), X(0x8c5),
    X(0x8c5), X(0x8c5), X(0x8c5), X(0x8c5), X(0x8c5), X(0x8c5), X(0x8c5),
    X(0x8c5), X(0x8c5), X(0x8c5), X(0x8c5), X(0x8c5), X(0x8c5), X(0x8c5),
    X(0x8c5), X(0x8c5), X(0x4c0), X(0x4c0), X(0x4c0), X(0x4c0), X(0x4c0),
    X(0x4c0), X(0x8d6), X(0x8d6), X(0x8d6), X(0x8d6), X(0x8d6), X(0x8d6),
    X(0x8c6), X(0x8c6), X(0x8c6), X(0x8c6), X(0x8c6), X(0x8c6), X(0x8c6),
    X(0x8c6), X(0x8c6), X(0x8c6), X(0x8c6), X(0x8c6), X(0x8c6), X(0x8c6),
    X(0x8c6), X(0x8c6), X(0x8c6), X(0x8c6), X(0x8c6), X(0x8c6), X(0x4c0),
    X(0x4c0), X(0x4c0), X(0x4c0), X(0x200), 0,        0,        0,
    0,        0,        0,        0,        0,        0,        0,
    0,        0,        0,        0,        0,        0,        0,
    0,        0,        0,        0,        0,        0,        0,
    0,        0,        0,        0,        0,        0,        0,
    0,        0,        0,        0,        0,        0,        0,
    0,        0,        0,        0,        0,        0,        0,
    0,        0,        0,        0,        0,        0,        0,
    0,        0,        0,        0,        0,        0,        0,
    0,        0,        0,        0,        0,        0,        0,
    0,        0,        0,        0,        0,        0,        0,
    0,        0,        0,        0,        0,        0,        0,
    0,        0,        0,        0,        0,        0,        0,
    0,        0,        0,        0,        0,        0,        0,
    0,        0,        0,        0,        0,        0,        0,
    0,        0,        0,        0,        0,        0,        0,
    0,        0,        0,        0,        0,        0,        0,
    0,        0,        0,        0,        0,        0,        0,
    0,        0,        0,        0,        0,        0,
};

#undef X

static const unsigned short *const ptable = table + 128;

const unsigned short **__ctype_b_loc(void) { return (void *)&ptable; }
