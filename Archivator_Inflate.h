//#pragma once?
#ifndef INFLATE_H_INCLUDED 
#define INFLATE_H_INCLUDED 

typedef struct {
    int bits, bitcnt;
    unsigned lits[288];
    unsigned dsts[32];
    unsigned lens[19];
    int tlit, tdist, tlen;
} inflate;

#endif /* INFLATE_H_INCLUDED */ 