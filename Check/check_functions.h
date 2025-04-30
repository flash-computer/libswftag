#include"../swftag.h"

#include<stdio.h>

/*-------------------------------------------------------Functions, prototypes-------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------|-----------------------------------------------------------------*/

err check_invalidtag(swf_tag *tag_data, pdata *state);

err check_end(swf_tag *tag_data, pdata *state);
err check_showframe(swf_tag *tag_data, pdata *state);
err check_defineshape(swf_tag *tag_data, pdata *state);
err check_freecharacter(swf_tag *tag_data, pdata *state);
err check_placeobject(swf_tag *tag_data, pdata *state);
err check_removeobject(swf_tag *tag_data, pdata *state);
err check_definebitsjpeg(swf_tag *tag_data, pdata *state);
err check_definebutton(swf_tag *tag_data, pdata *state);
err check_jpegtables(swf_tag *tag_data, pdata *state);
err check_setbackgroundcolor(swf_tag *tag_data, pdata *state);
err check_definefont(swf_tag *tag_data, pdata *state);
err check_definetext(swf_tag *tag_data, pdata *state);
err check_doaction(swf_tag *tag_data, pdata *state);
err check_definefontinfo(swf_tag *tag_data, pdata *state);
err check_definesound(swf_tag *tag_data, pdata *state);
err check_startsound(swf_tag *tag_data, pdata *state);
err check_definebuttonsound(swf_tag *tag_data, pdata *state);
err check_soundstreamhead(swf_tag *tag_data, pdata *state);
err check_soundstreamblock(swf_tag *tag_data, pdata *state);
err check_definebitslossless(swf_tag *tag_data, pdata *state);
err check_definebitsjpeg2(swf_tag *tag_data, pdata *state);
err check_defineshape2(swf_tag *tag_data, pdata *state);
err check_definebuttoncxform(swf_tag *tag_data, pdata *state);
err check_protect(swf_tag *tag_data, pdata *state);
err check_pathsarepostscript(swf_tag *tag_data, pdata *state);
err check_placeobject2(swf_tag *tag_data, pdata *state);
err check_removeobject2(swf_tag *tag_data, pdata *state);
err check_syncframe(swf_tag *tag_data, pdata *state);
err check_freeall(swf_tag *tag_data, pdata *state);
err check_defineshape3(swf_tag *tag_data, pdata *state);
err check_definetext2(swf_tag *tag_data, pdata *state);
err check_definebutton2(swf_tag *tag_data, pdata *state);
err check_definebitsjpeg3(swf_tag *tag_data, pdata *state);
err check_definebitslossless2(swf_tag *tag_data, pdata *state);
err check_defineedittext(swf_tag *tag_data, pdata *state);
err check_definevideo(swf_tag *tag_data, pdata *state);
err check_definesprite(swf_tag *tag_data, pdata *state);
err check_namecharacter(swf_tag *tag_data, pdata *state);
err check_productinfo(swf_tag *tag_data, pdata *state);
err check_definetextformat(swf_tag *tag_data, pdata *state);
err check_framelabel(swf_tag *tag_data, pdata *state);
err check_soundstreamhead2(swf_tag *tag_data, pdata *state);
err check_definemorphshape(swf_tag *tag_data, pdata *state);
err check_generateframe(swf_tag *tag_data, pdata *state);
err check_definefont2(swf_tag *tag_data, pdata *state);
err check_generatorcommand(swf_tag *tag_data, pdata *state);
err check_definecommandobject(swf_tag *tag_data, pdata *state);
err check_characterset(swf_tag *tag_data, pdata *state);
err check_externalfont(swf_tag *tag_data, pdata *state);
err check_export(swf_tag *tag_data, pdata *state);
err check_import(swf_tag *tag_data, pdata *state);
err check_enabledebugger(swf_tag *tag_data, pdata *state);
err check_doinitaction(swf_tag *tag_data, pdata *state);
err check_definevideostream(swf_tag *tag_data, pdata *state);
err check_videoframe(swf_tag *tag_data, pdata *state);
err check_definefontinfo2(swf_tag *tag_data, pdata *state);
err check_debugid(swf_tag *tag_data, pdata *state);
err check_enabledebugger2(swf_tag *tag_data, pdata *state);
err check_scriptlimits(swf_tag *tag_data, pdata *state);
err check_settabindex(swf_tag *tag_data, pdata *state);
err check_fileattributes(swf_tag *tag_data, pdata *state);
err check_placeobject3(swf_tag *tag_data, pdata *state);
err check_import2(swf_tag *tag_data, pdata *state);
err check_doabcdefine(swf_tag *tag_data, pdata *state);
err check_definefontalignzones(swf_tag *tag_data, pdata *state);
err check_csmtextsettings(swf_tag *tag_data, pdata *state);
err check_definefont3(swf_tag *tag_data, pdata *state);
err check_symbolclass(swf_tag *tag_data, pdata *state);
err check_metadata(swf_tag *tag_data, pdata *state);
err check_definescalinggrid(swf_tag *tag_data, pdata *state);
err check_doabc(swf_tag *tag_data, pdata *state);
err check_defineshape4(swf_tag *tag_data, pdata *state);
err check_definemorphshape2(swf_tag *tag_data, pdata *state);
err check_definesceneandframedata(swf_tag *tag_data, pdata *state);
err check_definebinarydata(swf_tag *tag_data, pdata *state);
err check_definefontname(swf_tag *tag_data, pdata *state);
err check_definebitsjpeg4(swf_tag *tag_data, pdata *state);

#define check_stopsound(data, state) check_startsound(data, state)

err_ptr check_tag(swf_tag *tag, pdata *state);
err_ptr spawn_tag(int tag, ui32 size, char *tag_data);
err_ptr get_tag(char *buffer, pdata *state);

err swf_rect_parse(RECT *rect, pdata *state, char *rect_buf, ui32 limit);
err swf_matrix_parse(MATRIX *mat, pdata *state, char *mat_buf, ui32 limit);
err swf_color_transform_parse(COLOR_TRANSFORM *colt, pdata *state, char *col_buf, ui32 limit);

err file_header_verification(pdata *state);
err check_tag_stream(pdata *state);
err check_validity(pdata *state);
err check_file_validity(FILE *swf, pdata *state);

/*------------------------------------------------------------Static Data------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------|-----------------------------------------------------------------*/

#ifndef CHECK_FUN_STATIC_ARRAY
	#define CHECK_FUN_STATIC_ARRAY

	const static err (*tag_check[])(swf_tag *, pdata *state) = {&check_end, &check_showframe, &check_defineshape, &check_freecharacter, &check_placeobject, &check_removeobject, &check_definebitsjpeg, &check_definebutton, &check_jpegtables, &check_setbackgroundcolor, &check_definefont, &check_definetext, &check_doaction, &check_definefontinfo, &check_definesound, &check_startsound, &check_invalidtag, &check_definebuttonsound, &check_soundstreamhead, &check_soundstreamblock, &check_definebitslossless, &check_definebitsjpeg2, &check_defineshape2, &check_definebuttoncxform, &check_protect, &check_pathsarepostscript, &check_placeobject2, &check_invalidtag, &check_removeobject2, &check_syncframe, &check_invalidtag, &check_freeall, &check_defineshape3, &check_definetext2, &check_definebutton2, &check_definebitsjpeg3, &check_definebitslossless2, &check_defineedittext, &check_definevideo, &check_definesprite, &check_namecharacter, &check_productinfo, &check_definetextformat, &check_framelabel, &check_invalidtag, &check_soundstreamhead2, &check_definemorphshape, &check_generateframe, &check_definefont2, &check_generatorcommand, &check_definecommandobject, &check_characterset, &check_externalfont, &check_invalidtag, &check_invalidtag, &check_invalidtag, &check_export, &check_import, &check_enabledebugger, &check_doinitaction, &check_definevideostream, &check_videoframe, &check_definefontinfo2, &check_debugid, &check_enabledebugger2, &check_scriptlimits, &check_settabindex, &check_invalidtag, &check_invalidtag, &check_fileattributes, &check_placeobject3, &check_import2, &check_doabcdefine, &check_definefontalignzones, &check_csmtextsettings, &check_definefont3, &check_symbolclass, &check_metadata, &check_definescalinggrid, &check_invalidtag, &check_invalidtag, &check_invalidtag, &check_doabc, &check_defineshape4, &check_definemorphshape2, &check_invalidtag, &check_definesceneandframedata, &check_definebinarydata, &check_definefontname, &check_invalidtag, &check_definebitsjpeg4};

#endif
