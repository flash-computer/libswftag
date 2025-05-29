#include<libswftag/swftag.h>
#include<libswftag/error.h>

#define SWFTAG_MATH_INLINE
#include<libswftag/swfmath.h>
#include<libswftag/decompression.h>

#include<stdio.h>
#include<stdlib.h>

// Considering just making a global variable to handle temp error values on these macros but that seems contrived
#define C_RAISE_ERR(error) {err handler_ret; ER_RAISE_ERROR_ERR(handler_ret, state, error);}
#define C_RAISE_ERR_PTR(pointer, error) {err handler_ret; ER_RAISE_ERROR_ERR_PTR(handler_ret, pointer, state, error);}
#define C_RAISE_ERR_INT(integer, error) {err handler_ret; ER_RAISE_ERROR_ERR_INT(handler_ret, integer, state, error);}

/*-------------------------------------------------------------Functions-------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------|-----------------------------------------------------------------*/

err_ptr get_tag(pdata *state, uchar *buffer)
{
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
	tag->tag_id = 0;	// Not all tag types have an id, infact few do, but it's still better to have that here in my humble opinion
	if(tag->size == 63)
	{
		if(M_BUF_BOUNDS_CHECK(buffer, 6, state))
		{
			C_RAISE_ERR_PTR(tag, ESW_SHORTFILE);
		}
		tag->size = geti32((uchar *)buffer + 2);
		tag->tag_data = buffer + 6;
	}
	else if(tag_long_exclusive(tag->tag))
	{
		err ret = push_peculiarity(state, PEC_UNCONVENTIONAL_SHORT_TAG, uchar_safe_ptrdiff(buffer, state->u_movie));
		if(ER_ERROR(ret))
		{
			return (err_ptr){NULL, ret};
		}
	}
	return (err_ptr){tag, 0};
}

// Generates a generic tag. Use with copy_push_tag to add it to the tag stream and then free it afterwards
err_ptr spawn_tag(pdata *state, int tag, ui32 size, uchar *tag_data)
{
	if(!tag_valid(tag) && tag != F_FILEHEADER)
	{
		C_RAISE_ERR_PTR(NULL, EFN_ARGS);
	}

	swf_tag *new_tag = malloc(sizeof(swf_tag));
	if(!new_tag)
	{
		C_RAISE_ERR_PTR(NULL, EMM_ALLOC);
	}
	new_tag->tag = tag;
	new_tag->size = size;
	new_tag->tag_and_size = ((tag<<6) & 0xFFC0) | ((size > 62 || ((tag != F_FILEHEADER)? tag_long_exclusive(tag) : 0))? 0x3F : (size & 0x3F));
	new_tag->tag_data = tag_data;
	new_tag->tag_id = 0;
	new_tag->parent_node = NULL;
	new_tag->tag_struct = NULL;
	return (err_ptr){new_tag, 0};
}

/*--------------------------------------------------------Parse substructures--------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------|-----------------------------------------------------------------*/

// Only for integrity checks in substructure parsing. ESW_SHORTFILE if errror
#define C_BOUNDS_EVAL(tagbuffer, offset, pdata, limit, insuf_err) if(M_BUF_BOUNDS_CHECK(tagbuffer, offset, pdata)){C_RAISE_ERR_INT(0, ESW_SHORTFILE);}if(limit < offset){C_RAISE_ERR_INT(0, insuf_err);}

err_int swf_rect_parse(pdata *state, RECT *rect, uchar *buf, swf_tag *tag)
{
	if(!state || !rect || !buf)
	{
		C_RAISE_ERR_INT(0, EFN_ARGS);
	}
	if(buf < tag->tag_data)
	{
		C_RAISE_ERR_INT(0, EFN_ARGS);
	}
	if(tag->size < uchar_safe_ptrdiff(buf, tag->tag_data))
	{
		C_RAISE_ERR_INT(0, EFN_ARGS);
	}
	ui32 limit = (tag->size - uchar_safe_ptrdiff(buf, tag->tag_data));

	C_BOUNDS_EVAL(buf, 1, state, limit, ESW_IMPROPER);

	rect->field_size = (M_SANITIZE_BYTE(buf[0]) & 0xF8)>>3;	// Since the byte is right aligned, works for higher values of CHAR_BIT just fine

	C_BOUNDS_EVAL(state->u_movie, M_ALIGN((5+(rect->field_size * 4)), 3)>>3, state, limit, ESW_IMPROPER);

	ui8 offset = 5;
	for(ui8 field = 0; field < 4; field++)
	{
		rect->fields[field] = get_signed_bitfield((uchar *)buf, 5 + (rect->field_size * field), rect->field_size);
		offset += rect->field_size;
	}

	if(get_bitfield_padding(buf, offset))
	{
		return (err_int){offset, push_peculiarity(state, PEC_BITFIELD_PADDING, (tag)? (tag->tag_data - state->u_movie) : 0)};
	}
	return (err_int){offset, 0};
}

err_int swf_matrix_parse(pdata *state, MATRIX *mat, uchar *buf, swf_tag *tag)
{
	if(!tag || !state || !mat || !buf)
	{
		C_RAISE_ERR_INT(0, EFN_ARGS);
	}
	if(buf < tag->tag_data)
	{
		C_RAISE_ERR_INT(0, EFN_ARGS);
	}
	if(tag->size < uchar_safe_ptrdiff(buf, tag->tag_data))
	{
		C_RAISE_ERR_INT(0, EFN_ARGS);
	}
	ui32 limit = (tag->size - uchar_safe_ptrdiff(buf, tag->tag_data));

	// Default values
	mat->scale_x = (uf16_16){1,0};
	mat->scale_x = (uf16_16){1,0};

	mat->rotate_skew0 = (uf16_16){0,0};
	mat->rotate_skew1 = (uf16_16){0,0};

	C_BOUNDS_EVAL(buf, 1, state, limit, ESW_IMPROPER);

	mat->bitfields = 0;
	ui32 offset = 1;
	if(M_SANITIZE_BYTE(buf[0]) & 0x80)
	{
		mat->bitfields |= 0x1;
		mat->scale_bits = get_bitfield(buf, 1, 5);

		C_BOUNDS_EVAL(buf, M_ALIGN(6+mat->scale_bits * 2, 3)>>3, state, limit, ESW_IMPROPER);

		mat->scale_x = get_signed_bitfield_fixed(buf, 6, mat->scale_bits);
		mat->scale_y = get_signed_bitfield_fixed(buf, 6 + mat->scale_bits, mat->scale_bits);
		offset += 5 + (mat->scale_bits * 2);
	}
	C_BOUNDS_EVAL(buf, M_ALIGN(offset + 1, 3)>>3, state, limit, ESW_IMPROPER);

	if(get_bitfield(buf, offset, 1))
	{
		mat->bitfields |= 0x10;

		C_BOUNDS_EVAL(buf, M_ALIGN(offset + 6, 3)>>3, state, limit, ESW_IMPROPER);
		mat->rotate_bits = get_bitfield(buf, offset + 1, 5);

		C_BOUNDS_EVAL(buf, M_ALIGN(offset + 6 + (mat->rotate_bits * 2), 3)>>3, state, limit, ESW_IMPROPER);
		mat->rotate_skew0 = get_signed_bitfield_fixed(buf, 6 + offset, mat->rotate_bits);
		mat->rotate_skew1 = get_signed_bitfield_fixed(buf, 6 + offset + mat->rotate_bits, mat->rotate_bits);
		offset += 5 + (mat->rotate_bits * 2);
	}
	offset++;

	C_BOUNDS_EVAL(buf, M_ALIGN(offset + 5, 3)>>3, state, limit, ESW_IMPROPER);
	mat->translate_bits = get_bitfield(buf, offset, 5);

	C_BOUNDS_EVAL(buf, M_ALIGN(offset + 5 + (mat->translate_bits * 2), 3)>>3, state, limit, ESW_IMPROPER);
	mat->translate_x = get_signed_bitfield_fixed(buf, 5 + offset, mat->translate_bits);
	mat->translate_y = get_signed_bitfield_fixed(buf, 5 + offset + mat->translate_bits, mat->translate_bits);
	offset += 5 + (mat->translate_bits * 2);

	if(get_bitfield_padding(buf, offset))
	{
		return (err_int){offset, push_peculiarity(state, PEC_BITFIELD_PADDING, 0)};
	}

	return (err_int){offset, 0};
}

err_int swf_color_transform_parse(pdata *state, COLOR_TRANSFORM *colt, uchar *buf, swf_tag *tag)
{
	if(!tag || !state || !colt || !buf)
	{
		C_RAISE_ERR_INT(0, EFN_ARGS);
	}
	if(buf < tag->tag_data)
	{
		C_RAISE_ERR_INT(0, EFN_ARGS);
	}
	if(tag->size < uchar_safe_ptrdiff(buf, tag->tag_data))
	{
		C_RAISE_ERR_INT(0, EFN_ARGS);
	}
	ui32 limit = (tag->size - uchar_safe_ptrdiff(buf, tag->tag_data));

	// Default Values
	colt->red_mult = (uf16_16){1,0};
	colt->green_mult = (uf16_16){1,0};
	colt->blue_mult = (uf16_16){1,0};
	colt->alpha_mult = (uf16_16){1,0};

	colt->red_add = (uf16_16){0,0};
	colt->green_add = (uf16_16){0,0};
	colt->blue_add = (uf16_16){0,0};
	colt->alpha_add = (uf16_16){0,0};

	C_BOUNDS_EVAL(buf, 1, state, limit, 0);
	colt->bitfields = get_bitfield(buf, 0, 1);
	colt->bitfields |= get_bitfield(buf, 1, 1)<<4;
	colt->color_bits = get_bitfield(buf, 2, 4);

	ui32 offset = 6;

	if(colt->bitfields & 0x10)
	{
		C_BOUNDS_EVAL(buf, M_ALIGN(offset + colt->color_bits * 3, 3)>>3, state, limit, 0);

		colt->red_mult = get_signed_bitfield_fixed(buf, offset, colt->color_bits);
		colt->green_mult = get_signed_bitfield_fixed(buf, offset + colt->color_bits, colt->color_bits);
		colt->blue_mult = get_signed_bitfield_fixed(buf, offset + (colt->color_bits<<1), colt->color_bits);
		offset += colt->color_bits * 3;
		if(tag->tag == T_PLACEOBJECT2)
		{
			C_BOUNDS_EVAL(buf, M_ALIGN(offset + colt->color_bits, 3)>>3, state, limit, 0);

			colt->alpha_mult = get_signed_bitfield_fixed(buf, offset, colt->color_bits);
			offset += colt->color_bits;
		}
	}

	if(colt->bitfields & 0x10)
	{
		C_BOUNDS_EVAL(buf, M_ALIGN(offset + colt->color_bits * 3, 3)>>3, state, limit, 0);

		colt->red_add = get_signed_bitfield_fixed(buf, offset, colt->color_bits);
		colt->green_add = get_signed_bitfield_fixed(buf, offset + colt->color_bits, colt->color_bits);
		colt->blue_add = get_signed_bitfield_fixed(buf, offset + (colt->color_bits<<1), colt->color_bits);
		offset += colt->color_bits * 3;
		if(tag->tag == T_PLACEOBJECT2)
		{
			C_BOUNDS_EVAL(buf, M_ALIGN(offset + colt->color_bits, 3)>>3, state, limit, 0);

			colt->alpha_add = get_signed_bitfield_fixed(buf, offset, colt->color_bits);
			offset += colt->color_bits;
		}
	}

	if(get_bitfield_padding(buf, offset))
	{
		return (err_int){offset, push_peculiarity(state, PEC_BITFIELD_PADDING, 0)};
	}

	return (err_int){offset, 0};
}

err_int swf_text_record_parse(pdata *state, TEXT_RECORD *trec, uchar *buf, swf_tag *tag)
{
	if(!tag || !state || !trec || !buf)
	{
		C_RAISE_ERR_INT(0, EFN_ARGS);
	}
	if(buf < tag->tag_data)
	{
		C_RAISE_ERR_INT(0, EFN_ARGS);
	}
	if(tag->tag != T_DEFINETEXT2 && tag->tag != T_DEFINETEXT)
	{
		C_RAISE_ERR_INT(0, EFN_ARGS)
	}
	if(!(tag->tag_struct))
	{
		C_RAISE_ERR_INT(0, EFN_ARGS);
	}
	if(tag->size < uchar_safe_ptrdiff(buf, tag->tag_data))
	{
		C_RAISE_ERR_INT(0, EFN_ARGS);
	}

	struct swf_tag_definetextx *text = (struct swf_tag_definetextx *)(tag->tag_struct);

	ui32 limit = (tag->size - uchar_safe_ptrdiff(buf, tag->tag_data));
	ui32 offset = 0;
	
	ui8 diff = (tag->tag == T_DEFINETEXT2);
	text->family_version = diff + 1;
	ui32 bit_itr = 0;

	C_BOUNDS_EVAL(buf + offset, 1, state, limit, ESW_IMPROPER);
	ui8 firstbyte = M_SANITIZE_BYTE(buf[offset]);
	if((firstbyte & 0xF0) != 0x80)
	{
		err ret = push_peculiarity(state, PEC_RESERVE_TAMPERED, 0);
		if(ER_ERROR(ret))
		{
			return (err_int){0, ret};
		}
	}
	offset++;
	firstbyte &= 0xF;
	trec->bitfields = firstbyte & 0xF;

	if(firstbyte & 0x8)
	{
		C_BOUNDS_EVAL(buf + offset, 2, state, limit, ESW_IMPROPER);
		trec->define_font_id = M_SANITIZE_UI16(geti16(buf + offset));
		offset += 2;
		// TODO: Find and connect the tag relevant to the id after the DefineFont tag check and registration is implemented
	}
	if(firstbyte & 0x4)
	{
		C_BOUNDS_EVAL(buf + offset, 3 + diff, state, limit, ESW_IMPROPER);
		trec->color.red = M_SANITIZE_BYTE(buf[offset]);
		trec->color.green = M_SANITIZE_BYTE(buf[offset+1]);
		trec->color.blue = M_SANITIZE_BYTE(buf[offset+2]);
		trec->color.alpha = 0xFF;
		if(diff)
		{
			trec->color.alpha = M_SANITIZE_BYTE(buf[offset+3]);
		}
		offset += 3+diff;
	}
	if(firstbyte & 0x2)
	{
		C_BOUNDS_EVAL(buf + offset, 2, state, limit, ESW_IMPROPER);
		trec->move_x = M_SANITIZE_UI16(geti16(buf + offset));
		offset += 2;
	}
	if(firstbyte & 0x1)
	{
		C_BOUNDS_EVAL(buf + offset, 2, state, limit, ESW_IMPROPER);
		trec->move_y = M_SANITIZE_UI16(geti16(buf + offset));
		offset += 2;
	}
	if(firstbyte & 0x8)
	{
		C_BOUNDS_EVAL(buf + offset, 2, state, limit, ESW_IMPROPER);
		trec->font_height = M_SANITIZE_UI16(geti16(buf + offset));
		offset += 2;
	}

	C_BOUNDS_EVAL(buf + offset, 1, state, limit, ESW_IMPROPER);
	trec->glyph_count = M_SANITIZE_BYTE(buf[offset]);

	offset++;

	ui8 glyph_width = text->glyph_bits;
	ui8 advance_width = text->advance_bits;

	C_BOUNDS_EVAL(buf + offset, M_ALIGN((trec->glyph_count) * (glyph_width + advance_width), 3)>>3, state, limit, ESW_IMPROPER);
	err_ptr ret_ptr = alloc_push_freelist(state, sizeof(GLYPHENTRY) * trec->glyph_count, tag->parent_node);
	if(ER_ERROR(ret_ptr.ret))
	{
		return (err_int){0, ret_ptr.ret};
	}
	trec->entries = (GLYPHENTRY *)(ret_ptr.pointer);

	for(ui32 i=0; i<(trec->glyph_count); i++)
	{
		trec->entries[i].glyph_index = get_bitfield(buf+offset, (i * (glyph_width + advance_width)), glyph_width);
		trec->entries[i].glyph_advance = get_signed_bitfield(buf+offset, (i * (glyph_width + advance_width)) + glyph_width, advance_width);
	}
	bit_itr = (trec->glyph_count) * (glyph_width + advance_width);

	if(get_bitfield_padding(buf + offset, bit_itr))
	{
		return (err_int){(offset<<3) + bit_itr, push_peculiarity(state, PEC_BITFIELD_PADDING, 0)};
	}
	return (err_int){(offset<<3) + bit_itr, 0};
}

err_int swf_text_record_list_parse(pdata *state, uchar *buf, swf_tag *tag)
{
	if(!tag || !state || !buf)
	{
		C_RAISE_ERR_INT(0, EFN_ARGS);
	}
	if(tag->tag != T_DEFINETEXT2 && tag->tag != T_DEFINETEXT)
	{
		C_RAISE_ERR_INT(0, EFN_ARGS)
	}
	if(!(tag->tag_struct))
	{
		C_RAISE_ERR_INT(0, EFN_ARGS);
	}
	if(buf < tag->tag_data) // N1256: "For the purposes of these operators, a pointer to an object that is not an element of an array behaves the same as a pointer to the first element of an array of length one with the type of the object as its element type.". Does this mean if buf is outside the movie, this comparision is still well defined? TODO. Regardless, the functions that call this ensure that it's within bounds.
	{
		C_RAISE_ERR_INT(0, EFN_ARGS);
	}
	if(tag->size < uchar_safe_ptrdiff(buf, tag->tag_data))
	{
		C_RAISE_ERR_INT(0, EFN_ARGS);
	}
	struct swf_tag_definetextx *text = (struct swf_tag_definetextx *)(tag->tag_struct);

	ui32 limit = (tag->size - uchar_safe_ptrdiff(buf, tag->tag_data));
	ui32 offset = 0;
	ui8 diff = (tag->tag == T_DEFINETEXT2);
	text->family_version = diff + 1;
	ui32 bit_itr = 0;
	dnode *head = text->records;

	while(1)
	{
		C_BOUNDS_EVAL(buf + offset, 1, state, limit, ESW_IMPROPER);
		ui8 checknull = M_SANITIZE_BYTE(buf[offset]);
		if(!checknull)
		{
			offset++;
			break;
		}

		err_ptr ret_ptr = alloc_push_freelist(state, sizeof(dnode) + sizeof(TEXT_RECORD), tag->parent_node);
		if(ER_ERROR(ret_ptr.ret))
		{
			return (err_int){0, ret_ptr.ret};
		}

		dnode *new_node = (dnode *)ret_ptr.pointer;
		new_node->prev = head;
		new_node->data = (void *)((uchar *)new_node + sizeof(dnode));
		new_node->to_free = NULL;
		new_node->next = NULL;

		if(head)
		{
			head->next = new_node;
		}
		else
		{
			text->records = new_node;
		}
		head = new_node;

		TEXT_RECORD *record = (TEXT_RECORD *)(new_node->data);
		err_int ret_int = swf_text_record_parse(state, record, buf+offset, tag);
		if(ER_ERROR(ret_int.ret))
		{
			return (err_int){0, ret_int.ret};
		}

		bit_itr = ret_int.integer;
		offset += M_ALIGN(bit_itr, 3)>>3;
		bit_itr &= 7;
	}
	// TODO: Just return the number of bytes. Number of bits can overflow ui32 for this.
	return (err_int){(offset<<3) + bit_itr, 0};
}

err_int swf_sound_info_parse(pdata *state, SOUND_INFO *soin, uchar *buf, swf_tag *tag)
{
	if(!tag || !state || !soin || !buf)
	{
		C_RAISE_ERR_INT(0, EFN_ARGS);
	}
	if(buf < tag->tag_data)
	{
		C_RAISE_ERR_INT(0, EFN_ARGS);
	}
	if(tag->size < uchar_safe_ptrdiff(buf, tag->tag_data))
	{
		C_RAISE_ERR_INT(0, EFN_ARGS);
	}
	ui32 limit = (tag->size - uchar_safe_ptrdiff(buf, tag->tag_data));
	ui32 offset = 0;

	C_BOUNDS_EVAL(buf, 2, state, limit, ESW_IMPROPER);

	soin->sound_id = geti16(buf);
	soin->sound_tag = NULL;
	if(tag->tag == T_DEFINEBUTTONSOUND && !(soin->sound_id))
	{
		return (err_int){2<<3, 0};
	}
	C_BOUNDS_EVAL(buf, 3, state, limit, ESW_IMPROPER);
	err_ptr ret = id_get_tag(state, soin->sound_id);
	if(ER_ERROR(ret.ret))
	{
		return (err_int){0, ret.ret};
	}
	soin->sound_tag = ret.pointer;
	soin->bitfields = M_SANITIZE_BYTE(buf[2]);
	if(soin->bitfields & 0xC0)
	{
		ret.ret = push_peculiarity(state, PEC_RESERVE_TAMPERED, 0);
		if(ER_ERROR(ret.ret))
		{
			return (err_int){0, ret.ret};
		}
		soin->bitfields = 0;
	}
	offset = 3;
	if(!(soin->sound_tag))
	{
		C_RAISE_ERR_INT(0, EFN_ARGS); // TODO : Add a new error type?
	}
	swf_tag *st = soin->sound_tag;
	if(st->tag != T_DEFINESOUND || st->tag_struct == NULL)
	{
		C_RAISE_ERR_INT(0, EFN_ARGS);
	}
	soin->in_point = 0;
	soin->out_point = ((struct swf_tag_definesound *)(st->tag_struct))->samples_count; // TODO: Refine defaults
	soin->loop_count = 0;
	soin->envelope_count = 0;
	soin->envelopes = NULL;

	if(soin->bitfields & 0x1)
	{
		C_BOUNDS_EVAL(buf+offset, 4, state, limit, ESW_IMPROPER);
		soin->in_point = geti32(buf+offset);
		offset += 4;
	}
	if(soin->bitfields & 0x2)
	{
		C_BOUNDS_EVAL(buf+offset, 4, state, limit, ESW_IMPROPER);
		soin->out_point = geti32(buf+offset);
		offset += 4;
	}
	if(soin->bitfields & 0x4)
	{
		C_BOUNDS_EVAL(buf+offset, 2, state, limit, ESW_IMPROPER);
		soin->loop_count = geti16(buf+offset);
		offset += 2;
	}
	if(soin->bitfields & 0x8)
	{
		C_BOUNDS_EVAL(buf+offset, 1, state, limit, ESW_IMPROPER);
		soin->envelope_count = M_SANITIZE_BYTE(buf[offset]);
		offset += 1;
		ret = alloc_push_freelist(state, (soin->envelope_count) * sizeof(ENVELOPE), tag->parent_node);
		if(ER_ERROR(ret.ret))
		{
			return (err_int){0, ret.ret};
		}
		soin->envelopes = (ENVELOPE *)ret.pointer;
		for(ui8 i=0; i<soin->envelope_count; i++)
		{
			C_BOUNDS_EVAL(buf+offset, 8, state, limit, ESW_IMPROPER);
			ENVELOPE *env = soin->envelopes + i;
			env->position = geti32(buf+offset);
			// TODO: position checks
			env->left_vol = geti16(buf+offset+4);
			env->right_vol = geti16(buf+offset+6);
			offset += 8;
		}
	}
	return (err_int){offset<<3, 0};
}

#undef C_BOUNDS_EVAL

/*---------------------------------------------------------Top-level parsing---------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------|-----------------------------------------------------------------*/

err file_header_verification(pdata *state)
{
	if(!state)
	{
		C_RAISE_ERR(EFN_ARGS);
	}

	err_ptr tag_ret = spawn_tag(state, F_FILEHEADER, state->movie_size, state->u_movie);
	if(ER_ERROR(tag_ret.ret))
	{
		return tag_ret.ret;
	}

	tag_ret.ret = copy_push_tag(state, *((swf_tag *)tag_ret.pointer));
	free(tag_ret.pointer);
	tag_ret.pointer = NULL;
	if(ER_ERROR(tag_ret.ret))
	{
		return tag_ret.ret;
	}

	tag_ret.ret = push_scope(state, state->tag_stream_end);
	if(ER_ERROR(tag_ret.ret))
	{
		return tag_ret.ret;
	}

	dnode *node = state->tag_stream_end;
	swf_tag *tag = (swf_tag *)(node->data);

	tag->parent_node = node;
	tag->tag_struct = &(state->header);

	FILEHEADER *header = &(state->header);

	CB_CALL(state, CB_PRE_TAG_CHECK);

	err_int ret = swf_rect_parse(state, &(header->movie_rect), (uchar *)state->u_movie, tag);
	if(ER_ERROR(ret.ret))
	{
		return ret.ret;
	}

	tag->size = M_ALIGN((ret.integer), 3)>>3;
	if(M_BUF_BOUNDS_CHECK(state->u_movie, (tag->size) + 4, state))
	{
		C_RAISE_ERR(ESW_SHORTFILE);
	}

	header->movie_fr.lo = state->u_movie[tag->size];
	header->movie_fr.hi = state->u_movie[tag->size + 1];
	header->movie_frame_count = geti16((uchar *)state->u_movie + tag->size + 2);

	tag->size += 4;
	tag->tag_and_size = (tag->tag_and_size & 0xFFC0) | ((tag->size < 0x3F)? tag->size : 0x3F);

	state->tag_buffer = state->u_movie + tag->size;

	CB_CALL(state, CB_POST_TAG_CHECK);

	return 0;
}

// TODO: Better bounds checking near the end after false size reporting is handled
// Checks tag stream
err check_tag_stream(pdata *state)
{
	state->n_tags = 0;
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
		err_ptr stream_val = get_tag(state, buffer);
		if(stream_val.ret)
		{
			// Diagnostics go here, for now just free tag
			if(stream_val.pointer)
			{
				free(stream_val.pointer);
			}
			return stream_val.ret;
		}

		err ret_check = copy_push_tag(state, *((swf_tag *)stream_val.pointer));
		free(stream_val.pointer);
		stream_val.pointer = NULL;
		if(ER_ERROR(ret_check))
		{
			return ret_check;
		}

		last_tag = (swf_tag *)(state->tag_stream_end->data);

		err_ptr tag_ret = check_tag(state, last_tag);
		if(tag_ret.ret)
		{
			// Diagnostics go here
			return tag_ret.ret;
		}
		if(M_BUF_BOUNDS_CHECK(last_tag->tag_data, last_tag->size, state))
		{
			C_RAISE_ERR(ESW_SHORTFILE);
		}
		buffer = last_tag->tag_data + last_tag->size;
		(state->n_tags)++;
		if((uchar *)buffer >= (uchar *)(state->u_movie + state->movie_size))	// This is well defined I think. And since movie_size is literally the size read from the file into the buffer, this should never point to an invalid object either
		{
			if(last_tag->tag != T_END || state->scope_stack)
			{
				err ret = push_peculiarity(state, PEC_ENDLESS, state->movie_size);	// TODO: Consider making a macro to check callback_peculiarity's return value and stuff.
				if(ER_ERROR(ret))
				{
					return ret;
				}
			}
			return 0;
		}
		if(!(state->scope_stack))
		{
			C_RAISE_ERR(WAF_PREMATURE_END);
		}
	}
}

err check_signature(pdata *state)
{
	uchar *signature = state->signature;

	if(M_SANITIZE_BYTE(signature[1]) != 'W' || M_SANITIZE_BYTE(signature[2]) != 'S')		// Not using short because we abide by standards to the best of our abilities here
	{
		C_RAISE_ERR(ESW_SIGNATURE);
	}
	state->version = M_SANITIZE_BYTE(signature[3]);

	if(state->version < T_VER_MIN || state->version > T_VER_MAX)
	{
		err handler_ret = push_peculiarity(state, PEC_ANOMALOUS_VERSION, 0);
		if(ER_ERROR(handler_ret))
		{
			return handler_ret;
		}
	}
	state->reported_movie_size = geti32((uchar *)signature + 4) - 8;	// We will be using these proxies for calculations because int size may differ from 32 bits
	if(signed_comparei32(state->reported_movie_size, 0) <= 0)
	{
		C_RAISE_ERR(ESW_SIGNATURE);
	}

	state->compression = M_SANITIZE_BYTE(signature[0]);

	return 0;
}

err check_buffer_validity(pdata *state, uchar *buffer, ui32 size)
{
	if(size < 8)
	{
		C_RAISE_ERR(ESW_SHORTFILE);
	}

	err ret_err = check_signature(state);

	switch(state->compression)
	{
		case 'F':
			ret_err = movie_buffer_uncomp(state, buffer, size-8);
			break;
		case 'C':
			if(state->version < VER_INTRO_DEFLATE)
			{
				ret_err = push_peculiarity(state, PEC_COMPRESSION_TIME_TRAVEL, 0);
				if(ER_ERROR(ret_err))
				{
					return ret_err;
				}
			}
			ret_err = movie_buffer_deflate(state, buffer, size-8);
			break;
		case 'Z':
			if(state->version < VER_INTRO_LZMA)
			{
				ret_err = push_peculiarity(state, PEC_COMPRESSION_TIME_TRAVEL, 0);
				if(ER_ERROR(ret_err))
				{
					return ret_err;
				}
			}
			ret_err = movie_buffer_lzma(state, buffer, size-8);
			break;
		default:
			ret_err = 0;
			C_RAISE_ERR(ESW_SIGNATURE);
	}
	if(ER_ERROR(ret_err))
	{
		return ret_err;
	}

	return check_tag_stream(state);
}

// The FILE cursor should point at the beginning of the swf signature/file
err check_file_validity(pdata *state, FILE *swf)
{
	if(fread(state->signature, 1, 8, swf) < 8)
	{
		if(feof(swf))
		{
			C_RAISE_ERR(ESW_SHORTFILE);
		}
		C_RAISE_ERR(EFL_READ);
	}

	err ret_err = check_signature(state);

	switch(state->compression)
	{
		case 'F':
			ret_err = movie_file_uncomp(state, swf);	// Returns error if file length is less than advertised in movie_size, otherwise loads movie_size bytes after the signature in u_movie and returns 0
			break;
		case 'C':
			if(state->version < VER_INTRO_DEFLATE)
			{
				ret_err = push_peculiarity(state, PEC_COMPRESSION_TIME_TRAVEL, 0); // TODO: Add a new peculiarity for this.
				if(ER_ERROR(ret_err))
				{
					return ret_err;
				}
			}
			ret_err = movie_file_deflate(state, swf); // Implemented with zlib for now
			break;
		case 'Z':
			if(state->version < VER_INTRO_LZMA)
			{
				ret_err = push_peculiarity(state, PEC_COMPRESSION_TIME_TRAVEL, 0);
				if(ER_ERROR(ret_err))
				{
					return ret_err;
				}
			}
			ret_err = movie_file_lzma(state, swf); // Unimplemented. simply raises a misc error
			break;
		default:
			ret_err = 0;
			C_RAISE_ERR(ESW_SIGNATURE);
	}
	if(ER_ERROR(ret_err))
	{
		return ret_err;
	}

	return check_tag_stream(state);
}

#undef C_RAISE_ERR_INT
#undef C_RAISE_ERR_PTR
#undef C_RAISE_ERR
