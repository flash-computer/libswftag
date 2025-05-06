/*-------------------------------------------------------Inclusions and Macros-------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------|-----------------------------------------------------------------*/
#include"swftag.h"

#define M_CEILDIV(dividend, divisor) (((((dividend)/(divisor))*(divisor)==(dividend))?0:1)+((dividend)/(divisor)))
#define M_ALIGN(value, exp) (((value) + ((1<<exp) - 1)) & ~((1<<exp) - 1))
#define M_NEXTMUL(number, base) (M_CEILDIV(number, base) * (base))

#define M_FLAG_GET(flags, index, unitbits) (flags[index/unitbits]&(1<<(index%unitbits)))

#define M_BUF_BOUNDS_CHECK(buffer, offset, pdata) ((((((uchar *)buffer) - pdata->u_movie) + ((offset) & ((ui32)0xFFFFFFFF))) > (pdata->movie_size)) || ((void *)(buffer) < (void *)(pdata->u_movie)) || (((((uchar *)buffer) - pdata->u_movie) + ((offset) & ((ui32)0xFFFFFFFF))) < ((offset) & ((ui32)0xFFFFFFFF))))

#define M_UNSIGNED_COMPARE(a, b) ((a == b) ? 0 : (a > b) ? 1 : -1)

ui32 geti32(uchar *inp);
ui16 geti16(uchar *inp);
uf16_16 getuf16_16(uchar *inp);
uf8_8 getuf8_8(uchar *inp);

int signed_comparei32(ui32 comparand_a, ui32 comparand_b);

ui32 get_bitfield(uchar *buffer, ui32 base, ui8 offset);
ui32 get_signed_bitfield(uchar *buffer, ui32 base, ui8 offset);
uf16_16 get_signed_bitfield_fixed(uchar *buffer, ui32 base, ui8 offset);

ui8 get_bitfield_padding(uchar *buffer, ui32 base);