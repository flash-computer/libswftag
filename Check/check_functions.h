/*-------------------------------------------------------Functions, prototypes-------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------|-----------------------------------------------------------------*/

int check_invalidtag(swf_tag *tag_data, pdata *state)
{
	return ESW_TAG;
}

int check_end(swf_tag *tag_data, pdata *state);
int check_showframe(swf_tag *tag_data, pdata *state);
int check_defineshape(swf_tag *tag_data, pdata *state);
int check_freecharacter(swf_tag *tag_data, pdata *state);
int check_placeobject(swf_tag *tag_data, pdata *state);
int check_removeobject(swf_tag *tag_data, pdata *state);
int check_definebitsjpeg(swf_tag *tag_data, pdata *state);
int check_definebutton(swf_tag *tag_data, pdata *state);
int check_jpegtables(swf_tag *tag_data, pdata *state);
int check_setbackgroundcolor(swf_tag *tag_data, pdata *state);
int check_definefont(swf_tag *tag_data, pdata *state);
int check_definetext(swf_tag *tag_data, pdata *state);
int check_doaction(swf_tag *tag_data, pdata *state);
int check_definefontinfo(swf_tag *tag_data, pdata *state);
int check_definesound(swf_tag *tag_data, pdata *state);
int check_startsound(swf_tag *tag_data, pdata *state);
int check_definebuttonsound(swf_tag *tag_data, pdata *state);
int check_soundstreamhead(swf_tag *tag_data, pdata *state);
int check_soundstreamblock(swf_tag *tag_data, pdata *state);
int check_definebitslossless(swf_tag *tag_data, pdata *state);
int check_definebitsjpeg2(swf_tag *tag_data, pdata *state);
int check_defineshape2(swf_tag *tag_data, pdata *state);
int check_definebuttoncxform(swf_tag *tag_data, pdata *state);
int check_protect(swf_tag *tag_data, pdata *state);
int check_pathsarepostscript(swf_tag *tag_data, pdata *state);
int check_placeobject2(swf_tag *tag_data, pdata *state);
int check_removeobject2(swf_tag *tag_data, pdata *state);
int check_syncframe(swf_tag *tag_data, pdata *state);
int check_freeall(swf_tag *tag_data, pdata *state);
int check_defineshape3(swf_tag *tag_data, pdata *state);
int check_definetext2(swf_tag *tag_data, pdata *state);
int check_definebutton2(swf_tag *tag_data, pdata *state);
int check_definebitsjpeg3(swf_tag *tag_data, pdata *state);
int check_definebitslossless2(swf_tag *tag_data, pdata *state);
int check_defineedittext(swf_tag *tag_data, pdata *state);
int check_definevideo(swf_tag *tag_data, pdata *state);
int check_definesprite(swf_tag *tag_data, pdata *state);
int check_namecharacter(swf_tag *tag_data, pdata *state);
int check_productinfo(swf_tag *tag_data, pdata *state);
int check_definetextformat(swf_tag *tag_data, pdata *state);
int check_framelabel(swf_tag *tag_data, pdata *state);
int check_soundstreamhead2(swf_tag *tag_data, pdata *state);
int check_definemorphshape(swf_tag *tag_data, pdata *state);
int check_generateframe(swf_tag *tag_data, pdata *state);
int check_definefont2(swf_tag *tag_data, pdata *state);
int check_generatorcommand(swf_tag *tag_data, pdata *state);
int check_definecommandobject(swf_tag *tag_data, pdata *state);
int check_characterset(swf_tag *tag_data, pdata *state);
int check_externalfont(swf_tag *tag_data, pdata *state);
int check_export(swf_tag *tag_data, pdata *state);
int check_import(swf_tag *tag_data, pdata *state);
int check_enabledebugger(swf_tag *tag_data, pdata *state);
int check_doinitaction(swf_tag *tag_data, pdata *state);
int check_definevideostream(swf_tag *tag_data, pdata *state);
int check_videoframe(swf_tag *tag_data, pdata *state);
int check_definefontinfo2(swf_tag *tag_data, pdata *state);
int check_debugid(swf_tag *tag_data, pdata *state);
int check_enabledebugger2(swf_tag *tag_data, pdata *state);
int check_scriptlimits(swf_tag *tag_data, pdata *state);
int check_settabindex(swf_tag *tag_data, pdata *state);
int check_fileattributes(swf_tag *tag_data, pdata *state);
int check_placeobject3(swf_tag *tag_data, pdata *state);
int check_import2(swf_tag *tag_data, pdata *state);
int check_doabcdefine(swf_tag *tag_data, pdata *state);
int check_definefontalignzones(swf_tag *tag_data, pdata *state);
int check_csmtextsettings(swf_tag *tag_data, pdata *state);
int check_definefont3(swf_tag *tag_data, pdata *state);
int check_symbolclass(swf_tag *tag_data, pdata *state);
int check_metadata(swf_tag *tag_data, pdata *state);
int check_definescalinggrid(swf_tag *tag_data, pdata *state);
int check_doabc(swf_tag *tag_data, pdata *state);
int check_defineshape4(swf_tag *tag_data, pdata *state);
int check_definemorphshape2(swf_tag *tag_data, pdata *state);
int check_definesceneandframedata(swf_tag *tag_data, pdata *state);
int check_definebinarydata(swf_tag *tag_data, pdata *state);
int check_definefontname(swf_tag *tag_data, pdata *state);
int check_definebitsjpeg4(swf_tag *tag_data, pdata *state);

#define check_stopsound(data, state) check_startsound(data, state)

err_ptr check_tag(swf_tag *tag, pdata *state);

int rect_parse(RECT *rect, pdata *state, char *rect_buf, size_t buf_size);
int file_header_verification(pdata *state);
err_ptr get_new_tag(char *buffer, pdata *state);
int check_tag_stream(pdata *state);

int check_validity(pdata *state);
int check_file_validity(FILE *swf, pdata *state);

/*------------------------------------------------------------Static Data------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------|-----------------------------------------------------------------*/

const static int (*tag_check[])(swf_tag *) = {&check_end, &check_showframe, &check_defineshape, &check_freecharacter, &check_placeobject, &check_removeobject, &check_definebitsjpeg, &check_definebutton, &check_jpegtables, &check_setbackgroundcolor, &check_definefont, &check_definetext, &check_doaction, &check_definefontinfo, &check_definesound, &check_startsound, &check_invalidtag, &check_definebuttonsound, &check_soundstreamhead, &check_soundstreamblock, &check_definebitslossless, &check_definebitsjpeg2, &check_defineshape2, &check_definebuttoncxform, &check_protect, &check_pathsarepostscript, &check_placeobject2, &check_invalidtag, &check_removeobject2, &check_syncframe, &check_invalidtag, &check_freeall, &check_defineshape3, &check_definetext2, &check_definebutton2, &check_definebitsjpeg3, &check_definebitslossless2, &check_defineedittext, &check_definevideo, &check_definesprite, &check_namecharacter, &check_productinfo, &check_definetextformat, &check_framelabel, &check_invalidtag, &check_soundstreamhead2, &check_definemorphshape, &check_generateframe, &check_definefont2, &check_generatorcommand, &check_definecommandobject, &check_characterset, &check_externalfont, &check_invalidtag, &check_invalidtag, &check_invalidtag, &check_export, &check_import, &check_enabledebugger, &check_doinitaction, &check_definevideostream, &check_videoframe, &check_definefontinfo2, &check_debugid, &check_enabledebugger2, &check_scriptlimits, &check_settabindex, &check_invalidtag, &check_invalidtag, &check_fileattributes, &check_placeobject3, &check_import2, &check_doabcdefine, &check_definefontalignzones, &check_csmtextsettings, &check_definefont3, &check_symbolclass, &check_metadata, &check_definescalinggrid, &check_invalidtag, &check_invalidtag, &check_invalidtag, &check_doabc, &check_defineshape4, &check_definemorphshape2, &check_invalidtag, &check_definesceneandframedata, &check_definebinarydata, &check_definefontname, &check_invalidtag, &check_definebitsjpeg4};
