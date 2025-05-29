/*-----------------------------------------------------------------------------------------------------------------------------------*/
/*--Unstable Internal Interface of the Library. Use externally only if you know what you're doing------------------------------------*/
/*--No Guarantees provided for the consistency of these internal features------------------------------------------------------------*/
/*--If planning external use. It is recommended to recompile the library with the flag ENABLE_INTERNAL_RUNTIME_CHECKS----------------*/
/*-----------------------------------------------------------------|-----------------------------------------------------------------*/

err check_invalidtag(pdata *state, swf_tag *tag_data);

err check_definebitslossless_common(pdata *state, swf_tag *tag_data);
err check_soundstreamhead_common(pdata *state, swf_tag *tag_data);
err check_definetext_common(pdata *state, swf_tag *tag_data);

err check_mythical_common(pdata *state, swf_tag *tag_data);

err check_end(pdata *state, swf_tag *tag_data);
err check_showframe(pdata *state, swf_tag *tag_data);
err check_defineshape(pdata *state, swf_tag *tag_data);
#define check_freecharacter check_mythical_common
err check_placeobject(pdata *state, swf_tag *tag_data);
err check_removeobject(pdata *state, swf_tag *tag_data);
err check_definebitsjpeg(pdata *state, swf_tag *tag_data);
err check_definebutton(pdata *state, swf_tag *tag_data);
err check_jpegtables(pdata *state, swf_tag *tag_data);
err check_setbackgroundcolor(pdata *state, swf_tag *tag_data);
err check_definefont(pdata *state, swf_tag *tag_data);
#define check_definetext check_definetext_common
err check_doaction(pdata *state, swf_tag *tag_data);
err check_definefontinfo(pdata *state, swf_tag *tag_data);
err check_definesound(pdata *state, swf_tag *tag_data);
err check_startsound(pdata *state, swf_tag *tag_data);
#define check_stopsound check_startsound
err check_definebuttonsound(pdata *state, swf_tag *tag_data);
#define check_soundstreamhead check_soundstreamhead_common
err check_soundstreamblock(pdata *state, swf_tag *tag_data);
#define check_definebitslossless check_definebitslossless_common
err check_definebitsjpeg2(pdata *state, swf_tag *tag_data);
err check_defineshape2(pdata *state, swf_tag *tag_data);
err check_definebuttoncxform(pdata *state, swf_tag *tag_data);
err check_protect(pdata *state, swf_tag *tag_data);
#define check_pathsarepostscript check_mythical_common
err check_placeobject2(pdata *state, swf_tag *tag_data);
err check_removeobject2(pdata *state, swf_tag *tag_data);
#define check_syncframe check_mythical_common
#define check_freeall check_mythical_common
err check_defineshape3(pdata *state, swf_tag *tag_data);
#define check_definetext2 check_definetext_common
err check_definebutton2(pdata *state, swf_tag *tag_data);
err check_definebitsjpeg3(pdata *state, swf_tag *tag_data);
#define check_definebitslossless2 check_definebitslossless_common
err check_defineedittext(pdata *state, swf_tag *tag_data);
err check_definevideo(pdata *state, swf_tag *tag_data);
err check_definesprite(pdata *state, swf_tag *tag_data);
err check_namecharacter(pdata *state, swf_tag *tag_data);
err check_productinfo(pdata *state, swf_tag *tag_data);
#define check_definetextformat check_mythical_common
err check_framelabel(pdata *state, swf_tag *tag_data);
#define check_soundstreamhead2 check_soundstreamhead_common
err check_definemorphshape(pdata *state, swf_tag *tag_data);
err check_generateframe(pdata *state, swf_tag *tag_data);
err check_definefont2(pdata *state, swf_tag *tag_data);
err check_generatorcommand(pdata *state, swf_tag *tag_data);
#define check_definecommandobject check_mythical_common
#define check_characterset check_mythical_common
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
