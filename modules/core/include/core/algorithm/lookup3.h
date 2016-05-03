//---------------------------------------------------------------------------

#ifndef LOOKUP3_H
#define LOOKUP3_H

//---------------------------------------------------------------------------

/**
  * Function declarations and macros from lookup3.c by Bob Jenkins.
  * lookup3.c was renamed to lookup3.cpp to use soft default parameters and
  * namespaces here. Note that "hashlittle" and "hashlittle2" functions were
  * renamed to "hashbytes" and "hashbytes2" accordingly. "hashbig" was
  * removed because we live in the world of Intel and AMD which are using
  * little-endian byte ordering.
  *	All other comments was written by the author and edited to conform with
  * the Doxygen documentation formatting and the current state of this
  * useful set of functions.
  * That's all.
  * Read lookup3.cpp for more information about the functions.
  */

#include <meta/Types.h>

//---------------------------------------------------------------------------

#define rot_bits(x,k) (((x)<<(k)) | ((x)>>(32-(k))))

/**
 *	mix -- mix 3 32-bit values reversibly.
 *
 *	ThisClass is reversible, so any information in (a,b,c) before mix() is
 *	still in (a,b,c) after mix().
 *
 *	If four pairs of (a,b,c) inputs are run through mix(), or through
 *	mix() in reverse, there are at least 32 bits of the output that
 *	are sometimes the same for one pair and different for another pair.
 *	ThisClass was tested for:
 *	- pairs that differed by one bit, by two bits, in any combination
 *	of top bits of (a,b,c), or in any combination of bottom bits of
 *	(a,b,c).
 *	- "differ" is defined as +, -, ^, or ~^.  For + and -, I transformed
 *	the output delta to a Gray code (a^(a>>1)) so a string of 1's (as
 *	is commonly produced by subtraction) look like a single 1-bit
 *	difference.
 *	- the base values were pseudorandom, all zero but one bit set, or
 *	all zero plus a counter that starts at zero.
 *
 *	Some k values for my "a-=c; a^=rot(c,k); c+=b;" arrangement that
 *	satisfy this are
 *	4  6  8 16 19  4
 *	9 15  3 18 27 15
 *	14  9  3  7 17  3
 *	Well, "9 15 3 18 27 15" didn't quite get 32 bits diffing
 *	for "differ" defined as + with a one-bit base and a two-bit delta.  I
 *	used http://burtleburtle.net/bob/hash/avalanche.html to choose
 *	the operations, constants, and arrangements of the variables.
 *
 *	ThisClass does not achieve avalanche.  There are input bits of (a,b,c)
 *	that fail to affect some output bits of (a,b,c), especially of a.  The
 *	most thoroughly mixed value is c, but it doesn't really even achieve
 *	avalanche in c.
 *
 *	ThisClass allows some parallelism.  Read-after-writes are good at doubling
 *	the number of bits affected, so the goal of mixing pulls in the opposite
 *	direction as the goal of parallelism.  I did what I could.  Rotates
 *	seem to cost as much as shifts on every machine I could lay my hands
 *	on, and rotates are much kinder to the top and bottom bits, so I used
 *	rotates.
 */

#define mix(a,b,c) \
{ \
	a -= c;  a ^= rot_bits(c, 4);  c += b; \
	b -= a;  b ^= rot_bits(a, 6);  a += c; \
	c -= b;  c ^= rot_bits(b, 8);  b += a; \
	a -= c;  a ^= rot_bits(c,16);  c += b; \
	b -= a;  b ^= rot_bits(a,19);  a += c; \
	c -= b;  c ^= rot_bits(b, 4);  b += a; \
}

/**
 * final -- final mixing of 3 32-bit values (a,b,c) into c
 *
 * Pairs of (a,b,c) values differing in only a few bits will usually
 * produce values of c that look totally different.  ThisClass was tested for
 * * pairs that differed by one bit, by two bits, in any combination
 *   of top bits of (a,b,c), or in any combination of bottom bits of
 *   (a,b,c).
 * * "differ" is defined as +, -, ^, or ~^.  For + and -, I transformed
 *   the output delta to a Gray code (a^(a>>1)) so a string of 1's (as
 *   is commonly produced by subtraction) look like a single 1-bit
 *   difference.
 * * the base values were pseudorandom, all zero but one bit set, or
 *   all zero plus a counter that starts at zero.
 *
 * These constants passed:
 *  14 11 25 16 4 14 24
 *  12 14 25 16 4 14 24
 * and these came close:
 *   4  8 15 26 3 22 24
 *  10  8 15 26 3 22 24
 *  11  8 15 26 3 22 24
 */

#define final(a,b,c) \
{ \
  c ^= b; c -= rot_bits(b,14); \
  a ^= c; a -= rot_bits(c,11); \
  b ^= a; b -= rot_bits(a,25); \
  c ^= b; c -= rot_bits(b,16); \
  a ^= c; a -= rot_bits(c,4);  \
  b ^= a; b -= rot_bits(a,14); \
  c ^= b; c -= rot_bits(b,24); \
}

namespace lookup3
{
	uint32_t hashbytes(const void * key, size_t length, uint32_t initval = 0);
	uint32_t hashword(const uint32_t * k, size_t length, uint32_t initval = 0);

	void hashbytes2(const void * key, size_t length, uint32_t * pc, uint32_t * pb);
	void hashword2(const uint32_t * k, size_t length, uint32_t * pc, uint32_t * pb);
}

using namespace lookup3;

//---------------------------------------------------------------------------
#endif
