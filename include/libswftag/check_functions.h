#include"swftag.h"

#include<stdio.h>

/*-------------------------------------------------------Functions, prototypes-------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------|-----------------------------------------------------------------*/

err_ptr get_tag(pdata *state, uchar *buffer);
err_ptr check_tag(pdata *state, swf_tag *tag);
err_ptr spawn_tag(pdata *state, int tag, ui32 size, uchar *tag_data);

err_int swf_rect_parse(pdata *state, RECT *rect, uchar *buf, swf_tag *tag);
err_int swf_matrix_parse(pdata *state, MATRIX *mat, uchar *buf, swf_tag *tag);
err_int swf_color_transform_parse(pdata *state, COLOR_TRANSFORM *colt, uchar *buf, swf_tag *tag);
err_int swf_text_record_parse(pdata *state, TEXT_RECORD *trec, uchar *buf, swf_tag *tag);
err_int swf_sound_info_parse(pdata *state, SOUND_INFO *soin, uchar *buf, swf_tag *tag);

err_int swf_text_record_list_parse(pdata *state, uchar *buf, swf_tag *tag);

err file_header_verification(pdata *state);
err check_tag_stream(pdata *state);
err check_file_validity(pdata *state, FILE *swf);
