/* =========================================================================
 * File:        uniode_tbl.c
 * Description: Functions to create and write a rudimentary table
 *              using unicode page U+25xx characters for nicely
 *              formatted output to a terminal.
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

#include <stdio.h>
#include <sys/param.h>  // To get MIN/MAX
#include <unistd.h>

//#include <stdlib.h>
//#include <string.h>
//#include <syslog.h>

#include "unicode_tbl.h"
#include "xstr.h"

// Always nice to have
#define FALSE 0
#define TRUE 1

// Utility macro to index table matrix
#define TIDX(_r, _c) ((_r)*t->nCol + (_c))

#define LOGPREFIXSIZE 80
#define LOGBUFFERSIZE 256

static t_log_func _logmsg = NULL;
static int _loglevel = 0;
static char _logprefix[LOGPREFIXSIZE];
static char _logbuffer[LOGBUFFERSIZE];

/**
 * Set the fucntion to be used to log messages
 * @param f Logfunction, takes an inte and char *
 * @param loglevel, The log level, always added as first argument when calling
 * logger
 * @param prefix, An optional prefix string to the log message
 */
void
utable_set_logfunc(t_log_func f, int loglevel, char *prefix) {
    _logmsg = f;
    _loglevel = loglevel;
    *_logprefix = 0;
    strncpy(_logprefix, prefix, LOGPREFIXSIZE - 1);
    _logprefix[LOGPREFIXSIZE - 1] = 0;
}

/**
 * Internal helper function to call the externally defined log function to use
 * @param msg Log message
 */
static void
logmsg(char *msg) {
    if (NULL != _logmsg) {
        snprintf(_logbuffer, LOGBUFFERSIZE - 1, "%s : %s", _logprefix, msg);
        _logmsg(_loglevel, _logbuffer);
    }
}

/**
 * Check that row and col arguments are within valid range before accessing
 * cells
 * @param t Table pointer
 * @param row Row of cell
 * @param col Column of cell
 * @return 0 on valid range, -1 otherwise
 */
static int
_utable_rc_chk(table_t *t, size_t row, size_t col) {
    if (row >= t->nRow || col >= t->nCol) {
        char buffer[80];
        snprintf(buffer, 79, "Table cell specified is out of range [%zu, %zu]",
                 row, col);
        logmsg(buffer);
        return -1;
    }
    return 0;
}

/**
 * Initialize a new cell
 * @param t Table pointer
 * @param row Row of cell
 * @param col Column of cell
 */
static void
_utable_init_cell(table_t *t, size_t row, size_t col) {
    tcell_t *c = &t->c[TIDX(row, col)];
    c->t = NULL;
    c->halign = LEFTALIGN;
    c->pRow = row;
    c->pCol = col;
    c->merged = FALSE;
    c->cspan = 1;
    c->rspan = 1;
}

/**
 * Create a new table of the specified size
 * @param nRow Number of rows
 * @param nCol Number of cols
 * @return NULL on error , pointer to the newly created table otherwise
 */
table_t *
utable_create(size_t nRow, size_t nCol) {
    table_t *t = calloc(1, sizeof(table_t));
    if (NULL == t) return NULL;
    t->nRow = nRow;
    t->nCol = nCol++;
    t->headerLine = TRUE;

    ++nRow;
    // We allocate one extra row in case a title should be displayed
    t->c = calloc(nRow * nCol, sizeof(tcell_t));
    t->colwidth = calloc(nCol, sizeof(size_t));
    t->mincolwidth = calloc(nCol, sizeof(size_t));
    if (t->c == NULL || t->colwidth == NULL || t->mincolwidth == NULL) {
        logmsg("CRITICAL : Failed to create table. Out of memory.");
        free(t->c);
        free(t->colwidth);
        free(t->mincolwidth);
        free(t);
        return NULL;
    }

    for (size_t r = 0; r < nRow; r++) {
        for (size_t c = 0; c < nCol; c++) {
            _utable_init_cell(t, r, c);
        }
    }

    return t;
}

/**
 * Initialize the table with string from a matrix. It is the calling routines
 * responsibility that the size of the data matrix matches that of the table.
 * @param t Table pointer
 * @param data A matrix of data to be used as initialization
 * @return 0 on success, -1 on failure
 */
int
utable_set(table_t *t, char *data[]) {
    for (size_t r = 0; r < t->nRow; r++) {
        for (size_t c = 0; c < t->nCol; c++) {
            if (utable_set_cell(t, r, c, data[TIDX(r, c)])) return -1;
        }
    }
    return 0;
}

/**
 * Combine table creation and initialization with error check
 * @param t Table pointer
 * @param nRow Number of rows
 * @param nCol Number of cols
 * @param data A matrix of data to be used as initialization
 * @return NULL on failure, pointer to new table otherwise
 */
table_t *
utable_create_set(size_t nRow, size_t nCol, char *data[]) {
    table_t *t = utable_create(nRow, nCol);
    if (NULL == t) {
        return NULL;
    }
    if (-1 == utable_set(t, data)) {
        utable_free(t);
        return NULL;
    }
    return t;
}

/**
 * Set the title row
 * @param t Table pointer
 * @param nRow Number of rows
 * @param nCol Number of cols
 * @param data A matrix of data to be used as initialization
 * @return NULL on failure, pointer to new table otherwise
 */
int
utable_set_coltitles(table_t *t, char *titles[]) {
    int row = 0;
    if (t->titleCopied) row = 1;
    for (size_t c = 0; c < t->nCol; c++) {
        if (utable_set_cell(t, row, c, titles[c])) return -1;
    }
    return 0;
}

/**
 * Free (destroy) a previously created table
 * @param t Table pointer
 */
void
utable_free(table_t *t) {
    for (size_t r = 0; r < t->nRow; r++) {
        for (size_t c = 0; c < t->nCol; c++) {
            if (t->c[TIDX(r, c)].t) {
                free(t->c[TIDX(r, c)].t);
            }
        }
    }
    free(t->c);
    free(t->colwidth);
    free(t->mincolwidth);
    free(t->title);
    free(t);
}

/**
 * Set a title to the table. How the title is displayed is determined
 * by the table style.
 * @param t Table pointer
 * @param title
 */
void
utable_set_title(table_t *t, char *title, titlestyle_t style) {
    if (t->title) free(t->title);
    t->title = strdup(title);
    t->titleStyle = style;
}

/**
 * Enable or disable the extra line below the header row (the first row in the
 * data). The style of the line is determined by the table style. The hedaer
 * line is by default enabled when the table is created.
 * @param t Table pointer
 * @param headerLine Flag to disable/enable header
 */
void
utable_set_headerline(table_t *t, _Bool headerLine) {
    t->headerLine = headerLine;
}

/**
 * Get a reference to the specified cell
 * @param t Table pointer
 * @param row Row of cell
 * @param col Column of cell
 * @return NULL if row/col is out of range, the cell otherwise
 */
static tcell_t *
_utable_get_cell(table_t *t, int row, int col) {
    if (_utable_rc_chk(t, row, col)) return NULL;
    return &t->c[TIDX(row, col)];
}

/**
 * Return the text in the specified table cell
 * @param t Table pointer
 * @param row Row of cell
 * @param col Column of cell
 * @return Pointer to text, NULL if out of range or no text
 */
char *
utable_get_cell(table_t *t, int row, int col) {
    if (_utable_rc_chk(t, row, col)) return NULL;
    return t->c[TIDX(row, col)].t;
}

/**
 * Set the number of columns this cell spans
 * @param t Table pointer
 * @param row Row of cell
 * @param col Column of cell
 * @param cspan Column span to set
 * @return 0 on success, -1 on failure
 */
int
utable_set_cell_colspan(table_t *t, size_t row, size_t col, size_t cspan) {
    if (_utable_rc_chk(t, row, col) || col + cspan - 1 >= t->nCol) return -1;
    tcell_t *cell = _utable_get_cell(t, row, col);
    cell->cspan = cspan;
    cell->merged = FALSE;
    cell->pRow = row;
    cell->pCol = col;

    for (size_t c = col + 1; c < col + cspan; c++) {
        cell = _utable_get_cell(t, row, c);
        cell->merged = TRUE;
        cell->pRow = row;
        cell->pCol = col;
    }
    return 0;
}

/**
 * Set the horizontal text alignment for this cell
 * @param t Table pointer
 * @param row Row of cell
 * @param col Column of cell
 * @param halign Horizontal align to use
 * @return 0 on success, -1 on failure
 */
int
utable_set_cell_halign(table_t *t, int row, int col, halign_t halign) {
    if (_utable_rc_chk(t, row, col)) return -1;
    t->c[TIDX(row, col)].halign = halign;
    return 0;
}

/**
 * Set the horizontal text alignment for this cell
 * @param t Table pointer
 * @param row Row of cell
 * @param col Column of cell
 * @return halign_t on success, -1 on failure
 */
int
utable_get_cell_halign(table_t *t, int row, int col) {
    if (_utable_rc_chk(t, row, col)) return -1;
    return (int) t->c[TIDX(row, col)].halign;
}

/**
 * Set the horizontal text alignment for specified row
 * @param t Table pointer
 * @param row Row to set
 * @param halign Horizontal align to use
 * @return 0 on success, -1 on failure
 */
int
utable_set_row_halign(table_t *t, int row, halign_t halign) {
    for (size_t c = 0; c < t->nCol; c++) {
        if (utable_set_cell_halign(t, row, c, halign)) return -1;
    }
    return 0;
}

/**
 * Set horizontal alignment for the entire table
 * @param t Table pointer
 * @param halign Horizontal align to use
 * @return 0 on success, -1 on failure
 */
int
utable_set_table_halign(table_t *t, halign_t halign) {
    for (size_t r = 0; r < t->nRow; r++) {
        if (-1 == utable_set_row_halign(t, r, halign)) return -1;
    }
    return 0;
}

/**
 * Set the horizontal text alignment for specified column
 * @param t Table pointer
 * @param col Column to set
 * @param halign Horizontal align to use
 * @return 0 on success, -1 on failure
 */
int
utable_set_col_halign(table_t *t, int col, halign_t halign) {
    for (size_t r = 0; r < t->nRow; r++) {
        if (utable_set_cell_halign(t, r, col, halign)) return -1;
    }
    return 0;
}

/**
 * Set the text value for the specified cell. The value stored in the cell will
 * be a newly allocated space for this string.
 * @param t Table pointer
 * @param row
 * @param col
 * @param val
 * @return 0 on success, -1 on failure
 */
int
utable_set_cell(table_t *t, size_t row, size_t col, char *val) {
    if (_utable_rc_chk(t, row, col) || t->c[TIDX(row, col)].merged) return -1;
    if (t->c[TIDX(row, col)].t) {
        free(t->c[TIDX(row, col)].t);
    }
    t->c[TIDX(row, col)].t = strdup(val);
    return 0;
}

/**
 * Set the cell left and right padding
 * @param t Table pointer
 * @param lpad
 * @param rpad
 * @return 0 on success, -1 on failure
 */
void
utable_set_table_cellpadding(table_t *t, size_t lpad, size_t rpad) {
    for (size_t r = 0; r < t->nRow; r++) {
        for (size_t c = 0; c < t->nCol; c++) {
            (void) utable_set_cellpadding(t, r, c, lpad, rpad);
        }
    }
}

/**
 * Set callback function for cell used to populate the cell with text
 * @param t Table pointer
 * @param r row
 * @param c column
 * @param cb Callback function
 */
int
utable_set_cellcallback(table_t *t, int row, int col, t_cell_cb cb) {
    if (_utable_rc_chk(t, row, col) || t->c[TIDX(row, col)].merged) return -1;
    // Only set callback if there is not already any text set
    if (!t->c[TIDX(row, col)].t) t->c[TIDX(row, col)].cb = cb;
    return 0;
}

/**
 * Set callback function for all cells in the table used to populate the cell
 * with text. If the callback returns NULL then no change to the cells
 * current text will be made.
 * @param t Table pointer
 * @param cb Callback function
 */
void
utable_set_table_cellcallback(table_t *t, t_cell_cb cb) {
    for (size_t r = 0; r < t->nRow; r++) {
        for (size_t c = 0; c < t->nCol; c++) {
            utable_set_cellcallback(t, r, c, cb);
        }
    }
}

/**
 * Set the cell left and right padding
 * @param t Table pointer
 * @param row
 * @param col
 * @param lpad
 * @param rpad
 * @return 0 on success, -1 on failure
 */
int
utable_set_cellpadding(table_t *t, size_t row, size_t col, size_t lpad,
                       size_t rpad) {
    if (_utable_rc_chk(t, row, col) || t->c[TIDX(row, col)].merged) return -1;
    t->c[TIDX(row, col)].lpad = lpad;
    t->c[TIDX(row, col)].rpad = rpad;
    return 0;
}

/**
 * Set the cell left and right padding
 * @param t Table pointer
 * @param lpad
 * @param rpad
 * @return 0 on success, -1 on failure
 */
void
utable_set_col_cellpadding(table_t *t, size_t col, size_t lpad,
                           size_t rpad) {
    for (size_t r = 0; r < t->nRow; r++) {
        utable_set_cellpadding(t, r, col, lpad, rpad);
    }
}

static void
_utable_get_cp(table_t *t, size_t row, size_t col, size_t *lpad,
               size_t *rpad) {
    if (t->c[TIDX(row, col)].merged) {
        row = t->c[TIDX(row, col)].pRow;
        col = t->c[TIDX(row, col)].pCol;
    }
    *lpad = t->c[TIDX(row, col)].lpad;
    *rpad = t->c[TIDX(row, col)].rpad;
}

/**
 * Set the width for the specified column
 * @param t Table pointer
 * @param col Column to set
 * @param width Width to set
 * @return 0 on success, -1 on failure
 */
int
utable_set_colwidth(table_t *t, size_t col, size_t width) {
    if (col >= t->nCol) return -1;
    t->colwidth[col] = width;
    return 0;
}

/**
 * Set the width for all columns in the table
 * @param t Table pointer
 * @param width Width to set
 */
void
utable_set_table_colwidth(table_t *t, size_t width) {
    for (size_t c = 0; c < t->nCol; c++) {
        utable_set_colwidth(t, c, width);
    }
}

/**
 * Reset column widths for entire table to auto
 * @param t Table pointer
 */
void
utable_reset_table_colwidth(table_t *t) {
    utable_set_table_colwidth(t, 0);
}

/**
 * Set the min width for the specified column
 * @param t Table pointer
 * @param col Column to set
 * @param width Width to set
 */
int
utable_set_mincolwidth(table_t *t, size_t col, size_t width) {
    if (col >= t->nCol) return -1;
    t->mincolwidth[col] = width;
    return 0;
}

/**
 * Set the minimum width for all columns in the table
 * @param t Table pointer
 * @param width Width to set
 */
void
utable_set_table_mincolwidth(table_t *t, size_t width) {
    for (size_t c = 0; c < t->nCol; c++) {
        utable_set_mincolwidth(t, c, width);
    }
}

static _Bool cut_in_padding = FALSE;

/**
 * Determine policy when table width is set too small to fit everythign that is
 * needed. If flag is set to true then the left padding will be kept as much as
 * possible and the text and the right padding will be cut as needed.
 * If false then the padding will be kept as much as possible (both left and
 * right) and the text cut as needed. Default is false sp the padding will be
 * kept as much as possible
 * @param cutInPadding
 */
void
utable_set_padding_policy(_Bool cutInPadding) {
    cut_in_padding = cutInPadding;
}

/**
 * Internal helper function to draw a single line of table data
 * @param file Filepointer to write to
 * @param t Table pointer
 * @param row Row to draw
 * @param style Table style
 */
#define MAXPADCHARS 256
#define MAXPBUFF (10 * 1024)

static void
_utable_draw_cellcontent_row(char *buff, int *buffleft, table_t *t,
                             size_t row, char *midleft,
                             char *midright, char *midvert) {
    size_t c = 0;
    int buffused = 0;
    char pbuff[MAXPBUFF];

    *pbuff = '\0';
    while (c < t->nCol) {
        int w = 0;

        // tcell_t *cell = &t->c[TIDX(row, c)];

        // Determine the total width of this cell. This needs to take
        // into account the fact that this could be a cell that is spanning
        // multiple other cells.
        const size_t cspan = t->c[TIDX(row, c)].cspan;
        size_t lpad, rpad;
        _utable_get_cp(t, row, c, &lpad, &rpad);
        // const size_t totpad = lpad + rpad;
        char lpadstr[MAXPADCHARS], rpadstr[MAXPADCHARS];

        for (size_t i = 0; i < lpad; ++i) {
            lpadstr[i] = ' ';
        }
        for (size_t i = 0; i < rpad; ++i) {
            rpadstr[i] = ' ';
        }
        lpadstr[lpad] = '\0';
        rpadstr[rpad] = '\0';
        for (size_t cs = 0; cs < cspan; cs++) {
            w += t->colwidth[c + cs] + 1;
        }
        w -= 1;  // Don't include the last border since that remains

        if (NULL != t->c[TIDX(row, c)].cb) {
            char *cb_str =
                    t->c[TIDX(row, c)].cb(row - (t->title ? 1 : 0), c, t->tag);
            if (NULL != cb_str) {
                if (t->c[TIDX(row, c)].t) {
                    free(t->c[TIDX(row, c)].t);
                }
                t->c[TIDX(row, c)].t = strdup(cb_str);
            }
        }

        const char *txt = t->c[TIDX(row, c)].t;
        char txtbuff[512], txtbuff2[1024];

        // A choice in how to handle the case when the column width is smaller
        // than what is needed. We can either include the padding chars in the
        // limiting or try to maintain the padding and only limiting the actual
        // content
        if (cut_in_padding) {
            // Cut the text so that even the right padding is cut
            snprintf(txtbuff2, sizeof(txtbuff2), "%s%s%s", lpadstr, txt,
                     rpadstr);
            size_t len = xmb_offset(txtbuff2, w);
            xstrlcpy(txtbuff, txtbuff2,
                     len + 1);  // +1 since the length also must include the
            // terminating 0
        } else {
            // Keep the padding chars as much as possible
            snprintf(txtbuff2, sizeof(txtbuff2), "%s%s", lpadstr, txt);
            size_t len = xmb_offset(txtbuff2, w - rpad);
            xstrlcpy(txtbuff, txtbuff2,
                     len + 1);  // +1 since the length also must include the
            // terminating 0
            snprintf(txtbuff2, sizeof(txtbuff2), "%s%s", txtbuff, rpadstr);
            len = xmb_offset(txtbuff2, w) + 1;  // Offset starts at 0
            xstrlcpy(txtbuff, txtbuff2,
                     len + 1);  // +1 since the length also must include the
            // terminating 0
        }

        // We reuse the rpadstr buffer to use for padding the cell to the
        // assigned width in order to make the text left, right or center
        // aligned We have to do this ourself since it is not safe to use "%*s"
        // format since this is not UTF safe since internally snprintf() uses
        // strlen())
        memset(rpadstr, 0, sizeof(rpadstr));
        if (w > (int) utf8len(txtbuff))
            for (size_t i = 0; i < w - utf8len(txtbuff); ++i) {
                rpadstr[i] = ' ';
            }

        // We need these values to center the text
        const int w_half = w / 2;
        const int len_half = (utf8len(txtbuff) /*+ totpad*/) / 2;
        int padlen;

        if (c == 0) {
            switch (t->c[TIDX(row, c)].halign) {
                case RIGHTALIGN:
                    buffused = snprintf(pbuff, MAXPBUFF, "%s%s%s", midleft,
                                        rpadstr, txtbuff);
                    xstrlcat(buff, pbuff, *buffleft);
                    *buffleft -= buffused;
                    break;
                case LEFTALIGN:
                    buffused = snprintf(pbuff, MAXPBUFF, "%s%s%s", midleft,
                                        txtbuff, rpadstr);
                    xstrlcat(buff, pbuff, *buffleft);
                    *buffleft -= buffused;
                    break;
                case CENTERALIGN:
                    memset(rpadstr, 0, sizeof(rpadstr));
                    padlen = (w - w_half + len_half) - utf8len(txtbuff);
                    for (int i = 0; i < padlen; ++i) {
                        rpadstr[i] = ' ';
                    }
                    buffused =
                            snprintf(pbuff, MAXPBUFF, "%s%*s%s%s", midleft,
                                     w_half - len_half, "", txtbuff, rpadstr);
                    xstrlcat(buff, pbuff, *buffleft);
                    *buffleft -= buffused;
                    break;
            }
        } else {
            switch (t->c[TIDX(row, c)].halign) {
                case RIGHTALIGN:
                    buffused = snprintf(pbuff, MAXPBUFF, "%s%s%s", midvert,
                                        rpadstr, txtbuff);
                    xstrlcat(buff, pbuff, *buffleft);
                    *buffleft -= buffused;
                    break;
                case LEFTALIGN:
                    buffused = snprintf(pbuff, MAXPBUFF, "%s%s%s", midvert,
                                        txtbuff, rpadstr);
                    xstrlcat(buff, pbuff, *buffleft);
                    *buffleft -= buffused;
                    break;
                case CENTERALIGN:
                    memset(rpadstr, 0, sizeof(rpadstr));
                    padlen = (w - w_half + len_half) - utf8len(txtbuff);
                    for (int i = 0; i < padlen; ++i) {
                        rpadstr[i] = ' ';
                    }
                    buffused =
                            snprintf(pbuff, MAXPBUFF, "%s%*s%s%s", midvert,
                                     w_half - len_half, "", txtbuff, rpadstr);
                    xstrlcat(buff, pbuff, *buffleft);
                    *buffleft -= buffused;
                    break;
            }
        }

        c += cspan;
    }
    buffused = snprintf(pbuff, MAXPBUFF, "%s\n", midright);
    xstrlcat(buff, pbuff, *buffleft);
    *buffleft -= buffused;
}

/**
 * Set automatic column width for columns with no user specified width
 * @param t Table pointer
 */
static void
_utable_set_autocolwidth(table_t *t) {
    for (size_t c = 0; c < t->nCol; c++) {
        if (t->colwidth[c] == 0) {
            // User has not yet set column width so find the widest text
            // If the table has a header row we must ignore that since it
            // a) only has one column allocated, and b) will be as wide as the
            // table
            size_t startrow = 0;
            if (t->title && t->titleCopied) startrow = 1;
            for (size_t r = startrow; r < t->nRow; r++) {
                size_t lpad, rpad;
                _utable_get_cp(t, r, c, &lpad, &rpad);
                if (utf8len(t->c[TIDX(r, c)].t) + lpad + rpad >
                    t->colwidth[c]) {
                    t->colwidth[c] = utf8len(t->c[TIDX(r, c)].t) + lpad + rpad;
                }
            }
            t->colwidth[c] = MAX(t->colwidth[c], t->mincolwidth[c]);
            // Make sure the column width is at least 1 character wide
            if (0 == t->colwidth[c]) t->colwidth[c] = 1;
        }
    }
}

/**
 * Internal helper function. This functions builds a logic view of the border
 * between two rows. Since the vertical borders up and down are not necessarily
 * always on the same position since cell can have been merged (spans over
 * several columns) this function sets a the logical map "eval" if it
 * discovers the need for vertical border. The same logical map are used
 * with both the "over" and "under" row of data. The value to logically or
 * is specified in the "mark" argument. To find the top verticals
 * we normally call this function with mark==1 and for the bottom verticals
 * we call this with mark==2. If the logical map is initialized with 0 this will
 * can the be interpretated as 0=only a horizontal, 1=horizontal with an "up"
 * vertical, 2=horizontal with an "down" vertical and 2=horizontal with both up
 * and down vertical
 * @param t Table pointer
 * @param eval
 * @param mark
 * @param row
 */
static void
_utable_mark_verticals(table_t *t, int eval[], int mark,
                       size_t row) {
    size_t c = 0;
    size_t absw = 0;
    while (c < t->nCol) {
        int w = 0;
        for (size_t cs = 0; cs < t->c[TIDX(row, c)].cspan; cs++) {
            w += t->colwidth[c + cs] + 1;
        }

        absw += w;
        eval[absw - 1] |= mark;
        c += t->c[TIDX(row, c)].cspan;
    }
}

/**
 * Internal helper functions to write out the border characters identified by
 * the _mark_verticals
 * @param file
 * @param totwidth
 * @param eval
 * @param s0
 * @param s1
 * @param s2
 * @param s3
 */
static void
_utable_stroke_verticals(char *buff, int *buffleft, int totwidth,
                         int eval[], char *s0, char *s1, char *s2,
                         char *s3) {
    int buffused = 0;
    char pbuff[MAXPBUFF];
    for (int i = 0; i < totwidth - 1; i++) {
        if (eval[i] == 0) {  // HORIZONTAL
            if (s0 == NULL)
                buffused = snprintf(pbuff, MAXPBUFF, "#ERR#");
            else
                buffused = snprintf(pbuff, MAXPBUFF, "%s", s0);

            xstrlcat(buff, pbuff, *buffleft);
            *buffleft -= buffused;
        } else if (eval[i] == 1) {  // TOP DOWN
            if (s1 == NULL)
                buffused = snprintf(pbuff, MAXPBUFF, "#ERR#");
            else
                buffused = snprintf(pbuff, MAXPBUFF, "%s", s1);

            xstrlcat(buff, pbuff, *buffleft);
            *buffleft -= buffused;

        } else if (eval[i] == 2) {  // BOTTOM UP
            if (s2 == NULL)
                buffused = snprintf(pbuff, MAXPBUFF, "#ERR#");
            else
                buffused = snprintf(pbuff, MAXPBUFF, "%s", s2);

            xstrlcat(buff, pbuff, *buffleft);
            *buffleft -= buffused;
        } else {  // CROSS
            if (s3 == NULL)
                buffused = snprintf(pbuff, MAXPBUFF, "#ERR#");
            else
                buffused = snprintf(pbuff, MAXPBUFF, "%s", s3);

            xstrlcat(buff, pbuff, *buffleft);
            *buffleft -= buffused;
        }
    }
    if (*buffleft < 1) {
        logmsg("Out of buffer space in _utable_stroke_verticals()");
        exit(EXIT_FAILURE);
    }
}

/**
 * Set display of interior horizontal and vertical border
 * @param t Table pointer
 * @param v Flag to enable/disable vertical interior borders
 * @param h Flag to enable/disable horizontal interior borders
 */
void
utable_set_interior(table_t *t, _Bool v, _Bool h) {
    t->interior_v = v;
    t->interior_h = h;
}

/**
 * Stroke the entire table to STDOUT
 * @param t     Table pointer
 * @param style Table layout style to use
 *              Possible styles are: TSTYLE_TOP_DL, TSTYLE_FULL_DL, TSTYLE_SL,
 * TSTYLE_TOPBOTTOM_DL
 * @return -1 on failure, 0 on success
 */
int
utable_stroke_stdout(table_t *t, tblstyle_t style) {
    return utable_stroke(t, STDOUT_FILENO, style);
}

/**
 * Stroke the entire table in the specified style to specified file descriptor
 * @param t     Table pointer
 * @param fd    File descriptor to write to
 * @param style Table layout style to use
 * @return -1 on failure, 0 on success
 */
int
utable_stroke(table_t *t, int fd, tblstyle_t style) {
    char *buff = calloc(MAXSTROKEBUFF, sizeof(char));
    if (NULL == buff || -1 == utable_strstroke(t, buff, MAXSTROKEBUFF, style)) {
        free(buff);
        return -1;
    }

    int ret = write(fd, buff, strlen(buff));

    free(buff);
    return ret;
}

/**
 * Internal helper fuction to set up the title
 * @param t	Table handle
 */
static void
_utable_strstroke_title(table_t *t) {
    if (t->title) {
        // We need to check if this has already been done since it is perfectly
        // legal to stroke the same table objct multiple times
        if (!t->titleCopied) {
            // Insert the title as the top row by moving existing row down one
            // step
            memmove(&t->c[t->nCol], &t->c[0], (t->nRow * t->nCol) * sizeof(tcell_t));
            memset(&t->c[0], 0, t->nCol * sizeof(tcell_t));
            t->c[0].t = strdup(t->title);
            t->c[0].halign = CENTERALIGN;
            utable_set_cell_colspan(t, 0, 0, t->nCol);
            t->nRow++;
            t->titleCopied = TRUE;
        } else {
            free(t->c[0].t);
            t->c[0].t = strdup(t->title);
        }
    }
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstack-protector"

/**
 * Stroke the entire table in the specified style to specified string buffer
 * @param t     Table pointer
 * @param buff  String buffer to write to
 * @param bufflen Length of string byffer in bytes
 * @param style Table layout style to use
 * @return -1 on failure, 0 on success
 */
int
utable_strstroke(table_t *t, char *buff, size_t bufflen, tblstyle_t style) {

    _utable_set_autocolwidth(t);
    _utable_strstroke_title(t);

    // Get the total width of the table in characters
    size_t totwidth = 0;
    for (size_t i = 0; i < t->nCol; i++) totwidth += t->colwidth[i] + 1;

    if (0 == totwidth) return -1;

    /* The eval is used to evaluate markers on the table */
    int eval[totwidth];

    /* Get characters to use for this style into style data (sd)*/
    style_t sd;
    get_style(style, &sd, t->interior_v);

    char pbuff[MAXPBUFF];
    *buff = '\0';
    memset(eval, 0, sizeof(int) * totwidth);
    int buffleft = bufflen;
    int len = snprintf(pbuff, MAXPBUFF, "%s", sd.top_left);
    xstrlcat(buff, pbuff, buffleft);
    buffleft -= len;
    if (buffleft < 1) return -1;

    if (t->title)
        _utable_stroke_verticals(buff, &buffleft, totwidth, eval,
                                 sd.top_horizontal, NULL, NULL, NULL);
    else {
        _utable_mark_verticals(t, eval, 1, 0);
        _utable_stroke_verticals(buff, &buffleft, totwidth, eval,
                                 sd.top_horizontal, sd.top_down, NULL, NULL);
    }
    if (buffleft < 1) return -1;

    len = snprintf(pbuff, MAXPBUFF, "%s\n", sd.top_right);
    xstrlcat(buff, pbuff, buffleft);
    buffleft -= len;
    if (buffleft < 1) return -1;

    for (size_t r = 0; r < t->nRow; r++) {
        _utable_draw_cellcontent_row(buff, &buffleft, t, r, sd.border_vertical,
                                     sd.border_vertical, sd.middle_vertical);
        if (buffleft < 1) return -1;

        memset(eval, 0, sizeof(int) * totwidth);
        _utable_mark_verticals(t, eval, 2, r);
        if (r < t->nRow - 1)
            _utable_mark_verticals(t, eval, 1, r + 1);  // Top verticals

        if (t->headerLine && ((r == 0 && !t->title) || (r == 1 && t->title))) {
            // The heavier line just beneath the header row before the data rows
            len = snprintf(pbuff, MAXPBUFF, "%s", sd.top_middle_left);
            xstrlcat(buff, pbuff, buffleft);
            buffleft -= len;
            if (buffleft < 1) return -1;

            _utable_stroke_verticals(buff, &buffleft, totwidth, eval,
                                     sd.top_middle_horizontal, NULL, NULL,
                                     sd.top_middle_cross);
            if (buffleft < 1) return -1;

            len = snprintf(pbuff, MAXPBUFF, "%s\n", sd.top_middle_right);
            xstrlcat(buff, pbuff, buffleft);
            buffleft -= len;

            buffleft -= len;
            if (buffleft < 1) return -1;

        } else if (r == 0 && t->title) {
            if (t->titleStyle == TITLESTYLE_LINE) {
                // The optional thin line beneath the title
                len = snprintf(pbuff, MAXPBUFF, "%s", sd.middle_left);
                xstrlcat(buff, pbuff, buffleft);
                buffleft -= len;
                if (buffleft < 1) return -1;

                _utable_stroke_verticals(
                        buff, &buffleft, totwidth, eval, sd.middle_horizontal,
                        sd.middle_horizontal_down, sd.middle_horizontal_up,
                        sd.middle_cross);
                if (buffleft < 1) return -1;

                len = snprintf(pbuff, MAXPBUFF, "%s\n", sd.middle_right);
                xstrlcat(buff, pbuff, buffleft);
                buffleft -= len;
                if (buffleft < 1) return -1;
            }
        } else if (t->interior_h) {
            // Add lines between each data row
            if (r < t->nRow - 1) {
                len = snprintf(pbuff, MAXPBUFF, "%s", sd.middle_left);
                xstrlcat(buff, pbuff, buffleft);
                buffleft -= len;
                if (buffleft < 1) return -1;

                _utable_stroke_verticals(
                        buff, &buffleft, totwidth, eval, sd.middle_horizontal,
                        sd.middle_horizontal_down, sd.middle_horizontal_up,
                        sd.middle_cross);
                if (buffleft < 1) return -1;

                len = snprintf(pbuff, MAXPBUFF, "%s\n", sd.middle_right);
                xstrlcat(buff, pbuff, buffleft);
                buffleft -= len;
                if (buffleft < 1) return -1;
            }
        }
    }

    if (sd.have_bottom_border) {
        len = snprintf(pbuff, MAXPBUFF, "%s", sd.bottom_left);
        xstrlcat(buff, pbuff, buffleft);
        buffleft -= len;
        if (buffleft < 1) return -1;

        _utable_stroke_verticals(buff, &buffleft, totwidth, eval,
                                 sd.bottom_horizontal, sd.bottom_up,
                                 sd.bottom_up, NULL);
        len = snprintf(pbuff, MAXPBUFF, "%s\n", sd.bottom_right);
        xstrlcat(buff, pbuff, buffleft);
        buffleft -= (len + 1);
        if (buffleft < 1) return -1;
    }

    return 0;
}

#pragma GCC diagnostic pop

// [EOF]]
