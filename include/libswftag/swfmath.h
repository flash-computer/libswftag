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

// Behold, stupidity at it's peak
// ONLY use on objects allocated in a multiple that fits inside 32 bits, where the pointer has been gradually differentiated from the base pointer doing a bounds check **before** any change
// ret_var should be of the type uintmax_t
#if PTRDIFF_MAX < 0xFFFFFFFF
	#define M_SAFE_PTRDIIFF32(minuend, subtrahend, ptr_type, ret_var)	{ret_var = 0; ptr_type * safe_diff_ptr = ((ptr_type *)subtrahend); while(1){ptr_type * temp_diff_ptr = (safe_diff_ptr + (uintmax_t)PTRDIFF_MAX);if(temp_diff_ptr > ((ptr_type *)minuend) || temp_diff_ptr < ((ptr_type *)subtrahend)){ret_var += ((ptr_type *)minuend) - safe_diff_ptr; break};ret_var += (uintmax_t)PTRDIFF_MAX; safe_diff_ptr += (uintmax_t)PTRDIFF_MAX;}}
	#ifndef UCHAR_PTRDIFF_FN
		#define UCHAR_PTRDIFF_FN
		uintmax_t uchar_safe_ptrdiff(uchar *minuend, uchar *subtrahend)
		{
			uintmax_t ret_val;
			M_SAFE_PTRDIIFF32(minuend, subtrahend, uchar, ret_val);
			return ret_val;
		}
	#endif
#else
	#define M_SAFE_PTRDIIFF32(minuend, subtrahend, ptr_type, ret_var) {ret_var=(((ptr_type *)minuend) - ((ptr_type *)subtrahend));}
	#define uchar_safe_ptrdiff(minuend, subtrahend) (((uchar *)minuend) - ((uchar *)subtrahend))
#endif

#define M_BUF_BOUNDS_CHECK(buffer, offset, pdata) (((uchar_safe_ptrdiff(buffer, pdata->u_movie) + ((offset) & ((ui32)0xFFFFFFFF))) > ((uintmax_t)(pdata->movie_size))) || ((void *)(buffer) < (void *)(pdata->u_movie)))
// Now to figure out a way to ensure that (((uchar *)buffer) - pdata->u_movie) is < PTRDIFF_MAX


#define M_UNSIGNED_COMPARE(a, b) ((a == b) ? 0 : (a > b) ? 1 : -1)

#if CHAR_BIT > 8
	#define M_SANITIZE_BYTE(byte) (byte) & ((ui8)0xFF)
#else
	#define M_SANITIZE_BYTE(byte) (byte)
#endif

ui32 geti32(uchar *inp);
ui16 geti16(uchar *inp);
uf16_16 getuf16_16(uchar *inp);
uf8_8 getuf8_8(uchar *inp);

int signed_comparei32(ui32 comparand_a, ui32 comparand_b);

ui32 get_bitfield(uchar *buffer, ui32 base, ui8 offset);
ui32 get_signed_bitfield(uchar *buffer, ui32 base, ui8 offset);
uf16_16 get_signed_bitfield_fixed(uchar *buffer, ui32 base, ui8 offset);

ui8 get_bitfield_padding(uchar *buffer, ui32 base);
