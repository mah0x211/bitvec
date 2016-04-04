/*
 *  Copyright 2013-2015 Masatoshi Teruya. All rights reserved.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to 
 *  deal in the Software without restriction, including without limitation the 
 *  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or 
 *  sell copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL 
 *  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 *  IN THE SOFTWARE.
 *
 *  bitvec.h
 *  Created by Masatoshi Teruya on 13/12/25.
 *
 */

#ifndef ___BITVEC_H___
#define ___BITVEC_H___

#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <stdint.h>
#include <string.h>

#define BV_TYPE     uint64_t
#define BV_TYPEMAX ((BV_TYPE)-1)

static const uint8_t BV_BIT = CHAR_BIT * sizeof( BV_TYPE );


typedef struct {
    size_t nbit;
    size_t nvec;
    BV_TYPE *vec;
} bitvec_t;


#define BIT2VEC_SIZE(nbit) \
    (nbit < BV_BIT ? 1 :( nbit / BV_BIT ) + !!( nbit % BV_BIT ))

static inline int bitvec_init( bitvec_t *bv, size_t nbit )
{
    bv->nbit = nbit;
    bv->nvec = BIT2VEC_SIZE( nbit );
    if( ( bv->vec = calloc( bv->nvec, sizeof( BV_TYPE ) ) ) ){
        return 0;
    }
    bv->vec = NULL;

    return -1;
}

static inline int bitvec_resize( bitvec_t *bv, size_t nbit )
{
    if( nbit == bv->nbit ){
        return 0;
    }
    else
    {
        size_t nvec = BIT2VEC_SIZE( nbit );
        BV_TYPE *vec = bv->vec;

        // mem error
        if( nvec != bv->nvec && 
            !( vec = realloc( vec, sizeof( BV_TYPE ) * nvec ) ) ){
            return -1;
        }

        // clear allocated bits
        if( nvec > bv->nvec ){
            memset( vec + bv->nvec, 0, ( nvec - bv->nvec ) * sizeof( BV_TYPE ) );
        }
        // clear unused bits
        else {
            vec[nvec - 1] &= ( ~((BV_TYPE)0) >> ( BV_BIT * nvec - nbit - 1 ) );
        }

        bv->vec = vec;
        bv->nvec = nvec;
        bv->nbit = nbit;

        return 0;
    }
}

#undef BIT2VEC_SIZE


static inline void bitvec_dispose( bitvec_t *bv )
{
    if( bv->vec ){
        free( bv->vec );
        memset( (void*)bv, 0, sizeof( bitvec_t ) );
    }
}



/*
 *  Bit Twiddling Hacks By Sean Eron Anderson
 *  Count the consecutive zero bits on the right with multiply and lookup
 *  http://graphics.stanford.edu/~seander/bithacks.html#ZerosOnRightMultLookup
 *
 *  m-sequence(maximal length sequence)
 *
 *  8 bit
 *    0x1D
 *
 *  16 bit
 *    0x0F2D
 *
 *  32 bit
 *    0x077CB531
 *
 *  64 bit
 *    0x0218A7A392DD9ABF
 *    0x02FCA8CF75A6C487
 *    0x03F566ED27179461
 *    0x03C953422DFAE33B
 *    0x03848D96BBCC54FD
 *    0x03731D7ED10B2A4F
 *
 *
 *  mapping table calculation (64 bit)
 *
 *    uint8_t tbl[64] = {0};
 *    uint64_t m = BIT_MLS;
 *    uint8_t i = 0;
 *
 *    for(; i < 64; i++ ){
 *    tbl[m >> 58] = i;
 *        m <<= 1;
 *    }
 *
 */

#define BV_MLS8     UINT8_C(0x1D)
#define BV_MLS16    UINT16_C(0x0F2D)

#define BV_MLS32    UINT32_C(0x077CB531)

static const uint8_t BV_NTZ32TBL[] = {
    0, 1, 28, 2, 29, 14, 24, 3, 30, 22, 20, 15, 25, 17, 4, 8, 31, 27, 13, 23,
    21, 19, 16, 7, 26, 12, 18, 6, 11, 5, 10, 9
};


#define BV_MLS64    UINT64_C(0x03F566ED27179461)

static const uint8_t BV_NTZ64TBL[64] = {
    0, 1, 59, 2, 60, 40, 54, 3, 61, 32, 49, 41, 55, 19, 35, 4, 62, 52, 30, 33,
    50, 12, 14, 42, 56, 16, 27, 20, 36, 23, 44, 5, 63, 58, 39, 53, 31, 48, 18,
    34, 51, 29, 11, 13, 15, 26, 22, 43, 57, 38, 47, 17, 28, 10, 25, 21, 37, 46, 
    9, 24, 45, 8, 7, 6
};


#define BV_NTZ(v) \
    BV_NTZ64TBL[(((v) & -(v)) * BV_MLS64) >> 58]


static inline size_t bitvec_ntz( bitvec_t *bv )
{
    BV_TYPE *vec = bv->vec;
    size_t idx = 0;

    for(; idx < bv->nvec; idx++ )
    {
        if( vec[idx] ){
            return ( BV_BIT * idx ) + BV_NTZ( vec[idx] );
        }
    }

    return bv->nbit;
}


static inline size_t bitvec_ffz( bitvec_t *bv )
{
    BV_TYPE *vec = bv->vec;
    size_t idx = 0;

    if( !*vec ){
        return 0;
    }

    for(; idx < bv->nvec; idx++ )
    {
        if( vec[idx] != BV_TYPEMAX )
        {
            idx = ( BV_BIT * idx ) + BV_NTZ( ~vec[idx] );
            if( idx > bv->nbit ){
                break;
            }
            return idx;
        }
    }

    return SIZE_MAX;
}


static inline int bitvec_get( bitvec_t *bv, BV_TYPE pos )
{
    if( bv->vec && pos <= bv->nbit ){
        return (int)((bv->vec[pos/BV_BIT] >> (pos % BV_BIT)) & (BV_TYPE)1);
    }

    return -1;
}


static inline int bitvec_set( bitvec_t *bv, BV_TYPE pos )
{
    if( bv->vec && pos <= bv->nbit ){
        bv->vec[pos / BV_BIT] |= (BV_TYPE)1 << ( pos % BV_BIT );
        return 0;
    }

    return -1;
}


static inline int bitvec_unset( bitvec_t *bv, BV_TYPE pos )
{
    if( bv->vec && pos <= bv->nbit ){
        bv->vec[pos / BV_BIT] &= ~( (BV_TYPE)1 << ( pos % BV_BIT ) );
        return 0;
    }

    return -1;
}


static inline int bitvec_set_range( bitvec_t *bv, BV_TYPE from, BV_TYPE to )
{
    if( bv->vec && to <= bv->nbit )
    {
        BV_TYPE *vec = bv->vec;
        size_t start = from / BV_BIT;
        size_t end = to / BV_BIT;

        if( start == end ){
            vec[start] |= (~( (~(BV_TYPE)1) << ( to - from ) ) << from);
        }
        else {
            size_t pos = start + 1;

            for(; pos < end; pos++ ){
                vec[pos] |= ~(BV_TYPE)0;
            }
            vec[start] |= (~(BV_TYPE)0) << ( from % BV_BIT );
            vec[end] |= ~((~(BV_TYPE)1) << ( to % BV_BIT ));
        }

        return 0;
    }

    return -1;
}


static inline int bitvec_unset_range( bitvec_t *bv, BV_TYPE from, BV_TYPE to )
{
    if( bv->vec && to <= bv->nbit )
    {
        BV_TYPE *vec = bv->vec;
        size_t start = from / BV_BIT;
        size_t end = to / BV_BIT;

        if( start == end ){
            vec[start] &= ~(~( (~(BV_TYPE)1) << ( to - from ) ) << from);
        }
        else {
            size_t pos = start + 1;

            for(; pos < end; pos++ ){
                vec[pos] &= (BV_TYPE)0;
            }
            vec[start] &= ~((~(BV_TYPE)0) << ( from % BV_BIT ));
            vec[end] &= ~(BV_TYPE)1 << ( to % BV_BIT );
        }

        return 0;
    }

    return -1;
}


#endif

