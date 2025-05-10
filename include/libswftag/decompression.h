#include"swftag.h"
#include"error.h"
#include<stdio.h>

err movie_uncomp(FILE *swf, pdata *state);
err movie_deflate(FILE *swf, pdata *state);
err movie_lzma(FILE *swf, pdata *state);
