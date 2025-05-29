# libswftag 0.2
Under construction flash verification and automation library

Things to do and fix:

- Implement all the check functions for every single tag. (I'm currently in the process of implementing tags 0-25, you can choose a later batch and start working on that if you want). Some ANTI-FEATURES(NOT for imageboard use):
    - Optional mp3/jpg/png/wav/svg/flv verification through third party programs. Anti feature in some ways but it's useful for personal use.
    - Optional recursive swf verification in DefineBinaryData tags.

- Implement the decompression functions, implement use of ~~liblzma~~(added in commit 23a968dc09523cd21498705e687867343e2b7d9e for files, and in 90c8dd7c27315b8f23e805e98e5095b26df152e4(untested) for buffers) and ~~zlib~~(added in commit 37af56b9d8f36820450c4080a08890636f9a2b32 for files, and in 32fc07f0ab491ccc7027ff77f34d4c8b1b366b00(untested) for buffers) in the meanwhile. (Working on it too, but it's lower in priority)

- Add some conditional macros for platforms to replace the slow and bulky standard conformant solutions with faster and more natural solutions (like signed compare and loads etc.) for the platforms that support those.

- Find the official upper bound on swf versions. Adobe has not released spec for any swfs after 19, but there doesn't seem to be significant deviation in the tag stream. The current higher bound is 43 but that's arbitary based on the reported version from one of the swfs I have.

- Thorough testing (It's testable on uncompressed swfs now! Use [swfcheck](https://github.com/flash-computer/swfcheck) for the testing. ~~And use [swfpack](https://github.com/arkq/swfpack) for the decompression until the decompression algorithms are implemented.~~)

Build Tips:

- If the target architecture is little endian and the environment has `uint32_t` and `uint16_t` defined in stdint.h (Or if you have defined them yourself prior to including the library), define the `SWFTAG_MATH_INLINE` `LITTLE_ENDIAN_MACHINE` and `UINTN_AVAILABLE` prior to inclusion to improve performance slightly.

- More callbacks are provided through the `EXTENDED_CALLBACKS` macro. Further, the `EXTENDED_CALLBACKS_TYPE` macro can be used to select the type of the extended callbacks, with 0 corresponding to Mandatory callbacks and 1 corresponding to conditional callbacks controlled by the `callback_flags` field of the pdata structure. The recommended way of setting, resetting and getting the flags is to use `set_callback_flag` and `get_callback_flag` functions.

Links/Resources:

- [Adobe's swf file format spec](https://web.archive.org/web/20210609225053/https://www.adobe.com/content/dam/acom/en/devnet/pdf/swf-file-format-spec.pdf)
- [Swf tags list](https://www.m2osw.com/swf_tags)
- [JPEXS's FFDec's wiki page section for additional links](https://github.com/jindrapetrik/jpexs-decompiler/wiki/Links)

<h1 align="center"><img src="https://github.com/flash-computer/branding/blob/main/libswftag/libswftag-logo-1024x1024.png"></h1>
