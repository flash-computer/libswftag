# libswftag
Under construction flash verification and automation library

Things to do and fix:

- Check validity and the prototyped decompression functions, currently only accept a FILE pointer, make a separate family of functions for examining an already filled buffer and separate out the common parts into a different function.

- Implement all the check functions for every single tag. (I'm currently in the process of implementing tags 0-25, you can choose a later batch and start working on that if you want).
		Some ANTI-FEATURES(NOT for imageboard use):
		2(a). Optional mp3/jpg/png/wav/svg/flv verification through third party programs. Anti feature in some ways but it's useful for personal use.
		2(b). Optional recursive swf verification in DefineBinaryData tags.

- Implement the decompression functions, implement use of liblzma and zlib in the meanwhile. (Working on it too, but it's lower in priority)

- add some conditional macros for platforms to replace the slow and bulky standard conformant solutions with faster and more natural solutions (like signed compare and loads etc.) for the platforms that support those.

- Better macro for an unsigned 32 bit integer type for a give platform to replace the unsigned long long everywhere.

- Add an option to memcpy the tag_data for a swf_tag from the main movie buffer to somewhere else. Goes hand in hand with the next bullet.

- A custom allocator for platforms that support it, to put some random airgap at runtime between allocations(possibly even order) to prevent exploits with a low bar. Maybe it's a little OpenBSDism but I think it's effective.

- Gather documentation on when tags were added into the format.

- Thorough testing (it's not really in the state for testing right now).

Links/Resources:

- [Adobe's swf file format spec](https://web.archive.org/web/20210609225053/https://www.adobe.com/content/dam/acom/en/devnet/pdf/swf-file-format-spec.pdf)
- [Swf tags list](https://www.m2osw.com/swf_tags)
- [JPEXS's FFDec's wiki page section for additional links](https://github.com/jindrapetrik/jpexs-decompiler/wiki/Links)

