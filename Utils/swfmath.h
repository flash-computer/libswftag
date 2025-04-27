/*-------------------------------------------------------Inclusions and Macros-------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------|-----------------------------------------------------------------*/
#include "../swftag.h"

#define M_CEILDIV(dividend, divisor) (((((dividend)/(divisor))*(divisor)==(dividend))?0:1)+((dividend)/(divisor)))
#define M_NEXTMUL(number, base) (M_CEILDIV(number, base) * (base))

#define M_FLAG_GET(flags, index, unitbits) (flags[index/unitbits]&(1<<(index%unitbits)))

#define M_BUF_BOUNDS_CHECK(tagbuffer, offset, pdata) ((tagbuffer - pdata->u_movie) + offset > (pdata->movie_size) || tagbuffer < pdata->u_movie)

#define M_UNSIGNED_COMPARE(a, b) ((a == b) ? 0 : (a > b) ? 1 : -1)

UI32_TYPE geti32(char *inp);
unsigned int geti16(char *inp);
int signed_comparei32(UI32_TYPE comparand_a, UI32_TYPE comparand_b);