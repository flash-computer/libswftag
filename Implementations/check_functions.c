#include"../Headers/swftag.h"
#include"../Headers/error.h"
#include"../Headers/check_functions.h"
#include"../Headers/swfmath.h"
#include"../Headers/decompression.h"

#include<stdio.h>
#include<stdlib.h>

// Considering just making a global variable to handle temp error values on these macros but that seems contrived
#define C_RAISE_ERR(error) ER_RAISE_ERROR_ERR(handler_ret, error, state)
#define C_RAISE_ERR_PTR(pointer, error) ER_RAISE_ERROR_ERR_PTR(handler_ret, pointer, error, state)
#define C_RAISE_ERR_INT(integer, error) ER_RAISE_ERROR_ERR_INT(handler_ret, integer, error, state)

/*------------------------------------------------------------Static Data------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------|-----------------------------------------------------------------*/

static err (*tag_check[])(swf_tag *, pdata *state) = {&check_end, &check_showframe, &check_defineshape, &check_freecharacter, &check_placeobject, &check_removeobject, &check_definebitsjpeg, &check_definebutton, &check_jpegtables, &check_setbackgroundcolor, &check_definefont, &check_definetext, &check_doaction, &check_definefontinfo, &check_definesound, &check_startsound, &check_invalidtag, &check_definebuttonsound, &check_soundstreamhead, &check_soundstreamblock, &check_definebitslossless, &check_definebitsjpeg2, &check_defineshape2, &check_definebuttoncxform, &check_protect, &check_pathsarepostscript, &check_placeobject2, &check_invalidtag, &check_removeobject2, &check_syncframe, &check_invalidtag, &check_freeall, &check_defineshape3, &check_definetext2, &check_definebutton2, &check_definebitsjpeg3, &check_definebitslossless2, &check_defineedittext, &check_definevideo, &check_definesprite, &check_namecharacter, &check_productinfo, &check_definetextformat, &check_framelabel, &check_invalidtag, &check_soundstreamhead2, &check_definemorphshape, &check_generateframe, &check_definefont2, &check_generatorcommand, &check_definecommandobject, &check_characterset, &check_externalfont, &check_invalidtag, &check_invalidtag, &check_invalidtag, &check_export, &check_import, &check_enabledebugger, &check_doinitaction, &check_definevideostream, &check_videoframe, &check_definefontinfo2, &check_debugid, &check_enabledebugger2, &check_scriptlimits, &check_settabindex, &check_invalidtag, &check_invalidtag, &check_fileattributes, &check_placeobject3, &check_import2, &check_doabcdefine, &check_definefontalignzones, &check_csmtextsettings, &check_definefont3, &check_symbolclass, &check_metadata, &check_definescalinggrid, &check_invalidtag, &check_invalidtag, &check_invalidtag, &check_doabc, &check_defineshape4, &check_definemorphshape2, &check_invalidtag, &check_definesceneandframedata, &check_definebinarydata, &check_definefontname, &check_invalidtag, &check_definebitsjpeg4};

/*-------------------------------------------------------------Functions-------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------|-----------------------------------------------------------------*/

err_ptr get_tag(uchar *buffer, pdata *state)
{
	err handler_ret;
	if(M_BUF_BOUNDS_CHECK(buffer, 2, state))
	{
		C_RAISE_ERR_PTR(NULL, ESW_SHORTFILE);
	}
	swf_tag *tag = malloc(sizeof(swf_tag));
	if(!tag)
	{
		C_RAISE_ERR_PTR(NULL, EMM_ALLOC);
	}
	tag->tag_and_size = geti16((uchar *)buffer);
	tag->tag = (tag->tag_and_size & 0xFFC0)>>6;
	tag->size = (tag->tag_and_size & 0x3F);
	tag->tag_data = buffer + 2;
	if(tag->size == 63 || tag_long_exclusive(tag->tag))
	{
		if(M_BUF_BOUNDS_CHECK(buffer, 4, state))
		{
			C_RAISE_ERR_PTR(tag, ESW_SHORTFILE);
		}
		tag->size = geti32((uchar *)buffer + 2);
		tag->tag_data = buffer + 6;
	}
	return (err_ptr){tag, 0};
}

// This is the biggest part of the project and thus the one I'd need the most help with.
// Evaluation would have to be conditional with the version outlined in the pdata structure. All of the prototypes above would have to be fleshed out.
// The struct that the pointer returns hasn't been made yet but it would be diagnostic struct outlining exactly what is wrong in case of an error.
err_ptr check_tag(swf_tag *tag, pdata *state)
{
	err handler_ret;
	if(!tag || !state)
	{
		C_RAISE_ERR_PTR(NULL, EFN_ARGS);
	}
	ui8 real_tag = tag_valid(tag->tag);
	if(!real_tag)
	{
		handler_ret = push_peculiarity(state, PEC_INVAL_TAG, tag->tag_data - state->u_movie);	// You can terminate at invalid tags in the callback here if you so wish
		if(ER_ERROR(handler_ret))
		{
			return (err_ptr){NULL, handler_ret};
		}
	}
	else
	{
		err_int ret = tag_version_compare(tag->tag, state);
		if(ER_ERROR(ret.ret))
		{
			return (err_ptr){NULL, ret.ret};
		}
		if(!ret.integer)
		{
			handler_ret = push_peculiarity(state, PEC_TIME_TRAVEL, tag->tag_data - state->u_movie);
			if(ER_ERROR(handler_ret))
			{
				return (err_ptr){NULL, handler_ret};
			}
		}
	}
	// Check function calls and the rest of the stuff will go here
	if(real_tag)
	{
		err ret_check = tag_check[tag->tag](tag, state);
		if(ER_ERROR(ret_check))
		{
			return (err_ptr){NULL, ret_check};
		}
	}
	return (err_ptr){NULL, 0};
}

err_ptr spawn_tag(int tag, ui32 size, uchar *tag_data, pdata *state)
{
	err handler_ret;
	if(!tag_valid(tag) && tag != F_FILEHEADER)
	{
		C_RAISE_ERR_PTR(NULL, EFN_ARGS);
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
		C_RAISE_ERR_PTR(NULL, EMM_ALLOC);
	}
	new_tag->tag = tag;
	new_tag->size = size;
	new_tag->tag_and_size = ((tag<<6) & 0xFFC0) | ((size > 62 || ((tag != F_FILEHEADER)? tag_long_exclusive(tag) : 0))? 0x3F : (size & 0x3F));
	new_tag->tag_data = tag_data;
	return (err_ptr){new_tag, 0};
}

/*--------------------------------------------------------Parse substructures--------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------|-----------------------------------------------------------------*/

// TODO: Make these context aware by making them return err_int to output the size of the structure in bits and pass the swf_tag in arguments instead of limit because some substructures have additional properties based on that
// Only for integrity checks in substructure parsing. ESW_SHORTFILE if errror
#define C_BOUNDS_EVAL(tagbuffer, offset, pdata, limit, insuf_err) if(M_BUF_BOUNDS_CHECK(tagbuffer, offset, pdata)){C_RAISE_ERR_INT(0, ESW_SHORTFILE);}if(limit < offset){C_RAISE_ERR_INT(0, insuf_err);}

err_int swf_rect_parse(RECT *rect, pdata *state, uchar *rect_buf, swf_tag *tag)
{
	err handler_ret;
	if(!state || !rect || !rect_buf)
	{
		C_RAISE_ERR_INT(0, EFN_ARGS);
	}
	ui32 limit = state->movie_size;
	if(tag)		// Additional test for when the tag is F_FILEHEADER, and the argument consequently passed is null, later I'll separate out movie_rect, movie_frame_rate and movie_frame_count from pdata into a swf_pseudotag_fileheader struct, and then the tag it can just be any normal tag
	{
		limit = tag->size - (rect_buf - tag->tag_data);
	}
	C_BOUNDS_EVAL(rect_buf, 1, state, limit, ESW_IMPROPER);

	rect->field_size = (rect_buf[0] & 0xF8)>>3;	// Since the byte is right aligned, works for higher values of CHAR_BIT just fine

	C_BOUNDS_EVAL(state->u_movie, M_ALIGN((5+(rect->field_size * 4)), 3)>>3, state, limit, ESW_IMPROPER);

	ui8 offset = 5;
	for(ui8 field = 0; field < 4; field++)
	{
		rect->fields[field] = get_signed_bitfield((uchar *)rect_buf, 5 + (rect->field_size * field), rect->field_size);
		offset += rect->field_size;
	}

	if(get_bitfield_padding(rect_buf, offset))
	{
		return (err_int){offset, push_peculiarity(state, PEC_RECTPADDING, (tag)? (tag->tag_data - state->u_movie) : 0)};
	}
	return (err_int){offset, 0};
}

err_int swf_matrix_parse(MATRIX *mat, pdata *state, uchar *mat_buf, swf_tag *tag)
{
	err handler_ret;
	if(!tag || !state || !mat || !mat_buf)
	{
		C_RAISE_ERR_INT(0, EFN_ARGS);
	}
	ui32 limit = tag->size - (mat_buf - tag->tag_data);

	// Default values
	mat->scale_x = (uf16_16){1,0};
	mat->scale_x = (uf16_16){1,0};

	mat->rotate_skew0 = (uf16_16){0,0};
	mat->rotate_skew1 = (uf16_16){0,0};

	C_BOUNDS_EVAL(mat_buf, 1, state, limit, ESW_IMPROPER);

	mat->bitfields = 0;
	ui32 offset = 1;
	if(mat_buf[0] & 0x80)
	{
		mat->bitfields |= 0x1;
		mat->scale_bits = get_bitfield(mat_buf, 1, 5);

		C_BOUNDS_EVAL(mat_buf, M_ALIGN(6+mat->scale_bits * 2, 3)>>3, state, limit, ESW_IMPROPER);

		mat->scale_x = get_signed_bitfield_fixed(mat_buf, 6, mat->scale_bits);
		mat->scale_y = get_signed_bitfield_fixed(mat_buf, 6 + mat->scale_bits, mat->scale_bits);
		offset += 5 + (mat->scale_bits * 2);
	}
	C_BOUNDS_EVAL(mat_buf, M_ALIGN(offset + 1, 3)>>3, state, limit, ESW_IMPROPER);

	if(get_bitfield(mat_buf, offset, 1))
	{
		mat->bitfields |= 0x10;

		C_BOUNDS_EVAL(mat_buf, M_ALIGN(offset + 6, 3)>>3, state, limit, ESW_IMPROPER);
		mat->rotate_bits = get_bitfield(mat_buf, offset + 1, 5);

		C_BOUNDS_EVAL(mat_buf, M_ALIGN(offset + 6 + (mat->rotate_bits * 2), 3)>>3, state, limit, ESW_IMPROPER);
		mat->rotate_skew0 = get_signed_bitfield_fixed(mat_buf, 6 + offset, mat->rotate_bits);
		mat->rotate_skew1 = get_signed_bitfield_fixed(mat_buf, 6 + offset + mat->rotate_bits, mat->rotate_bits);
		offset += 5 + (mat->rotate_bits * 2);
	}
	offset++;

	C_BOUNDS_EVAL(mat_buf, M_ALIGN(offset + 5, 3)>>3, state, limit, ESW_IMPROPER);
	mat->translate_bits = get_bitfield(mat_buf, offset + 1, 5);

	C_BOUNDS_EVAL(mat_buf, M_ALIGN(offset + 5 + (mat->translate_bits * 2), 3)>>3, state, limit, ESW_IMPROPER);
	mat->translate_x = get_signed_bitfield_fixed(mat_buf, 5 + offset, mat->translate_bits);
	mat->translate_y = get_signed_bitfield_fixed(mat_buf, 5 + offset + mat->translate_bits, mat->translate_bits);
	offset += 5 + (mat->translate_bits * 2);

	if(get_bitfield_padding(mat_buf, offset))
	{
		return (err_int){offset, push_peculiarity(state, PEC_RECTPADDING, 0)};
	}

	return (err_int){offset, 0};
}

err_int swf_color_transform_parse(COLOR_TRANSFORM *colt, pdata *state, uchar *colt_buf, swf_tag *tag)
{
	err handler_ret;
	if(!tag || !state || !colt || !colt_buf)
	{
		C_RAISE_ERR_INT(0, EFN_ARGS);
	}

	ui32 limit = tag->size - (colt_buf - tag->tag_data);

	// Default Values
	colt->red_mult = (uf16_16){1,0};
	colt->green_mult = (uf16_16){1,0};
	colt->blue_mult = (uf16_16){1,0};
	colt->alpha_mult = (uf16_16){1,0};

	colt->red_add = (uf16_16){0,0};
	colt->green_add = (uf16_16){0,0};
	colt->blue_add = (uf16_16){0,0};
	colt->alpha_add = (uf16_16){0,0};

	C_BOUNDS_EVAL(colt_buf, 1, state, limit, 0);
	colt->bitfields = get_bitfield(colt_buf, 0, 1);
	colt->bitfields |= get_bitfield(colt_buf, 1, 1)<<4;
	colt->color_bits = get_bitfield(colt_buf, 2, 4);

	ui32 offset = 6;

	if(colt->bitfields & 0x10)
	{
		C_BOUNDS_EVAL(colt_buf, M_ALIGN(offset + colt->color_bits * 3, 3)>>3, state, limit, 0);

		colt->red_mult = get_signed_bitfield_fixed(colt_buf, offset, colt->color_bits);
		colt->green_mult = get_signed_bitfield_fixed(colt_buf, offset + colt->color_bits, colt->color_bits);
		colt->blue_mult = get_signed_bitfield_fixed(colt_buf, offset + (colt->color_bits<<1), colt->color_bits);
		offset += colt->color_bits * 3;
		if(tag->tag == T_PLACEOBJECT2)
		{
			C_BOUNDS_EVAL(colt_buf, M_ALIGN(offset + colt->color_bits, 3)>>3, state, limit, 0);

			colt->alpha_mult = get_signed_bitfield_fixed(colt_buf, offset, colt->color_bits);
			offset += colt->color_bits;
		}
	}

	if(colt->bitfields & 0x10)
	{
		C_BOUNDS_EVAL(colt_buf, M_ALIGN(offset + colt->color_bits * 3, 3)>>3, state, limit, 0);

		colt->red_add = get_signed_bitfield_fixed(colt_buf, offset, colt->color_bits);
		colt->green_add = get_signed_bitfield_fixed(colt_buf, offset + colt->color_bits, colt->color_bits);
		colt->blue_add = get_signed_bitfield_fixed(colt_buf, offset + (colt->color_bits<<1), colt->color_bits);
		offset += colt->color_bits * 3;
		if(tag->tag == T_PLACEOBJECT2)
		{
			C_BOUNDS_EVAL(colt_buf, M_ALIGN(offset + colt->color_bits, 3)>>3, state, limit, 0);

			colt->alpha_add = get_signed_bitfield_fixed(colt_buf, offset, colt->color_bits);
			offset += colt->color_bits;
		}
	}

	if(get_bitfield_padding(colt_buf, offset))
	{
		return (err_int){offset, push_peculiarity(state, PEC_RECTPADDING, 0)};
	}

	return (err_int){offset, 0};
}

#undef C_BOUNDS_EVAL

/*---------------------------------------------------------Top-level parsing---------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------|-----------------------------------------------------------------*/

err file_header_verification(pdata *state)
{
	err handler_ret;
	err_int ret = swf_rect_parse(&(state->movie_rect), state, (uchar *)state->u_movie, NULL);
	if(ER_ERROR(ret.ret))
	{
		return ret.ret;
	}

	int offset_end = M_ALIGN((ret.integer), 3)>>3;
	if(M_BUF_BOUNDS_CHECK(state->u_movie, offset_end + 4, state))
	{
		C_RAISE_ERR(ESW_SHORTFILE);
	}

	state->movie_fr.lo = state->u_movie[offset_end];
	state->movie_fr.hi = state->u_movie[offset_end + 1];
	state->movie_frame_count = geti16((uchar *)state->u_movie + offset_end + 2);

	offset_end += 4;

	state->tag_buffer = state->u_movie + offset_end;

	err_ptr new_tag = spawn_tag(F_FILEHEADER, offset_end, state->u_movie, state);
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

// TODO: Better bounds checking near the end after false size reporting is handled
// Checks tag stream
err check_tag_stream(pdata *state)
{
	err handler_ret;
	state->tag_stream = NULL;
	state->tag_stream_end = NULL;
	err ret_err = file_header_verification(state);
	if(ret_err)
	{
		return ret_err;
	}

	swf_tag *last_tag = NULL;
	uchar *buffer = state->tag_buffer;
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
		if((uchar *)buffer >= (uchar *)(state->u_movie + state->movie_size))
		{
			if(last_tag->tag != T_END || state->scope_stack)
			{
				C_RAISE_ERR(ESW_IMPROPER);
			}
			return 0;
		}
	}
}

// The FILE cursor should point at the beginning of the swf signature/file
err check_file_validity(FILE *swf, pdata *state)
{
	err handler_ret;
	uchar *signature = state->signature;

	if(fread(signature, 1, 8, swf) < 8)
	{
		C_RAISE_ERR(EFL_READ);
	}

	if(signature[1] != 'W' || signature [2] != 'S')		// Not using short because we abide by standards to the best of our abilities here
	{
		C_RAISE_ERR(ESW_SIGNATURE);
	}
	state->version = signature[3];
	state->movie_size = geti32((uchar *)signature + 4) - 8;	// We will be using these proxies for calculations because int size may differ from 32 bits
	if(signed_comparei32(state->movie_size, 0) <= 0)
	{
		C_RAISE_ERR(ESW_SIGNATURE);
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
			C_RAISE_ERR(ESW_SIGNATURE);
	}
	if(ret_err)
	{
		return ret_err;
	}

	return check_tag_stream(state);
}

#undef C_RAISE_ERR_INT
#undef C_RAISE_ERR_PTR
#undef C_RAISE_ERR
