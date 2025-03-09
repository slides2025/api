#ifndef XTRICK_H
#define XTRICK_H
#include <stdint.h>
#include <stddef.h>
#include <time.h>
#define KSIZE 16384
#define SUBK 1024
#define RNDS 16
#define BLKSIZE 16384
typedef struct trickster {
    uint8_t mkey[KSIZE >> 3];
    uint8_t skeys[RNDS][SUBK / 8];
    uint64_t timey;
    uint8_t onepad[BLKSIZE / 8];
} trickster;
void initTrick(trickster* t, const char* p);
void genSubKeys(trickster* t);
void encX(trickster* t, const uint8_t* plain, uint8_t* cipher, size_t sz);
void decX(trickster* t, const uint8_t* cipher, uint8_t* plain, size_t sz);
void flipBits(uint8_t* d, const uint8_t* s1, const uint8_t* s2, size_t len);
void chaosTwist(uint8_t* buf, size_t len, uint64_t seed);
#endif
