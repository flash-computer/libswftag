#include"../Headers/swftag.h"
#include"../Headers/error.h"
#include"../Headers/check_functions.h"
#include"../Headers/swfmath.h"

#define ANALYZE_DEEP 0	// Placeholder flag to set whether or not to analyze tags in depth. Anti-feature for security and thus disabled by default

// Note that currently, few of them save the data they parse somewhere. only verify it, but for the greater library
// the whole structure of the swf should be optionally accessible and editable through the tag stream, so this is something to note for the future.
// All optional ofcourse.


// These macros are not library features, they're here just to make implementation simpler within the check_functions
#define C_TAG_BOUNDS_EVAL(buffer, offset) if(M_BUF_BOUNDS_CHECK(buffer, offset, state))return ESW_SHORTFILE;if(((char *)buffer + offset) > (tag_data->tag_data + tag_data->size))return ESW_IMPROPER

// Yes it's horrible, but it's less horrible than other options imho
#define C_INIT_TAG(newstruct) err_ptr tag_ret=alloc_push_freelist(sizeof(struct newstruct), tag_data->parent_node);if(ER_ERROR(tag_ret.ret))return tag_ret.ret;struct newstruct *tag_struct=tag_ret.pointer;tag_data->tag_struct=tag_struct

err check_invalidtag(swf_tag *tag_data, pdata *state)
{
	return ESW_TAG;
}

err check_end(swf_tag *tag_data, pdata *state) //--TODO: STARTED, BUT NOT FINISHED--//
{
	if(!tag_data || !state)
	{
		return EFN_ARGS;
	}

	if(tag_data->size)
	{
		err ret = push_peculiarity(state, PEC_TAG_EXTRA, tag_data->tag_data - state->u_movie);
		if(ret)
		{
			return ret;
		}
		C_TAG_BOUNDS_EVAL(tag_data->tag_data, tag_data->size);
	}
	return pop_scope(state);
}

err check_showframe(swf_tag *tag_data, pdata *state) //--DONE--//
{
	if(!tag_data || !state)
	{
		return EFN_ARGS;
	}

	if(tag_data->size)
	{
		err ret = push_peculiarity(state, PEC_TAG_EXTRA, tag_data->tag_data - state->u_movie);
		if(ret)
		{
			return ret;
		}
		C_TAG_BOUNDS_EVAL(tag_data->tag_data, tag_data->size);
	}
	return 0;
}

err check_defineshape(swf_tag *tag_data, pdata *state) //--TODO: NOT STARTED YET--//
{
	if(!tag_data || !state)
	{
		return EFN_ARGS;
	}
	return 0;
}

err check_freecharacter(swf_tag *tag_data, pdata *state) //--DONE--//
{
	if(!tag_data || !state)
	{
		return EFN_ARGS;
	}
	push_peculiarity(state, PEC_MYTHICAL_TAG, tag_data->tag_data - state->u_movie);
	return 0;
}

err check_placeobject(swf_tag *tag_data, pdata *state) //--TODO: STARTED, BUT NOT FINISHED--//
{
	if(!tag_data || !state)
	{
		return EFN_ARGS;
	}
	char *base = tag_data->tag_data;
	ui32 offset = 0;

	C_INIT_TAG(swf_tag_placeobject);

	C_TAG_BOUNDS_EVAL(base, 4);

	tag_struct->id = geti16((uchar *)base);
	tag_struct->obj_depth = geti16((uchar *)base + 2);
	tag_struct->has_color_transform = 0;
	/*
	TODO: Checks to verify these values?
	*/
	offset += 4;
	err ret = swf_matrix_parse(&(tag_struct->matrix), state, base + offset, tag_data->size - offset);
	if(ER_ERROR(ret))
	{
		return ret;
	}
	offset += 2 + (tag_struct->matrix.bitfields & 0x1) * (5 + tag_struct->matrix.scale_bits * 2) + ((tag_struct->matrix.bitfields & 0x10)>>4) * (5 + tag_struct->matrix.rotate_bits * 2) + (5+ tag_struct->matrix.translate_bits * 2);

	if(tag_data->size > offset)
	{
		ret = swf_color_transform_parse(&(tag_struct->color_transform), state, base + offset, tag_data->size - offset);
		if(ER_ERROR(ret))
		{
			/* Considering changing the return value here from an Error to expand into the non-zero-high-byte range of values as ESW errors will be spun out to have their own callback and this will break. But it's a little ugly so I need some time to think */
			if(ret == ESW_IMPROPER)	// Aka the difference between the filesize and limit isn't enough to fit the substructure
			{
				return push_peculiarity(state, PEC_TAG_EXTRA, offset + (base - state->u_movie));
			}
			return ret;
		}
		tag_struct->has_color_transform = 1;
		/* TODO: Check if there's more space after the COLOR_TRANSFORM. */
	}
	return 0;
}

err check_removeobject(swf_tag *tag_data, pdata *state) //--TODO: STARTED, BUT NOT FINISHED--//
{
	if(!tag_data || !state)
	{
		return EFN_ARGS;
	}
	char *base = tag_data->tag_data;

	C_INIT_TAG(swf_tag_removeobject);

	C_TAG_BOUNDS_EVAL(base, 4);

	tag_struct->id = geti16((uchar *)base);
	tag_struct->obj_depth = geti16((uchar *)base + 2);
	/*
	TODO: Checks to verify these values?
	*/
	return 0;
}

err check_definebitsjpeg(swf_tag *tag_data, pdata *state) //--TODO: STARTED, BUT NOT FINISHED--//
{
	if(!tag_data || !state)
	{
		return EFN_ARGS;
	}
	char *base = tag_data->tag_data;
	ui32 offset = 0;

	C_INIT_TAG(swf_tag_definebitsx);
	tag_struct->family_version = 1;

	C_TAG_BOUNDS_EVAL(base, 2);

	tag_struct->id = geti16((uchar *)base);
	if(ANALYZE_DEEP)
	{
		/*
		TODO: The rest of the tag is the jpeg image. Analyzing that comes later
		*/
	}
	return 0;
}

err check_definebutton(swf_tag *tag_data, pdata *state) //--TODO: STARTED, BUT NOT FINISHED--//
{
	if(!tag_data || !state)
	{
		return EFN_ARGS;
	}
	char *base = tag_data->tag_data;
	ui32 offset = 0;
	C_TAG_BOUNDS_EVAL(base, 2);

	ui16 id = geti16((uchar *)base);
	offset += 2;

	// TODO: A lot, starting with defining it properly in tag_structs. The chain for it is a little big so I'm saving it for later
	return 0;
}

err check_jpegtables(swf_tag *tag_data, pdata *state) //--TODO: STARTED, BUT NOT FINISHED--//
{
	if(!tag_data || !state)
	{
		return EFN_ARGS;
	}
	// To make or not to make a new struct for this if all it's data is just tag_data->tag_data
	if(ANALYZE_DEEP)
	{
		/*
		TODO: Analyze the jpeg tables and bind it to the last T_DEFINEBITSJPEG tag
		*/
	}
	return 0;
}

err check_setbackgroundcolor(swf_tag *tag_data, pdata *state) //--DONE--//
{
	if(!tag_data || !state)
	{
		return EFN_ARGS;
	}
	C_INIT_TAG(swf_tag_setbackgroundcolor);

	C_TAG_BOUNDS_EVAL(tag_data->tag_data, 3);
	tag_struct->color.red = tag_data->tag_data[0];
	tag_struct->color.green = tag_data->tag_data[1];
	tag_struct->color.blue = tag_data->tag_data[2];
	return 0;
}

err check_definefont(swf_tag *tag_data, pdata *state) //--TODO: NOT STARTED YET--//
{
	if(!tag_data || !state)
	{
		return EFN_ARGS;
	}
	return 0;
}

err check_definetext(swf_tag *tag_data, pdata *state) //--TODO: NOT STARTED YET--//
{
	if(!tag_data || !state)
	{
		return EFN_ARGS;
	}
	return 0;
}

err check_doaction(swf_tag *tag_data, pdata *state) //--TODO: STARTED, BUT NOT FINISHED--//
{
	if(!tag_data || !state)
	{
		return EFN_ARGS;
	}
	C_INIT_TAG(swf_tag_avm1action);

	if(ANALYZE_DEEP)
	{
		/* TODO: Only for analyzing if all actions are valid for the given version */
	}
	return 0;
}

err check_definefontinfo(swf_tag *tag_data, pdata *state) //--TODO: STARTED, BUT NOT FINISHED--//
{
	if(!tag_data || !state)
	{
		return EFN_ARGS;
	}
	char *base = tag_data->tag_data;
	ui32 offset = 0;
	C_INIT_TAG(swf_tag_fontinfox);
	tag_struct->family_version = 1;

	C_TAG_BOUNDS_EVAL(base, 3);
	offset += 3;
	tag_struct->id = geti16((uchar *)base);
	tag_struct->name_length = base[2];

	C_TAG_BOUNDS_EVAL(base+offset, base[2]);
	tag_struct->name = base+offset;
	offset += base[2];

	tag_struct->bitfields = base[offset];

	tag_struct->font_info_map = base + offset + 1;
	// Find and connect tag_struct->font_tag to the tag with id tag_struct->id, and do a bounds check with the "((tag_struct->bitfields & 1) + 1)" times "glyphs_count" value of that on tag_struct->font_info_map. Then check if that same size exceeds the tag_size and if so, push a peculiarity

	return 0;
}

err check_definesound(swf_tag *tag_data, pdata *state) //--TODO: NOT STARTED YET--//
{
	if(!tag_data || !state)
	{
		return EFN_ARGS;
	}
	return 0;
}

err check_startsound(swf_tag *tag_data, pdata *state) //--TODO: NOT STARTED YET--//
{
	if(!tag_data || !state)
	{
		return EFN_ARGS;
	}
	return 0;
}

err check_definebuttonsound(swf_tag *tag_data, pdata *state) //--TODO: NOT STARTED YET--//
{
	if(!tag_data || !state)
	{
		return EFN_ARGS;
	}
	return 0;
}

err check_soundstreamhead(swf_tag *tag_data, pdata *state) //--TODO: NOT STARTED YET--//
{
	if(!tag_data || !state)
	{
		return EFN_ARGS;
	}
	return 0;
}

err check_soundstreamblock(swf_tag *tag_data, pdata *state) //--TODO: NOT STARTED YET--//
{
	if(!tag_data || !state)
	{
		return EFN_ARGS;
	}
	return 0;
}

err check_definebitslossless(swf_tag *tag_data, pdata *state) //--TODO: NOT STARTED YET--//
{
	if(!tag_data || !state)
	{
		return EFN_ARGS;
	}
	return 0;
}

err check_definebitsjpeg2(swf_tag *tag_data, pdata *state) //--TODO: NOT STARTED YET--//
{
	if(!tag_data || !state)
	{
		return EFN_ARGS;
	}
	return 0;
}

err check_defineshape2(swf_tag *tag_data, pdata *state) //--TODO: NOT STARTED YET--//
{
	if(!tag_data || !state)
	{
		return EFN_ARGS;
	}
	return 0;
}

err check_definebuttoncxform(swf_tag *tag_data, pdata *state) //--TODO: NOT STARTED YET--//
{
	if(!tag_data || !state)
	{
		return EFN_ARGS;
	}
	return 0;
}

err check_protect(swf_tag *tag_data, pdata *state) //--TODO: NOT STARTED YET--//
{
	if(!tag_data || !state)
	{
		return EFN_ARGS;
	}
	return 0;
}

err check_pathsarepostscript(swf_tag *tag_data, pdata *state) //--TODO: NOT STARTED YET--//
{
	if(!tag_data || !state)
	{
		return EFN_ARGS;
	}
	return 0;
}

err check_placeobject2(swf_tag *tag_data, pdata *state) //--TODO: STARTED, BUT NOT FINISHED--//
{
	if(!tag_data || !state)
	{
		return EFN_ARGS;
	}
	char *base = tag_data->tag_data;
	ui16 flags = 0;
	ui32 offset = 0;
	if(state->version >= 8)
	{
		if(M_BUF_BOUNDS_CHECK(base, 2, state))
		{
			return ESW_SHORTFILE;
		}
		flags = geti32((uchar *)base);
	}

	return 0;
}

err check_removeobject2(swf_tag *tag_data, pdata *state) //--TODO: NOT STARTED YET--//
{
	if(!tag_data || !state)
	{
		return EFN_ARGS;
	}
	return 0;
}

err check_syncframe(swf_tag *tag_data, pdata *state) //--TODO: NOT STARTED YET--//
{
	if(!tag_data || !state)
	{
		return EFN_ARGS;
	}
	return 0;
}

err check_freeall(swf_tag *tag_data, pdata *state) //--TODO: NOT STARTED YET--//
{
	if(!tag_data || !state)
	{
		return EFN_ARGS;
	}
	return 0;
}

err check_defineshape3(swf_tag *tag_data, pdata *state) //--TODO: NOT STARTED YET--//
{
	if(!tag_data || !state)
	{
		return EFN_ARGS;
	}
	return 0;
}

err check_definetext2(swf_tag *tag_data, pdata *state) //--TODO: NOT STARTED YET--//
{
	if(!tag_data || !state)
	{
		return EFN_ARGS;
	}
	return 0;
}

err check_definebutton2(swf_tag *tag_data, pdata *state) //--TODO: NOT STARTED YET--//
{
	if(!tag_data || !state)
	{
		return EFN_ARGS;
	}
	return 0;
}

err check_definebitsjpeg3(swf_tag *tag_data, pdata *state) //--TODO: NOT STARTED YET--//
{
	if(!tag_data || !state)
	{
		return EFN_ARGS;
	}
	return 0;
}

err check_definebitslossless2(swf_tag *tag_data, pdata *state) //--TODO: NOT STARTED YET--//
{
	if(!tag_data || !state)
	{
		return EFN_ARGS;
	}
	return 0;
}

err check_defineedittext(swf_tag *tag_data, pdata *state) //--TODO: NOT STARTED YET--//
{
	if(!tag_data || !state)
	{
		return EFN_ARGS;
	}
	return 0;
}

err check_definevideo(swf_tag *tag_data, pdata *state) //--TODO: NOT STARTED YET--//
{
	if(!tag_data || !state)
	{
		return EFN_ARGS;
	}
	return 0;
}

err check_definesprite(swf_tag *tag_data, pdata *state) //--TODO: NOT STARTED YET--//
{
	if(!tag_data || !state)
	{
		return EFN_ARGS;
	}
	return 0;
}

err check_namecharacter(swf_tag *tag_data, pdata *state) //--TODO: NOT STARTED YET--//
{
	if(!tag_data || !state)
	{
		return EFN_ARGS;
	}
	return 0;
}

err check_productinfo(swf_tag *tag_data, pdata *state) //--TODO: NOT STARTED YET--//
{
	if(!tag_data || !state)
	{
		return EFN_ARGS;
	}
	return 0;
}

err check_definetextformat(swf_tag *tag_data, pdata *state) //--TODO: NOT STARTED YET--//
{
	if(!tag_data || !state)
	{
		return EFN_ARGS;
	}
	return 0;
}

err check_framelabel(swf_tag *tag_data, pdata *state) //--TODO: NOT STARTED YET--//
{
	if(!tag_data || !state)
	{
		return EFN_ARGS;
	}
	return 0;
}

err check_soundstreamhead2(swf_tag *tag_data, pdata *state) //--TODO: NOT STARTED YET--//
{
	if(!tag_data || !state)
	{
		return EFN_ARGS;
	}
	return 0;
}

err check_definemorphshape(swf_tag *tag_data, pdata *state) //--TODO: NOT STARTED YET--//
{
	if(!tag_data || !state)
	{
		return EFN_ARGS;
	}
	return 0;
}

err check_generateframe(swf_tag *tag_data, pdata *state) //--TODO: NOT STARTED YET--//
{
	if(!tag_data || !state)
	{
		return EFN_ARGS;
	}
	return 0;
}

err check_definefont2(swf_tag *tag_data, pdata *state) //--TODO: NOT STARTED YET--//
{
	if(!tag_data || !state)
	{
		return EFN_ARGS;
	}
	return 0;
}

err check_generatorcommand(swf_tag *tag_data, pdata *state) //--TODO: NOT STARTED YET--//
{
	if(!tag_data || !state)
	{
		return EFN_ARGS;
	}
	return 0;
}

err check_definecommandobject(swf_tag *tag_data, pdata *state) //--TODO: NOT STARTED YET--//
{
	if(!tag_data || !state)
	{
		return EFN_ARGS;
	}
	return 0;
}

err check_characterset(swf_tag *tag_data, pdata *state) //--TODO: NOT STARTED YET--//
{
	if(!tag_data || !state)
	{
		return EFN_ARGS;
	}
	return 0;
}

err check_externalfont(swf_tag *tag_data, pdata *state) //--TODO: NOT STARTED YET--//
{
	if(!tag_data || !state)
	{
		return EFN_ARGS;
	}
	return 0;
}

err check_export(swf_tag *tag_data, pdata *state) //--TODO: NOT STARTED YET--//
{
	if(!tag_data || !state)
	{
		return EFN_ARGS;
	}
	return 0;
}

err check_import(swf_tag *tag_data, pdata *state) //--TODO: NOT STARTED YET--//
{
	if(!tag_data || !state)
	{
		return EFN_ARGS;
	}
	return 0;
}

err check_enabledebugger(swf_tag *tag_data, pdata *state) //--TODO: NOT STARTED YET--//
{
	if(!tag_data || !state)
	{
		return EFN_ARGS;
	}
	return 0;
}

err check_doinitaction(swf_tag *tag_data, pdata *state) //--TODO: NOT STARTED YET--//
{
	if(!tag_data || !state)
	{
		return EFN_ARGS;
	}
	return 0;
}

err check_definevideostream(swf_tag *tag_data, pdata *state) //--TODO: NOT STARTED YET--//
{
	if(!tag_data || !state)
	{
		return EFN_ARGS;
	}
	return 0;
}

err check_videoframe(swf_tag *tag_data, pdata *state) //--TODO: NOT STARTED YET--//
{
	if(!tag_data || !state)
	{
		return EFN_ARGS;
	}
	return 0;
}

err check_definefontinfo2(swf_tag *tag_data, pdata *state) //--TODO: NOT STARTED YET--//
{
	if(!tag_data || !state)
	{
		return EFN_ARGS;
	}
	return 0;
}

err check_debugid(swf_tag *tag_data, pdata *state) //--TODO: NOT STARTED YET--//
{
	if(!tag_data || !state)
	{
		return EFN_ARGS;
	}
	return 0;
}

err check_enabledebugger2(swf_tag *tag_data, pdata *state) //--TODO: NOT STARTED YET--//
{
	if(!tag_data || !state)
	{
		return EFN_ARGS;
	}
	return 0;
}

err check_scriptlimits(swf_tag *tag_data, pdata *state) //--TODO: NOT STARTED YET--//
{
	if(!tag_data || !state)
	{
		return EFN_ARGS;
	}
	return 0;
}

err check_settabindex(swf_tag *tag_data, pdata *state) //--TODO: NOT STARTED YET--//
{
	if(!tag_data || !state)
	{
		return EFN_ARGS;
	}
	return 0;
}

err check_fileattributes(swf_tag *tag_data, pdata *state) //--TODO: NOT STARTED YET--//
{
	if(!tag_data || !state)
	{
		return EFN_ARGS;
	}
	return 0;
}

err check_placeobject3(swf_tag *tag_data, pdata *state) //--TODO: NOT STARTED YET--//
{
	if(!tag_data || !state)
	{
		return EFN_ARGS;
	}
	return 0;
}

err check_import2(swf_tag *tag_data, pdata *state) //--TODO: NOT STARTED YET--//
{
	if(!tag_data || !state)
	{
		return EFN_ARGS;
	}
	return 0;
}

err check_doabcdefine(swf_tag *tag_data, pdata *state) //--TODO: NOT STARTED YET--//
{
	if(!tag_data || !state)
	{
		return EFN_ARGS;
	}
	return 0;
}

err check_definefontalignzones(swf_tag *tag_data, pdata *state) //--TODO: NOT STARTED YET--//
{
	if(!tag_data || !state)
	{
		return EFN_ARGS;
	}
	return 0;
}

err check_csmtextsettings(swf_tag *tag_data, pdata *state) //--TODO: NOT STARTED YET--//
{
	if(!tag_data || !state)
	{
		return EFN_ARGS;
	}
	return 0;
}

err check_definefont3(swf_tag *tag_data, pdata *state) //--TODO: NOT STARTED YET--//
{
	if(!tag_data || !state)
	{
		return EFN_ARGS;
	}
	return 0;
}

err check_symbolclass(swf_tag *tag_data, pdata *state) //--TODO: NOT STARTED YET--//
{
	if(!tag_data || !state)
	{
		return EFN_ARGS;
	}
	return 0;
}

err check_metadata(swf_tag *tag_data, pdata *state) //--TODO: NOT STARTED YET--//
{
	if(!tag_data || !state)
	{
		return EFN_ARGS;
	}
	return 0;
}

err check_definescalinggrid(swf_tag *tag_data, pdata *state) //--TODO: NOT STARTED YET--//
{
	if(!tag_data || !state)
	{
		return EFN_ARGS;
	}
	return 0;
}

err check_doabc(swf_tag *tag_data, pdata *state) //--TODO: NOT STARTED YET--//
{
	if(!tag_data || !state)
	{
		return EFN_ARGS;
	}
	return 0;
}

err check_defineshape4(swf_tag *tag_data, pdata *state) //--TODO: NOT STARTED YET--//
{
	if(!tag_data || !state)
	{
		return EFN_ARGS;
	}
	return 0;
}

err check_definemorphshape2(swf_tag *tag_data, pdata *state) //--TODO: NOT STARTED YET--//
{
	if(!tag_data || !state)
	{
		return EFN_ARGS;
	}
	return 0;
}

err check_definesceneandframedata(swf_tag *tag_data, pdata *state) //--TODO: NOT STARTED YET--//
{
	if(!tag_data || !state)
	{
		return EFN_ARGS;
	}
	return 0;
}

err check_definebinarydata(swf_tag *tag_data, pdata *state) //--TODO: NOT STARTED YET--//
{
	if(!tag_data || !state)
	{
		return EFN_ARGS;
	}
	return 0;
}

err check_definefontname(swf_tag *tag_data, pdata *state) //--TODO: NOT STARTED YET--//
{
	if(!tag_data || !state)
	{
		return EFN_ARGS;
	}
	return 0;
}

err check_definebitsjpeg4(swf_tag *tag_data, pdata *state) //--TODO: NOT STARTED YET--//
{
	if(!tag_data || !state)
	{
		return EFN_ARGS;
	}
	return 0;
}

#undef C_TAG_BOUNDS_EVAL
#undef C_INIT_TAG
