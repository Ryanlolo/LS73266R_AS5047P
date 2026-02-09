// Copyright 2014 Werdroid (modified for single chip)
// Author Vladimir Kosmala
// Modified for single LS7366R chip

#include "LS7366R_Single.h"

#include <SPI.h>
#include <Arduino.h>

// Constructors ////////////////////////////////////////////////////////////////

LS7366R_Single::LS7366R_Single(unsigned char _csPin, unsigned char mdr0_conf, unsigned char mdr1_conf)
{
	csPin = _csPin;

	pinMode(csPin, OUTPUT);
	SPI.begin();

	// Configure MDR0 register
	digitalWrite(csPin, LOW);
	SPI.transfer(WR | MDR0);
	SPI.transfer(mdr0_conf);
	digitalWrite(csPin, HIGH);
	
	// Configure MDR1 register
	digitalWrite(csPin, LOW);
	SPI.transfer(WR | MDR1);
	SPI.transfer(mdr1_conf);
	digitalWrite(csPin, HIGH);

	reset();
}

// Public Methods //////////////////////////////////////////////////////////////

void LS7366R_Single::reset()
{
	digitalWrite(csPin, LOW);
	SPI.transfer(CLR | CNTR);
	digitalWrite(csPin, HIGH);
}

void LS7366R_Single::sync()
{
	long count;

	// Load counter value to output register
	digitalWrite(csPin, LOW);
	SPI.transfer(LOAD | OTR);
	digitalWrite(csPin, HIGH);

	// Read 32-bit counter value
	digitalWrite(csPin, LOW);
	SPI.transfer(RD | OTR);
	count = SPI.transfer(0x00);
	count <<= 8;
	count |= SPI.transfer(0x00);
	count <<= 8;
	count |= SPI.transfer(0x00);
	count <<= 8;
	count |= SPI.transfer(0x00);
	digitalWrite(csPin, HIGH);
	
	countValue = count;
}

long LS7366R_Single::getCount()
{
	return countValue;
}
