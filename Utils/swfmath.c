#include"../swftag.h"
#include"swfmath.h"

UI32_TYPE geti32(char *inp)
{
	UI32_TYPE ret_val = inp[0];
	ret_val += ((UI32_TYPE)inp[1])<<8;
	ret_val += ((UI32_TYPE)inp[2])<<16;
	ret_val += ((UI32_TYPE)inp[3])<<24;
	return ret_val;
}

ui16 geti16(char *inp)
{
	ui16 ret_val = inp[0];
	ret_val += ((UI32_TYPE)inp[1])<<8;	// It's a little endian world in a byte-ordering agnostic language
	return ret_val;
}

int signed_comparei32(UI32_TYPE comparand_a, UI32_TYPE comparand_b)
{
	UI32_TYPE sign_mask = ((UI32_TYPE)1)<<31;
	return (((comparand_a & sign_mask) == (comparand_b & sign_mask)) ? M_UNSIGNED_COMPARE(comparand_a, comparand_b) : (comparand_a & sign_mask) ? -1 : 1);
}