#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "bitvec.h"

int main (int argc, const char * argv[])
{
    bitvec_t bv;
    bitvec_t *ptr = &bv;
    
    // init 128 bit
    if( bitvec_init( ptr, 128 ) == -1 ){
        perror( "failed to bitvec_alloc()" );
        return -1;
    }
    
    // returns 0
    printf( "3: %d\n", bitvec_get( ptr, 3 ) );
    
    // set bit
    bitvec_set( ptr, 101 );
    bitvec_set( ptr, 3 );
    // returns 1
    printf( "101: %d\n", bitvec_get( ptr, 101 ) );
    printf( "3: %d\n", bitvec_get( ptr, 3 ) );
    
    // unset bit
    bitvec_unset( ptr, 3 );
    // returns 0
    printf( "3: %d\n", bitvec_get( ptr, 3 ) );
    
    // cannot set a bit greater than 128 bit
    assert( bitvec_set( ptr, 129 ) == -1 );
    // returns -1
    printf( "129: %d\n", bitvec_get( ptr, 129 ) );
    
    // resize
    bitvec_resize( ptr, 140 );
    assert( bitvec_set( ptr, 129 ) == 0 );
    // returns 1
    printf( "129: %d\n", bitvec_get( ptr, 129 ) );
    
    // resize
    bitvec_resize( ptr, 107 );
    // return -1
    printf( "129: %d\n", bitvec_get( ptr, 129 ) );
    // returns 1
    printf( "101: %d\n", bitvec_get( ptr, 101 ) );
    
    // dispose
    bitvec_dispose( ptr );
    
    return 0;
}
