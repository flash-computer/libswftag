#include"../Headers/swftag.h"
#include"../Headers/error.h"
#include"../Headers/check_functions.h"
#include"../Headers/swfmath.h"
#include"../Headers/decompression.h"

#include<stdio.h>
#include<stdlib.h>

/*-------------------------------------------------------------Functions-------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------|-----------------------------------------------------------------*/

err_ptr get_tag(char *buffer, pdata *state)
{
	if(M_BUF_BOUNDS_CHECK(buffer, 2, state))
	{
		return (err_ptr){NULL, ESW_SHORTFILE};
	}
	swf_tag *tag = malloc(sizeof(swf_tag));
	if(!tag)
	{
		return (err_ptr){NULL, EMM_ALLOC};
	}
	tag->tag_and_size = geti16(buffer);
	tag->tag = (tag->tag_and_size & 0xFFC0)>>6;
	tag->size = (tag->tag_and_size & 0x3F);
	if(!tag_valid(tag->tag))
	{
		return (err_ptr){tag, ESW_TAG};	// Invalid Tag
	}
	tag->tag_data = buffer + 2;
	if(tag->size == 63 || tag_long_exclusive(tag->tag).integer)
	{
		if(M_BUF_BOUNDS_CHECK(buffer, 4, state))
		{
			return (err_ptr){tag, ESW_SHORTFILE};
		}
		tag->size = geti32(buffer + 2);
		tag->tag_data = buffer + 6;
	}
	return (err_ptr){tag, 0};
}

// This is the biggest part of the project and thus the one I'd need the most help with.
// Evaluation would have to be conditional with the version outlined in the pdata structure. All of the prototypes above would have to be fleshed out.
// The struct that the pointer returns hasn't been made yet but it would be diagnostic struct outlining exactly what is wrong in case of an error.
err_ptr check_tag(swf_tag *tag, pdata *state)
{
	if(!tag || !state)
	{
		return (err_ptr){NULL, EFN_ARGS};
	}
	if(!tag_valid(tag->tag))
	{
		return (err_ptr){NULL, ESW_TAG};	// In the future SWF structure errors would be instead turned into their own callback that can exit non_destructively. For now, it's an error.
	}
	err_int ret = tag_version_valid(tag->tag, state->version);
	if(ER_ERROR(ret.ret))
	{
		return (err_ptr){NULL, ret.ret};
	}
	if(!ret.integer)
	{
		push_peculiarity(state, PEC_TIME_TRAVEL, tag->tag_data - state->u_movie);
	}
	// Check function calls and the rest of the stuff will go here
	err ret_check = tag_check[tag->tag](tag, state);
	if(ER_ERROR(ret_check))
	{
		return (err_ptr){NULL, ret_check};
	}
	return (err_ptr){NULL, 0};
}

err_ptr spawn_tag(int tag, ui32 size, char *tag_data)
{
	if(!tag_valid(tag) && tag != F_FILEHEADER)
	{
		return (err_ptr){NULL, EFN_ARGS};
	}
	/*
	 * --------------------------------------------------------------------------
	 * --------------------------------------------------------------------------
	 * --------------------------------------------------------------------------
	Todo - Add additional verifications for tag_data and size
	 * --------------------------------------------------------------------------
	 * --------------------------------------------------------------------------
	 * --------------------------------------------------------------------------
	*/
	swf_tag *new_tag = malloc(sizeof(swf_tag));
	if(!new_tag)
	{
		return (err_ptr){NULL, EMM_ALLOC};
	}
	new_tag->tag = tag;
	new_tag->size = size;
	new_tag->tag_and_size = ((tag<<6) & 0xFFC0) | ((size > 62 || ((tag != F_FILEHEADER)? tag_long_exclusive(tag).integer : 0))? 0x3F : (size & 0x3F));
	new_tag->tag_data = tag_data;
	return (err_ptr){new_tag, 0};
}

/*--------------------------------------------------------Parse substructures--------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------|-----------------------------------------------------------------*/

// TODO: Make these context aware by making them return err_int to output the size of the structure in bits and pass the swf_tag in arguments instead of limit because some substructures have additional properties based on that
// Only for integrity checks in substructure parsing. ESW_SHORTFILE if errror
#define C_BOUNDS_EVAL(tagbuffer, offset, pdata, limit) if(M_BUF_BOUNDS_CHECK(tagbuffer, offset, pdata))return ESW_SHORTFILE;if(limit < offset)return ESW_IMPROPER

/* TODO : Spin out the bit matching logic into a separate function for all the other functions to analyze substructures that need it */
err swf_rect_parse(RECT *rect, pdata *state, char *rect_buf, ui32 limit)
{
	C_BOUNDS_EVAL(rect_buf, 1, state, limit);

	rect->field_size = (rect_buf[0] & 0xF8)>>3;	// Since the byte is right aligned, works for higher values of CHAR_BIT just fine

	C_BOUNDS_EVAL(state->u_movie, M_ALIGN((5+(rect->field_size * 4)), 3)>>3, state, limit);

	int offset = 5;
	int cur_beg, nex_beg;
	ui32 *to_write;
	for(int field = 0; field < 4; field++)
	{
		to_write = rect->fields + field;
		*to_write = 0;
		cur_beg = (5 + (field)*(rect->field_size));
		nex_beg = (5 + (field+1)*(rect->field_size));
		while(offset < nex_beg)
		{
			ui8 byte_left_bound = (offset == cur_beg)? (offset & 0x7) : 0;
			ui8 byte_right_bound = ((nex_beg - offset) <= (nex_beg & 0x7))? (nex_beg & 0x7) : 8;
			ui8 int_shift = rect->field_size - ((offset - cur_beg) + (byte_right_bound - byte_left_bound));
			ui8 byte_mask = (((ui8)(~0))<<(8-byte_right_bound)) & (((ui8)(~0))>>byte_left_bound) & 0xFF;
			ui32 read_byte = (((ui32)(rect_buf[offset>>3] & byte_mask)>>(8-byte_right_bound))<<int_shift);
			*to_write |= (((rect_buf[offset>>3] & byte_mask)>>(8-byte_right_bound))<<int_shift);

			offset += byte_right_bound - byte_left_bound;
		}
	}

	if(rect_buf[(5+(rect->field_size * 4))>>3] & (((~(ui8)0) & 0xFF)>>((5+(rect->field_size * 4)) & 0x7)))
	{
		return push_peculiarity(state, PEC_RECTPADDING, 0);
	}
	return 0;
}

/* TODO : After the bit logic has been spun out from rect_parse, do this */
err swf_matrix_parse(MATRIX *mat, pdata *state, char *mat_buf, ui32 limit)
{
	return 0;
}

/* TODO : After the bit logic has been spun out from rect_parse, do this */
err swf_color_transform_parse(COLOR_TRANSFORM *colt, pdata *state, char *col_buf, ui32 limit)
{
	return 0;
}

#undef C_BOUNDS_EVAL

/*---------------------------------------------------------Top-level parsing---------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------|-----------------------------------------------------------------*/

err file_header_verification(pdata *state)
{
	err ret = swf_rect_parse(&(state->movie_rect), state, state->u_movie, state->movie_size);
	if(ER_ERROR(ret))
	{
		return ret;
	}

	int offset_end = M_ALIGN((5+(state->movie_rect.field_size * 4)), 3)>>3;
	if(M_BUF_BOUNDS_CHECK(state->u_movie, offset_end + 4, state))
	{
		return ESW_SHORTFILE;
	}

	state->movie_fr.lo = state->u_movie[offset_end];
	state->movie_fr.hi = state->u_movie[offset_end + 1];
	state->movie_frame_count = geti16(state->u_movie + offset_end + 2);

	offset_end += 4;

	state->tag_buffer = state->u_movie + offset_end;

	err_ptr new_tag = spawn_tag(F_FILEHEADER, offset_end, state->u_movie);
	if(ER_ERROR(new_tag.ret))
	{
		return new_tag.ret;
	}

	new_tag.ret = push_tag(state, new_tag.pointer);
	if(ER_ERROR(new_tag.ret))
	{
		return new_tag.ret;
	}

	new_tag.ret = push_scope(state, new_tag.pointer);
	if(ER_ERROR(new_tag.ret))
	{
		return new_tag.ret;
	}

	return 0;
}

// Checks tag stream
err check_tag_stream(pdata *state)
{
	state->tag_stream = NULL;
	state->tag_stream_end = NULL;
	err ret_err = file_header_verification(state);
	if(ret_err)
	{
		return ret_err;
	}

	swf_tag *last_tag = NULL;
	char *buffer = state->tag_buffer;
	while(1)
	{
		err_ptr stream_val = get_tag(buffer, state);
		if(stream_val.ret)
		{
			// Diagnostics go here, for now just free tag
			if(stream_val.pointer)
			{
				free(stream_val.pointer);
			}
			return stream_val.ret;
		}

		err ret_check = push_tag(state, stream_val.pointer);
		if(ER_ERROR(ret_check))
		{
			return ret_check;
		}

		last_tag = (swf_tag *)(state->tag_stream_end->data);

		err_ptr tag_ret = check_tag(last_tag, state);
		if(tag_ret.ret)
		{
			// Diagnostics go here
			return tag_ret.ret;
		}
		buffer = last_tag->tag_data + last_tag->size;
		if(buffer > state->u_movie + state->movie_size)
		{
			if(last_tag->tag != T_END || state->scope_stack)
			{
				return ESW_IMPROPER;
			}
			return 0;
		}
	}
}

// The FILE cursor should point at the beginning of the swf signature/file
err check_file_validity(FILE *swf, pdata *state)
{
	char signature[8];

	if(fread(signature, 1, 8, swf) < 8)
	{
		return EFL_READ;
	}

	if(signature[1] != 'W' || signature [2] != 'S')		// Not using short because we abide by standards to the best of our abilities here
	{
		return ESW_SIGNATURE;
	}
	state->version = signature[3];
	state->movie_size = geti32(signature+4);	// We will be using these proxies for calculations because int size may differ from 32 bits
	if(signed_comparei32(state->movie_size, 0) <= 0)
	{
		return ESW_SIGNATURE;
	}

	state->compression = signature[0];
	err ret_err;

	switch(state->compression)
	{
		case 'F':
			ret_err = movie_uncomp(swf, state);	// Returns error if file length is less than advertised in movie_size, otherwise loads movie_size bytes after the signature in u_movie and returns 0
			break;
		case 'C':
			ret_err = movie_deflate(swf, state); // For now, the compressed ones simply exit
			break;
		case 'Z':
			ret_err = movie_lzma(swf, state);
			break;
		default:
			return ESW_SIGNATURE;
	}
	if(ret_err)
	{
		return ret_err;
	}

	return check_tag_stream(state);
}
