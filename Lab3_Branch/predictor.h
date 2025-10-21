#ifndef __PREDICTOR_H__
#define __PREDICTO_H__
/*
 * 2B Saturation Stuff
 */
#define B2_ADDR_BITS 16 // Number of bits to use from PC for index
#define B2_TABLE_SIZE ((1 << B2_ADDR_BITS) - 1) // Variable state used for TWO_BIT_PREDICTOR


/*
 * G-Select Stuff
 */
// Size of prediction table: If M>N: 2^(M) If N>M: 2^(N)
#define G_SEL_ADDR_BITS 16 // (M) Number of bits to use from PC for index
#define G_SEL_HIS_BITS 8 // (N) Number of bits to use from history for index

#if G_SEL_ADDR_BITS > G_SEL_HIS_BITS
   #define G_SEL_TABLE_SIZE ((1 << G_SEL_ADDR_BITS) - 1)
#else
   #define G_SEL_TABLE_SIZE ((1 << G_SEL_HIS_BITS) - 1)
#endif


/*
 * G-Share Stuff
 */
// Size of prediction table: 2^(M+N)
#define G_SHARE_ADDR_BITS 8 // (M) Number of bits to use from PC for index
#define G_SHARE_HIS_BITS 8 // (N) Number of bits to use from history for index

#endif // __PREDICTOR_H__
