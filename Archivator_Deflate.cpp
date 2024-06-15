#include "archivator_deflate.h" 
#include <string.h> /* memcpy */ 

static const unsigned char deflate_mirror[256] = {
    #define R2(n) n, n + 128, n + 64, n + 192 
    #define R4(n) R2(n), R2(n + 32), R2(n + 16), R2(n + 48) 
    #define R6(n) R4(n), R4(n +  8), R4(n +  4), R4(n + 12) 
    R6(0), R6(2), R6(1), R6(3),
};

static int deflate_npow2(int n) {
    n--;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    return (int)++n;
}

static int deflate_ilog2(int n) {
#define lt(n) n,n,n,n, n,n,n,n, n,n,n,n ,n,n,n,n 
    static const char tbl[256] = { -1,0,1,1,2,2,2,2,3,3,3,3,
        3,3,3,3,lt(4),lt(5),lt(5),lt(6),lt(6),lt(6),lt(6),
        lt(7),lt(7),lt(7),lt(7),lt(7),lt(7),lt(7),lt(7)
    }; int tt, t;

    if ((tt = (n >> 16))) return (t = (tt >> 8)) ? 24 + tbl[t] : 16 + tbl[tt];
    else return (t = (n >> 8)) ? 8 + tbl[t] : tbl[n];
#undef lt 
}

static unsigned deflate_uload32(const void* p) {
    /* hopefully will be optimized to an unaligned read */
    unsigned int n = 0;
    memcpy(&n, p, sizeof(n));
    return n;
}

static unsigned deflate_hash32(const void* p) {
    unsigned n = deflate_uload32(p);
    return (n * 0x9E377989) >> (32 - DEFLATE_HASH_BITS);
}

static unsigned char* deflate_put(unsigned char* dest, deflate* s, int code, int bitcnt) {
    s->bits |= (code << s->cnt);
    s->cnt += bitcnt;

    while (s->cnt >= 8) {
        *dest++ = (unsigned char)(s->bits & 0xFF);
        s->bits >>= 8;
        s->cnt -= 8;
    } return dest;
}

static unsigned char* deflate_match(unsigned char* dest, deflate* s, int dist, int len) {
    static const short lxmin[] = { 0,11,19,35,67,131 };
    static const short dxmax[] = { 0,6,12,24,48,96,192,384,768,1536,3072,6144,12288,24576 };
    static const short lmin[] = { 11,13,15,17,19,23,27,31,35,43,51,59,67,83,99,115,131,163,195,227 };
    static const short dmin[] = { 1,2,3,4,5,7,9,13,17,25,33,49,65,97,129,193,257,
                385,513,769,1025,1537,2049,3073,4097,6145,8193,12289,16385,24577 };

    /* length encoding */
    int lc = len,
        lx = deflate_ilog2(len - 3) - 2;

    if (!(lx = (lx < 0) ? 0 : lx)) lc += 254;
    else if (len >= 258) lx = 0, lc = 285;
    else lc = ((lx - 1) << 2) + 265 + ((len - lxmin[lx]) >> lx);

    if (lc <= 279) dest = deflate_put(dest, s, deflate_mirror[(lc - 256) << 1], 7);
    else dest = deflate_put(dest, s, deflate_mirror[0xc0 - 280 + lc], 8);

    if (lx) dest = deflate_put(dest, s, len - lmin[lc - 265], lx);

    /* distance encoding */
    {
        int dc = dist - 1;
        int dx = deflate_ilog2(deflate_npow2(dist) >> 2);
        if ((dx = (dx < 0) ? 0 : dx))
            dc = ((dx + 1) << 1) + (dist > dxmax[dx]);
        dest = deflate_put(dest, s, deflate_mirror[dc << 3], 5);
        if (dx) dest = deflate_put(dest, s, dist - dmin[dc], dx);
    }
    return dest;
}

static unsigned char* deflate_lit(unsigned char* dest, deflate* s, int c) {
    if (c <= 143) return deflate_put(dest, s, deflate_mirror[0x30 + c], 8);
    else return deflate_put(dest, s, 1 + 2 * deflate_mirror[0x90 - 144 + c], 9);
}

extern int archivator_deflate(deflate* s, unsigned char* out, const unsigned char* in, int in_len, int lvl) {
    int p = 0,
        max_chain = (lvl < 8) ? (1 << (lvl + 1)) : (1 << 13);
    unsigned char* q = out;

    s->bits = s->cnt = 0;
    for (p = 0; p < DEFLATE_HASH_SIZ; ++p) s->tbl[p] = DEFLATE_NIL;

    p = 0;
    q = deflate_put(q, s, 0x01, 1); /* block */
    q = deflate_put(q, s, 0x01, 2); /* static huffman */

    while (p < in_len) {
        int run, best_len = 0, dist = 0,
            max_match = ((in_len - p) > DEFLATE_MAX_MATCH) ? DEFLATE_MAX_MATCH : (in_len - p);

        if (max_match > DEFLATE_MIN_MATCH) {
            int limit = ((p - DEFLATE_WIN_SIZ) < DEFLATE_NIL) ? DEFLATE_NIL : (p - DEFLATE_WIN_SIZ),
                chain_len = max_chain,
                i = s->tbl[deflate_hash32(&in[p])];

            while (i > limit) {
                if (in[i + best_len] == in[p + best_len] && (deflate_uload32(&in[i]) == deflate_uload32(&in[p]))) {
                    int n = DEFLATE_MIN_MATCH;
                    while (n < max_match && in[i + n] == in[p + n]) n++;

                    if (n > best_len) {
                        best_len = n;
                        dist = p - i;
                        if (n == max_match) break;
                    }
                }
                if (!(--chain_len)) break;
                i = s->prv[i & DEFLATE_WIN_MSK];
            }
        }

        if (lvl >= 5 && best_len >= DEFLATE_MIN_MATCH && best_len < max_match) {
            const int x = p + 1;
            int tar_len = best_len + 1,
                limit = ((x - DEFLATE_WIN_SIZ) < DEFLATE_NIL) ? DEFLATE_NIL : (x - DEFLATE_WIN_SIZ),
                chain_len = max_chain,
                i = s->tbl[deflate_hash32(&in[p])];

            while (i > limit) {

                if (in[i + best_len] == in[x + best_len] && (deflate_uload32(&in[i]) == deflate_uload32(&in[x]))) {
                    int n = DEFLATE_MIN_MATCH;

                    while (n < tar_len && in[i + n] == in[x + n]) n++;

                    if (n == tar_len) {
                        best_len = 0;
                        break;
                    }
                }
                if (!(--chain_len)) break;
                i = s->prv[i & DEFLATE_WIN_MSK];
            }
        }

        if (best_len >= DEFLATE_MIN_MATCH) {
            q = deflate_match(q, s, dist, best_len);
            run = best_len;
        }
        else {
            q = deflate_lit(q, s, in[p]);
            run = 1;
        }

        while (run-- != 0) {
            unsigned h = deflate_hash32(&in[p]);
            s->prv[p & DEFLATE_WIN_MSK] = s->tbl[h];
            s->tbl[h] = p++;
        }
    }

    /* zlib partial flush */
    q = deflate_put(q, s, 0, 7);
    q = deflate_put(q, s, 2, 10);
    q = deflate_put(q, s, 2, 3);

    return (int)(q - out);
}

static int deflate_bound(int len) {
    int a = 128 + (len * 110) / 100;
    int b = 128 + len + ((len / (31 * 1024)) + 1) * 5;
    return (a > b) ? a : b;
}