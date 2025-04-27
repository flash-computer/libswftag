#include"../../swftag.h"
#include"../../error.h"

#include<stdlib.h>
#include<stdio.h>

/*-----------------------------------------------------------Error Strings-----------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------|-----------------------------------------------------------------*/

/* This is ugly with lots of repetition and wastage, but I kept it this way so that we have a simple static logic to actually print these things. RIP memory */
#define FATAL_ERR_MSG "\x1b[31;1;4;5;7m" "FATAL ERROR:" "\x1b[0m" " "
#define WARN_MSG "\x1b[35;1m" "WARNING:" "\x1b[0m" " "

const static char *unknown_err_msg = "Unknown Error Occured";

const static char *undefined_categories_messages[] = {unknown_err_msg, unknown_err_msg, unknown_err_msg, unknown_err_msg, unknown_err_msg, unknown_err_msg, unknown_err_msg, unknown_err_msg, unknown_err_msg, unknown_err_msg, unknown_err_msg, unknown_err_msg, unknown_err_msg, unknown_err_msg, unknown_err_msg, unknown_err_msg};

const static char *swf_signature_err_msg = "Invalid file signature";
const static char *swf_shortfile_err_msg = "Unexpected end of file";
const static char *swf_tag_err_msg = "Invalid tag encountered";
const static char *swf_improper_err_msg = "File is improper";
const static char *swf_error_messages = {unknown_err_msg, swf_signature_err_msg, swf_shortfile_err_msg, swf_tag_err_msg, swf_improper_err_msg, unknown_err_msg, unknown_err_msg, unknown_err_msg, unknown_err_msg, unknown_err_msg, unknown_err_msg, unknown_err_msg, unknown_err_msg, unknown_err_msg, unknown_err_msg, unknown_err_msg};

const static char *prog_misc_err_msg = "Misc Program Error, likely due to the use of an unimplemented feature";
const static char *prog_arguments_err_msg = "Invalid Argument received";
const static char *prog_error_messages[] = {prog_misc_err_msg, prog_arguments_err_msg, unknown_err_msg, unknown_err_msg, unknown_err_msg, unknown_err_msg, unknown_err_msg, unknown_err_msg, unknown_err_msg, unknown_err_msg, unknown_err_msg, unknown_err_msg, unknown_err_msg, unknown_err_msg, unknown_err_msg, unknown_err_msg};

const static char *file_misc_err_msg = "Undefined File related error";
const static char *file_open_err_msg = "Could not read from file";
const static char *file_close_err_msg = "Could not close file";
const static char *file_read_err_msg = "Requested read failed";
const static char *file_write_err_msg = "Requested write failed";
const static char *file_seek_err_msg = "Requested seek failed. Ensure the file is not a stream like standard input.";
const static char *file_error_messages[] = {file_misc_err_msg, file_open_err_msg, file_close_err_msg, file_read_err_msg, file_write_err_msg, file_seek_err_msg, unknown_err_msg, unknown_err_msg, unknown_err_msg, unknown_err_msg, unknown_err_msg, unknown_err_msg, unknown_err_msg, unknown_err_msg, unknown_err_msg, unknown_err_msg};

const static char *memory_allocfailure_err_msg = "Memory allocation faliure";
const static char *memory_error_messages[] = {unknown_err_msg, memory_allocfailure_err_msg, unknown_err_msg, unknown_err_msg, unknown_err_msg, unknown_err_msg, unknown_err_msg, unknown_err_msg, unknown_err_msg, unknown_err_msg, unknown_err_msg, unknown_err_msg, unknown_err_msg, unknown_err_msg, unknown_err_msg, unknown_err_msg};

const static char *all_clear_msg = "This is not an error. You should never see this";
const static char *unknown_peculiar_exit_msg = "Verification successful with unknown but present caveats";
const static char *peculiar_exit_messages[] = {all_clear_msg, unknown_peculiar_exit_msg, unknown_peculiar_exit_msg, unknown_peculiar_exit_msg, unknown_peculiar_exit_msg, unknown_peculiar_exit_msg, unknown_peculiar_exit_msg, unknown_peculiar_exit_msg, unknown_peculiar_exit_msg, unknown_peculiar_exit_msg, unknown_peculiar_exit_msg, unknown_peculiar_exit_msg, unknown_peculiar_exit_msg, unknown_peculiar_exit_msg, unknown_peculiar_exit_msg, unknown_peculiar_exit_msg};

const static char (* error_messages)[16][] = {peculiar_exit_messages, undefined_categories_messages, memory_error_messages, undefined_categories_messages, file_error_messages, undefined_categories_messages, prog_error_messages, undefined_categories_messages, swf_error_messages, undefined_categories_messages, undefined_categories_messages, undefined_categories_messages, undefined_categories_messages, undefined_categories_messages, undefined_categories_messages, undefined_categories_messages};

/*----------------------------------------------------------Implementations----------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------|-----------------------------------------------------------------*/

int error_handler(unsigned char code, pdata *state)
{
	if(!code)
	{
		return 0;
	}
	fprintf(stderr, "%s%s\n", (code & 0xF0)?FATAL_ERR_MSG:WARN_MSG, error_messages[(code & 0xF0)>>4][code & 0xF]);
	exit(code);
}