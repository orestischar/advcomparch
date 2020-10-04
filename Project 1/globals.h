#ifndef GLOBALS_H
#define GLOBALS_H

#include <sstream>   // ostringstream type


#define KILO 1024
#define MEGA (KILO*KILO)
#define GIGA (KILO*MEGA)


/**
 * decimal - string conversion
 **/
static string dec2str(UINT64 v, UINT32 w)
{
    ostringstream o;
    o.width(w);
    o << v;
    string str(o.str());
    return str;
}


/**
 * determines if n is power of 2
 **/
static inline bool IsPowerOf2(UINT32 n)
{
    return ((n & (n - 1)) == 0);
}


/**
 *  Computes floor(log2(n))
 *  Works by finding position of MSB set.
 *  Returns -1 if n == 0.
 **/
static inline INT32 FloorLog2(UINT32 n)
{
    INT32 p = 0;

    if (n == 0) return -1;

    if (n & 0xffff0000) { p += 16; n >>= 16; }
    if (n & 0x0000ff00) { p +=  8; n >>=  8; }
    if (n & 0x000000f0) { p +=  4; n >>=  4; }
    if (n & 0x0000000c) { p +=  2; n >>=  2; }
    if (n & 0x00000002) { p +=  1; }

    return p;
}



#endif //GLOBALS_H




