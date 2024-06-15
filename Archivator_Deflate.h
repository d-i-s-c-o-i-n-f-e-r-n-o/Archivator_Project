//#pragma once?
#ifndef DEFLATE_H_INCLUDED 
#define DEFLATE_H_INCLUDED 

#define DEFLATE_MAX_OFF       (1 << 15) 
#define DEFLATE_WIN_SIZ       DEFLATE_MAX_OFF 
#define DEFLATE_WIN_MSK       (DEFLATE_WIN_SIZ-1) 


#define DEFLATE_MIN_MATCH     4 
#define DEFLATE_MAX_MATCH     258 


#define DEFLATE_HASH_BITS     19 
#define DEFLATE_HASH_SIZ      (1 << DEFLATE_HASH_BITS) 
#define DEFLATE_HASH_MSK      (DEFLATE_HASH_SIZ-1) 
#define DEFLATE_NIL           (-1) 


#define DEFLATE_LVL_MIN       0 
#define DEFLATE_LVL_DEF       5 
#define DEFLATE_LVL_MAX       8 


typedef struct {
    int bits, cnt;
    int tbl[DEFLATE_HASH_SIZ];
    int prv[DEFLATE_WIN_SIZ];
} deflate;

#endif /* DEFLATE_H_INCLUDED */