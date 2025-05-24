# libswftag 0.2
Under construction flash verification and automation library

Things to do and fix:

- Implement all the check functions for every single tag. (I'm currently in the process of implementing tags 0-25, you can choose a later batch and start working on that if you want). Some ANTI-FEATURES(NOT for imageboard use):
    - Optional mp3/jpg/png/wav/svg/flv verification through third party programs. Anti feature in some ways but it's useful for personal use.
    - Optional recursive swf verification in DefineBinaryData tags.

- Implement the decompression functions, implement use of ~~liblzma~~(added in commit 23a968dc09523cd21498705e687867343e2b7d9e for files) and ~~zlib~~(added in commit 37af56b9d8f36820450c4080a08890636f9a2b32 for files) in the meanwhile. (Working on it too, but it's lower in priority)

- Add some conditional macros for platforms to replace the slow and bulky standard conformant solutions with faster and more natural solutions (like signed compare and loads etc.) for the platforms that support those.

- Find the official upper bound on swf versions. Adobe has not released spec for any swfs after 19, but there doesn't seem to be significant deviation in the tag stream. The current higher bound is 43 but that's arbitary based on the reported version from one of the swfs I have.

- Thorough testing (It's testable on uncompressed swfs now! Use [swfcheck](https://github.com/flash-computer/swfcheck) for the testing. And use [swfpack](https://github.com/arkq/swfpack) for the decompression until the decompression algorithms are implemented.)

Links/Resources:

- [Adobe's swf file format spec](https://web.archive.org/web/20210609225053/https://www.adobe.com/content/dam/acom/en/devnet/pdf/swf-file-format-spec.pdf)
- [Swf tags list](https://www.m2osw.com/swf_tags)
- [JPEXS's FFDec's wiki page section for additional links](https://github.com/jindrapetrik/jpexs-decompiler/wiki/Links)
