/* =========================================================================
 * File:        xstr.c
 * Description: Internal string helper function with extra protection
 * Author:      Johan Persson (johan162@gmail.com)
 *
 * Copyright (C) 2021 Johan Persson
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>
 * =========================================================================
 */

// We want the full POSIX and C99 standard
#define _GNU_SOURCE

#include "xstr.h"

#ifdef __APPLE__
static void *mempcpy(void *to, const void *from, size_t size) {
    memcpy(to, from, size);
    return ((void *)((char *)to + size));
}
#endif

/**
 * Copy the string src to dst, but no more than size - 1 bytes, and
 * null-terminate dst. NOTE that this is not UTF8 safe. Please see
 * xmbstrncpy() for a UTF8 safe version.
 *
 * This function is the same as BSD strlcpy().
 *
 * @param dst destination buffer
 * @param src source string
 * @param size copy at maximum this number of bytes
 * @return the length of src
 */
size_t 
xstrlcpy(char *dst, const char *src, size_t size) {
    *dst = '\0';
    if (size == 0) return 0;
    const size_t size2 = strnlen(src, size) + 1;  // Must include terminating 0
    if (size2 == 1) return 0;
    *((char *)mempcpy(dst, src, (size < size2 ? size : size2) - 1)) = '\0';
    return strnlen(dst, size);
}

/**
 * String concatenation with extra safety. Note that this is not UTF8 safe
 * @param dst Target string buffer
 * @param src Source string buffer
 * @param size Maximum new total size (in bytes)
 * @return final length of string
 */
size_t 
xstrlcat(char *dst, const char *src, size_t size) {
    if (size == 0) return 0;

    if (strnlen(dst, size - 1) == size - 1) return size - 1;
    if (strlen(src) + strlen(dst) < size) {
        strncat(dst, src, size - 1 - strlen(dst));
        dst[size - 1] = '\0';
    }
    return strnlen(dst, size);
}

// The start of a UTF8 sequence always have the top two bits set
// This macro tests if we are at the start of a UTF character
#define isutf(c) (((c)&0xc0) != 0x80)

/**
 * Find the byte offset in buffer for the charnum:th multibyte character
 * @param s
 * @param charnum
 * @return Offset
 */
size_t 
xmb_offset(const char *s, int charnum) {
    size_t offs = 0;
    while (charnum > 0 && s[offs]) {
        (void)(isutf(s[++offs]) || isutf(s[++offs]) || isutf(s[++offs]) ||
               ++offs);
        charnum--;
    }
    return offs;
}

/**
 * Find the number of Unicode code point (characters) in the byte sequence
 * pointed to by 's' Note on implementation: A unicode sequence is one of:
 * 00000000 -- 0000007F: 	0xxxxxxx
 * 00000080 -- 000007FF: 	110xxxxx 10xxxxxx
 * 00000800 -- 0000FFFF: 	1110xxxx 10xxxxxx 10xxxxxx
 * 00010000 -- 001FFFFF: 	11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
 * And we can count the code points by masking the top nibbles first 2 bits and
 * check if these are not 0x80
 * @param s
 * @return Number of UTF8 code points (characters)
 */

size_t utf8len(const char *s) {
    if (NULL == s) return 0;
    size_t len = 0;
    for (; *s; ++s) {
        if (isutf(*s)) ++len;
    }
    return len;
}

/* EOF */
