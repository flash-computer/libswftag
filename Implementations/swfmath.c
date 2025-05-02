#include"../Headers/swftag.h"
#include"../Headers/swfmath.h"

ui32 geti32(uchar *inp)
{
	ui32 ret_val = (ui8)inp[0];
	ret_val += ((ui32)inp[1])<<8;
	ret_val += ((ui32)inp[2])<<16;
	ret_val += ((ui32)inp[3])<<24;
	return ret_val;
}

ui16 geti16(uchar *inp)
{
	ui16 ret_val = inp[0];
	ret_val += ((ui32)inp[1])<<8;	// It's a little endian world in a byte-ordering agnostic language
	return ret_val;
}

int signed_comparei32(ui32 comparand_a, ui32 comparand_b)
{
	ui32 sign_mask = ((ui32)1)<<31;
	return (((comparand_a & sign_mask) == (comparand_b & sign_mask)) ? M_UNSIGNED_COMPARE(comparand_a, comparand_b) : (comparand_a & sign_mask) ? -1 : 1);
}

ui32 get_bitfield(uchar *buffer, ui8 base, ui8 offset)
{
	if(offset == 0)
	{
		return 0;
	}
	else if(offset > 32)
	{
		offset = 32;
	}
	uchar *start = buffer + (base>>3);		// Since bytes are right aligned in big endian bit ordering for the C standard, this works for other values of CHAR_BIT
	ui8 nbase = base & 0x7;
	ui8 limit = M_ALIGN((ui32)(nbase + offset), 3);
	uchar *end = start + (limit>>3);
	uchar *itr = start + 1;
	ui32 hi = *start;
	ui32 lo = 0;
	ui32 slh = 0;
	ui32 srh = 8-(offset+nbase);
	ui32 srl = limit-(offset+nbase);
	ui32 mask = ((ui32)0xFFFFFFFF)>>(32-offset);
	while(itr < end)
	{
		srh = 0;
		slh = (offset+nbase)-8;
		lo <<= 8;
		lo |= *itr;
		itr++;
	}
	lo = (((hi>>srh)<<slh) | ((lo>>srl) & mask)) & mask;
	return (lo | (((lo & 1<<(offset-1))>>(offset-1)) * (0xFFFFFFFF ^ ((1<<offset)-1))));	// Sign extension, to 32 bits exactly
}
