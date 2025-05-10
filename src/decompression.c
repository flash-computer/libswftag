#include<libswftag/decompression.h>

#include<stdlib.h>

/*
To implement:
	Rolling/Normal lzma compression/decompression
	Rolling/Normal DEFLATE compression/decompression
*/

#define C_RAISE_ERR(error) ER_RAISE_ERROR_ERR(handler_ret, error, state)

err movie_uncomp(FILE *swf, pdata *state)
{
	err handler_ret;
	state->u_movie = malloc(state->reported_movie_size);
	if(!(state->u_movie))
	{
		return EMM_ALLOC;
	}
	state->movie_size = fread(state->u_movie, 1, state->reported_movie_size, swf);
	if(state->movie_size < state->reported_movie_size)
	{
		if(feof(swf))
		{
			handler_ret = push_peculiarity(state, PEC_FILESIZE_SMALL, 0);
			if(ER_ERROR(handler_ret))
			{
				return handler_ret;
			}
		}
		else
		{
			C_RAISE_ERR(EFL_READ);
		}
	}
	return 0;
}

err movie_deflate(FILE *swf, pdata *state)
{
	err handler_ret;
	C_RAISE_ERR(EFN_NIB_HI);
	return 0;
}

err movie_lzma(FILE *swf, pdata *state)
{
	err handler_ret;
	C_RAISE_ERR(EFN_NIB_HI);
	return 0;
}

#undef C_RAISE_ERROR
