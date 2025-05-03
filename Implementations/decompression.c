#include"../Headers/decompression.h"

#include<stdlib.h>

/*
To implement:
	Rolling/Normal lzma compression/decompression
	Rolling/Normal DEFLATE compression/decompression
*/

err movie_uncomp(FILE *swf, pdata *state)
{
	state->u_movie = malloc(state->movie_size);
	if(!(state->u_movie))
	{
		return EMM_ALLOC;
	}
	ui32 read_size = fread(state->u_movie, 1, state->movie_size, swf);
	state->read_movie_size = read_size;
	if(read_size < state->movie_size)
	{
		push_peculiarity(state, PEC_FILESIZE_SMALL, 0);
	}
	return 0;
}

err movie_deflate(FILE *swf, pdata *state)
{
	return EFN_NIB_HI;
}

err movie_lzma(FILE *swf, pdata *state)
{
	return EFN_NIB_HI;
}
