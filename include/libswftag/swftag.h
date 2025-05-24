/*--------------------------------------------------------------Version--------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------|-----------------------------------------------------------------*/

#define VER_MAJ 0
#define VER_MIN 1

/*-------------------------------------------------------VT100 Text Formatting-------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------|-----------------------------------------------------------------*/

#define COL_RD "\x1b[31m"
#define COL_GR "\x1b[32m"
#define COL_MG "\x1b[35m"

#define BG_RD "\x1b[41m"
#define BG_GR "\x1b[42m"
#define BG_MG "\x1b[45m"

#define FM_BOLD "\x1b[1m"
#define FM_THIN "\x1b[2m"
#define FM_ITAL "\x1b[3m"
#define FM_UNLN "\x1b[4m"
#define FM_BLN1 "\x1b[5m"
#define FM_BLN2 "\x1b[6m"
#define FM_INVR "\x1b[7m"
#define FM_DISA "\x1b[8m"
#define FM_STRK "\x1b[9m"

#define FM_RESET "\x1b[0m"

/*----------------------------------------------------------Tag Stream Info----------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------|-----------------------------------------------------------------*/

#define	T_END 0
#define	T_SHOWFRAME 1
#define	T_DEFINESHAPE 2
#define	T_FREECHARACTER 3
#define	T_PLACEOBJECT 4
#define	T_REMOVEOBJECT 5
#define	T_DEFINEBITSJPEG 6
#define	T_DEFINEBUTTON 7
#define	T_JPEGTABLES 8
#define	T_SETBACKGROUNDCOLOR 9
#define	T_DEFINEFONT 10
#define	T_DEFINETEXT 11
#define	T_DOACTION 12
#define	T_DEFINEFONTINFO 13
#define	T_DEFINESOUND 14
#define	T_STARTSOUND 15
#define	T_STOPSOUND 15
#define	T_DEFINEBUTTONSOUND 17
#define	T_SOUNDSTREAMHEAD 18
#define	T_SOUNDSTREAMBLOCK 19
#define	T_DEFINEBITSLOSSLESS 20
#define	T_DEFINEBITSJPEG2 21
#define	T_DEFINESHAPE2 22
#define	T_DEFINEBUTTONCXFORM 23
#define	T_PROTECT 24
#define	T_PATHSAREPOSTSCRIPT 25
#define	T_PLACEOBJECT2 26
#define	T_REMOVEOBJECT2 28
#define	T_SYNCFRAME 29
#define	T_FREEALL 31
#define	T_DEFINESHAPE3 32
#define	T_DEFINETEXT2 33
#define	T_DEFINEBUTTON2 34
#define	T_DEFINEBITSJPEG3 35
#define	T_DEFINEBITSLOSSLESS2 36
#define	T_DEFINEEDITTEXT 37
#define	T_DEFINEVIDEO 38
#define	T_DEFINESPRITE 39
#define	T_NAMECHARACTER 40
#define	T_PRODUCTINFO 41
#define	T_DEFINETEXTFORMAT 42
#define	T_FRAMELABEL 43
#define	T_SOUNDSTREAMHEAD2 45
#define	T_DEFINEMORPHSHAPE 46
#define	T_GENERATEFRAME 47
#define	T_DEFINEFONT2 48
#define	T_GENERATORCOMMAND 49
#define	T_DEFINECOMMANDOBJECT 50
#define	T_CHARACTERSET 51
#define	T_EXTERNALFONT 52
#define	T_EXPORT 56
#define	T_IMPORT 57
#define	T_ENABLEDEBUGGER 58
#define	T_DOINITACTION 59
#define	T_DEFINEVIDEOSTREAM 60
#define	T_VIDEOFRAME 61
#define	T_DEFINEFONTINFO2 62
#define	T_DEBUGID 63
#define	T_ENABLEDEBUGGER2 64
#define	T_SCRIPTLIMITS 65
#define	T_SETTABINDEX 66
#define	T_FILEATTRIBUTES 69
#define	T_PLACEOBJECT3 70
#define	T_IMPORT2 71
#define	T_DOABCDEFINE 72
#define	T_DEFINEFONTALIGNZONES 73
#define	T_CSMTEXTSETTINGS 74
#define	T_DEFINEFONT3 75
#define	T_SYMBOLCLASS 76
#define	T_METADATA 77
#define	T_DEFINESCALINGGRID 78
#define	T_DOABC 82
#define	T_DEFINESHAPE4 83
#define	T_DEFINEMORPHSHAPE2 84
#define	T_DEFINESCENEANDFRAMEDATA 86
#define	T_DEFINEBINARYDATA 87
#define	T_DEFINEFONTNAME 88
#define	T_DEFINEBITSJPEG4 90
#define T_DEFINEFONT4 91 // "Soul Brother.swf" and "Flight.swf" seem to be the only swfs to contain this tag and so my automatic scraping did not include this
#define T_ENABLETELEMETRY 93 // Yes

#define TAG_IDX_MAX T_ENABLETELEMETRY
#define TAG_IDX_MIN T_END

#define	F_FILEHEADER -1 // Not a real tag, just for ease of reference so we can treat it as one, borrowing from m2osw.com's convention (no idea if they were the ones who originated it though)

#define FLTR_DROPSHADOW 0
#define FLTR_BLUR 1
#define FLTR_GLOW 2
#define FLTR_BEVEL 3
#define FLTR_GRADIENTGLOW 4
#define FLTR_CONVOLUTION 5
#define FLTR_COLORMATRIX 6
#define FLTR_GRADIENTBEVEL 7

#define VER_INTRO_DEFLATE 6
#define VER_INTRO_LZMA 13

#include<stdint.h>
#include<stddef.h>
#include<stdio.h>

#ifndef CUSTOM_TYPES
	#define CUSTOM_TYPES

	#ifndef UI32_TYPE
		#define UI32_TYPE uint32_t
		typedef UI32_TYPE ui32;
	#endif

	#ifndef ERR_TYPE
		#define ERR_TYPE uint16_t
		typedef ERR_TYPE err;
	#endif

	#ifndef UI16_TYPE
		#define UI16_TYPE unsigned int
		typedef UI16_TYPE ui16;
	#endif

	#ifndef UI8_TYPE
		#define UI8_TYPE uint8_t
		typedef UI8_TYPE ui8;
	#endif

	#ifndef OTHER_TYPES
		#define OTHER_TYPES

		typedef unsigned char uchar;
	#endif
#endif

#define S_FREE(ptr)	if(ptr)free(ptr);ptr=NULL;

#define T_VER_MIN 1
#define T_VER_MAX 43	// One swf I have claims to have this version. I don't know what the actual the max version is (Pokemon Tower Defense claims 50, so I need some documentation to resolve this asap). TODO

/*--------------------------------------------------------------Structs--------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------|-----------------------------------------------------------------*/


// If you're scared about the choice of unsigned long long as UI32_TYPE, that's because as far as I understand the standard, long long is the only int type guaranteed to be atleast 4 bytes(32 bits for minimum value of CHAR_BIT being 8)
// And unsigned to ensure uniformity in case it's bigger. The arithmetic ops in swfmath.h will treat it as signed because that's what the swf format does, virtually anywhere
#include"tag_structs.h"

/*----------------------------------------------------Extended Callback Functions----------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------|-----------------------------------------------------------------*/

#if defined(EXTENDED_CALLBACKS)
		#include "callbacks_extended.h"
		#define CB_CALL_BASE_TEMPLATE(pdata, callbackindex, value_to_return) {err internal_extended_cb_ret = callback_functions[(callbackindex % CB_N_CALLBACKS) & 0x1F](pdata); if(ER_ERROR(internal_extended_cb_ret))return value_to_return;}

		#define CB_CALL_BASE(pdata, callbackindex) CB_CALL_BASE_TEMPLATE(pdata, callbackindex, internal_extended_cb_ret)
		#define CB_CALL_BASE_INT(pdata, callbackindex, int_to_return) CB_CALL_BASE_TEMPLATE(pdata, callbackindex, (err_int){int_to_return, internal_extended_cb_ret})
		#define CB_CALL_BASE_PTR(pdata, callbackindex, ptr_to_return) CB_CALL_BASE_TEMPLATE(pdata, callbackindex, (err_ptr){ptr_to_return, internal_extended_cb_ret})

		#ifndef EXTENDED_CALLBACKS_TYPE
			#define EXTENDED_CALLBACKS_TYPE 0
		#endif

		#if (EXTENDED_CALLBACKS_TYPE == 0)
			// Mandatory extended callbacks
			#define CB_CALL(pdata, callbackindex) CB_CALL_BASE(pdata, callbackindex)
			#define CB_CALL_INT(pdata, callbackindex, int_to_return) CB_CALL_BASE_INT(pdata, callbackindex, int_to_return)
			#define CB_CALL_PTR(pdata, callbackindex, ptr_to_return) CB_CALL_BASE_PTR(pdata, callbackindex, ptr_to_return)
		#else
			// Conditional extended callbacks
			#define CB_CALL(pdata, callbackindex) {if(get_callback_flag(pdata, callbackindex))CB_CALL_BASE(pdata, callbackindex)}
			#define CB_CALL_INT(pdata, callbackindex, int_to_return) {if(get_callback_flag(pdata, callbackindex))CB_CALL_BASE_INT(pdata, callbackindex, int_to_return)}
			#define CB_CALL_PTR(pdata, callbackindex, ptr_to_return) {if(get_callback_flag(pdata, callbackindex))CB_CALL_BASE_PTR(pdata, callbackindex, ptr_to_return)}
		#endif
#else
		// No extended callbacks
		#define CB_CALL(pdata, callbackindex)
		#define CB_CALL_INT(pdata, callbackindex, int_to_return)
		#define CB_CALL_PTR(pdata, callbackindex, ptr_to_return)
#endif

/*-----------------------------------------------------------Peculiarities-----------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------|-----------------------------------------------------------------*/

// The current Peculiarity implementation is a little lacking os the numbers are just sequential and provisional
// I know there's an awful lot of stuff like this in this library but I'm still finalizing the overarching design
// Should not stop the straightforward work needed to get the basic verification running

#define PEC_MIN 0x10

#define PEC_BITFIELD_PADDING 0x10	// Bitfield Padding isn't all 0s
#define PEC_TAG_EXTRA 0x11	// Size of tag exceeds what it needs
#define PEC_MYTHICAL_TAG 0x12	// Tags not defined by the standard. No proper implementation available for these and thus these tags only raise a peculiarity and pass the checks
#define PEC_TIME_TRAVEL 0x13	// Feature or Tag used in a version where it wasn't introduced yet
#define PEC_FILESIZE_SMALL 0x14	// File size smaller than reported
#define PEC_INVAL_TAG 0x15	// Invalid tag encountered
#define PEC_ENDLESS 0x16 // File does not terminate with a T_END tag
#define PEC_ANOMALOUS_VERSION 0x17 // Anomalous swf version
#define PEC_DATA_AFTER_MOVIE 0x18 // File does not end after the movie. This is a relatively serious peculiarity and should likely be filtered for any service using swfs. However, the sane approach should be to just truncate the file upto the movie and discard the rest.
#define PEC_RESERVE_TAMPERED 0x19 // Some reserved bit in some structure/substructure bitfield was tampered (usually only raised for where the reserved bit is specified a state, like if the spec says it's Always 1 or 0). Not thoroughly enforced yet. TODO
#define PEC_WIDTH_TOO_BIG 0x1A // Width too big for specified field. Gets auto truncated
#define PEC_UNCONVENTIONAL_SHORT_TAG 0x1B // Short tag used for a tag that is usually long tag exclusive
#define PEC_COMPRESSION_TIME_TRAVEL 0x1C // Compression type not supported by swf version
#define PEC_MISUSED_FAMILY_FEATURE 0x1D // A family member of a certain tag family used a feature supported by some other family member, but not by itself
#define PEC_MANDATORY_FIELD_SKIPPED 0x1E // A mandatory but relatively harmless to skip field is skipped
#define PEC_UNTERMINATED_STRING 0x1F // String is not null terminated
#define PEC_MD5_HASH_INVALID 0x20 // MD5 Hash is invalid
#define PEC_FILEATTR_MISSING 0x21 // T_FILEATTRIBUTES tag missing from the beginning of tagstream on swf 8+
#define PEC_FILEATTR_MISPLACED 0x22 // T_FILEATTRIBUTES tag is present elsewhere in the file, and not at the beginning of the tag stream

#define PEC_MAX 0x22

/*--------------------------------------------------------Function prototypes--------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------|-----------------------------------------------------------------*/

ui8 tag_valid(int tag_code);
const char *tag_name(int tag_code);
ui8 tag_long_exclusive(int tag_code);
ui8 tag_version(int tag_code);
ui8 tag_version_compare(pdata *state, int tag_code);

ui8 id_tag_exists(pdata *state, ui16 id);
err_ptr id_get_tag(pdata *state, ui16 id);
err id_wipe_list(pdata *state);
err id_register(pdata *state, ui16 id, swf_tag *tag);

err init_parse_data(pdata *state);
err destroy_parse_data(pdata *state);

err_ptr append_list(pdata *state, dnode *node, size_t data_sz);
err remove_list(pdata *state, dnode *node);

err push_peculiarity(pdata *state, ui32 pattern, size_t offset);
err pop_peculiarity(pdata *state);
err remove_peculiarity(pdata *state, dnode *node);

err push_tag(pdata *state, swf_tag *new_tag);
err pop_tag(pdata *state);
err remove_tag(pdata *state, dnode *node);

err push_scope(pdata *state, dnode *tag);
err pop_scope(pdata *state);

err_ptr alloc_push_freelist(pdata *state, size_t size, dnode *node);
err free_freelist(pdata *state, dnode *to_free);

err_ptr get_tag(pdata *state, uchar *buffer);
err_ptr check_tag(pdata *state, swf_tag *tag);
err_ptr spawn_tag(pdata *state, int tag, ui32 size, uchar *tag_data);

err_int swf_rect_parse(pdata *state, RECT *rect, uchar *buf, swf_tag *tag);
err_int swf_matrix_parse(pdata *state, MATRIX *mat, uchar *buf, swf_tag *tag);
err_int swf_color_transform_parse(pdata *state, COLOR_TRANSFORM *colt, uchar *buf, swf_tag *tag);
err_int swf_text_record_parse(pdata *state, TEXT_RECORD *trec, uchar *buf, swf_tag *tag);
err_int swf_sound_info_parse(pdata *state, SOUND_INFO *soin, uchar *buf, swf_tag *tag);

err_int swf_text_record_list_parse(pdata *state, uchar *buf, swf_tag *tag);

err file_header_verification(pdata *state);
err check_tag_stream(pdata *state);
err check_file_validity(pdata *state, FILE *swf);