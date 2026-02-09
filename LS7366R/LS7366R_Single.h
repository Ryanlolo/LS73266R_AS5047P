// Copyright 2014 Werdroid (modified for single chip)
// Author Vladimir Kosmala
// Modified for single LS7366R chip

#ifndef LS7366R_SINGLE_H
#define LS7366R_SINGLE_H

#define CLR B00000000
#define RD B01000000
#define WR B10000000
#define LOAD B11000000

#define MDR0 B00001000
#define MDR1 B00010000
#define DTR B00011000
#define CNTR B00100000
#define OTR B00101000
#define STR B00110000

// filter factor 1
// async index
// no index
// free-running
// 4x quadrature
#define MDR0_CONF B00000011

// no flag
// enabled
// 32 bits
#define MDR1_CONF B00000000

class LS7366R_Single {
	public:
		unsigned char csPin;
		long countValue;
		
		/** Creates LS7366R_Single object for one chip
		 *
		 *  @param csPin      CS (Chip Select) pin
		 *  @param mdr0_conf  MDR0 register configuration
		 *  @param mdr1_conf  MDR1 register configuration
		 */
		LS7366R_Single(unsigned char csPin, unsigned char mdr0_conf, unsigned char mdr1_conf);
		
		/** Reset the counter to zero */
		void reset();
		
		/** Synchronize and read the counter value from chip */
		void sync();
		
		/** Get the counter value (from cache, call sync() first)
		 *
		 *  @return     Counter value (32-bit signed long)
		 */
		long getCount();
};

#endif
