/* -*- mode:c; c-file-style:"k&r"; c-basic-offset: 4; tab-width:4; indent-tabs-mode:nil; mode:auto-fill; fill-column:78; -*- */
/* vim: set ts=4 sw=4 et tw=78 fo=cqt wm=0: */

/* Copyright (C) 2014 OSCAR lab, Stony Brook University
   This file is part of Graphene Library OS.

   Graphene Library OS is free software: you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

   Graphene Library OS is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/*
 * path.c
 *
 * This file contains functions to read app config (manifest) file and create
 * a tree to lookup / access config values.
 */

#include <api.h>
#include <pal_error.h>

int get_norm_path (const char * path, char * buf, int offset, int size)
{
    int head = offset;
    char c, c1;
    const char * p = path;

    while (head) { /* find the real head, not interrupted by dot-dot */
        if (head > 1 && buf[head - 1] == '.' && buf[head - 2] == '.')
            break;
        head--;
    }

    for (c = '/' ; c ; c = c1, p++) {
        c1 = *p;
        if (c == '/') {     /* find a slash, or the beginning of the path */
            if (c1 == 0)    /* no more path */
                break;
            if (c1 == '/')  /* consequential slashes */
                continue;
            if (c1 == '.') {    /* find a dot, can be dot-dot or a file */
                c1 = *(++p);
                if (c1 == 0)    /* no more path */
                    break;
                if (c1 == '/')  /* a dot, skip it */
                    continue;
                if (c1 == '.') {    /* must be dot-dot */
                    c1 = *(++p);
                    if (c1 != 0 && c1 != '/')   /* must be the end or a slash */
                        return -PAL_ERROR_INVAL;
                    if (offset > head) {    /* remove the last token */
                        while (offset > head && buf[--offset] != '/');
                    } else {
                        if (offset) {   /* add a slash */
                            if (offset >= size - 1)
                                return -PAL_ERROR_TOOLONG;
                            buf[offset++] = '/';
                        }               /* add a dot-dot */
                        if (offset >= size - 2)
                            return -PAL_ERROR_TOOLONG;
                        buf[offset++] = '.';
                        buf[offset++] = '.';
                        head = offset;
                    }
                } else { /* it's a file */
                    if (offset) {   /* add a slash */
                        if (offset >= size - 1)
                            return -PAL_ERROR_TOOLONG;
                        buf[offset++] = '/';
                    }
                    if (offset >= size - 1)
                        return -PAL_ERROR_TOOLONG;
                    buf[offset++] = '.';
                }
                continue;
            }
        }
        if (offset || c != '/' || *path == '/') {
            if (offset >= size - 1)
                return -PAL_ERROR_TOOLONG;
            buf[offset++] = c;
        }
    }

    buf[offset] = 0;
    return offset;
}

