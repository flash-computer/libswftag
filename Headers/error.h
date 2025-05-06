#include"swftag.h"

/*------------------------------------------------------------Error Codes------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------|-----------------------------------------------------------------*/

/* If the high byte is not zero, then it's not an error.
 * Use ER_ERROR to check if the return value is an error
 * use ER_TESTCATEGORY to check if the return value is an error belonging to a particular category
*/

/* Code 0 for every category is reserved for Miscellaneous errors that don't fit neatly under any preexisting category, with the exception of 0x00 */

/* All codes are 1 byte long(0-255) to standardize the library for the default program, which returns the Error code as exit code which are limited to be one 8 bit byte on POSIX systems
   I don't know if that's part of the POSIX standard though, so if anybody can educate me, that'd be helpful */

#define ESW_NIB_HI   0x80  // Swf structure errors
#define ESW_SIGNATURE (0x1 | ESW_NIB_HI)  // Invalid Signature
#define ESW_SHORTFILE (0x2 | ESW_NIB_HI) // File Ended abruptly
#define ESW_IMPROPER (0x3 | ESW_NIB_HI)   // Improper file

#define EFN_NIB_HI   0x60  // Program execution errors
#define EFN_ARGS  (0x1 | EFN_NIB_HI)   // Malformed Arguments

#define EFL_NIB_HI	0x40	// File Handling Errors
#define EFL_OPEN	(0x1 | EFL_NIB_HI)
#define EFL_CLOSE	(0x2 | EFL_NIB_HI)
#define EFL_READ	(0x3 | EFL_NIB_HI)
#define EFL_WRITE	(0x4 | EFL_NIB_HI)
#define EFL_SEEK	(0x5 | EFL_NIB_HI)

#define EMM_NIB_HI	0x20	// Memory Errors
#define EMM_ALLOC	(0x1 | EMM_NIB_HI)

#define WAF_NIB_HIGH 0x00	// Misc Warnings. Valid SWF but with Notable structures, like including both AVM1 and AVM2 tags for example

#define ALL_CLEAR	0x0		// All clear

#define ER_ERROR(code) (!(code & 0xFF00) && (code & 0xFF))
#define ER_TESTCATEGORY(code, category) ((((code & 0xF0) ^ (category & 0xF0))? 0 : 1) && ER_ERROR(code))

// For convinience, don't consider these reliable library features
#define ER_RAISE_ERROR_ERR(variable, error, pdata) (variable)=error_handler(error, pdata);if(ER_ERROR(variable))return(variable);
#define ER_RAISE_ERROR_ERR_PTR(variable, pointer, error, pdata) (variable)=error_handler(error, pdata);if(ER_ERROR(variable))return((err_ptr){pointer, variable});
#define ER_RAISE_ERROR_ERR_INT(variable, integer, error, pdata) (variable)=error_handler(error, pdata);if(ER_ERROR(variable))return((err_int){integer, variable});

/*-----------------------------------------------------------Error Handler-----------------------------------------------------------*/
/*----------------------------------------------------------Defined by user----------------------------------------------------------*/
/*-----------------------------------------------------------------|-----------------------------------------------------------------*/

err error_handler(err code, pdata *state);

// While for now it's a void callback, I'm considering adding return values that control to some extent the flow where the peculiarity was identified. But this is definitely an anti-feature for security so that'll have to wait
void callback_peculiarity(pdata *state, dnode *node);
