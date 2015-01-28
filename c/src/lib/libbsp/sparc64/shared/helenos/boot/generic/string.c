/*
 * Copyright (c) 2001-2004 Jakub Jermar
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * - Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 * - The name of the author may not be used to endorse or promote products
 *   derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/** @addtogroup generic	
 * @{
 */

/*
 * Modifications are made to compile for RTEMS. Remove strncpy() and atoi() 
 *
 */


#include <string.h>

/**
 * @file
 * @brief	String manipulation functions.
 */

/** Return number of characters in a string.
 *
 * @param str		NULL terminated string.
 *
 * @return		Number of characters in str.
 */
size_t strlen(const char *str)
{
	int i;
	
	for (i = 0; str[i]; i++)
		;
	
	return i;
}

/** Compare two NULL terminated strings.
 *
 * Do a char-by-char comparison of two NULL terminated strings.
 * The strings are considered equal iff they consist of the same
 * characters on the minimum of their lengths.
 *
 * @param src		First string to compare.
 * @param dst		Second string to compare.
 *
 * @return		0 if the strings are equal, -1 if first is smaller,
 * 			1 if second smaller.
 *
 */
int strcmp(const char *src, const char *dst)
{
	for (; *src && *dst; src++, dst++) {
		if (*src < *dst)
			return -1;
		if (*src > *dst)
			return 1;
	}
	if (*src == *dst)
		return 0;
	if (!*src)
		return -1;
	return 1;
}


/** Compare two NULL terminated strings.
 *
 * Do a char-by-char comparison of two NULL terminated strings.
 * The strings are considered equal iff they consist of the same
 * characters on the minimum of their lengths and specified maximal
 * length.
 *
 * @param src		First string to compare.
 * @param dst		Second string to compare.
 * @param len		Maximal length for comparison.
 *
 * @return		0 if the strings are equal, -1 if first is smaller,
 * 			1 if second smaller.
 *
 */
int strncmp(const char *src, const char *dst, size_t len)
{
	int i;
	
	for (i = 0; *src && *dst && i < len; src++, dst++, i++) {
		if (*src < *dst)
			return -1;
		if (*src > *dst)
			return 1;
	}
	if (i == len || *src == *dst)
		return 0;
	if (!*src)
		return -1;
	return 1;
}
#if 0
/** Copy NULL terminated string.
 *
 * Copy at most 'len' characters from string 'src' to 'dest'.
 * If 'src' is shorter than 'len', '\0' is inserted behind the
 * last copied character.
 *
 * @param src		Source string.
 * @param dest		Destination buffer.
 * @param len		Size of destination buffer.
 */
void strncpy(char *dest, const char *src, size_t len)
{
	int i;
	for (i = 0; i < len; i++) {
		if (!(dest[i] = src[i]))
			return;
	}
	dest[i-1] = '\0';
}

/** Convert ascii representation to unative_t.
 *
 * Supports 0x for hexa & 0 for octal notation.
 * Does not check for overflows, does not support negative numbers
 *
 * @param text		Textual representation of number.
 * @return		Converted number or 0 if no valid number found.
 */
unative_t atoi(const char *text)
{
	int base = 10;
	unative_t result = 0;

	if (text[0] == '0' && text[1] == 'x') {
		base = 16;
		text += 2;
	} else if (text[0] == '0')
		base = 8;

	while (*text) {
		if (base != 16 &&
		    ((*text >= 'A' && *text <= 'F') ||
		    (*text >='a' && *text <='f')))
			break;
		if (base == 8 && *text >='8')
			break;

		if (*text >= '0' && *text <= '9') {
			result *= base;
			result += *text - '0';
		} else if (*text >= 'A' && *text <= 'F') {
			result *= base;
			result += *text - 'A' + 10;
		} else if (*text >= 'a' && *text <= 'f') {
			result *= base;
			result += *text - 'a' + 10;
		} else
			break;
		text++;
	}

	return result;
}
#endif
/** Move piece of memory to another, possibly overlapping, location.
 *
 * @param dst		Destination address.
 * @param src		Source address.
 * @param len		Number of bytes to move.
 *
 * @return		Destination address.
 */
void *memmove(void *dst, const void *src, size_t len)
{
	char *d = dst;
	const char *s = src;
	if (s < d) {
		while (len--)
			*(d + len) = *(s + len);
	} else {
		while (len--)
			*d++ = *s++;
	}
	
	return dst;
}

/** @}
 */
