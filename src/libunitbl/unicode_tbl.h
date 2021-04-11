/* =========================================================================
 * File:        uniode_tbl.c
 * Description: Functions to create and write a rudimentary table
 *              using unicode page U+25xx characters for output nicely
 *              formatted output to a terminal
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

#ifndef UNICODE_TBL_H
#define	UNICODE_TBL_H

#ifdef	__cplusplus
extern "C" {
#endif

#define FALSE 0
#define TRUE 1
 
  
/**
 * Possible horizontal alignment for cell conetnt
 */
typedef enum {
    LEFTALIGN,   /**< Left alignment in cell  */                
    RIGHTALIGN,  /**< Right alignment in cell */             
    CENTERALIGN  /**< Center alignment in cell */
} halign_t;

/**
 * Possible table styles
 */
typedef enum {
    /* Almost no border */
    TSTYLE_SIMPLE_V1,       /**< A simple single horizontal line under the header */
    TSTYLE_SIMPLE_V2,       /**< A simple double horizontal line under the header */
    TSTYLE_SIMPLE_V3,       /**< A simple bold horizontal line under the header */      
    TSTYLE_SIMPLE_V4,       /**< A simple single horizontal line under the header, and single line at table bottom */
    TSTYLE_SIMPLE_V5,       /**< A simple double horizontal line under the header, and single line at and table bottom */
    TSTYLE_SIMPLE_V6,       /**< A simple bold horizontal line under the header, and single line at and table bottom */      
            

    /* ASCII Styles */
    TSTYLE_ASCII_V0,    /**< Plain ASCII only one horizontal double line under header row */            
    TSTYLE_ASCII_V4,   /**< Plain ASCII one horizontal double line under header row, single bottom line */              
    TSTYLE_ASCII_V1,    /**< Plain ASCII only horizontal lines, double lines top and bottom */
    TSTYLE_ASCII_V2,    /**< Plain ASCII with border, double lines top and bottom */
    TSTYLE_ASCII_V3,    /**< Plain ASCII with border, only single lines */ 

    /* Unicode box style using double lines */
    TSTYLE_DOUBLE_V1,   /**< Double horizontal box, no vertical border */          
    TSTYLE_DOUBLE_V2,   /**< Double box */            
    TSTYLE_DOUBLE_V3,   /**< Double horizontal, single vertical box */
    TSTYLE_DOUBLE_V4,   /**< Double horizontal, no special header line */

    /* Unicode box style using single lines*/
    TSTYLE_SINGLE_V1,   /**< Single horizontal */
    TSTYLE_SINGLE_V2,   /**< Single box */

    /* Unicode box style using heavy lines **/
    TSTYLE_HEAVY_V1,    /**< Heavy horizontal, no vertical border */
    TSTYLE_HEAVY_V2,    /**< Full heavy border */
    TSTYLE_HEAVY_V3     /**< Heavy border,no header */
} tblstyle_t;

extern tblstyle_t table_styles[];

#define NBR_TSTYLES 20

/**
 * Buffer allocated when we stroke a table. This effectively limits the size of the
 * table we can stroke. We set this to 10 MB
 */
#define MAXSTROKEBUFF (1024*1024*10)

/**
 *
 */
typedef struct {
  char *top_horizontal;
  char *top_left, *top_right;
  char *top_down;
  char *top_middle_left, *top_middle_cross;
  char *top_middle_horizontal;
  char *top_middle_right;    
  char *middle_left;    
  char *middle_horizontal;
  char *middle_right;
  char *middle_horizontal_up;
  char *middle_horizontal_down;
  char *middle_cross;
  char *middle_vertical;
  char *border_vertical;
  char *bottom_horizontal;
  char *bottom_left, *bottom_right, *bottom_up;
  int have_bottom_border;
} style_t;
  
/**
 * Table title line style  
 */
typedef enum {
    TITLESTYLE_LINE, TITLESTYLE_NOLINE
} titlestyle_t;

/**
 * Type for cell callback function to set the text in a cell
 * The callback is populated with the row, column and table 'tag'
 * to be able to differentiate from which table the callback came
 * from. The string to be used should be used in the cell is returned
 * as a pointer. The cell will make its own copy of the string as needed
 */
typedef char* (*t_cell_cb)(int,int,void*);

/**
 * Data structure that represents one cell in the table
 */
typedef struct {
    t_cell_cb cb;       //!< Cell callback as an alternative way to set the text
    char *t;            //!< A pointer to the text in te cell
    halign_t halign;    //!<  What horizontal alignment to use for text
    int pRow, pCol;     //!<  Parent row and column
    _Bool merged;       //!<  Is this cell part of a merged cell
    size_t rspan;       //!<  How many rows this cell spans
    size_t cspan;       //!<  How many columns thi cell spans (not used)
    size_t lpad, rpad;  //!<  Left and right padding
} tcell_t;

/**
 * Data structure that represents the table
 */
typedef struct {
    void *tag;          //!< Arbitrary pointer to a "tag" used to identify the table in callbacks
    size_t nRow, nCol;  //!< Number of rows and columns in the table
    tcell_t *c;         //!< Pointer to the data matrix
    size_t *colwidth;   //!< A vector with comuted or forced column widths
    size_t *mincolwidth;    //!< The set minimum column width
    char *title;        //!< Title of the table
    _Bool titleCopied;  //!< State variables to indicate if the title has been allocated
    titlestyle_t titleStyle;    //!< Style of line or not under the title
    _Bool interior_v, interior_h;   //!< Should the interior lines in the table be shown
    _Bool headerLine;   //!<  Should the header line be added
} table_t;

typedef void (*t_log_func)(int,char*);

void
utable_set_logfunc(t_log_func f, int loglevel, char *prefix);

int
utable_set_cell_colspan(table_t *t, size_t row, size_t col, size_t cspan);

int
utable_set_cell_halign(table_t *t, int row, int col, halign_t halign);

int
utable_get_cell_halign(table_t *t, int row, int col);

int
utable_set_cellpadding(table_t *t, size_t row, size_t col, size_t lpad, size_t rpad);

void
utable_set_col_cellpadding(table_t *t, size_t col, size_t lpad, size_t rpad);

void
utable_set_table_cellpadding(table_t *t, size_t lpad, size_t rpad);

int
utable_set_table_halign(table_t *t, halign_t halign);

int
utable_set_cell(table_t *t, size_t row, size_t col, char *txt);

int
utable_set_row_halign(table_t *t, int row, halign_t halign);

int
utable_set_col_halign(table_t *t, int col, halign_t halign);

table_t *
utable_create(size_t nRow, size_t nCol);

table_t *
utable_create_set(size_t nRow, size_t nCol, char *data[]);

int
utable_set_mincolwidth(table_t *t, size_t col, size_t width);

int
utable_set_colwidth(table_t *t, size_t col, size_t width);

void
utable_reset_table_colwidth(table_t *t);

void
utable_set_table_mincolwidth(table_t *t, size_t width);

void
utable_set_table_colwidth(table_t *t, size_t width);

int
utable_set(table_t *t, char *data[]);

void
utable_free(table_t *t);

void
utable_set_interior(table_t *t, _Bool v, _Bool h);

int
utable_stroke_stdout(table_t *t, tblstyle_t style);

int
utable_stroke(table_t *t, int fd, tblstyle_t style);

int
utable_strstroke(table_t *t, char *dets, size_t maxlen, tblstyle_t style);

void
utable_set_title(table_t *t, char *title, titlestyle_t style);

void
utable_set_headerline(table_t *t, _Bool headerLine);

void
utable_set_padding_policy(_Bool cutInPadding);

int
utable_set_cellcallback(table_t *t, int row, int col, t_cell_cb cb);

void
utable_set_table_cellcallback(table_t *t, t_cell_cb cb);

int
utable_set_coltitles(table_t *t, char *titles[]);

#ifdef	__cplusplus
}
#endif

#endif	/* UNICODE_TBL_H */

