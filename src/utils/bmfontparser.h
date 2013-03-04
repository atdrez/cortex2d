/*
Copyright (c) 2013, Adriano Tinoco d'Oliveira Rezende
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. All advertising materials mentioning features or use of this software
   must display the following acknowledgement:
   This product includes software developed by the <organization>.
4. Neither the name of the <organization> nor the
   names of its contributors may be used to endorse or promote products
   derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY <COPYRIGHT HOLDER> ''AS IS'' AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef BMFONTPARSER_H
#define BMFONTPARSER_H

#include <stdio.h>


#ifdef __cplusplus
extern "C" {
#endif


struct _BMFontPage {
    int id;
    char fileName[512];

    struct _BMFontPage *next;
};

struct _BMFontChar
{
    int id;
    int x;
    int y;
    int width;
    int height;
    int xoffset;
    int yoffset;
    int xadvance;
    int page;
    int channel;

    struct _BMFontChar *next;
};

struct _BMFontKerning {
    int first;
    int second;
    int amount;

    struct _BMFontKerning *next;
};

struct _BMFont
{
    struct {
        char face[512];
        int size;
        int bold;
        int italic;
        char charset[512];
        int unicode;
        int stretchH;
        int smooth;
        int aa;
        int vSpacing;
        int hSpacing;
        int outline;
        int topPadding;
        int bottomPadding;
        int leftPadding;
        int rightPadding;
    } info;

    struct {
        int lineHeight;
        int base;
        int scaleW;
        int scaleH;
        int pages;
        int packed;
        int redChannel;
        int greenChannel;
        int blueChannel;
        int alphaChannel;
    } common;

    struct _BMFontPage *pages;
    struct _BMFontChar *chars;
    struct _BMFontKerning *kernings;
};


typedef struct _BMFont BMFont;
typedef struct _BMFontPage BMFontPage;
typedef struct _BMFontChar BMFontChar;
typedef struct _BMFontKerning BMFontKerning;


/*
  Parse a given string and returns the BMFont.

  Returns null if it could not parse the string.

  If an error occurs, the method returns an error string
  that should be freed by the caller of the method.

  Parameters:
  - text: The string that contains the BMFont specification
  - error: An address to store an error message if an error
           occurs. If no error occurs, the pointer will be null.
*/
BMFont *bmFontParse(const char *text, char **error);


/*
  The user should call this method in order to free
  the BMFont instance.

  Parameters:
  - font: the reference of the font that should be deleted
*/
void bmFontDelete(BMFont *font);


#ifdef __cplusplus
}
#endif


#endif
