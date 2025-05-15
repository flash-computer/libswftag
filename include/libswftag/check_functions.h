#include"swftag.h"

#include<stdio.h>

/*-------------------------------------------------------Functions, prototypes-------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------|-----------------------------------------------------------------*/

err check_invalidtag(pdata *state, swf_tag *tag_data);

err check_end(pdata *state, swf_tag *tag_data);
err check_showframe(pdata *state, swf_tag *tag_data);
err check_defineshape(pdata *state, swf_tag *tag_data);
err check_freecharacter(pdata *state, swf_tag *tag_data);
err check_placeobject(pdata *state, swf_tag *tag_data);
err check_removeobject(pdata *state, swf_tag *tag_data);
err check_definebitsjpeg(pdata *state, swf_tag *tag_data);
err check_definebutton(pdata *state, swf_tag *tag_data);
err check_jpegtables(pdata *state, swf_tag *tag_data);
err check_setbackgroundcolor(pdata *state, swf_tag *tag_data);
err check_definefont(pdata *state, swf_tag *tag_data);
err check_definetext(pdata *state, swf_tag *tag_data);
err check_doaction(pdata *state, swf_tag *tag_data);
err check_definefontinfo(pdata *state, swf_tag *tag_data);
err check_definesound(pdata *state, swf_tag *tag_data);
err check_startsound(pdata *state, swf_tag *tag_data);
err check_definebuttonsound(pdata *state, swf_tag *tag_data);
err check_soundstreamhead(pdata *state, swf_tag *tag_data);
err check_soundstreamblock(pdata *state, swf_tag *tag_data);
err check_definebitslossless(pdata *state, swf_tag *tag_data);
err check_definebitsjpeg2(pdata *state, swf_tag *tag_data);
err check_defineshape2(pdata *state, swf_tag *tag_data);
err check_definebuttoncxform(pdata *state, swf_tag *tag_data);
err check_protect(pdata *state, swf_tag *tag_data);
err check_pathsarepostscript(pdata *state, swf_tag *tag_data);
err check_placeobject2(pdata *state, swf_tag *tag_data);
err check_removeobject2(pdata *state, swf_tag *tag_data);
err check_syncframe(pdata *state, swf_tag *tag_data);
err check_freeall(pdata *state, swf_tag *tag_data);
err check_defineshape3(pdata *state, swf_tag *tag_data);
err check_definetext2(pdata *state, swf_tag *tag_data);
err check_definebutton2(pdata *state, swf_tag *tag_data);
err check_definebitsjpeg3(pdata *state, swf_tag *tag_data);
err check_definebitslossless2(pdata *state, swf_tag *tag_data);
err check_defineedittext(pdata *state, swf_tag *tag_data);
err check_definevideo(pdata *state, swf_tag *tag_data);
err check_definesprite(pdata *state, swf_tag *tag_data);
err check_namecharacter(pdata *state, swf_tag *tag_data);
err check_productinfo(pdata *state, swf_tag *tag_data);
err check_definetextformat(pdata *state, swf_tag *tag_data);
err check_framelabel(pdata *state, swf_tag *tag_data);
err check_soundstreamhead2(pdata *state, swf_tag *tag_data);
err check_definemorphshape(pdata *state, swf_tag *tag_data);
err check_generateframe(pdata *state, swf_tag *tag_data);
err check_definefont2(pdata *state, swf_tag *tag_data);
err check_generatorcommand(pdata *state, swf_tag *tag_data);
err check_definecommandobject(pdata *state, swf_tag *tag_data);
err check_characterset(pdata *state, swf_tag *tag_data);
err check_externalfont(pdata *state, swf_tag *tag_data);
err check_export(pdata *state, swf_tag *tag_data);
err check_import(pdata *state, swf_tag *tag_data);
err check_enabledebugger(pdata *state, swf_tag *tag_data);
err check_doinitaction(pdata *state, swf_tag *tag_data);
err check_definevideostream(pdata *state, swf_tag *tag_data);
err check_videoframe(pdata *state, swf_tag *tag_data);
err check_definefontinfo2(pdata *state, swf_tag *tag_data);
err check_debugid(pdata *state, swf_tag *tag_data);
err check_enabledebugger2(pdata *state, swf_tag *tag_data);
err check_scriptlimits(pdata *state, swf_tag *tag_data);
err check_settabindex(pdata *state, swf_tag *tag_data);
err check_fileattributes(pdata *state, swf_tag *tag_data);
err check_placeobject3(pdata *state, swf_tag *tag_data);
err check_import2(pdata *state, swf_tag *tag_data);
err check_doabcdefine(pdata *state, swf_tag *tag_data);
err check_definefontalignzones(pdata *state, swf_tag *tag_data);
err check_csmtextsettings(pdata *state, swf_tag *tag_data);
err check_definefont3(pdata *state, swf_tag *tag_data);
err check_symbolclass(pdata *state, swf_tag *tag_data);
err check_metadata(pdata *state, swf_tag *tag_data);
err check_definescalinggrid(pdata *state, swf_tag *tag_data);
err check_doabc(pdata *state, swf_tag *tag_data);
err check_defineshape4(pdata *state, swf_tag *tag_data);
err check_definemorphshape2(pdata *state, swf_tag *tag_data);
err check_definesceneandframedata(pdata *state, swf_tag *tag_data);
err check_definebinarydata(pdata *state, swf_tag *tag_data);
err check_definefontname(pdata *state, swf_tag *tag_data);
err check_definebitsjpeg4(pdata *state, swf_tag *tag_data);
err check_definefont4(pdata *state, swf_tag *tag_data);
err check_enabletelemetry(pdata *state, swf_tag *tag_data);

#define check_stopsound(data, state) check_startsound(data, state)

err_ptr get_tag(pdata *state, uchar *buffer);
err_ptr check_tag(pdata *state, swf_tag *tag);
err_ptr spawn_tag(pdata *state, int tag, ui32 size, uchar *tag_data);

err_int swf_rect_parse(pdata *state, RECT *rect, uchar *buf, swf_tag *tag);
err_int swf_matrix_parse(pdata *state, MATRIX *mat, uchar *buf, swf_tag *tag);
err_int swf_color_transform_parse(pdata *state, COLOR_TRANSFORM *colt, uchar *buf, swf_tag *tag);
err_int swf_text_record_parse(pdata *state, TEXT_RECORD *trec, uchar *buf, swf_tag *tag);

err_int swf_text_record_list_parse(pdata *state, uchar *buf, swf_tag *tag);

err file_header_verification(pdata *state);
err check_tag_stream(pdata *state);
err check_file_validity(pdata *state, FILE *swf);
