#include"../../Headers/swftag.h"
#include"../../Headers/error.h"

#include<stdlib.h>
#include<stdio.h>

/*-----------------------------------------------------------Error Strings-----------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------|-----------------------------------------------------------------*/

/* DO NOT USE. DOES NOT WORK RIGHT NOW LMAO */
/* It honestly brings me immense shame that I do not know how to make multidimensional arrays of strings in c */

/* This is ugly with lots of repetition and wastage, but I kept it this way so that we have a simple static logic to actually print these things. RIP memory */
#define FATAL_ERR_MSG "\x1b[31;1;4;5;7m" "FATAL ERROR:" "\x1b[0m" " "
#define WARN_MSG "\x1b[35;1m" "WARNING:" "\x1b[0m" " "

#define UNKNOWN_ERR_MSG "Unknown Error"

#define undefined_categories_messages {UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG}

/*
swf_signature_err_msg
swf_shortfile_err_msg
swf_tag_err_msg
swf_improper_err_msg
*/
#define swf_error_messages {UNKNOWN_ERR_MSG, "Invalid file signature", "Unexpected end of file", "Invalid tag encountered", "File is improper", UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG}

/*
prog_misc_err_msg
prog_arguments_err_msg
*/
#define prog_error_messages {"Misc Program Error, likely due to the use of an unimplemented feature", "Invalid Argument received", UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG}

/*
file_misc_err_msg
file_open_err_msg
file_close_err_msg
file_read_err_msg
file_write_err_msg
file_seek_err_msg
*/
#define file_error_messages {"Undefined File related error", "Could not read from file", "Could not close file", "Requested read failed", "Requested write failed", "Requested seek failed. Ensure the file is not a stream like standard input.", UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG}

/*
memory_allocfailure_err_msg
*/
#define memory_error_messages {UNKNOWN_ERR_MSG, "Memory allocation faliure", UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG}

/*
all_clear_msg
*/
#define UNKNOWN_PECULIAR_EXIT_MSG "Verification successful with unknown but present caveats"
#define peculiar_exit_messages {"This is not an error. You should never see this", UNKNOWN_PECULIAR_EXIT_MSG, UNKNOWN_PECULIAR_EXIT_MSG, UNKNOWN_PECULIAR_EXIT_MSG, UNKNOWN_PECULIAR_EXIT_MSG, UNKNOWN_PECULIAR_EXIT_MSG, UNKNOWN_PECULIAR_EXIT_MSG, UNKNOWN_PECULIAR_EXIT_MSG, UNKNOWN_PECULIAR_EXIT_MSG, UNKNOWN_PECULIAR_EXIT_MSG, UNKNOWN_PECULIAR_EXIT_MSG, UNKNOWN_PECULIAR_EXIT_MSG, UNKNOWN_PECULIAR_EXIT_MSG, UNKNOWN_PECULIAR_EXIT_MSG, UNKNOWN_PECULIAR_EXIT_MSG, UNKNOWN_PECULIAR_EXIT_MSG}

const static char error_messages[16][16][100] = {peculiar_exit_messages, undefined_categories_messages, memory_error_messages, undefined_categories_messages, file_error_messages, undefined_categories_messages, prog_error_messages, undefined_categories_messages, swf_error_messages, undefined_categories_messages, undefined_categories_messages, undefined_categories_messages, undefined_categories_messages, undefined_categories_messages, undefined_categories_messages, undefined_categories_messages};

/*----------------------------------------------------------Implementations----------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------|-----------------------------------------------------------------*/

err error_handler(err code, pdata *state)
{
	if(!ER_ERROR(code))
	{
		return code;
	}
	fprintf(stderr, "%s%s\n", (code & 0xF0)?FATAL_ERR_MSG:WARN_MSG, error_messages[(code & 0xF0)>>4][code & 0xF]);
	exit(code);
}

void callback_peculiarity(pdata *state, dnode *node)
{
	fprintf(stdout, "Peculiarity encountered: %u", ((peculiar *)(node->data))->pattern);
	return;
}
