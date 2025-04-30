#include"swftag.h"
#include"error.h"
#include"stdlib.h"

/*------------------------------------------------------------Static Data------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------|-----------------------------------------------------------------*/

const static char *swf_tag_names = "Invalid Tag\0End\0ShowFrame\0DefineShape\0FreeCharacter\0PlaceObject\0RemoveObject\0DefineBitsJPEG\0DefineButton\0JPEGTables\0SetBackgroundColor\0DefineFont\0DefineText\0DoAction\0DefineFontInfo\0DefineSound\0StartSound/StopSound\0DefineButtonSound\0SoundStreamHead\0SoundStreamBlock\0DefineBitsLossless\0DefineBitsJPEG2\0DefineShape2\0DefineButtonCxform\0Protect\0PathsArePostscript\0PlaceObject2\0RemoveObject2\0SyncFrame\0FreeAll\0DefineShape3\0DefineText2\0DefineButton2\0DefineBitsJPEG3\0DefineBitsLossless2\0DefineEditText\0DefineVideo\0DefineSprite\0NameCharacter\0ProductInfo\0DefineTextFormat\0FrameLabel\0SoundStreamHead2\0DefineMorphShape\0GenerateFrame\0DefineFont2\0GeneratorCommand\0DefineCommandObject\0CharacterSet\0ExternalFont\0Export\0Import\0EnableDebugger\0DoInitAction\0DefineVideoStream\0VideoFrame\0DefineFontInfo2\0DebugID\0EnableDebugger2\0ScriptLimits\0SetTabIndex\0FileAttributes\0PlaceObject3\0Import2\0DoABCDefine\0DefineFontAlignZones\0CSMTextSettings\0DefineFont3\0SymbolClass\0Metadata\0DefineScalingGrid\0DoABC\0DefineShape4\0DefineMorphShape2\0DefineSceneAndFrameData\0DefineBinaryData\0DefineFontName\0DefineBitsJPEG4";
const static int swf_tag_name_idx[] = {12,16,26,38,52,64,77,92,105,116,135,146,157,166,181,193,0,214,232,248,265,284,300,313,332,340,359,0,372,386,0,396,404,417,429,443,459,479,494,506,519,533,545,562,0,573,590,607,621,633,650,670,683,0,0,0,696,703,710,725,738,756,767,783,791,807,820,0,0,832,847,860,868,880,901,917,929,941,950,0,0,0,968,974,987,0,1005,1029,1046,0,1061};
const static int swf_long_tag_exclusive[] = {0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1};
const static int swf_tag_intro_version[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 0, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 0, 3, 3, 0, 3, 3, 3, 3, 3, 3, 4, 4, 3, 3, 3, 1, 3, 0, 3, 3, 3, 3, 3, 5, 5, 5, 0, 0, 0, 5, 5, 5, 6, 6, 6, 6, 6, 6, 7, 7, 0, 0, 8, 8, 8, 9, 8, 8, 8, 9, 8, 8, 0, 0, 0, 9, 8, 8, 0, 9, 9, 9, 0, 10};

/*WARNING: Only use if you know what you're doing, use tag_valid(), tag_name(), tag_long_exclusive(), tag_version() and tag_version_valid() otherwise. They add an additional call but they do proper bounds checking and checking secondary variables.
Inlining them is preferred to using these macros. ONLY use these when it is guranteed that the index lies between 0 and 90 inclusive.*/
#define T_TagValid(index) (swf_tag_name_idx[index])?1:0
#define T_TagName(index) (swf_tag_name_idx[index + 1] + swf_tag_names)
#define T_TagLongExclusive(index) (swf_long_tag_exclusive[index])
#define T_TagVersion(index) (swf_tag_intro_version[index])
#define T_TagVersionValid(index, swf_ver) ((swf_tag_intro_version[index] && (swf_tag_intro_version[index] <= swf_ver)) ? 1 : 0)

/*-------------------------------------------------------Data Access Functions-------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------|-----------------------------------------------------------------*/

err_ptr append_list(dnode *node, size_t data_sz)
{
	dnode *new_node = malloc(sizeof(dnode) + data_sz);
	if(!new_node)
	{
		return (err_ptr){NULL, EMM_ALLOC};
	}

	new_node->prev = node;
	new_node->data = (void *)new_node + sizeof(dnode);
	new_node->to_free = NULL;

	if(node)
	{
		if(node->next)
		{
			node->next->prev = new_node;
		}
		new_node->next = node->next;
		node->next = new_node;
		return (err_ptr){new_node, 0};
	}
	new_node->next = NULL;
	return (err_ptr){new_node, 0};
}

err remove_list(dnode *node)
{
	if(!node)
	{
		return EFN_ARGS;
	}
	if(node->to_free)
	{
		free_freelist(node->to_free);
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

int tag_valid(int tag_code)
{
	return (tag_code <= TAG_IDX_MAX && tag_code >= TAG_IDX_MIN) ? T_TagValid(tag_code) : 0;
}

const char *tag_name(int tag_code)
{
	return (tag_code <= TAG_IDX_MAX && tag_code >= TAG_IDX_MIN) ? T_TagName(tag_code) : swf_tag_names;
}

err_int tag_long_exclusive(int tag_code)
{
	return (tag_code <= TAG_IDX_MAX && tag_code >= TAG_IDX_MIN) ? (err_int){T_TagLongExclusive(tag_code), 0} : (err_int){0, EFN_ARGS};
}

err_int tag_version(int tag_code)
{
	return (tag_code <= TAG_IDX_MAX && tag_code >= TAG_IDX_MIN) ? (T_TagVersion(tag_code) ? (err_int){T_TagVersion(tag_code), 0} : (err_int){0, EFN_ARGS}) : (err_int){0, EFN_ARGS};
}

err_int tag_version_valid(int tag_code, int swf_ver)	// TODO: update //
{
	err_int ret = tag_version(tag_code);
	if(ER_ERROR(ret.ret))
	{
		return ret;
	}
	if(swf_ver < T_VER_MIN || swf_ver > T_VER_MAX)
	{
		return (err_int){0, EFN_ARGS};
	}
	return (err_int){ret.integer <= swf_ver, 0};
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
// TODO: Implement add functions for pec_list and tag_stream to add nodes at any given node
err push_peculiarity(pdata *state, ui16 pattern, size_t offset)
{
	if(!state)
	{
		return EFN_ARGS;
	}
	err_ptr check_val = append_list(state->pec_list_end, sizeof(peculiar));
	if(ER_ERROR(check_val.ret))
	{
		S_FREE(check_val.pointer);
		return check_val.ret;
	}
	peculiar *node = ((dnode *)check_val.pointer)->data;
	node->pattern = pattern;
	node->offset = offset;

	if(state->pec_list == NULL)
	{
		state->pec_list = check_val.pointer;
	}
	state->pec_list_end = check_val.pointer;
	callback_peculiarity(state, state->pec_list_end);

	return 0;
}

err pop_peculiarity(pdata *state)
{
	if(!state)
	{
		return EFN_ARGS;
	}
	if(!(state->pec_list_end))
	{
		return EFN_ARGS;
	}
	dnode *to_free = state->pec_list_end;
	state->pec_list_end = state->pec_list_end->prev;
	if(!(state->pec_list_end))
	{
		state->pec_list = NULL;
	}
	return remove_list(to_free);
}

err remove_peculiarity(pdata *state, dnode *node)
{
	if(!state)
	{
		return EFN_ARGS;
	}
	if(!node)
	{
		return EFN_ARGS;
	}
	if(node == state->pec_list_end)
	{
		return pop_peculiarity(state);
	}
	if(node == state->pec_list)
	{
		state->pec_list = state->pec_list->next;
	}
	return remove_list(node);
}

// Linked list ops for swf_tag linked lists
err push_tag(pdata *state, swf_tag *new_tag)
{
	if(!state)
	{
		return EFN_ARGS;
	}
	if(!new_tag)
	{
		return EFN_ARGS;
	}
	err_ptr check_val = append_list(state->tag_stream_end, sizeof(swf_tag));
	if(ER_ERROR(check_val.ret))
	{
		S_FREE(check_val.pointer);
		return check_val.ret;
	}
	swf_tag *node = ((dnode *)check_val.pointer)->data;
	*node = *new_tag;
	free(new_tag);
	node->parent_node = check_val.pointer;

	if(!(state->tag_stream))
	{
		state->tag_stream = check_val.pointer;
	}
	state->tag_stream_end = check_val.pointer;

	return 0;
}

err pop_tag(pdata *state)
{
	if(!state)
	{
		return EFN_ARGS;
	}
	if(!(state->tag_stream_end))
	{
		return EFN_ARGS;
	}
	dnode *to_free = state->tag_stream_end;
	state->tag_stream_end = state->tag_stream_end->prev;
	if(!(state->tag_stream_end))
	{
		state->tag_stream = NULL;
	}
	return remove_list(to_free);
}

err remove_tag(pdata *state, dnode *node)
{
	if(!state)
	{
		return EFN_ARGS;
	}
	if(!node)
	{
		return EFN_ARGS;
	}
	if(node == state->tag_stream_end)
	{
		return pop_tag(state);
	}
	if(node == state->tag_stream)
	{
		state->tag_stream = state->tag_stream->next;
	}
	return remove_list(node);
}

err push_scope(pdata *state, dnode *tag)
{
	if(!state)
	{
		return EFN_ARGS;
	}
	if(!tag)
	{
		return EFN_ARGS;
	}
	err_ptr check_val = append_list(state->scope_stack, 0);
	if(ER_ERROR(check_val.ret))
	{
		S_FREE(check_val.pointer);
		return check_val.ret;
	}

	state->scope_stack = check_val.pointer;
	state->scope_stack->data = tag;

	return 0;
}

err pop_scope(pdata *state)
{
	if(!state)
	{
		return EFN_ARGS;
	}
	if(!(state->scope_stack))
	{
		return EFN_ARGS;
	}
	dnode *to_free = state->scope_stack;
	state->scope_stack = state->scope_stack->prev;

	return remove_list(to_free);
}

// err tie_scope(pdata *state);

// Simply push and pop operations on the scope stack are not useful as that results in a loss of structure and you have to parse the swf stream again to figure it out, but I'm still hung up on how to optimally implement a stack that has the following features.
// 1. It's trivial to judge what falls inside a given scope without having to parse it. The idea here is that when a T_END for the current scope is encountered, the address to that tag is tied up to the stack by a second pointer which does not delete higher elements.
// 2. The problem here is that it would basically mean managing two stacks in one and given that one of the aims of this library is to be flexible, that would mean that destructive or constructive actions on the stack would be a mess.

err_ptr alloc_push_freelist(size_t size, dnode *node)
{
	if(!node)
	{
		return (err_ptr){NULL, EFN_ARGS};
	}
	void *ret_ptr = malloc(size);
	if(!ret_ptr)
	{
		return (err_ptr){NULL, EMM_ALLOC};
	}

	err_ptr ret_node = append_list(node->to_free, 0);
	if(ER_ERROR(ret_node.ret))
	{
		S_FREE(ret_node.pointer);
		S_FREE(ret_ptr);
		return (err_ptr){NULL, ret_node.ret};
	}
	dnode *new_node = ret_node.pointer;
	new_node->to_free = NULL;	// Making double sure
	new_node->data = ret_ptr;
	node->to_free = new_node;

	return (err_ptr){ret_ptr, 0};
}

err free_freelist(dnode *to_free)
{
	dnode *temp;
	err ret;
	while(to_free)
	{
		temp = to_free;
		to_free = to_free->prev;
		S_FREE(temp->data);
		ret = remove_list(temp);
		if(ER_ERROR(ret))
		{
			return ret;
		}
	}
	return 0;
}