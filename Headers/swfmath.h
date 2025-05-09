/*-------------------------------------------------------Inclusions and Macros-------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------|-----------------------------------------------------------------*/
#include"swftag.h"

#define M_CEILDIV(dividend, divisor) (((((dividend)/(divisor))*(divisor)==(dividend))?0:1)+((dividend)/(divisor)))
#define M_ALIGN(value, exp) (((value) + ((1<<exp) - 1)) & ~((1<<exp) - 1))
#define M_NEXTMUL(number, base) (M_CEILDIV(number, base) * (base))

#define M_FLAG_GET(flags, index, unitbits) (flags[index/unitbits]&(1<<(index%unitbits)))

// (-1)-diffval for negative diffval will generate a value between 0(incl.) and PTRDIFF_MAX(inclusive when the using 2's complement, exclusive for signed magnitude and 1's complement)
// Okay, apparently SIZE_MAX is not guaranteed to be (2*PTRDIFF_MAX)+1 for c99. SIZE_MAX is guaranteed to be atleast 65535, which is the same as PTRDIFF_MAX. Which I mean, one of these is a signed type and the other is not. This is stupid. So we're expanding the overflow checks
#define CHECK_PTRDIFF_OVERFLOW(diffval, addend) ((((ptrdiff_t)diffval) >= (ptrdiff_t)0)? ((uintmax_t)((uintmax_t)(PTRDIFF_MAX - ((ptrdiff_t)diffval))) < (uintmax_t)(addend)) : ((uintmax_t)addend < (uintmax_t)PTRDIFF_MAX) ? 0 : (((uintmax_t)addend - (uintmax_t)PTRDIFF_MAX) > (uintmax_t)((ptrdiff_t)(-1)-((ptrdiff_t)diffval))))
#define M_BUF_BOUNDS_CHECK(buffer, offset, pdata) ((((((uchar *)buffer) - pdata->u_movie) + ((offset) & ((ui32)0xFFFFFFFF))) > (pdata->movie_size)) || ((void *)(buffer) < (void *)(pdata->u_movie)) || CHECK_PTRDIFF_OVERFLOW(((((uchar *)buffer) - pdata->u_movie) + ((offset) & ((ui32)0xFFFFFFFF))), ((offset) & ((ui32)0xFFFFFFFF))))
// Now to figure out a way to ensure that (((uchar *)buffer) - pdata->u_movie) is < PTRDIFF_MAX
// Okay, there is a pretty stupid Idea I have for this but it's basically to have a loop that runs, successively adding PTRDIFF_MAX to a uintmax_t "difference" variable while it's smaller than buffer and then just adding the difference from the last pointer once it crosses it. This is naturally, dogshit and slow, but also the only standard compliant way I can think of. Obviously, since buffer - pdata->u_movie is guaranteed to be < 0xFFFFFFFF (we'll do more bounds check to ensure that in check_tag_stream), this will be enforced conditionally only when PTRDIFF_MAX < 0xFFFFFFFF through some #if macros

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
