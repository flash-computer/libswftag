#include"../Headers/swftag.h"
#include"../Headers/error.h"

#include<stdlib.h>

/*------------------------------------------------------------Static Data------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------|-----------------------------------------------------------------*/

struct tag_information
{
	char *name;
	ui8 long_tag_exlusive;
	ui8 intro_version;
};

static struct tag_information tag_info[] = {{"End", 0, 1}, {"ShowFrame", 0, 1}, {"DefineShape", 0, 1}, {"FreeCharacter", 0, 1}, {"PlaceObject", 0, 1}, {"RemoveObject", 0, 1}, {"DefineBitsJPEG", 1, 1}, {"DefineButton", 0, 1}, {"JPEGTables", 0, 1}, {"SetBackgroundColor", 0, 1}, {"DefineFont", 0, 1}, {"DefineText", 0, 1}, {"DoAction", 0, 1}, {"DefineFontInfo", 0, 1}, {"DefineSound", 0, 2}, {"StartSound/StopSound", 0, 2}, {"Invalid Tag", 0, 0}, {"DefineButtonSound", 0, 2}, {"SoundStreamHead", 0, 2}, {"SoundStreamBlock", 1, 2}, {"DefineBitsLossless", 1, 2}, {"DefineBitsJPEG2", 1, 2}, {"DefineShape2", 0, 2}, {"DefineButtonCxform", 0, 2}, {"Protect", 0, 2}, {"PathsArePostscript", 0, 3}, {"PlaceObject2", 0, 3}, {"Invalid Tag", 0, 0}, {"RemoveObject2", 0, 3}, {"SyncFrame", 0, 3}, {"Invalid Tag", 0, 0}, {"FreeAll", 0, 3}, {"DefineShape3", 0, 3}, {"DefineText2", 0, 3}, {"DefineButton2", 0, 3}, {"DefineBitsJPEG3", 1, 3}, {"DefineBitsLossless2", 1, 3}, {"DefineEditText", 0, 4}, {"DefineVideo", 0, 4}, {"DefineSprite", 0, 3}, {"NameCharacter", 0, 3}, {"ProductInfo", 0, 3}, {"DefineTextFormat", 0, 1}, {"FrameLabel", 0, 3}, {"Invalid Tag", 0, 0}, {"SoundStreamHead2", 0, 3}, {"DefineMorphShape", 0, 3}, {"GenerateFrame", 0, 3}, {"DefineFont2", 0, 3}, {"GeneratorCommand", 0, 3}, {"DefineCommandObject", 0, 5}, {"CharacterSet", 0, 5}, {"ExternalFont", 0, 5}, {"Invalid Tag", 0, 0}, {"Invalid Tag", 0, 0}, {"Invalid Tag", 0, 0}, {"Export", 0, 5}, {"Import", 0, 5}, {"EnableDebugger", 0, 5}, {"DoInitAction", 0, 6}, {"DefineVideoStream", 0, 6}, {"VideoFrame", 0, 6}, {"DefineFontInfo2", 0, 6}, {"DebugID", 0, 6}, {"EnableDebugger2", 0, 6}, {"ScriptLimits", 0, 7}, {"SetTabIndex", 0, 7}, {"Invalid Tag", 0, 0}, {"Invalid Tag", 0, 0}, {"FileAttributes", 0, 8}, {"PlaceObject3", 0, 8}, {"Import2", 0, 8}, {"DoABCDefine", 0, 9}, {"DefineFontAlignZones", 0, 8}, {"CSMTextSettings", 0, 8}, {"DefineFont3", 0, 8}, {"SymbolClass", 0, 9}, {"Metadata", 0, 8}, {"DefineScalingGrid", 0, 8}, {"Invalid Tag", 0, 0}, {"Invalid Tag", 0, 0}, {"Invalid Tag", 0, 0}, {"DoABC", 0, 9}, {"DefineShape4", 0, 8}, {"DefineMorphShape2", 0, 8}, {"Invalid Tag", 0, 0}, {"DefineSceneAndFrameData", 0, 9}, {"DefineBinaryData", 0, 9}, {"DefineFontName", 0, 9}, {"Invalid Tag", 0, 0}, {"DefineBitsJPEG4", 1, 10}};

/*WARNING: Only use if you know what you're doing, use tag_valid(), tag_name(), tag_long_exclusive(), tag_version() and tag_version_valid() otherwise. They add an additional call but they do proper bounds checking and checking secondary variables.
Inlining them is preferred to using these macros. ONLY use these when it is guranteed that the index lies between 0 and 90 inclusive.*/
#define T_TagValid(index) (tag_info[index].intro_version)?1:0
#define T_TagName(index) (tag_info[index].name)
#define T_TagLongExclusive(index) (tag_info[index].long_tag_exlusive)
#define T_TagVersion(index) (tag_info[index].intro_version)
#define T_TagVersionValid(index, swf_ver) ((tag_info[index].intro_version && (tag_info[index].intro_version <= swf_ver)) ? 1 : 0)

/*-------------------------------------------------------Data Access Functions-------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------|-----------------------------------------------------------------*/

int tag_valid(int tag_code)
{
	return (tag_code <= TAG_IDX_MAX && tag_code >= TAG_IDX_MIN) ? T_TagValid(tag_code) : 0;
}

const char *tag_name(int tag_code)
{
	return (tag_code <= TAG_IDX_MAX && tag_code >= TAG_IDX_MIN) ? T_TagName(tag_code) : "Invalid Tag";
}

err_int tag_long_exclusive(int tag_code)
{
	return (tag_code <= TAG_IDX_MAX && tag_code >= TAG_IDX_MIN) ? (err_int){T_TagLongExclusive(tag_code), 0} : (err_int){0, EFN_ARGS};
}

err_int tag_version(int tag_code)
{
	return (tag_code <= TAG_IDX_MAX && tag_code >= TAG_IDX_MIN) ? (T_TagVersion(tag_code) ? (err_int){T_TagVersion(tag_code), 0} : (err_int){0, EFN_ARGS}) : (err_int){0, EFN_ARGS};
}

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

err init_parse_data(pdata *state)
{
	if(!state)
	{
		return EFN_ARGS;
	}
	state->version = 0;
	state->movie_size = 0;
	state->avm1 = 0;
	state->avm2 = 0;
	state->movie_rect.field_size = 0;
	state->movie_rect.fields[0] = state->movie_rect.fields[1] = state->movie_rect.fields[2] = state->movie_rect.fields[3] = 0;
	state->movie_fr.hi = state->movie_fr.lo = 0;
	state->movie_frame_count = 0;
	state->u_movie = NULL;
	state->pec_list = NULL;
	state->pec_list_end = NULL;
	state->tag_buffer = NULL;
	state->tag_stream = NULL;
	state->tag_stream_end = NULL;
	state->scope_stack = NULL;
	return 0;
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
