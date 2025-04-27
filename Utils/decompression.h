#include<stdlib.h>
#include <stdio.h>

#include"../swftag.h"
#include"../error.h"

/*
To implement:
	Rolling/Normal lzma compression/decompression
	Rolling/Normal DEFLATE compression/decompression
*/

int movie_uncomp(FILE *swf, pdata *state)
{
	state->u_movie = malloc(state->movie_size);
	if(state->u_movie)
	{
		return EMM_ALLOC;
	}
	if(fread(state->u_movie, 1, state->movie_size, swf) < state->movie_size)
	{
		return EFL_READ;
	}
	return 0;
}

int movie_deflate(FILE *swf, pdata *state)
{
	return EFN_NIB_HI;
}

int movie_lzma(FILE *swf, pdata *state)
{
	return EFN_NIB_HI;
}