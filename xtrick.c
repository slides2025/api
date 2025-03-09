#include "xtrick.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
uint64_t mash(const char* stuff, size_t howmuch) {
    uint64_t h = 0xfeedface;
    for(size_t j=0; j<howmuch; j++)
        h ^= (h<<3) + (h>>1) + stuff[j];
    return h;
}
void initTrick(trickster* t, const char* p) {
    time_t tick = time(0);
    t->timey = mash((char*)&tick, sizeof tick);
    uint64_t s = mash(p, strlen(p)) ^ t->timey;
    for(int i=0; i<(KSIZE>>3); i++) {
        s = s * 0x41c64e6d + 0x6073;
        t->mkey[i] = s & 0xff;
    }
    genSubKeys(t);
    chaosTwist(t->onepad, BLKSIZE/8, s);
}
void genSubKeys(trickster* t) {
    int r;
    for(r=0; r<RNDS; r++) {
        memcpy(t->skeys[r], t->mkey + r*(SUBK/8), SUBK>>3);
        chaosTwist(t->skeys[r], SUBK/8, t->timey+r);
    }
}
void flipBits(uint8_t* d, const uint8_t* s1, const uint8_t* s2, size_t len) {
    while(len--) *d++ = *s1++ ^ *s2++;
}
void chaosTwist(uint8_t* buf, size_t len, uint64_t seed) {
    double x = (seed & 0xffff) / 65535.0;
    double r = 3.998;
    size_t i = 0;
    do {
        x = r * x * (1-x);
        buf[i] ^= (uint8_t)(x*255);
    } while(++i < len);
}
void encX(trickster* t, const uint8_t* plain, uint8_t* cipher, size_t sz) {
    if(sz > BLKSIZE/8) sz = BLKSIZE/8;
    uint8_t tmp[BLKSIZE/8];
    memcpy(tmp, plain, sz);
    for(int k=0; k<RNDS; k++) {
        flipBits(tmp, tmp, t->skeys[k], sz);
        chaosTwist(tmp, sz, t->timey+k);
    }
    flipBits(cipher, tmp, t->onepad, sz);
}
void decX(trickster* t, const uint8_t* cipher, uint8_t* plain, size_t sz) {
    if(sz > BLKSIZE/8) sz = BLKSIZE/8;
    uint8_t tmp[BLKSIZE/8];
    flipBits(tmp, cipher, t->onepad, sz);
    int k = RNDS-1;
    while(k >= 0) {
        chaosTwist(tmp, sz, t->timey+k);
        flipBits(tmp, tmp, t->skeys[k], sz);
        k--;
    }
    memcpy(plain, tmp, sz);
}
int main() {
    trickster t;
    initTrick(&t, "1337");
    uint8_t p[] = "slides-private-key1337";
    uint8_t c[BLKSIZE/8] = {0};
    encX(&t, p, c, strlen((char*)p));
    for(size_t i=0; i<strlen((char*)p); i++)
        printf("%02x ", c[i]);
    printf("\n");
    return 0;
}
