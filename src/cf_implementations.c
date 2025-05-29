#include<libswftag/swftag.h>
#include<libswftag/error.h>
#include<libswftag/internal/internal_check_functions.h>

#define SWFTAG_MATH_INLINE
#include<libswftag/swfmath.h>

#define ANALYZE_DEEP 0	// Placeholder flag to set whether or not to analyze tags in depth. Anti-feature for security and thus disabled by default

// Note that currently, few of them save the data they parse somewhere. only verify it, but for the greater library
// the whole structure of the swf should be optionally accessible and editable through the tag stream, so this is something to note for the future.
// All optional ofcourse.

// Considering just making a global variable to handle temp error values on these macros but that seems contrived
#define C_RAISE_ERR(error) {err handler_ret; ER_RAISE_ERROR_ERR(handler_ret, state, error)}
#define C_RAISE_ERR_PTR(pointer, error) {err handler_ret; ER_RAISE_ERROR_ERR_PTR(handler_ret, pointer, state, error)}
#define C_RAISE_ERR_INT(integer, error) {err handler_ret; ER_RAISE_ERROR_ERR_INT(handler_ret, integer, state, error)}

// These macros are not library features, they're here just to make implementation simpler within the check_functions
#define C_TAG_BOUNDS_EVAL(buffer, offset) if(M_BUF_BOUNDS_CHECK(buffer, offset, state)){C_RAISE_ERR(ESW_SHORTFILE)};if(((uchar *)buffer + offset) > ((uchar *)(tag_data->tag_data) + tag_data->size)){C_RAISE_ERR(ESW_IMPROPER)};

// Yes it's horrible, but it's less horrible than other options imho
#define C_INIT_TAG(newstruct) err_ptr tag_ret=alloc_push_freelist(state, sizeof(struct newstruct), tag_data->parent_node);if(ER_ERROR(tag_ret.ret))return tag_ret.ret;struct newstruct *tag_struct=tag_ret.pointer;tag_data->tag_struct=tag_struct

// Only really a need to use this if you're using the internal api
#if defined(ENABLE_INTERNAL_RUNTIME_CHECKS)
	#define C_INTERNAL_RUNTIME_CHECK() if(!tag_data || !state){C_RAISE_ERR(EFN_ARGS);}
#else
	#define C_INTERNAL_RUNTIME_CHECK()
#endif

/*----------------------------------------------------------Duplicated code----------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------|-----------------------------------------------------------------*/

err check_definebitslossless_common(pdata *state, swf_tag *tag_data) //--TODO: STARTED BUT NOT FINISHED--//
{
	C_INTERNAL_RUNTIME_CHECK();
	uchar *base = tag_data->tag_data;
	ui32 offset = 0;
	C_INIT_TAG(swf_tag_definebitslosslessx);

	tag_struct->family_version = 1;
	if(tag_data->tag == T_DEFINEBITSLOSSLESS2)
	{
		tag_struct->family_version = 2;
	}

	C_TAG_BOUNDS_EVAL(base, 7);

	tag_struct->id = geti16(base);
	tag_struct->format = M_SANITIZE_BYTE(base[2]);
	tag_struct->width = geti16(base + 3);
	tag_struct->height = geti16(base + 5);
	offset = 7;

	if(ANALYZE_DEEP)
	{
		/*
		TODO: Add bitmap decompression and parsing

		For T_DEFINEBITSLOSSLESS: 3 = RGB Colormap, 4 = 15 bit RGB15 pre-padded to 16 bit, 5 = 32 bit XRGB (24 bit RGB pre-padded to 32 bit)
		For T_DEFINEBITSLOSSLESS2: 3 = RGBA Colormap, 4 or 5 = 32 bit ARGB (RGBA with alpha channel moved to the front)
		*/
	}
	else
	{
		C_TAG_BOUNDS_EVAL(base, tag_data->size);
		offset = tag_data->size;
	}

	if(offset < tag_data->size)
	{
		return push_peculiarity(state, PEC_TAG_EXTRA, uchar_safe_ptrdiff((tag_data->tag_data + offset), state->u_movie));
	}
	return 0;
}

err check_soundstreamhead_common(pdata *state, swf_tag *tag_data) //--TODO: STARTED BUT NOT FINISHED--//
{
	C_INTERNAL_RUNTIME_CHECK();
	uchar *base = tag_data->tag_data;
	ui32 offset = 0;
	C_INIT_TAG(swf_tag_soundstreamheadx);

	tag_struct->family_version = 1;
	if(tag_data->tag == T_SOUNDSTREAMHEAD2)
	{
		tag_struct->family_version = 2;
	}

	C_TAG_BOUNDS_EVAL(base, 4);

	ui16 flags = geti16(base);
	tag_struct->compression = (flags & 0xF000)>>12;
	tag_struct->sound_rate = (flags & 0xC00)>>10;
	tag_struct->playback_rate = (flags & 0xC)>>2;
	tag_struct->bitfields = flags & 0x303;
	tag_struct->sample_size = geti16(base+2);
	ui8 format = tag_struct->compression;
	if(!SND_FORMAT_VALID(format))
	{
		C_RAISE_ERR(ESW_IMPROPER);
	}

	ui8 ver = state->version;
	if(!SND_FORMAT_VER_VALID(format, ver))
	{
		err ret = push_peculiarity(state, PEC_TIME_TRAVEL, uchar_safe_ptrdiff(base, state->u_movie));
		if(ER_ERROR(ret))
		{
			return ret;
		}
	}

	if(tag_data->tag == T_SOUNDSTREAMHEAD && format > 2)
	{
		err ret = push_peculiarity(state, PEC_MISUSED_FAMILY_FEATURE, uchar_safe_ptrdiff(base, state->u_movie));
		if(ER_ERROR(ret))
		{
			return ret;
		}
	}
	if(flags & 0xF0)
	{
		err ret = push_peculiarity(state, PEC_RESERVE_TAMPERED, uchar_safe_ptrdiff(base, state->u_movie));
		if(ER_ERROR(ret))
		{
			return ret;
		}
	}
	if((~(tag_struct->bitfields)) & (TS_SNDSTREAMHD_PB_SIZE | TS_SNDSTREAMHD_SO_SIZE))
	{
		if(tag_struct->family_version == 1 || (((~(tag_struct->bitfields)) & TS_SNDSTREAMHD_SO_SIZE) && (format > 1 && format != 3)))
		{
			err ret = push_peculiarity(state, PEC_RESERVE_TAMPERED, uchar_safe_ptrdiff(base, state->u_movie));
			if(ER_ERROR(ret))
			{
				return ret;
			}
		}
	}
	offset = 4;
	tag_struct->latency_seek = 0;
	if(format == SND_FORMAT_MP3)
	{
		// Ugly hack. I'm not pleased with this but if the effect is relatively harmless, compatibility considerations rule supreme.
		if(tag_data->size == 4)
		{
			return push_peculiarity(state, PEC_MANDATORY_FIELD_SKIPPED, offset + uchar_safe_ptrdiff(base, state->u_movie));
		}
		else
		{
			C_TAG_BOUNDS_EVAL(base, 6);
			tag_struct->latency_seek = geti16(base+4);
			offset += 2;
		}
	}

	if(offset < tag_data->size)
	{
		return push_peculiarity(state, PEC_TAG_EXTRA, uchar_safe_ptrdiff((tag_data->tag_data + offset), state->u_movie));
	}
	return 0;
}

err check_definetext_common(pdata *state, swf_tag *tag_data) //--TODO: STARTED BUT NOT FINISHED--//
{
	C_INTERNAL_RUNTIME_CHECK();
	uchar *base = tag_data->tag_data;
	ui32 offset = 0;

	C_INIT_TAG(swf_tag_definetextx);
	C_TAG_BOUNDS_EVAL(base, 2);

	tag_struct->id = geti16((uchar *)base);
	offset += 2;
	err_int ret = swf_rect_parse(state, &(tag_struct->rect), base + offset, tag_data);
	if(ER_ERROR(ret.ret))
	{
		return ret.ret;
	}
	offset += M_ALIGN(ret.integer, 3)>>3;

	ret = swf_matrix_parse(state, &(tag_struct->mat), base + offset, tag_data);
	if(ER_ERROR(ret.ret))
	{
		return ret.ret;
	}
	offset += M_ALIGN(ret.integer, 3)>>3;

	C_TAG_BOUNDS_EVAL(base + offset, 2);

	tag_struct->glyph_bits = M_SANITIZE_BYTE(base[offset]);
	tag_struct->advance_bits = M_SANITIZE_BYTE(base[offset+1]);
	offset += 2;

	tag_struct->records = NULL;
	ret = swf_text_record_list_parse(state, base+offset, tag_data);
	if(ER_ERROR(ret.ret))
	{
		return ret.ret;
	}
	offset += M_ALIGN(ret.integer, 3)>>3;

	if(offset < tag_data->size)
	{
		return push_peculiarity(state, PEC_TAG_EXTRA, uchar_safe_ptrdiff((tag_data->tag_data + offset), state->u_movie));
	}
	return 0;
}

err check_mythical_common(pdata *state, swf_tag *tag_data) //--DONE--//
{
	C_INTERNAL_RUNTIME_CHECK();
	return push_peculiarity(state, PEC_MYTHICAL_TAG, uchar_safe_ptrdiff(tag_data->tag_data, state->u_movie));
}

/*-------------------------------------------------------------Functions-------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------|-----------------------------------------------------------------*/

err check_invalidtag(pdata *state, swf_tag *tag_data)
{
	return push_peculiarity(state, PEC_INVAL_TAG, uchar_safe_ptrdiff(tag_data->tag_data, state->u_movie));
}

err check_end(pdata *state, swf_tag *tag_data) //--TODO: STARTED, BUT NOT FINISHED--//
{
	C_INTERNAL_RUNTIME_CHECK();

	if(tag_data->size)
	{
		err ret = push_peculiarity(state, PEC_TAG_EXTRA, uchar_safe_ptrdiff(tag_data->tag_data, state->u_movie));
		if(ER_ERROR(ret))
		{
			return ret;
		}
		C_TAG_BOUNDS_EVAL(tag_data->tag_data, tag_data->size);
	}
	return pop_scope(state);
}

err check_showframe(pdata *state, swf_tag *tag_data) //--DONE--//
{
	C_INTERNAL_RUNTIME_CHECK();

	if(tag_data->size)
	{
		err ret = push_peculiarity(state, PEC_TAG_EXTRA, uchar_safe_ptrdiff(tag_data->tag_data, state->u_movie));
		if(ER_ERROR(ret))
		{
			return ret;
		}
		C_TAG_BOUNDS_EVAL(tag_data->tag_data, tag_data->size);
	}
	return 0;
}

err check_defineshape(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	C_INTERNAL_RUNTIME_CHECK();
	return 0;
}

err check_placeobject(pdata *state, swf_tag *tag_data) //--TODO: STARTED, BUT NOT FINISHED--//
{
	C_INTERNAL_RUNTIME_CHECK();
	uchar *base = tag_data->tag_data;
	ui32 offset = 0;

	C_INIT_TAG(swf_tag_placeobject);

	C_TAG_BOUNDS_EVAL(base, 4);

	tag_data->tag_id = geti16((uchar *)base);

	tag_struct->obj_depth = geti16((uchar *)base + 2);
	tag_struct->has_color_transform = 0;
	/*
	TODO: Checks to verify these values?
	*/
	offset += 4;
	err_int ret = swf_matrix_parse(state, &(tag_struct->matrix), base + offset, tag_data);
	if(ER_ERROR(ret.ret))
	{
		return ret.ret;
	}
	offset += M_ALIGN(ret.integer, 3)>>3;

	ret = swf_color_transform_parse(state, &(tag_struct->color_transform), base + offset, tag_data);

	if(ER_ERROR(ret.ret))
	{
		return ret.ret;
	}
	if(ret.integer)
	{
		tag_struct->has_color_transform = 1;
	}

	offset += M_ALIGN(ret.integer, 3)>>3;
	return 0;
}

err check_removeobject(pdata *state, swf_tag *tag_data) //--TODO: STARTED, BUT NOT FINISHED--//
{
	C_INTERNAL_RUNTIME_CHECK();
	uchar *base = tag_data->tag_data;

	C_INIT_TAG(swf_tag_removeobject);

	C_TAG_BOUNDS_EVAL(base, 4);

	tag_data->tag_id = geti16((uchar *)base);

	tag_struct->obj_depth = geti16((uchar *)base + 2);
	/*
	TODO: Checks to verify these values?
	*/
	return 0;
}

err check_definebitsjpeg(pdata *state, swf_tag *tag_data) //--TODO: STARTED, BUT NOT FINISHED--//
{
	C_INTERNAL_RUNTIME_CHECK();
	uchar *base = tag_data->tag_data;
	ui32 offset = 0;

	C_INIT_TAG(swf_tag_definebitsx);
	tag_struct->family_version = 1;

	C_TAG_BOUNDS_EVAL(base, 2);

	tag_data->tag_id = geti16((uchar *)base);
	err ret = id_register(state, tag_data->tag_id, tag_data);
	if(ER_ERROR(ret))
	{
		return ret;
	}

	if(ANALYZE_DEEP)
	{
		/*
		TODO: The rest of the tag is the jpeg image. Analyzing that comes later
		*/
	}
	return 0;
}

err check_definebutton(pdata *state, swf_tag *tag_data) //--TODO: STARTED, BUT NOT FINISHED--//
{
	C_INTERNAL_RUNTIME_CHECK();
	uchar *base = tag_data->tag_data;
	ui32 offset = 0;
	C_TAG_BOUNDS_EVAL(base, 2);

	// ui16 id = geti16((uchar *)base);
	offset += 2;

	// TODO: A lot, starting with defining it properly in tag_structs. The chain for it is a little big so I'm saving it for later
	return 0;
}

err check_jpegtables(pdata *state, swf_tag *tag_data) //--TODO: STARTED, BUT NOT FINISHED--//
{
	C_INTERNAL_RUNTIME_CHECK();
	// To make or not to make a new struct for this if all it's data is just tag_data->tag_data
	if(ANALYZE_DEEP)
	{
		/*
		TODO: Analyze the jpeg tables and bind it to the last T_DEFINEBITSJPEG tag
		*/
	}
	return 0;
}

err check_setbackgroundcolor(pdata *state, swf_tag *tag_data) //--DONE--//
{
	C_INTERNAL_RUNTIME_CHECK();
	C_INIT_TAG(swf_tag_setbackgroundcolor);

	C_TAG_BOUNDS_EVAL(tag_data->tag_data, 3);
	tag_struct->color.red = M_SANITIZE_BYTE(tag_data->tag_data[0]);
	tag_struct->color.green = M_SANITIZE_BYTE(tag_data->tag_data[1]);
	tag_struct->color.blue = M_SANITIZE_BYTE(tag_data->tag_data[2]);
	return 0;
}

err check_definefont(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	C_INTERNAL_RUNTIME_CHECK();
	return 0;
}

err check_doaction(pdata *state, swf_tag *tag_data) //--TODO: STARTED, BUT NOT FINISHED--//
{
	C_INTERNAL_RUNTIME_CHECK();
	C_INIT_TAG(swf_tag_avm1action);
	tag_struct->initaction = 0;

	if(ANALYZE_DEEP)
	{
		/* TODO: Only for analyzing if all actions are valid for the given version */
	}
	return 0;
}

err check_definefontinfo(pdata *state, swf_tag *tag_data) //--TODO: STARTED, BUT NOT FINISHED--//
{
	C_INTERNAL_RUNTIME_CHECK();
	uchar *base = tag_data->tag_data;
	ui32 offset = 0;
	C_INIT_TAG(swf_tag_fontinfox);
	tag_struct->family_version = 1;

	C_TAG_BOUNDS_EVAL(base, 3);
	offset += 3;
	tag_struct->define_font_id = geti16((uchar *)base);

	/*TODO: Uncomment after define font is properly implemented

	err_ptr ret_tag = id_get_tag(state, tag_struct->define_font_id);
	if(ER_ERROR(ret_tag.ret))
	{
		return ret_tag.ret;
	}
	tag_struct->font_tag = (swf_tag *)ret_tag.pointer;
	if(tag_struct->font_tag->tag != T_DEFINEFONT)
	{
		C_RAISE_ERR(ESW_IMPROPER);
	}

	*/

	tag_struct->name_length = M_SANITIZE_BYTE(base[2]);

	C_TAG_BOUNDS_EVAL(base+offset, M_SANITIZE_BYTE(base[2]));
	tag_struct->name = base+offset;
	offset += M_SANITIZE_BYTE(base[2]);

	tag_struct->bitfields = base[offset];

	tag_struct->font_info_map = base + offset + 1;
	// TODO:: do a bounds check with the "((tag_struct->bitfields & 1) + 1)" times "glyphs_count" value of that on tag_struct->font_info_map. Then check if that same size exceeds the tag_size and if so, push a peculiarity

	return 0;
}

err check_definesound(pdata *state, swf_tag *tag_data) //--TODO: STARTED BUT NOT FINISHED--//
{
	C_INTERNAL_RUNTIME_CHECK();
	uchar *base = tag_data->tag_data;
	ui32 offset = 0;
	C_INIT_TAG(swf_tag_definesound);

	C_TAG_BOUNDS_EVAL(base, 7);
	tag_struct->id = geti16(base);
	id_register(state, tag_struct->id, tag_data);
	tag_struct->format = (base[2] & 0xF0)>>4;
	tag_struct->rate_power = (base[2] & 0xC)>>2;
	tag_struct->bitfields = (base[2] & 3);

	tag_struct->samples_count = geti32(base+3);

	if(ANALYZE_DEEP)
	{
		// Thorough checks will go here.
	}
	else
	{
		C_TAG_BOUNDS_EVAL(base, tag_data->size);
		offset = tag_data->size;
	}

	if(offset < tag_data->size)
	{
		return push_peculiarity(state, PEC_TAG_EXTRA, uchar_safe_ptrdiff((tag_data->tag_data + offset), state->u_movie));
	}

	return 0;
}

err check_startsound(pdata *state, swf_tag *tag_data) //--TODO: STARTED BUT NOT FINISHED--//
{
	C_INTERNAL_RUNTIME_CHECK();
	uchar *base = tag_data->tag_data;
	ui32 offset = 0;
	C_INIT_TAG(swf_tag_startsound);
	err_int ret = swf_sound_info_parse(state, &(tag_struct->info), base, tag_data);
	if(ER_ERROR(ret.ret))
	{
		return ret.ret;
	}

	offset = M_ALIGN(ret.integer, 3)>>3;
	if(offset < tag_data->size)
	{
		return push_peculiarity(state, PEC_TAG_EXTRA, uchar_safe_ptrdiff((tag_data->tag_data + offset), state->u_movie));
	}
	return 0;
}

err check_definebuttonsound(pdata *state, swf_tag *tag_data) //--TODO: STARTED BUT NOT FINISHED--//
{
	C_INTERNAL_RUNTIME_CHECK();
	uchar *base = tag_data->tag_data;
	ui32 offset = 0;
	C_INIT_TAG(swf_tag_definebuttonsound);

/* TODO: Uncomment this after check_definebutton is properly implemented with id registration
	C_TAG_BOUNDS_EVAL(base, 2);
	tag_struct->button_id = geti16(base);
	err_ptr ret = id_get_tag(tag_struct->button_id);
	if(ER_ERROR(ret.ret))
	{
		return ret.ret;
	}

	tag_struct->font_tag = (swf_tag *)ret_tag.pointer;
	if(tag_struct->font_tag->tag != T_DEFINEFONT)
	{
		C_RAISE_ERR(ESW_IMPROPER);
	}
	*/
	offset = 2;
	for (int i=0; i<TS_DEFBTNSND_COND_TOTAL; i++)
	{
		err_int ret = swf_sound_info_parse(state, (tag_struct->sounds) + i, base + offset, tag_data);
		if(ER_ERROR(ret.ret))
		{
			return ret.ret;
		}
		offset += ret.integer>>3;
	}

	if(offset < tag_data->size)
	{
		return push_peculiarity(state, PEC_TAG_EXTRA, uchar_safe_ptrdiff((tag_data->tag_data + offset), state->u_movie));
	}
	return 0;
}

err check_soundstreamblock(pdata *state, swf_tag *tag_data) //--TODO: STARTED, BUT NOT FINISHED--//
{
	C_INTERNAL_RUNTIME_CHECK();
	uchar *base = tag_data->tag_data;
	ui32 offset = 0;
	C_INIT_TAG(swf_tag_soundstreamblock);
	tag_struct->data = base;
	if(ANALYZE_DEEP)
	{
		// TODO: Analyze more
	}
	else
	{
		offset = tag_data->size;
	}

	if(offset < tag_data->size)
	{
		return push_peculiarity(state, PEC_TAG_EXTRA, uchar_safe_ptrdiff((tag_data->tag_data + offset), state->u_movie));
	}
	return 0;
}

err check_definebitsjpeg2(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	C_INTERNAL_RUNTIME_CHECK();
	return 0;
}

err check_defineshape2(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	C_INTERNAL_RUNTIME_CHECK();
	return 0;
}

err check_definebuttoncxform(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	C_INTERNAL_RUNTIME_CHECK();
	return 0;
}

err check_protect(pdata *state, swf_tag *tag_data) //--TODO: STARTED, BUT NOT FINISHED--//
{
	C_INTERNAL_RUNTIME_CHECK();
	C_INIT_TAG(swf_tag_protect);

	tag_struct->hash.string = NULL;
	tag_struct->hash.salt_offset = 0;
	tag_struct->hash.pass_offset = 0;
	if(!(tag_data->size))
	{
		return 0;
	}
	uchar *base = tag_data->tag_data;
	ui32 offset = 0;

	C_TAG_BOUNDS_EVAL(base, 5);

	if(geti16(base))
	{
		err ret = push_peculiarity(state, PEC_RESERVE_TAMPERED, uchar_safe_ptrdiff(base, state->u_movie));
		if(ER_ERROR(ret))
		{
			return ret;
		}
	}

	tag_struct->hash.string = base+2;
	uchar *hash = base + 2;

	if(M_SANITIZE_BYTE(base[2]) != '$' || M_SANITIZE_BYTE(base[3]) != '1' || M_SANITIZE_BYTE(base[4]) != '$')
	{
		err ret = push_peculiarity(state, PEC_MD5_HASH_INVALID, 2 + uchar_safe_ptrdiff(base, state->u_movie));
		if(ER_ERROR(ret))
		{
			return ret;
		}
	}
	offset = 5;

	tag_struct->hash.salt_offset = 3;
	ui32 itr = 3;
	ui32 nullpos = 0;
	ui32 passpos = 0;

	while(itr < ((tag_data->size) - 2))
	{
		if(!hash[itr])
		{
			nullpos = itr;
			break;
		}
		if(hash[itr] == '$')
		{
			if(itr + 1 < ((tag_data->size) - 2))
			{
				itr++;
				tag_struct->hash.pass_offset = passpos = itr;
			}
		}
		itr++;
	}
	offset += itr-3;
	// Remember to always check before using these values, the salt_offset and pass_offset may be empty. The pass_offset may actually point to a single byte, unterminated at the end.
	// It's not handled explicitly because it falls under the PEC_UNTERMINATED_STRING peculiarity.
	// There does not seem to be any strict requirements on the length of the salt
	if(!(passpos))
	{
		err ret = push_peculiarity(state, PEC_MD5_HASH_INVALID, 5 + uchar_safe_ptrdiff(base, state->u_movie));
		if(ER_ERROR(ret))
		{
			return ret;
		}
	}
	if(!nullpos)
	{
		// Ideally, you should use alloc_push_freelist to allocate a buffer the size of the string + 1 and the memcpy the string into it and terminate it with a '\0' if you want to let this peculiarity pass by.
		err ret = push_peculiarity(state, PEC_UNTERMINATED_STRING, 2 + uchar_safe_ptrdiff(base, state->u_movie));
		if(ER_ERROR(ret))
		{
			return ret;
		}
	}
	else
	{

	}
	if((((nullpos)? nullpos : ((tag_data->size) - 2)) - passpos) != 22)
	{
		err ret = push_peculiarity(state, PEC_MD5_HASH_INVALID, 2 + passpos + uchar_safe_ptrdiff(base, state->u_movie));
		if(ER_ERROR(ret))
		{
			return ret;
		}
	}

	if(offset < tag_data->size)
	{
		return push_peculiarity(state, PEC_TAG_EXTRA, uchar_safe_ptrdiff((tag_data->tag_data + offset), state->u_movie));
	}
	return 0;
}

err check_placeobject2(pdata *state, swf_tag *tag_data) //--TODO: STARTED, BUT NOT FINISHED--//
{
	C_INTERNAL_RUNTIME_CHECK();
	uchar *base = tag_data->tag_data;
	ui16 flags = 0;
	ui32 offset = 0;
	if(state->version >= 8)
	{
		if(M_BUF_BOUNDS_CHECK(base, 2, state))
		{
			C_RAISE_ERR(ESW_SHORTFILE);
		}
		flags = geti32((uchar *)base);
	}

	return 0;
}

err check_removeobject2(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	C_INTERNAL_RUNTIME_CHECK();
	return 0;
}

err check_defineshape3(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	C_INTERNAL_RUNTIME_CHECK();
	return 0;
}

err check_definebutton2(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	C_INTERNAL_RUNTIME_CHECK();
	return 0;
}

err check_definebitsjpeg3(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	C_INTERNAL_RUNTIME_CHECK();
	return 0;
}

err check_defineedittext(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	C_INTERNAL_RUNTIME_CHECK();
	return 0;
}

err check_definevideo(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	C_INTERNAL_RUNTIME_CHECK();
	return 0;
}

err check_definesprite(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	C_INTERNAL_RUNTIME_CHECK();
	return 0;
}

err check_namecharacter(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	C_INTERNAL_RUNTIME_CHECK();
	return 0;
}

err check_productinfo(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	C_INTERNAL_RUNTIME_CHECK();
	return 0;
}

err check_framelabel(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	C_INTERNAL_RUNTIME_CHECK();
	return 0;
}

err check_definemorphshape(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	C_INTERNAL_RUNTIME_CHECK();
	return 0;
}

err check_generateframe(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	C_INTERNAL_RUNTIME_CHECK();
	return 0;
}

err check_definefont2(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	C_INTERNAL_RUNTIME_CHECK();
	return 0;
}

err check_generatorcommand(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	C_INTERNAL_RUNTIME_CHECK();
	return 0;
}

err check_externalfont(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	C_INTERNAL_RUNTIME_CHECK();
	return 0;
}

err check_export(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	C_INTERNAL_RUNTIME_CHECK();
	return 0;
}

err check_import(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	C_INTERNAL_RUNTIME_CHECK();
	return 0;
}

err check_enabledebugger(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	C_INTERNAL_RUNTIME_CHECK();
	return 0;
}

err check_doinitaction(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	C_INTERNAL_RUNTIME_CHECK();
	return 0;
}

err check_definevideostream(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	C_INTERNAL_RUNTIME_CHECK();
	return 0;
}

err check_videoframe(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	C_INTERNAL_RUNTIME_CHECK();
	return 0;
}

err check_definefontinfo2(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	C_INTERNAL_RUNTIME_CHECK();
	return 0;
}

err check_debugid(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	C_INTERNAL_RUNTIME_CHECK();
	return 0;
}

err check_enabledebugger2(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	C_INTERNAL_RUNTIME_CHECK();
	return 0;
}

err check_scriptlimits(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	C_INTERNAL_RUNTIME_CHECK();
	return 0;
}

err check_settabindex(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	C_INTERNAL_RUNTIME_CHECK();
	return 0;
}

err check_fileattributes(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	C_INTERNAL_RUNTIME_CHECK();
	uchar *base = tag_data->tag_data;
	ui32 offset = 0;

	FILEATTRIBUTES *tag_struct = &(state->attributes);

	C_TAG_BOUNDS_EVAL(base, 4);
	tag_struct->bitfields = M_SANITIZE_BYTE(base[0]);
	if(tag_struct->bitfields & (0xE0 | ((state->version < 9)? (TS_FILEATTR_ALLOW_ABC | TS_FILEATTR_SUPPRESS_CD_CACHING | TS_FILEATTR_RELATIVE_URLS) : 0)))
	{
		err ret = push_peculiarity(state, PEC_RESERVE_TAMPERED, uchar_safe_ptrdiff(base, state->u_movie));
		if(ER_ERROR(ret))
		{
			return ret;
		}
	}
	if(geti32(base) & (ui32)0xFFFFFF00)
	{
		err ret = push_peculiarity(state, PEC_RESERVE_TAMPERED, uchar_safe_ptrdiff(base, state->u_movie));
		if(ER_ERROR(ret))
		{
			return ret;
		}		
	}

	return 0;
}

err check_placeobject3(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	C_INTERNAL_RUNTIME_CHECK();
	return 0;
}

err check_import2(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	C_INTERNAL_RUNTIME_CHECK();
	return 0;
}

err check_doabcdefine(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	C_INTERNAL_RUNTIME_CHECK();
	return 0;
}

err check_definefontalignzones(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	C_INTERNAL_RUNTIME_CHECK();
	return 0;
}

err check_csmtextsettings(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	C_INTERNAL_RUNTIME_CHECK();
	return 0;
}

err check_definefont3(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	C_INTERNAL_RUNTIME_CHECK();
	return 0;
}

err check_symbolclass(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	C_INTERNAL_RUNTIME_CHECK();
	return 0;
}

err check_metadata(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	C_INTERNAL_RUNTIME_CHECK();
	return 0;
}

err check_definescalinggrid(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	C_INTERNAL_RUNTIME_CHECK();
	return 0;
}

err check_doabc(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	C_INTERNAL_RUNTIME_CHECK();
	return 0;
}

err check_defineshape4(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	C_INTERNAL_RUNTIME_CHECK();
	return 0;
}

err check_definemorphshape2(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	C_INTERNAL_RUNTIME_CHECK();
	return 0;
}

err check_definesceneandframedata(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	C_INTERNAL_RUNTIME_CHECK();
	return 0;
}

err check_definebinarydata(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	C_INTERNAL_RUNTIME_CHECK();
	return 0;
}

err check_definefontname(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	C_INTERNAL_RUNTIME_CHECK();
	return 0;
}

err check_definebitsjpeg4(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	C_INTERNAL_RUNTIME_CHECK();
	return 0;
}

err check_definefont4(pdata *state, swf_tag *tag_data)  //--TODO: NOT STARTED YET--//
{
	C_INTERNAL_RUNTIME_CHECK();
	return 0;
}

err check_enabletelemetry(pdata *state, swf_tag *tag_data)  //--TODO: NOT STARTED YET--//
{
	C_INTERNAL_RUNTIME_CHECK();
	return 0;
}

/*------------------------------------------------------------Static Data------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------|-----------------------------------------------------------------*/

static err (*tag_check[])(pdata *, swf_tag *) = {&check_end, &check_showframe, &check_defineshape, &check_freecharacter, &check_placeobject, &check_removeobject, &check_definebitsjpeg, &check_definebutton, &check_jpegtables, &check_setbackgroundcolor, &check_definefont, &check_definetext, &check_doaction, &check_definefontinfo, &check_definesound, &check_startsound, &check_invalidtag, &check_definebuttonsound, &check_soundstreamhead, &check_soundstreamblock, &check_definebitslossless, &check_definebitsjpeg2, &check_defineshape2, &check_definebuttoncxform, &check_protect, &check_pathsarepostscript, &check_placeobject2, &check_invalidtag, &check_removeobject2, &check_syncframe, &check_invalidtag, &check_freeall, &check_defineshape3, &check_definetext2, &check_definebutton2, &check_definebitsjpeg3, &check_definebitslossless2, &check_defineedittext, &check_definevideo, &check_definesprite, &check_namecharacter, &check_productinfo, &check_definetextformat, &check_framelabel, &check_invalidtag, &check_soundstreamhead2, &check_definemorphshape, &check_generateframe, &check_definefont2, &check_generatorcommand, &check_definecommandobject, &check_characterset, &check_externalfont, &check_invalidtag, &check_invalidtag, &check_invalidtag, &check_export, &check_import, &check_enabledebugger, &check_doinitaction, &check_definevideostream, &check_videoframe, &check_definefontinfo2, &check_debugid, &check_enabledebugger2, &check_scriptlimits, &check_settabindex, &check_invalidtag, &check_invalidtag, &check_fileattributes, &check_placeobject3, &check_import2, &check_doabcdefine, &check_definefontalignzones, &check_csmtextsettings, &check_definefont3, &check_symbolclass, &check_metadata, &check_definescalinggrid, &check_invalidtag, &check_invalidtag, &check_invalidtag, &check_doabc, &check_defineshape4, &check_definemorphshape2, &check_invalidtag, &check_definesceneandframedata, &check_definebinarydata, &check_definefontname, &check_invalidtag, &check_definebitsjpeg4, &check_definefont4, &check_invalidtag, &check_enabletelemetry};

/*------------------------------------------------------------Upper Level------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------|-----------------------------------------------------------------*/

// This is the biggest part of the project and thus the one I'd need the most help with.
// Evaluation would have to be conditional with the version outlined in the pdata structure. All of the prototypes above would have to be fleshed out.
// The struct that the pointer returns hasn't been made yet but it would be diagnostic struct outlining exactly what is wrong in case of an error.
err_ptr check_tag(pdata *state, swf_tag *tag)
{
	if(!tag || !state)
	{
		C_RAISE_ERR_PTR(NULL, EFN_ARGS);
	}

	if(state->n_tags == 0 && state->version >= 8 && tag->tag != T_FILEATTRIBUTES)
	{
		err handler_ret = push_peculiarity(state, PEC_FILEATTR_MISSING, uchar_safe_ptrdiff(tag->tag_data, state->u_movie));
		if(ER_ERROR(handler_ret))
		{
			return (err_ptr){NULL, handler_ret};
		}
	}

	if(tag->tag == T_FILEATTRIBUTES && state->n_tags != 0)
	{
		err handler_ret = push_peculiarity(state, PEC_FILEATTR_MISPLACED, uchar_safe_ptrdiff(tag->tag_data, state->u_movie));
		if(ER_ERROR(handler_ret))
		{
			return (err_ptr){NULL, handler_ret};
		}
	}

	CB_CALL_PTR(state, CB_PRE_TAG_CHECK, NULL);

	ui8 real_tag = tag_valid(tag->tag);
	if(!real_tag)
	{
		err handler_ret = push_peculiarity(state, PEC_INVAL_TAG, tag->tag_data - state->u_movie);	// You can terminate at invalid tags in the callback here if you so wish
		if(ER_ERROR(handler_ret))
		{
			return (err_ptr){NULL, handler_ret};
		}
	}
	else
	{
		if(!tag_version_compare(state, tag->tag))
		{
			err handler_ret = push_peculiarity(state, PEC_TIME_TRAVEL, tag->tag_data - state->u_movie);
			if(ER_ERROR(handler_ret))
			{
				return (err_ptr){NULL, handler_ret};
			}
		}
	}
	// Check function calls and the rest of the stuff will go here
	if(M_BUF_BOUNDS_CHECK(tag->tag_data, tag->size, state))
	{
		C_RAISE_ERR_PTR(tag, ESW_SHORTFILE);
	}
	if(real_tag)
	{
		err ret_check = tag_check[tag->tag](state, tag);
		if(ER_ERROR(ret_check))
		{
			return (err_ptr){NULL, ret_check};
		}
	}

	CB_CALL_PTR(state, CB_POST_TAG_CHECK, NULL);

	return (err_ptr){NULL, 0};
}

#undef C_INIT_TAG
#undef C_TAG_BOUNDS_EVAL

#undef C_RAISE_ERR_INT
#undef C_RAISE_ERR_PTR
#undef C_RAISE_ERR

