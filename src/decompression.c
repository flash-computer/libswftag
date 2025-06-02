#include<libswftag/decompression.h>

#define SWFTAG_MATH_INLINE
#include<libswftag/swfmath.h>

#include<stdlib.h>

#include<zlib.h>
#include<lzma.h>

/*
To implement:
	Rolling/Normal lzma compression/decompression
	Rolling/Normal DEFLATE compression/decompression
*/

// Move the decompression logic inside these later
err movie_deflate(pdata *state);
err movie_lzma(pdata *state);

#define C_RAISE_ERR(error) {err handler_ret; ER_RAISE_ERROR_ERR(handler_ret, state, error);}
#define STATE_ALLOC(pdata, size_to_alloc) ((!(pdata->alloc_fun))?malloc(size_to_alloc) : (pdata->alloc_fun)(size_to_alloc)) 
#define STATE_FREE(pdata, pointer_to_free) ((!(pdata->free_fun))?free(pointer_to_free) : (pdata->free_fun)(pointer_to_free))

err movie_buffer_uncomp(pdata *state, uchar *buffer, ui32 size)
{
	if(!state || !buffer)
	{
		C_RAISE_ERR(EFN_ARGS);
	}

	state->u_movie = buffer;
	state->movie_size = size;

	if(size < (state->reported_movie_size))
	{
		err ret = push_peculiarity(state, PEC_FILESIZE_SMALL, 0);
		if(ER_ERROR(ret))
		{
			return ret;
		}
	}
	else if(size > (state->reported_movie_size))
	{
		state->movie_size = state->reported_movie_size;
		err ret = push_peculiarity(state, PEC_DATA_AFTER_MOVIE, state->movie_size);
		if(ER_ERROR(ret))
		{
			return ret;
		}
	}

	return 0;
}

err movie_file_uncomp(pdata *state, FILE *swf)
{
	if(!state || !swf)
	{
		C_RAISE_ERR(EFN_ARGS);
	}
	state->u_movie = (uchar *)STATE_ALLOC(state, state->reported_movie_size);
	state->mgmt_flags |= PDATA_FLAG_MOVIE_ALLOC;
	if(!(state->u_movie))
	{
		C_RAISE_ERR(EMM_ALLOC);
	}
	state->movie_size = fread(state->u_movie, 1, state->reported_movie_size, swf);
	if(state->movie_size < state->reported_movie_size)
	{
		if(feof(swf))
		{
			err ret = push_peculiarity(state, PEC_FILESIZE_SMALL, 0);
			if(ER_ERROR(ret))
			{
				return ret;
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
			err ret = push_peculiarity(state, PEC_DATA_AFTER_MOVIE, state->movie_size);
			if(ER_ERROR(ret))
			{
				return ret;
			}
		}
	}
	return 0;
}

err movie_buffer_deflate(pdata *state, uchar *buffer, ui32 size)
{
	C_RAISE_ERR(EFN_NIB_HI);
	if(!state || !buffer)
	{
		C_RAISE_ERR(EFN_ARGS);
	}

	state->u_movie = (uchar *)STATE_ALLOC(state, state->reported_movie_size);;
	if (!(state->u_movie))
	{
		C_RAISE_ERR(EMM_ALLOC);
	}

	z_stream zstr;

	zstr.next_out = state->u_movie;
	zstr.avail_out = state->reported_movie_size;
	zstr.next_in = buffer;
	zstr.avail_in = size;

	if (inflateInit(&zstr) != Z_OK)
	{
		STATE_FREE(state, state->u_movie);
		state->u_movie = NULL;
		C_RAISE_ERR(EMM_ALLOC);
	}

	int zret = inflate(&zstr, Z_NO_FLUSH);

	if(zstr.avail_out == 0)
	{
		if(zret != Z_STREAM_END || zstr.avail_in != 0)
		{
			err ret = push_peculiarity(state, PEC_DATA_AFTER_MOVIE, state->movie_size);
			if(ER_ERROR(ret))
			{
				STATE_FREE(state, state->u_movie);
				state->u_movie = NULL;
				inflateEnd(&zstr);
				return ret;
			}
		}
	}
	else if(zret == Z_STREAM_END)
	{
		state->movie_size = uchar_safe_ptrdiff(zstr.next_out, state->u_movie);
		err ret = push_peculiarity(state, PEC_FILESIZE_SMALL, state->movie_size);
		if(ER_ERROR(ret))
		{
			STATE_FREE(state, state->u_movie);
			state->u_movie = NULL;
			inflateEnd(&zstr);
			return ret;
		}
	}
	else if(zret != Z_OK)
	{
			STATE_FREE(state, state->u_movie);
			state->u_movie = NULL;
			inflateEnd(&zstr);
			C_RAISE_ERR(EFN_DECOMP);
	}

	inflateEnd(&zstr);
	return 0;
}

err movie_file_deflate(pdata *state, FILE *swf)
{
	if(!state || !swf)
	{
		C_RAISE_ERR(EFN_ARGS);
	}
	uchar *uncomp = (uchar *)STATE_ALLOC(state, state->reported_movie_size);;
	if (!uncomp)
	{
		C_RAISE_ERR(EMM_ALLOC);
	}

	// Never overflows because reported_movie_size never exceeds 0xFFFFFFFF, and the minimum value of UINTMAX_MAX conferred by N1256 is (1<<64)-1
	// uintmax_t max_compressed_size = (state->reported_movie_size) +  (5 * (uintmax_t)M_CEILDIV(state->reported_movie_size, 65535));
	// Naive Estimation. Assuming optimal compression for each block (AKA in the worst case, the entire file will be stored in Raw blocks, which confer a 5 byte overhead over a maximum of 65535 raw bytes)
	// To my knowledge, nothing forbids unoptimal DEFLATE blocks, which can cause the size to baloon up. To calculate it would need more work, it would be the size of the most unoptimal Huffman tree(then it isn't really a huffman tree but the standard still allows it so our hands are ties) + A noisy assortment of the heaviest words from the tree. If it even is a finite bound, let alone that fits inside a 64 bit word for uintmax_t, I'll put off finding out for later.
	// TODO: As a heuristic in the self implemented inflate, we will reject any file that contains a non raw block that is larger than the output block.
	// Whether or not that is consistent by the standard, I do not know.
	// RFC 1951 states this: "a compliant decompressor must be able to accept and decompress any data set that conforms to all the specifications presented here; a compliant compressor must produce data sets that conform to all the specifications presented here."
	// Reading the spec, it is not clear to me that if a compliant compressor needs to always store the block raw if both the dynamic and static compression result in a larger result than the raw representation
	// If that were true, The heuristic would ensure that the decompressor can decompress all data stream compressed by a compliant compressor.
	// It would not necessarily ensure however: 1. That it's a compliant decompressor, as in it can decompress any data stream which is valid according to the spec, regardless or not of whether it can be generated by a compliant compressor
	// This is a restriction imposed by the standard so, failing this would mean that the inflate is not a compliant decompressor. But paradoxically, it's also less relevant as a focus is on security, not compatibility.
	// 2. That it can only decompress compliant data. This is not a condition imposed by the standard, but paradoxically, it's important to ensure this.
	// TODO: Study the RFC 1951 standard more thoroughly to resolve these meaningfully. If no resolution exists, assume that a compliant compressor must store a raw block if other methods fail. Better safe than sorry.

	// TODO: sizes cast to unsigned int here
	z_stream zs = {
		.next_out = uncomp,
		.avail_out = state->reported_movie_size,
	};

	if (inflateInit(&zs) != Z_OK)
	{
		STATE_FREE(state, uncomp);
		C_RAISE_ERR(EMM_ALLOC);
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


	state->movie_size = uchar_safe_ptrdiff(zs.next_out, uncomp);
	state->u_movie = uncomp;
	state->mgmt_flags |= PDATA_FLAG_MOVIE_ALLOC;

	if (zret < 0)
	{
		// decompression error
		// make this fatal if nothing was decompressed
		if (!state->movie_size)
		{
			STATE_FREE(state, uncomp);
			state->u_movie = NULL;
			C_RAISE_ERR(ESW_IMPROPER);
			return 0;
		}
		else
		{
			// just a corrupt file
			// flash could still play this
			// fc: Detecting corrupt files is sort of the point though? A decompression error is as fatal as they come imho, because nothing can be ascertained about what little was decoded. But, I guess I'd wait for more results from testing before making a decision on this.
			// TODO
			err ret = push_peculiarity(state, PEC_FILESIZE_SMALL, state->movie_size);	// In all honesty, the offsets throughout the libraray for PEC_FILESIZE_SMALL are not consistent, so I should fix that before changing this, but since the peculiarity interface isn't really mature yet, I think this is fine for now.
			if(ER_ERROR(ret))
			{
				STATE_FREE(state, uncomp);
				state->u_movie = NULL;
				return ret;
			}
		}
	}
	if (ferror(swf))
	{
		// read error
		STATE_FREE(state, uncomp);
		state->u_movie = NULL;
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
			err ret = push_peculiarity(state, PEC_FILESIZE_SMALL, state->movie_size);
			if(ER_ERROR(ret))
			{
				STATE_FREE(state, uncomp);
				state->u_movie = NULL;
				return ret;
			}
		}
		else if (data_remaining && !zs.avail_out)
		{
			// header size too short
			err ret = push_peculiarity(state, PEC_DATA_AFTER_MOVIE, 0);	// TODO: May need it's own peculiarity
			if(ER_ERROR(ret))
			{
				STATE_FREE(state, uncomp);
				state->u_movie = NULL;
				return ret;
			}
		}
	}
	else
	{
		// decompressed the entire stream

		if (data_remaining)
		{
			// junk data after compressed body
			err ret = push_peculiarity(state, PEC_DATA_AFTER_MOVIE, state->movie_size);
			if(ER_ERROR(ret))
			{
				STATE_FREE(state, uncomp);
				state->u_movie = NULL;
				return ret;
			}
		}
		else if (!data_remaining && zs.avail_out)
		{
			// header size too long
			err ret = push_peculiarity(state, PEC_FILESIZE_SMALL, 0);
			if(ER_ERROR(ret))
			{
				STATE_FREE(state, uncomp);
				state->u_movie = NULL;
				return ret;
			}
		}
	}

	return 0;
}


err movie_buffer_lzma(pdata *state, uchar *buffer, ui32 size)
{
	if(!state || !buffer)
	{
		C_RAISE_ERR(EFN_ARGS);
	}

	state->u_movie = (uchar *)STATE_ALLOC(state, state->reported_movie_size);
	if(!(state->u_movie))
	{
		C_RAISE_ERR(EMM_ALLOC);
	}

	if(size < (state->reported_movie_size))
	{
		err ret = push_peculiarity(state, PEC_FILESIZE_SMALL, 0);
		if(ER_ERROR(ret))
		{
			STATE_FREE(state, state->u_movie);
			state->u_movie = NULL;
			return ret;
		}
	}
	else if(size > (state->reported_movie_size))
	{
		state->movie_size = state->reported_movie_size;
		err ret = push_peculiarity(state, PEC_DATA_AFTER_MOVIE, state->movie_size);
		if(ER_ERROR(ret))
		{
			STATE_FREE(state, state->u_movie);
			state->u_movie = NULL;
			return ret;
		}
	}

	uchar headerbuf[13];

	lzma_stream lzstr = LZMA_STREAM_INIT;
	lzstr.next_out = state->u_movie;
	lzstr.avail_out = state->movie_size;

	lzstr.next_in = headerbuf;
	lzstr.avail_in = 13;

	if(size < 9)
	{
		STATE_FREE(state, state->u_movie);
		state->u_movie = NULL;
		C_RAISE_ERR(EFN_DECOMP);
	}

	lzma_ret lzret = lzma_alone_decoder(&lzstr, SIZE_MAX);
	if(lzret != LZMA_OK)
	{
		STATE_FREE(state, state->u_movie);
		state->u_movie = NULL;
		C_RAISE_ERR(EFN_DECOMP);
	}

	for(size_t i=0; i<5; i++)
	{
		headerbuf[i] = buffer[i+4];
	}
	seti32(headerbuf+5, state->movie_size);
	seti32(headerbuf+9, 0);

	lzret = lzma_code(&lzstr, LZMA_RUN);
	if(lzret != LZMA_OK)
	{
		STATE_FREE(state, state->u_movie);
		state->u_movie = NULL;
		lzma_end(&lzstr);
		C_RAISE_ERR(EFN_DECOMP);
	}

	lzstr.next_in = buffer + 9;
	lzstr.avail_in = size - 9;

	lzret = lzma_code(&lzstr, LZMA_RUN);

	if(lzstr.avail_out == 0)
	{
		if(lzret != LZMA_STREAM_END || lzstr.avail_in != 0)
		{
			err ret = push_peculiarity(state, PEC_DATA_AFTER_MOVIE, state->movie_size);
			if(ER_ERROR(ret))
			{
				STATE_FREE(state, state->u_movie);
				state->u_movie = NULL;
				lzma_end(&lzstr);
				return ret;
			}
		}
	}
	else if(lzret == LZMA_STREAM_END)
	{
		state->movie_size = uchar_safe_ptrdiff(lzstr.next_out, state->u_movie);
		err ret = push_peculiarity(state, PEC_FILESIZE_SMALL, state->movie_size);
		if(ER_ERROR(ret))
		{
			STATE_FREE(state, state->u_movie);
			state->u_movie = NULL;
			lzma_end(&lzstr);
			return ret;
		}
	}
	else if(lzret != LZMA_OK)
	{
		switch(lzret)
		{
			case LZMA_MEM_ERROR:
				STATE_FREE(state, state->u_movie);
				state->u_movie = NULL;
				lzma_end(&lzstr);
				C_RAISE_ERR(EMM_ALLOC);
				break;
			default:
				STATE_FREE(state, state->u_movie);
				state->u_movie = NULL;
				lzma_end(&lzstr);
				C_RAISE_ERR(EFN_DECOMP);
				break;
		}
	}
	lzma_end(&lzstr);
	return 0;
}

err movie_file_lzma(pdata *state, FILE *swf)
{
	if(!state || !swf)
	{
		C_RAISE_ERR(EFN_ARGS);
	}

	state->u_movie = (uchar *)STATE_ALLOC(state, state->reported_movie_size);
	if(!(state->u_movie))
	{
		C_RAISE_ERR(EMM_ALLOC);
	}

	state->movie_size = state->reported_movie_size;
	uchar headerbuf[13];

	uchar readbuf[0x10000];
	ui32 read_so_far = 0;

	lzma_stream lzstr = LZMA_STREAM_INIT;
	lzstr.next_out = state->u_movie;
	lzstr.avail_out = state->movie_size;

	lzstr.next_in = readbuf;
	lzstr.avail_in = 0;

	// Is SIZE_MAX the right choice here?
	lzma_ret lzret = lzma_alone_decoder(&lzstr, SIZE_MAX);
	if(lzret != LZMA_OK)
	{
		STATE_FREE(state, state->u_movie);
		state->u_movie = NULL;
		C_RAISE_ERR(EFN_DECOMP);
	}

	lzma_action lzact = LZMA_RUN;
	ui8 header_read = 0;

	while(1)
	{
		size_t readSize;
		if(!header_read)
		{
			readSize = fread(readbuf, 1, 9, swf);
			if(readSize < 9)
			{
				STATE_FREE(state, state->u_movie);
				state->u_movie = NULL;
				lzma_end(&lzstr);
				C_RAISE_ERR(EFN_DECOMP);
			}
			for(size_t i=0; i<5; i++)
			{
				headerbuf[i] = readbuf[i+4];
			}
			seti32(headerbuf+5, state->movie_size);
			seti32(headerbuf+9, 0);

			readSize = 13;

			lzstr.next_in = headerbuf;
			lzstr.avail_in = 13;

			header_read = 1;
		}
		else
		{
			readSize = fread(readbuf, 1, sizeof(readbuf), swf);
			if(!readSize)
			{
				if(feof(swf))
				{
					break;
				}
				STATE_FREE(state, state->u_movie);
				state->u_movie = NULL;
				lzma_end(&lzstr);
				C_RAISE_ERR(EFL_READ);
			}

			lzstr.next_in = readbuf;
			lzstr.avail_in = readSize;
		}

		lzret = lzma_code(&lzstr, lzact);

		if(lzstr.avail_out == 0)
		{
			if(lzret == LZMA_STREAM_END && lzstr.avail_in == 0)
			{
				uchar testbuf[1];
				ui8 test = fread(testbuf, 1, 1, swf);
				if(!feof(swf))
				{
					err ret = push_peculiarity(state, PEC_DATA_AFTER_MOVIE, state->movie_size);
					if(ER_ERROR(ret))
					{
						STATE_FREE(state, state->u_movie);
						state->u_movie = NULL;
						lzma_end(&lzstr);
						return ret;
					}
				}
			}
			else
			{
				err ret = push_peculiarity(state, PEC_DATA_AFTER_MOVIE, state->movie_size);
				if(ER_ERROR(ret))
				{
					STATE_FREE(state, state->u_movie);
					state->u_movie = NULL;
					lzma_end(&lzstr);
					return ret;
				}
			}
			break;
		}
		else if(lzret == LZMA_STREAM_END)
		{
			state->movie_size = uchar_safe_ptrdiff(lzstr.next_out, state->u_movie);
			err ret = push_peculiarity(state, PEC_FILESIZE_SMALL, state->movie_size);
			if(ER_ERROR(ret))
			{
				STATE_FREE(state, state->u_movie);
				state->u_movie = NULL;
				lzma_end(&lzstr);
				return ret;
			}
			break;
		}

		if(lzret != LZMA_OK)
		{
			switch(lzret)
			{
				case LZMA_MEM_ERROR:
					STATE_FREE(state, state->u_movie);
					state->u_movie = NULL;
					lzma_end(&lzstr);
					C_RAISE_ERR(EMM_ALLOC);
					break;
				default:
					STATE_FREE(state, state->u_movie);
					state->u_movie = NULL;
					lzma_end(&lzstr);
					C_RAISE_ERR(EFN_DECOMP);
					break;
			}
		}
	}
	lzma_end(&lzstr);
	return 0;
}

#undef STATE_ALLOC
#undef STATE_FREE
#undef C_RAISE_ERROR
