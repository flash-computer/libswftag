#include<libswftag/decompression.h>

#include<stdlib.h>

#include<zlib.h>

/*
To implement:
	Rolling/Normal lzma compression/decompression
	Rolling/Normal DEFLATE compression/decompression
*/

#define C_RAISE_ERR(error) ER_RAISE_ERROR_ERR(handler_ret, state, error)

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
	else
	{
		int read_more = fgetc(swf);
		if(read_more != EOF)
		{
			handler_ret = push_peculiarity(state, PEC_DATA_AFTER_MOVIE, state->movie_size);
			if(ER_ERROR(handler_ret))
			{
				return handler_ret;
			}
		}
	}
	return 0;
}

err movie_deflate(FILE *swf, pdata *state)
{
	err handler_ret;

	uchar *uncomp = malloc(state->reported_movie_size);
	if (!uncomp)
		return EMM_ALLOC;

	// TODO: sizes cast to unsigned int here
	// fc: That seems unavoidable as long as the dependence is on zlib being used.
	z_stream zs = {
		.next_out = uncomp,
		.avail_out = state->reported_movie_size,
	};

	if (inflateInit(&zs) != Z_OK)
	{
		free(uncomp);
		return EMM_ALLOC;
	}

	int zret = 0;
	while (zs.avail_out)
	{
		uchar buf[16*1024];

		size_t rv = fread(buf, 1, sizeof(buf), swf);
		if (!rv)
			break;

		zs.next_in = buf;
		zs.avail_in = rv;

		while (zs.avail_in && zs.avail_out)
		{
			zret = inflate(&zs, Z_NO_FLUSH);

			if (zret != Z_OK && zret != Z_STREAM_END)
			{
				if (zret == Z_NEED_DICT)
					zret = Z_DATA_ERROR;

				goto end;
			}
		}
	}

end:

	inflateEnd(&zs);

	// TODO: Making this movie_size ptrdiff calculation c99 spec compliant after the M_SAFE_PTRDIIF macro is added to swfmath.h
	state->movie_size = (zs.next_out - uncomp);
	state->u_movie = uncomp;

	if (zret < 0)
	{
		// decompression error
		// make this fatal if nothing was decompressed
		if (!state->movie_size)
		{
			C_RAISE_ERR(ESW_IMPROPER);
			return 0;
		}
		else
		{
			// just a corrupt file
			// flash could still play this
			// fc: Detecting corrupt files is sort of the point though? A decompression error is as fatal as they come imho, because nothing can be ascertained about what little was decoded. But, I guess I'd wait for more results from testing before making a decision on this.
			// TODO
			handler_ret = push_peculiarity(state, PEC_FILESIZE_SMALL, state->movie_size);	// In all honesty, the offsets throughout the libraray for PEC_FILESIZE_SMALL are not consistent, so I should fix that before changing this, but since the peculiarity interface isn't really mature yet, I think this is fine for now.
			if(ER_ERROR(handler_ret))
			{
				return handler_ret;
			}
		}
	}
	if (ferror(swf))
	{
		// read error
		C_RAISE_ERR(EFL_READ);
		return 0;
	}

	// file cut short -> !end
	// header size too short -> !end && output full && data remaining in file
	// header size too long -> end && no more data, output has unused space
	// extra data -> end && data remaining in buffer or file

	// is there file data remaining that we didn't use?
	// check both the buffer and the actual file
	// feof isn't necessarily true if it didn't try reading past the last byte
	int data_remaining;
	if (zs.avail_in)
		data_remaining = 1;
	else if (feof(swf))
		data_remaining = 0;
	else
	{
		char tmp;
		data_remaining = !!fread(&tmp, 1, 1, swf);
	}

	if (zret != Z_STREAM_END)
	{
		// didn't decompress the entire stream

		if (!data_remaining)
		{
			// unexpected end of file
			handler_ret = push_peculiarity(state, PEC_FILESIZE_SMALL, state->movie_size);
			if(ER_ERROR(handler_ret))
			{
				return handler_ret;
			}
		}
		else if (data_remaining && !zs.avail_out)
		{
			// header size too short
			handler_ret = push_peculiarity(state, PEC_FILESIZE_SMALL, 0);	// TODO: May need it's own peculiarity
			if(ER_ERROR(handler_ret))
			{
				return handler_ret;
			}
		}
	}
	else
	{
		// decompressed the entire stream

		if (data_remaining)
		{
			// junk data after compressed body
			handler_ret = push_peculiarity(state, PEC_DATA_AFTER_MOVIE, state->movie_size);
			if(ER_ERROR(handler_ret))
			{
				return handler_ret;
			}
		}
		else if (!data_remaining && zs.avail_out)
		{
			// header size too long
			handler_ret = push_peculiarity(state, PEC_FILESIZE_SMALL, 0);
			if(ER_ERROR(handler_ret))
			{
				return handler_ret;
			}
		}
	}

	return 0;
}

err movie_lzma(FILE *swf, pdata *state)
{
	err handler_ret;
	C_RAISE_ERR(EFN_NIB_HI);
	return 0;
}

#undef C_RAISE_ERROR
