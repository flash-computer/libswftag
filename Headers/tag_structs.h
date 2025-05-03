/*----------------------------------------------------------Data Structures----------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------|-----------------------------------------------------------------*/

// If you're scared about the choice of unsigned long long as UI32_TYPE, that's because as far as I understand the standard, long long is the only int type guaranteed to be atleast 4 bytes(32 bits for minimum value of CHAR_BIT being 8)
// And unsigned to ensure uniformity in case it's bigger. The arithmetic ops in swfmath.h will treat it as signed because that's what the swf format does, virtually anywhere

#ifndef DATA_STRUCTS
	#define DATA_STRUCTS

	// Linked lists everywhere. That's what you pay for the modularity this library affords you
	struct doubly_linked_list_node
	{
		struct doubly_linked_list_node *next;
		struct doubly_linked_list_node *prev;

		struct doubly_linked_list_node *to_free;	// A list of elements to free when the node is freed. Yes this is pretty ugly.

		void *data;
	};
	typedef struct doubly_linked_list_node dnode;

	struct fixed16_16
	{
		ui16 hi;
		ui16 lo;
	};
	typedef struct fixed16_16 f16_16;

	struct fixed8_8
	{
		ui8 hi;
		ui8 lo;
	};
	typedef struct fixed8_8 f8_8;

	struct fixed32_32
	{
		ui32 hi;
		ui32 lo;
	};
	typedef struct fixed32_32 f32_32;

	struct fixedvar
	{
		// Specified the width of the fixed point number in hi.lo format
		f8_8 width;
		ui32 hi;
		ui32 lo;
	};
	typedef struct fixedvar fvar;
#endif

/*----------------------------------------------------------Library Structs----------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------|-----------------------------------------------------------------*/


#ifndef LIB_STRUCTS
	#define LIB_STRUCTS

/*------------------------ TAG SUBSTRUCTURES ------------------------*/
/*---------------------------------|---------------------------------*/

	struct swf_rect
	{
		ui32 fields[4];
		ui8 field_size;
	};
	typedef struct swf_rect RECT;

	struct swf_matrix
	{
		// has_scale : 0x1
		// has_rotate : 0x10
		ui8 bitfields;

		ui8 scale_bits;
		f16_16 scale_x;
		f16_16 scale_y;

		ui8 rotate_bits;
		f16_16 rotate_skew0;
		f16_16 rotate_skew1;

		ui8 translate_bits;
		f16_16 translate_x;
		f16_16 translate_y;
	};
	typedef struct swf_matrix MATRIX;

	struct swf_color_transform
	{
		// has_add  : 0x1
		// has_mult  : 0x10
		ui8 bitfields;
		ui8 color_bits;

		ui16 red_mult;
		ui16 green_mult;
		ui16 blue_mult;

		ui16 alpha_mult;

		ui16 red_add;
		ui16 green_add;
		ui16 blue_add;

		ui16 alpha_add;
	};
	typedef struct swf_color_transform COLOR_TRANSFORM;

	struct swf_action
	{
		ui8 id;
		ui16 action_length;

		uchar *action_data;
	};
	typedef struct swf_action ACTION;

	struct swf_rgb
	{
		ui8 red;
		ui8 green;
		ui8 blue;
	};
	typedef struct swf_rgb RGB;

/*
	struct swf_button
	{
		// Only for version 8+
		// blend_mode : 0x1
		// filter_list : 0x10
		ui8 bitfields;

		// square_test : 0x1
		// down : 0x4
		// hover : 0x10
		// up : 0x40
		ui8 button_state;

		ui16 id;
		ui16 layer;
		MATRIX matrix;

		COLOR_TRANSFORM color_transform;

		ui8 filter_count;
		///////////////// FILTER filter;

		ui8 blend_mode;
	};
*/

/*--------------------------- LIB STRUCTS ---------------------------*/
/*---------------------------------|---------------------------------*/

	struct parse_peculiarity
	{
		ui16 pattern;
		size_t offset;
	};
	typedef struct parse_peculiarity peculiar;

	// Compound return types
	struct return_pointer_with_error
	{
		void *pointer;
		err ret;
	};
	typedef struct return_pointer_with_error err_ptr;

	struct return_integer_with_error
	{
		ui32 integer;
		err ret;
	};
	typedef struct return_integer_with_error err_int;

	struct swf_tag
	{
		int tag;
		ui32 size;
		uchar *tag_data;
		ui16 tag_and_size;

		void *tag_struct;
		dnode *parent_node;
	};
	typedef struct swf_tag swf_tag;

	// FileHeader pseudo-tag and DefineSprite are the only tags for which a new swf_scope opens. END tags close the scope

	struct parse_data
	{
		ui8 compression;
		ui8 version;
		ui32 movie_size;
		uchar signature[8];
		uchar *u_movie;	// Uncompressed movie data

		ui32 read_movie_size;

		ui8 avm1;
		ui8 avm2;

		RECT movie_rect;
		// 8.8 fixed point
		f8_8 movie_fr;
		ui16 movie_frame_count;

		dnode *pec_list;	// List of parsing peculiarities that are not necessarily errors
		dnode *pec_list_end;

		uchar *tag_buffer;
		dnode *tag_stream;
		dnode *tag_stream_end;

		dnode *scope_stack;
	};
	typedef struct parse_data pdata;

/*----------------------- MAIN TAG STRUCTURES -----------------------*/
/*---------------------------------|---------------------------------*/

	// defineshape will go here

	struct swf_tag_placeobject
	{
		ui16 id;
		ui16 obj_depth;

		MATRIX matrix;
		ui8 has_color_transform;
		COLOR_TRANSFORM color_transform;
	};

	struct swf_tag_removeobject
	{
		ui16 id;
		ui16 obj_depth;
	};

	struct swf_tag_definebitsx
	{
		ui8 family_version;

		ui16 id;
		ui32 alpha_offset;
		ui16 deblocking_filter_parameter;

		uchar *encoding_tables;
		uchar *image_data;
		uchar *alpha;
	};

/*
	swf_tag_definebutton
	{
	};
*/

	struct swf_tag_setbackgroundcolor
	{
		RGB color;
	};

	struct swf_tag_jpegtables
	{
		uchar *data;
		swf_tag bind_tag;
	};

/*
	struct swf_tag_definefont
	{
		ui16 id;
		ui16 glyphs_count;

		ui16 glyph_offsets*

		////////SHAPE glyph_shapes*;
	};
*/

/*
	struct swf_tag_definetext
	{
		ui16 id;
		RECT rect;
		MATRIX mat;
		ui8 glyph_bits;
		ui8 advance_bits;
		//////////TEXT_RECORD record;
	};
*/

	struct swf_tag_avm1action
	{
		ui8 initaction; // Boolean
		ui16 action_sprite;
		ACTION *actions;
	};

	struct swf_tag_fontinfox
	{
		ui8 family_version;

		ui16 id;	// Refers to the define font id. Map the define font tag with the matching id before the font info tag in the current stream to. If there are more than one tags with the same id, that's an error.
		ui8 name_length;
		uchar *name;

		// For font info 2
		// V7+ small_text : 0x20
		// For font info 1
		// is_unicode : 0x20
		// is_shiftjis : 0x10
		// is_ansi (latin) : 0x08
		// Always
		// italic : 0x4
		// bold : 0x2
		// wide: 0x1	; Must be 1 for font info 2 and font info 1 v6+
		ui8 bitfields;
		ui8 lang;

		swf_tag *font_tag;

		void *font_info_map;
	};

	struct swf_tag_definesound
	{
		ui16 id;
		ui8 format;
		ui8 rate_power;	// Determines the exponent of 2 to multiply with 5512.5Hz

		// 16bit/compressed audio : 0x2
		// stereo : 0x1
		ui8 bitfields;

		ui32 samples_count;
		uchar *sound_data;
	};

#endif
