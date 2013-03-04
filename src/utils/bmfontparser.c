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

#include "bmfontparser.h"
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>

/*******************************************************************************

  Section "info" fields

  - face       This is the name of the true type font.
  - size       The size of the true type font.
  - bold       The font is bold.
  - italic     The font is italic.
  - charset    The name of the OEM charset used (when not unicode).
  - unicode    Set to 1 if it is the unicode charset.
  - stretchH   The font height stretch in percentage. 100% means no stretch.
  - smooth     Set to 1 if smoothing was turned on.
  - aa         The supersampling level used. 1 means no supersampling was used.
  - padding    The padding for each character (up, right, down, left).
  - spacing    The spacing for each character (horizontal, vertical).
  - outline    The outline thickness for the characters.

  Section "common" fields

  - lineHeight   This is the distance in pixels between each line of text.
  - base         The number of pixels from the absolute top of the line to the
                 base of the characters.
  - scaleW       The width of the texture, normally used to scale the x pos of
                 the character image.
  - scaleH       The height of the texture, normally used to scale the y pos of
                 the character image.
  - pages        The number of texture pages included in the font.
  - packed       Set to 1 if the monochrome characters have been packed into each
                 of the texture channels. In this case alphaChnl describes what is
                 stored in each channel.
  - alphaChnl    Set to 0 if the channel holds the glyph data, 1 if it holds the
                 outline, 2 if it holds the glyph and the outline, 3 if its set to
                 zero, and 4 if its set to one.
  - redChnl      Set to 0 if the channel holds the glyph data, 1 if it holds the
                 outline, 2 if it holds the glyph and the outline, 3 if its set to
                 zero, and 4 if its set to one.
  - greenChnl    Set to 0 if the channel holds the glyph data, 1 if it holds the
                 outline, 2 if it holds the glyph and the outline, 3 if its set to
                 zero, and 4 if its set to one.
  - blueChnl     Set to 0 if the channel holds the glyph data, 1 if it holds the
                 outline, 2 if it holds the glyph and the outline, 3 if its set to
                 zero, and 4 if its set to one.

  Section "char" fields

  - id        The character id.
  - x         The left position of the character image in the texture.
  - y         The top position of the character image in the texture.
  - width     The width of the character image in the texture.
  - height    The height of the character image in the texture.
  - xoffset   How much the current position should be offset when copying
              the image from the texture to the screen.
  - yoffset   How much the current position should be offset when copying
              the image from the texture to the screen.
  - xadvance  How much the current position should be advanced after drawing the character.
  - page      The texture page where the character image is found.
  - chnl      The texture channel where the character image is found
              (1 = blue, 2 = green, 4 = red, 8 = alpha, 15 = all channels).

  Section "kerning" fields

  - first     The first character id.
  - second    The second character id.
  - amount    How much the x position should be adjusted when drawing the second
               character immediately following the first.

  Section "page" fields

  - id        The page id.
  - file      The texture file name.

*******************************************************************************/


static inline char *bmFontSkipSpaces(char *str)
{
    char *ptr = str;

    while (*ptr == ' ' || *ptr == '\t')
        ptr++;

    return ptr;
}

static inline int bmFontParseInt(char *value, int *intValue)
{
    if (strspn(value, "-+0123456789") != strlen(value))
        return 0;

    long int v = strtol(value, 0, 10);

    *intValue = v;
    return (v || strcmp(value, "0") == 0);
}

static inline char *bmFontStrChr(char *str, char c1, char c2, char c3)
{
    char *c;

    for (c = str; *c != 0; c++) {
        if (*c == c1 || *c == c2 || *c == c3)
            return c;
    }

    return 0;
}

static char *bmFontParseField(char *str, char **fieldName, char **fieldValue, int *ok)
{
    int operatorFound = 0;
    char *fieldCursor = str;

    // remove left spaces
    fieldCursor = bmFontSkipSpaces(fieldCursor);

    // no more fields to parse
    if (*fieldCursor == 0) {
        *ok = 0;
        return 0;
    }

    // fieldName index
    *fieldName = fieldCursor;

    // find fieldName limit
    fieldCursor = bmFontStrChr(fieldCursor, ' ', '=', '\t');

    if (fieldCursor == 0) {
        *ok = 0;
        return 0;
    }

    // check if it's operator char
    operatorFound = (*fieldCursor ==  '=');

    // split field name and move cursor
    *fieldCursor = 0;
    fieldCursor += 1;

    if (*fieldCursor == 0) {
        *ok = 0;
        return 0;
    }

    // skip spaces and check for equal operators
    while (*fieldCursor == ' ' || *fieldCursor == '=' || *fieldCursor == '\t') {
        if (*fieldCursor == '=') {
            // just one operator is expected
            if (operatorFound) {
                *ok = 0;
                return 0;
            }

            operatorFound = 1;
        }

        fieldCursor++;
    }

    if (!operatorFound || *fieldCursor == 0) {
        *ok = 0;
        return 0;
    }

    *fieldValue = fieldCursor;

    // handle string values with spaces
    if (*fieldCursor == '\"') {
        fieldCursor++;

        while (*fieldCursor != '\"') {
            if (*fieldCursor == 0) {
                *ok = 0;
                return 0;
            }

            fieldCursor++;
        }

        fieldCursor++;
    }

    // find fieldValue limit
    fieldCursor = bmFontStrChr(fieldCursor, ' ', ' ', '\t');

    if (fieldCursor) {
        *fieldCursor = 0; // split fieldValue
        fieldCursor += 1;
    }

    *ok = 1;
    return fieldCursor;
}

static int bmFontParseInfo(BMFont *font, char *str)
{
    int success;
    int intValue;
    char *fieldName;
    char *fieldValue;

    while (str) {
        str = bmFontParseField(str, &fieldName, &fieldValue, &success);

        if (!success)
            return 0;

        if (strcmp(fieldName, "face") == 0) {
            strcpy(font->info.face, fieldValue);
        } else if (strcmp(fieldName, "charset") == 0) {
            strcpy(font->info.charset, fieldValue);
        } else if (strcmp(fieldName, "padding") == 0) {
            // TODO
        } else if (strcmp(fieldName, "spacing") == 0) {
            // TODO
        } else {
            if (!bmFontParseInt(fieldValue, &intValue))
                return 0;

            if (strcmp(fieldName, "size") == 0)
                font->info.size = intValue;
            else if (strcmp(fieldName, "bold") == 0)
                font->info.bold = intValue;
            else if (strcmp(fieldName, "italic") == 0)
                font->info.italic = intValue;
            else if (strcmp(fieldName, "unicode") == 0)
                font->info.unicode = intValue;
            else if (strcmp(fieldName, "stretchH") == 0)
                font->info.stretchH = intValue;
            else if (strcmp(fieldName, "smooth") == 0)
                font->info.smooth = intValue;
            else if (strcmp(fieldName, "aa") == 0)
                font->info.aa = intValue;
            else if (strcmp(fieldName, "outline") == 0)
                font->info.outline = intValue;
        }
    }

    return 1;
}

static int bmFontParseCommon(BMFont *font, char *str)
{
    int success;
    int intValue;
    char *fieldName;
    char *fieldValue;

    while (str) {
        str = bmFontParseField(str, &fieldName, &fieldValue, &success);

        if (!success)
            return 0;

        if (!bmFontParseInt(fieldValue, &intValue))
            return 0;

        if (strcmp(fieldName, "scaleW") == 0)
            font->common.scaleW = intValue;
        else if (strcmp(fieldName, "scaleH") == 0)
            font->common.scaleH = intValue;
        else if (strcmp(fieldName, "pages") == 0)
            font->common.pages = intValue;
        else if (strcmp(fieldName, "lineHeight") == 0)
            font->common.lineHeight = intValue;
        else if (strcmp(fieldName, "base") == 0)
            font->common.base = intValue;
        else if (strcmp(fieldName, "packed") == 0)
            font->common.packed = intValue;
        else if (strcmp(fieldName, "redChnl") == 0)
            font->common.redChannel = intValue;
        else if (strcmp(fieldName, "greenChnl") == 0)
            font->common.greenChannel = intValue;
        else if (strcmp(fieldName, "blueChnl") == 0)
            font->common.blueChannel = intValue;
        else if (strcmp(fieldName, "alphaChnl") == 0)
            font->common.alphaChannel = intValue;
    }

    return 1;
}

static BMFontChar *bmFontParseChar(char *str)
{
    int success;
    int intValue;
    char *fieldName;
    char *fieldValue;

    BMFontChar *node = (BMFontChar *)malloc(sizeof(BMFontChar));
    memset(node, 0x0, sizeof(BMFontChar));

    while (str) {
        str = bmFontParseField(str, &fieldName, &fieldValue, &success);

        if (!success)
            break;

        if (!bmFontParseInt(fieldValue, &intValue)) {
            success = 0;
            break;
        }

        if (strcmp(fieldName, "id") == 0)
            node->id = intValue;
        else if (strcmp(fieldName, "x") == 0)
            node->x = intValue;
        else if (strcmp(fieldName, "y") == 0)
            node->y = intValue;
        else if (strcmp(fieldName, "width") == 0)
            node->width = intValue;
        else if (strcmp(fieldName, "height") == 0)
            node->height = intValue;
        else if (strcmp(fieldName, "xoffset") == 0)
            node->xoffset = intValue;
        else if (strcmp(fieldName, "yoffset") == 0)
            node->yoffset = intValue;
        else if (strcmp(fieldName, "xadvance") == 0)
            node->xadvance = intValue;
        else if (strcmp(fieldName, "page") == 0)
            node->page = intValue;
        else if (strcmp(fieldName, "chnl") == 0)
            node->channel = intValue;
    }

    if (!success) {
        free(node);
        return 0;
    }

    return node;
}

static BMFontKerning *bmFontParseKerning(char *str)
{
    int success;
    int intValue;
    char *fieldName;
    char *fieldValue;

    BMFontKerning *node = (BMFontKerning *)malloc(sizeof(BMFontKerning));
    memset(node, 0x0, sizeof(BMFontKerning));

    while (str) {
        str = bmFontParseField(str, &fieldName, &fieldValue, &success);

        if (!success)
            break;

        if (!bmFontParseInt(fieldValue, &intValue)) {
            success = 0;
            break;
        }

        if (strcmp(fieldName, "first") == 0)
            node->first = intValue;
        else if (strcmp(fieldName, "second") == 0)
            node->second = intValue;
        else if (strcmp(fieldName, "amount") == 0)
            node->amount = intValue;
    }

    if (!success) {
        free(node);
        return 0;
    }

    return node;
}

static BMFontPage *bmFontParsePage(char *str)
{
    int success;
    int intValue;
    char *fieldName;
    char *fieldValue;

    BMFontPage *node = (BMFontPage *)malloc(sizeof(BMFontPage));
    memset(node, 0x0, sizeof(BMFontPage));

    while (str) {
        str = bmFontParseField(str, &fieldName, &fieldValue, &success);

        if (!success)
            break;

        if (strcmp(fieldName, "file") == 0) {
            strcpy(node->fileName, fieldValue);
        } else if (strcmp(fieldName, "id") == 0) {
            if (!bmFontParseInt(fieldValue, &intValue)) {
                success = 0;
                break;
            }

            node->id = intValue;
        }
    }

    if (!success) {
        free(node);
        return 0;
    }

    return node;
}

BMFont *bmFontCreate()
{
    BMFont *result = (BMFont *)malloc(sizeof(BMFont));
    memset(result, 0x0, sizeof(BMFont));
    return result;
}

BMFont *bmFontParse(const char *text, char **error)
{
    char *str = strdup(text);
    const size_t len = strlen(str);

    int success = 1;
    char *cursor = str;
    char *endOfFile = str + len;

    BMFontChar *charNode = 0;
    BMFontChar *lastCharNode = 0;
    BMFontKerning *kerningNode = 0;
    BMFontKerning *lastKerningNode = 0;
    BMFontPage *pageNode = 0;
    BMFontPage *lastPageNode = 0;

    BMFont *result = bmFontCreate();

    char *line;
    char *endOfLine;
    char *firstToken;
    char *fieldCursor;
    int lineNumber = 0;
    char errorMessage[100];

    while (cursor < endOfFile) {
        // find end of line
        endOfLine = strchr(cursor, '\n');

        if (endOfLine) {
            *endOfLine = 0;
            line = cursor;
            cursor = (endOfLine + 1);
        } else {
            line = cursor;
            cursor = endOfFile;
            endOfLine = endOfFile;
        }

        lineNumber++;

        // remove left spaces in line
        line = bmFontSkipSpaces(line);

        // skip empty lines
        if (*line == 0)
            continue;

        firstToken = line;

        // get fields initial index
        fieldCursor = bmFontStrChr(firstToken, ' ', ' ', '\t');

        if (fieldCursor == 0) {
            success = 0;
            break;
        }

        // split first token and move to first field pos
        *fieldCursor = 0;
        fieldCursor = bmFontSkipSpaces(fieldCursor + 1);

        if (*fieldCursor == 0) {
            success = 0;
            break;
        }

        if (strcmp(firstToken, "info") == 0) {
            success = bmFontParseInfo(result, fieldCursor);
        }
        else if (strcmp(firstToken, "common") == 0) {
            success = bmFontParseCommon(result, fieldCursor);
        }
        else if (strcmp(firstToken, "char") == 0) {
            charNode = bmFontParseChar(fieldCursor);

            if (!charNode)
                success = 0;
            else {
                if (!lastCharNode) {
                    result->chars = charNode;
                    lastCharNode = charNode;
                } else {
                    lastCharNode->next = charNode;
                    lastCharNode = charNode;
                }
            }
        }
        else if (strcmp(firstToken, "kerning") == 0) {
            kerningNode = bmFontParseKerning(fieldCursor);

            if (!kerningNode)
                success = 0;
            else {
                if (!lastKerningNode) {
                    result->kernings = kerningNode;
                    lastKerningNode = kerningNode;
                } else {
                    lastKerningNode->next = kerningNode;
                    lastKerningNode = kerningNode;
                }
            }
        }
        else if (strcmp(firstToken, "page") == 0) {
            pageNode = bmFontParsePage(fieldCursor);

            if (!pageNode)
                success = 0;
            else {
                if (!lastPageNode) {
                    result->pages = pageNode;
                    lastPageNode = pageNode;
                } else {
                    lastPageNode->next = pageNode;
                    lastPageNode = pageNode;
                }
            }
        }
        else {
            success = 0;
        }

        if (!success)
            break;
    }

    free(str);

    if (!success) {
        sprintf(errorMessage, "Syntax error on line %d", lineNumber);

        if (error)
            *error = strdup(errorMessage);

        // delete list if not successful
        if (result)
            bmFontDelete(result);

        return 0;
    }

    if (error)
        *error = 0;

    return result;
}

void bmFontDeleteChars(BMFont *font)
{
    BMFontChar *tmp;
    BMFontChar *fontChar = font->chars;

    while (fontChar) {
        tmp = fontChar->next;
        free(fontChar);
        fontChar = tmp;
    }

    font->chars = 0;
}

void bmFontDeleteKernings(BMFont *font)
{
    BMFontKerning *tmp;
    BMFontKerning *kerning = font->kernings;

    while (kerning) {
        tmp = kerning->next;
        free(kerning);
        kerning = tmp;
    }

    font->kernings = 0;
}

void bmFontDeletePages(BMFont *font)
{
    BMFontPage *tmp;
    BMFontPage *page = font->pages;

    while (page) {
        tmp = page->next;
        free(page);
        page = tmp;
    }

    font->pages = 0;
}

void bmFontDelete(BMFont *font)
{
    bmFontDeleteChars(font);
    bmFontDeleteKernings(font);
    bmFontDeletePages(font);

    free(font);
}
