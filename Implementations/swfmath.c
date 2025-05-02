#include"../Headers/swftag.h"
#include"../Headers/swfmath.h"

ui32 geti32(char *inp)
{
	ui32 ret_val = inp[0];
	ret_val += ((ui32)inp[1])<<8;
	ret_val += ((ui32)inp[2])<<16;
	ret_val += ((ui32)inp[3])<<24;
	return ret_val;
}

ui16 geti16(char *inp)
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
