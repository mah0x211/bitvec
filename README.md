# bitvec.h

bit-vector header library.

---

## Data Types

### Data type of vector

- **#define BV_TYPE uint64_t**
  you can be changed to the following types;
  `uint8_t`, `uint16_t` or `uint32_t`


### Structure

**typedef struct bitvec_t**

- `size_t nbit`: number of bits.
- `size_t nvec`: size of vector.
- `BV_TYPE *vec`: vector.


## Managing the bitvec_t data structure.

### int bitvec_init( bitvec_t *bv, size_t nbit )

this function initialize `bv` member fields and allocate a `nbit` size memory.

### int bitvec_resize( bitvec_t *bv, size_t nbit )

this function tries to change the bit size of `bv`.

**Parameters**

- `*bv`: non-Null bitvec_t pointer.
- `nbit`: number of bits.

**Return Values**

returns `0` on success, or `-1` on failure.

**Errors**

The function may fail and set the external variable errno for any of the errors specified for the library functions `calloc(3)` and `realloc(3)`.


### void bitvec_dispose( bitvec_t *bv )

deallocates the memory allocation of data inside of `bv`.

**Parameters**

- `*bv`: non-Null bitvec_t pointer.


## Getting a bit value

### int bitvec_get( bitvec_t *bv, BV_TYPE pos )

returns bit value `0` or `1` at `pos` position, or `-1` if a `pos` is out of range.

**Parameters**

- `*bv`: non-Null bitvec_t pointer.
- `BV_TYPE pos`: position of bit.


## Setting a bit value

### int bitvec_set( bitvec_t *bv, BV_TYPE pos )

returns `0` on success, or `-1` if a `pos` is out of range.

### int bitvec_unset( bitvec_t *bv, BV_TYPE pos )

returns `0` on success, or `-1` if a `pos` is out of range.

**Parameters**

- `*bv`: non-Null bitvec_t pointer.
- `BV_TYPE pos`: position of bit.


## Setting the range of bits

### int bitvec_set_range( bitvec_t *bv, BV_TYPE from, BV_TYPE to )

returns `0` on success, or `-1` if a `pos` is out of range.

### int bitvec_unset_range( bitvec_t *bv, BV_TYPE from, BV_TYPE to )

returns `0` on success, or `-1` if a `pos` is out of range.

**Parameters**

- `*bv`: non-Null bitvec_t pointer.
- `BV_TYPE from`: the start position of bit.
- `BV_TYPE to`: the end position of bit.


## Getting a number of trailing zeros

### size_t bitvec_ntz( bitvec_t *bv )

returns a number of trailing zeros.

**Parameters**

- `*bv`: non-Null bitvec_t pointer.


## Find first zero bit

### size_t bitvec_ffz( bitvec_t *bv )

returns a position of first zero bit. if not found, returns a SIZE_MAX.


**Parameters**

- `*bv`: non-Null bitvec_t pointer.


## Usage

```c
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

    // set range
    assert( bitvec_set_range( ptr, 62, 90 ) == 0 );
    for( size_t i = 62; i <= 90; i++ ){
        assert( bitvec_get( ptr, i ) == 1 );
    }
    // unset range
    assert( bitvec_unset_range( ptr, 62, 90 ) == 0 );
    for( size_t i = 62; i <= 90; i++ ){
        assert( bitvec_get( ptr, i ) == 0 );
    }

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
```


## WARNING

the following API's currently must be used for 64 bit vector.

- `int bitvec_ntz( bitvec_t *bv )`
- `int bitvec_ffz( bitvec_t *bv )`

