#include"swftag.h"
#include"error.h"
#include"stdlib.h"

/*------------------------------------------------------------Static Data------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------|-----------------------------------------------------------------*/

const static char *swf_tag_names = "Invalid Tag\0End\0ShowFrame\0DefineShape\0FreeCharacter\0PlaceObject\0RemoveObject\0DefineBitsJPEG\0DefineButton\0JPEGTables\0SetBackgroundColor\0DefineFont\0DefineText\0DoAction\0DefineFontInfo\0DefineSound\0StartSound/StopSound\0DefineButtonSound\0SoundStreamHead\0SoundStreamBlock\0DefineBitsLossless\0DefineBitsJPEG2\0DefineShape2\0DefineButtonCxform\0Protect\0PathsArePostscript\0PlaceObject2\0RemoveObject2\0SyncFrame\0FreeAll\0DefineShape3\0DefineText2\0DefineButton2\0DefineBitsJPEG3\0DefineBitsLossless2\0DefineEditText\0DefineVideo\0DefineSprite\0NameCharacter\0ProductInfo\0DefineTextFormat\0FrameLabel\0SoundStreamHead2\0DefineMorphShape\0GenerateFrame\0DefineFont2\0GeneratorCommand\0DefineCommandObject\0CharacterSet\0ExternalFont\0Export\0Import\0EnableDebugger\0DoInitAction\0DefineVideoStream\0VideoFrame\0DefineFontInfo2\0DebugID\0EnableDebugger2\0ScriptLimits\0SetTabIndex\0FileAttributes\0PlaceObject3\0Import2\0DoABCDefine\0DefineFontAlignZones\0CSMTextSettings\0DefineFont3\0SymbolClass\0Metadata\0DefineScalingGrid\0DoABC\0DefineShape4\0DefineMorphShape2\0DefineSceneAndFrameData\0DefineBinaryData\0DefineFontName\0DefineBitsJPEG4";
const static int swf_tag_name_idx[] = {12,16,26,38,52,64,77,92,105,116,135,146,157,166,181,193,0,214,232,248,265,284,300,313,332,340,359,0,372,386,0,396,404,417,429,443,459,479,494,506,519,533,545,562,0,573,590,607,621,633,650,670,683,0,0,0,696,703,710,725,738,756,767,783,791,807,820,0,0,832,847,860,868,880,901,917,929,941,950,0,0,0,968,974,987,0,1005,1029,1046,0,1061};
const static int swf_long_tag_exclusive[] = {0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1};

/*WARNING: Only use if you know what you're doing, use tag_valid(), tag_name() and tag_long_exclusive() otherwise. They add an additional call but they do proper bounds checking.
Inlining them is preferred to using these macros. ONLY use these when it is guranteed that the index lies between 0 and 90 inclusive.*/
#define T_TagValid(index) (swf_tag_name_idx[index])?1:0
#define T_TagName(index) (swf_tag_name_idx[index] + swf_tag_names)
#define T_TagLongExclusive(index) (swf_long_tag_exclusive[index])

/*-------------------------------------------------------Data Access Functions-------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------|-----------------------------------------------------------------*/

int tag_valid(size_t tag_code)
{
	return (tag_code <= TAG_IDX_MAX && tag_code >= TAG_IDX_MIN) ? T_TagValid(tag_code) : 0;
}

const char *tag_name(size_t tag_code)
{
	return (tag_code <= TAG_IDX_MAX && tag_code >= TAG_IDX_MIN) ? T_TagName(tag_code) : swf_tag_names;
}

err_int tag_long_exclusive(size_t tag_code)
{
	return (tag_code <= TAG_IDX_MAX && tag_code >= TAG_IDX_MIN) ? (err_int){T_TagLongExclusive(tag_code), 0} : (err_int){0, EFN_ARGS};
}

err_ptr new_parse_data()
{
	pdata *state = malloc(sizeof(pdata));
	if(!state)
	{
		return (err_ptr){NULL, EMM_ALLOC};
	}
	state->u_movie = NULL;
	state->pec_list = NULL;
	state->pec_list_end = NULL;
	state->tag_stream = NULL;
	return (err_ptr){state, 0};
}

// Linked list ops for pec_node linked lists
err_ptr append_peculiarity(pec_node **list_start, pec_node *node, unsigned int pattern, size_t offset)
{
	if(!node)
	{
		return (err_ptr){NULL, EFN_ARGS};	// Malformed Argument Error
	}
	pec_node *new_node = malloc(sizeof(pec_node));
	if(!new_node)
	{
		return (err_ptr){NULL, EMM_ALLOC};
	}
	new_node->next = NULL;
	new_node->data.pattern = pattern;
	new_node->data.offset_in_movie = offset;
	if(!*list_start)
	{
		*list_start = new_node;
		new_node->prev = NULL;
		return (err_ptr){new_node, 0};
	}
	new_node->prev = node;
	if(node->next)
	{
		node->next->prev = new_node;
		new_node->next = node->next;
	}
	node->next = new_node;
	return (err_ptr){new_node, 0};
}

int remove_peculiarity(pec_node **list_start, pec_node *node)
{
	if(node == *list_start)
	{
		*list_start = node->next;
		node->next->prev = NULL;
		free(node);
	}
	if(!node)
	{
		return EFN_ARGS;
	}
	if(node->prev)
	{
		node->prev->next = node->next;
	}
	if(node->next)
	{
		node->next->prev = node->prev;
	}
	free(node);
	return 0;
}

// Linked list ops for swf_tag linked lists
err_ptr append_tag(swf_tag_node **list_start, swf_tag_node *node, swf_tag *new_tag)
{

	if(!node || !new_tag)
	{
		return (err_ptr){NULL, EFN_ARGS};	// Malformed Argument Error
	}
	swf_tag_node *new_node = malloc(sizeof(swf_tag_node));
	if(!new_node)
	{
		return (err_ptr){NULL, EMM_ALLOC};
	}
	new_node->tag.tag_and_size = new_tag->tag_and_size;
	new_node->tag.tag = new_tag->tag;
	new_node->tag.size = new_tag->size;
	free(new_tag);

	if(!*list_start)
	{
		*list_start = new_node;
		new_node->prev = NULL;
		return (err_ptr){new_node, 0};
	}
	new_node->prev = node;
	new_node->next = node->next;
	if(node->next)
	{
		node->next->prev = new_node;
	}
	node->next = new_node;
	return (err_ptr){new_node, 0};
}

int remove_tag(swf_tag_node **list_start, swf_tag_node *node)
{
	if(node == *list_start)
	{
		*list_start = node->next;
		node->next->prev = NULL;
		free(node);
	}
	if(!node)
	{
		return EFN_ARGS;
	}
	if(node->prev)
	{
		node->prev->next = node->next;
	}
	if(node->next)
	{
		node->next->prev = node->prev;
	}
	free(node);
	return 0;
}
