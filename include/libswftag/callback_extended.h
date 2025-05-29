/*--------------------------------------------------------------Indexes--------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------|-----------------------------------------------------------------*/

#define CB_PRE_TAG_CHECK 0	// Callback at check_tag's beginning
#define CB_POST_TAG_CHECK 1	// Callback at check_tag's conclusion

#define CB_N_CALLBACKS 2	// Number of defined callbacks
// Currently only a maximum of 32 callbacks can be supported. We'll see if we need to increase that number

err callback_pre_tag_check(pdata *state);
err callback_post_tag_check(pdata *state);

static err (*callback_functions[CB_N_CALLBACKS])(pdata *, swf_tag *) = {&callback_pre_tag_check, &callback_post_tag_check};

void set_callback_flag(pdata *state, ui8 callbackindex, ui8 level)
{
	callbackindex &= 0x1F;
	callbackindex %= CB_POST_TAG_CHECK;
	state->callback_flags &= ~((ui32)(1<<callbackindex));
	state->callback_flags |= (level)? 1 * (1<<callbackindex): 0;
	return;
}

ui8 get_callback_flag(pdata *state, ui8 callbackindex)
{
	callbackindex &= 0x1F;
	callbackindex %= CB_POST_TAG_CHECK;
	return ((state->callback_flags) & (1<<callbackindex));
}
