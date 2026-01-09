#include <memtest.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <yaul.h>

int memtest_test_lines(void * address, int size)
{
	// non-destructive quick address and data lines test
	// implementation from https://www.ganssle.com/item/how-to-test-ram.htm 
	// by Jack Ganssle 
    int errors = 0; 
    unsigned int *address32 = (unsigned int *)address;
	unsigned int save_lo;
	unsigned int save_hi;
	unsigned int data = 1;
	unsigned int offset;
  
    save_lo = *address32; 

	for (offset=1; offset<(size/4); offset=offset<<1) { 
		save_hi = *(address32+offset);
		*address32 = data;
		*(address32+offset) =  ~data;
		if (*address32 !=  data) 
			errors++;
		if (*(address32+offset) != ~data)
			errors++;
		*(address32+offset) = save_hi;
		data = (data<<1) | (data >> (8*sizeof(int)-1));
	}

	*address32 = save_lo;
	return errors;
}

static inline uint32_t mem_read32(volatile uint32_t *p) { return *p; }
static inline void     mem_write32(volatile uint32_t *p, uint32_t v) { *p = v; }

static int march_c_plus_with_patterns(volatile uint32_t *base, size_t words,
                                      uint32_t p0, uint32_t p1) {
    int errors = 0;

    /* 1) ⇑ (w p0) */
    for (size_t i = 0; i < words; i++) mem_write32(&base[i], p0);

    /* 2) ⇑ (r p0, w p1) */
    for (size_t i = 0; i < words; i++) {
        uint32_t v = mem_read32(&base[i]);
        if (v != p0) errors++;
        mem_write32(&base[i], p1);
    }

    /* 3) ⇑ (r p1, w p0) */
    for (size_t i = 0; i < words; i++) {
        uint32_t v = mem_read32(&base[i]);
        if (v != p1) errors++;
        mem_write32(&base[i], p0);
    }

    /* 4) ⇓ (r p0, w p1) */
    for (size_t i = words; i-- > 0;) {
        uint32_t v = mem_read32(&base[i]);
        if (v != p0) errors++;
        mem_write32(&base[i], p1);
    }

    /* 5) ⇓ (r p1, w p0) */
    for (size_t i = words; i-- > 0;) {
        uint32_t v = mem_read32(&base[i]);
        if (v != p1) errors++;
        mem_write32(&base[i], p0);
    }

    /* 6) ⇑ (r p0) */
    for (size_t i = 0; i < words; i++) {
        uint32_t v = mem_read32(&base[i]);
        if (v != p0) errors++;
    }

    return errors;
}


int memtest_test_area(void * address, int size)
{
	// destructive memory surface test, with backup
	// using LWRAM backup if area is not LWRAM. for LWRAM test using destructive mode
    int errors = 0; 
	unsigned int words = size/4;

	if ( (((int)address)&0xFFF00000) != 0x20200000 ) {
		//not LWRAM, doing backup
		memcpy(LWRAM(0),address,size);
	}
  
    /* For each bit position, run March C+ with patterns that isolate that bit. */
    for (uint32_t b = 0; b < 32; b++) {
        uint32_t w1 = (uint32_t)1u << b;      /* walking 1 */
        uint32_t w0 = ~w1;                    /* walking 0 (all 1s except bit b) */

        /* Test the bit driven high against the inverse pattern. */
        errors += march_c_plus_with_patterns((volatile uint32_t *)address, words, 0u, w1);
        /* Test the bit driven low against the inverse pattern. */
        errors += march_c_plus_with_patterns((volatile uint32_t *)address, words, w0, 0u);

        /* Also run directly between walking-0 and walking-1 to strengthen coupling coverage. */
        errors += march_c_plus_with_patterns((volatile uint32_t *)address, words, w0, w1);
    }

	if ( (((int)address)&0xFFF00000) != 0x20200000 ) {
		//not LWRAM, doing restore
		memcpy(address,LWRAM(0),size);
	}

	return errors;
}
