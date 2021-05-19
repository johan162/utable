/* =========================================================================
 * File:        xstr.h
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

#ifndef XSTR_H
#define	XSTR_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <string.h>

size_t xstrlcpy(char *dst, const char *src, size_t size);

size_t xstrlcat(char *dst, const char *src, size_t size);

size_t xmb_offset(const char *s, int charnum);

size_t utf8len(const char *s);


#ifdef	__cplusplus
}
#endif

#endif	/* XSTR_H */
