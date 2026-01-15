#include <memtest.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <yaul.h>

int memtest_test_lines_relocatable(void * address, unsigned int size)
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

int memtest_test_lines(void * address, unsigned int size)
{
	//disable all interrupts
	const uint8_t sr_mask = cpu_intc_mask_get();
    cpu_intc_mask_set(15);

	int errors = memtest_test_lines_relocatable(address, size);

	//restore interrupts
    cpu_intc_mask_set(sr_mask);

	return errors;
}

static inline uint32_t __attribute__((always_inline))  mem_read32(volatile uint32_t *p) { return *p; }
static inline void     __attribute__((always_inline))  mem_write32(volatile uint32_t *p, uint32_t v) { *p = v; }

static inline int __attribute__((always_inline))  march_c_plus_with_patterns(volatile uint32_t *base, size_t words,
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

int memtest_test_area_relocatable(void * address, unsigned int size)
{
	int errors = 0; 
	unsigned int words = size/4;
	uint32_t * dst, * src;

	if ( (((int)address)&0x0FF00000) != 0x00200000 ) {
		//not LWRAM, doing backup without memcpy, relocatable way
		dst = (uint32_t *)LWRAM(0);
		src = (uint32_t *)address;
		for (unsigned int i = 0; i < words; i++)
			dst[i] = src[i];
	}

	/* For each bit position, run March C+ with patterns that isolate that bit. */
	uint32_t w1 = (uint32_t)1u;
    for (uint32_t b = 0; b < 32; b++) {
        uint32_t w0 = ~w1;                    /* walking 0 (all 1s except bit b) */

        /* Test the bit driven high against the inverse pattern. */
        errors += march_c_plus_with_patterns((volatile uint32_t *)address, words, 0u, w1);
        /* Test the bit driven low against the inverse pattern. */
        errors += march_c_plus_with_patterns((volatile uint32_t *)address, words, w0, 0u);

        /* Also run directly between walking-0 and walking-1 to strengthen coupling coverage. */
        errors += march_c_plus_with_patterns((volatile uint32_t *)address, words, w0, w1);

		w1 = w1 << 1; /* walking 1 */
    }

	//restoring here, because non-relocatable part might be destroyed
	if ( (((int)address)&0x0FF00000) != 0x00200000 ) {
		//not LWRAM, doing restore without memcpy, relocatable way
		dst = (uint32_t *)address;
		src = (uint32_t *)LWRAM(0);
		for (unsigned int i = 0; i < words; i++)
			dst[i] = src[i];
	}

	return errors;
}


int memtest_test_area(void * address, unsigned int size)
{
	// semi-destructive memory surface test, with backup
	// using LWRAM backup if area is not LWRAM. for LWRAM test using destructive mode
    int errors = 0; 
	int (*relocated_func)(void *, unsigned int) = LWRAM(0x10000);
	uint32_t * dst, * src;
	int requested_buffer;
	vdp1_transfer_status_t transfer_status;

	if ( (((int)address)&0x0FF80000) == 0x05C80000 ) {
		requested_buffer = (((int)address)&0x00040000) ? 1 : 0;
		//for VDP1 FB, waiting for hsync first
		//vdp2_tvmd_vblank_out_wait();
		//if the buffer is wrong wait until it's rigth
		volatile uint16_t *p16 = (uint16_t*)(VDP1_FB(0x3FFFE));
		if (0 == requested_buffer) {
			while ( p16[0] == 0xAA55 );
			while ( p16[0] != 0xAA55 );
			//waiting for plot end
			/*transfer_status = vdp1_transfer_status_get();
			while (transfer_status.cef == 0)
				transfer_status = vdp1_transfer_status_get();*/
		} else {
			return 0;
			while ( p16[0] != 0xAA55 );
			while ( p16[0] == 0xAA55 );
			address = (void*)( (unsigned int)address & 0xFFFBFFFF); //remapping upper address to second framebuffer
			//waiting for plot end
			/*transfer_status = vdp1_transfer_status_get();
			while (transfer_status.cef == 0)
				transfer_status = vdp1_transfer_status_get();*/
		}	
	}

	//disable all interrupts
	uint8_t sr_mask = cpu_intc_mask_get();
    cpu_intc_mask_set(15);

	if ( (((int)address)&0x0FF00000) == 0x06000000 ) {
		//testing HWRAM, cannot use the normal func, need to relocate it to LWRAM and run from there
		dst = (uint32_t *)LWRAM(0x10000);
		src = (uint32_t *)memtest_test_area_relocatable;
		for (unsigned int i = 0; i < 200; i++)
			dst[i] = src[i];
		errors = relocated_func(address,size);
	}
	else
		errors = memtest_test_area_relocatable(address,size);

	//restore interrupts
	cpu_intc_mask_set(sr_mask);

	return errors;
}
