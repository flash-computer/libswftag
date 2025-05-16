#include"swftag.h"
#include"error.h"
#include<stdio.h>

err movie_file_uncomp(pdata *state, FILE *swf);
err movie_file_deflate(pdata *state, FILE *swf);
err movie_file_lzma(pdata *state, FILE *swf);

err movie_buffer_uncomp(pdata *state, uchar *buffer, ui32 size);
err movie_buffer_deflate(pdata *state, uchar *buffer, ui32 size);
err movie_buffer_lzma(pdata *state, uchar *buffer, ui32 size);