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

#define TAG_IDX_MAX T_DEFINEBITSJPEG4
#define TAG_IDX_MIN T_END

#define	F_FILEHEADER -1 // Not a real tag, just for ease of reference so we can treat it as one

#define UI32_TYPE unsigned long long

#include<stddef.h>

/*--------------------------------------------------------------Structs--------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------|-----------------------------------------------------------------*/


// If you're scared about the choice of unsigned long long as UI32_TYPE, that's because as far as I understand the standard, long long is the only int type guaranteed to be atleast 4 bytes(32 bits for minimum value of CHAR_BIT being 8)
// And unsigned to ensure uniformity in case it's bigger. The arithmetic ops in swfmath.h will treat it as signed because that's what the swf format does, virtually anywhere
#ifndef STRUCT_DEFS
#define STRUCT_DEFS

struct swf_rect
{
	char field_size;
	UI32_TYPE x_min;
	UI32_TYPE y_min;
	UI32_TYPE x_max;
	UI32_TYPE y_max;
};
typedef struct swf_rect RECT;

struct parse_peculiarity
{
	unsigned int pattern;
	size_t offset_in_movie;
};
typedef struct parse_peculiarity peculiar;

struct peculiar_node
{
	peculiar data;
	struct peculiar_node *next;
	struct peculiar_node *prev;
};
typedef struct peculiar_node pec_node;

// Compound return types
struct return_pointer_with_error
{
	void *pointer;
	int ret;
};
typedef struct return_pointer_with_error err_ptr;

struct return_integer_with_error
{
	UI32_TYPE integer;
	int ret;
};
typedef struct return_integer_with_error err_int;

struct swf_tag
{
	unsigned int tag_and_size;
	unsigned int tag;
	UI32_TYPE size;
	char *tag_data;
};
typedef struct swf_tag swf_tag;

struct swf_tag_node
{
	swf_tag tag;
	struct swf_tag_node *next;
	struct swf_tag_node *prev;
};
typedef struct swf_tag_node swf_tag_node;

struct parse_data
{
	char compression;
	char version;
	UI32_TYPE movie_size;
	char *u_movie;	// Uncompressed movie data

	RECT movie_rect;
	unsigned char movie_fr_hi;
	unsigned char movie_fr_lo;
	unsigned int movie_frame_count;

	pec_node *pec_list;	// List of parsing peculiarities that are not necessarily errors
	pec_node *pec_list_end;

	char *tag_buffer;
	swf_tag_node *tag_stream;
	swf_tag_node *tag_stream_end;
};
typedef struct parse_data pdata;

#endif

/*-----------------------------------------------------------Peculiarities-----------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------|-----------------------------------------------------------------*/

#define PEC_RECTPADDING 0x10	// Rect Padding isn't all 0s

/*--------------------------------------------------------Function prototypes--------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------|-----------------------------------------------------------------*/

int tag_valid(size_t tag_code);
const char *tag_name(size_t tag_code);
err_int tag_long_exclusive(size_t tag_code);

err_ptr new_parse_data();

err_ptr append_peculiarity(pec_node **list_start, pec_node *node, unsigned int pattern, size_t offset);
int remove_peculiarity(pec_node **list_start, pec_node *node);

err_ptr append_tag(swf_tag_node **list_start, swf_tag_node *node, swf_tag *new_tag);
int remove_tag(swf_tag_node **list_start, swf_tag_node *node);