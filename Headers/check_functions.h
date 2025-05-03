#include"swftag.h"

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
err_ptr spawn_tag(int tag, ui32 size, uchar *tag_data);
err_ptr get_tag(uchar *buffer, pdata *state);

err_int swf_rect_parse(RECT *rect, pdata *state, uchar *rect_buf, swf_tag *tag);
err_int swf_matrix_parse(MATRIX *mat, pdata *state, uchar *mat_buf, swf_tag *tag);
err_int swf_color_transform_parse(COLOR_TRANSFORM *colt, pdata *state, uchar *colt_buf, swf_tag *tag);

err file_header_verification(pdata *state);
err check_tag_stream(pdata *state);
err check_file_validity(FILE *swf, pdata *state);
