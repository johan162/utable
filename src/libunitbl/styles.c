/* =========================================================================
 * File:        styles.c
 * Description: Routines to set and configure the various style of tables
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
#include <stdlib.h>

#include "styles.h"
#include "unicode_tbl.h"

static void 
style_body_heavy(tblstyle_t style, style_t *sd);

static void 
style_body_single(tblstyle_t style, style_t *sd);

static void 
style_body_double(tblstyle_t style, style_t *sd);

static void 
style_body_simple(tblstyle_t style, style_t *sd);

static void 
style_body_ascii(tblstyle_t style, style_t *sd);

static void
init_style(style_t *sd);
tblstyle_t table_styles[] = {
    TSTYLE_SIMPLE_V1, TSTYLE_SIMPLE_V2, TSTYLE_SIMPLE_V3, TSTYLE_SIMPLE_V4,
    TSTYLE_SIMPLE_V5, TSTYLE_SIMPLE_V6, TSTYLE_ASCII_V0,  TSTYLE_ASCII_V4,
    TSTYLE_ASCII_V1,  TSTYLE_ASCII_V2,  TSTYLE_ASCII_V3,  TSTYLE_DOUBLE_V1,
    TSTYLE_DOUBLE_V2, TSTYLE_DOUBLE_V3, TSTYLE_DOUBLE_V4, TSTYLE_SINGLE_V1,
    TSTYLE_SINGLE_V2, TSTYLE_HEAVY_V1,  TSTYLE_HEAVY_V2,  TSTYLE_HEAVY_V3};

style_func_t sjt[] = {
    [TSTYLE_SIMPLE_V1] = style_body_simple,      
    [TSTYLE_SIMPLE_V2] = style_body_simple,     
    [TSTYLE_SIMPLE_V3] = style_body_simple,     
    [TSTYLE_SIMPLE_V4] = style_body_simple,     
    [TSTYLE_SIMPLE_V5] = style_body_simple,     
    [TSTYLE_SIMPLE_V6] = style_body_simple,     
            
    /* ASCII Styles */
    [TSTYLE_ASCII_V0] = style_body_ascii,  
    [TSTYLE_ASCII_V4] = style_body_ascii,  
    [TSTYLE_ASCII_V1] = style_body_ascii,  
    [TSTYLE_ASCII_V2] = style_body_ascii,  
    [TSTYLE_ASCII_V3] = style_body_ascii,  

    /* Unicode box style using double lines */
    [TSTYLE_DOUBLE_V1] = style_body_double,  
    [TSTYLE_DOUBLE_V2] = style_body_double,  
    [TSTYLE_DOUBLE_V3] = style_body_double,  
    [TSTYLE_DOUBLE_V4] = style_body_double,  

    /* Unicode box style using single lines*/
    [TSTYLE_SINGLE_V1] = style_body_single,  
    [TSTYLE_SINGLE_V2] = style_body_single,  

    /* Unicode box style using heavy lines **/
    [TSTYLE_HEAVY_V1] = style_body_heavy,  
    [TSTYLE_HEAVY_V2] = style_body_heavy,  
    [TSTYLE_HEAVY_V3] = style_body_heavy     
};
 

static void 
style_body_ascii(tblstyle_t style, style_t *sd) {

  if (style == TSTYLE_ASCII_V0 || style == TSTYLE_ASCII_V4) {
        sd->top_middle_horizontal = "=";
        sd->top_middle_cross = "+";
        sd->middle_vertical = "|";
        sd->bottom_horizontal = "-";
        sd->bottom_up = "+";
        sd->bottom_left = sd->bottom_horizontal;
        sd->bottom_right = sd->bottom_horizontal;
        sd->top_middle_left = sd->top_middle_horizontal;
        sd->top_middle_right = sd->top_middle_horizontal;

        if (style == TSTYLE_ASCII_V0) sd->have_bottom_border = FALSE;

    } else if (style == TSTYLE_ASCII_V2 || style == TSTYLE_ASCII_V1 ||
               style == TSTYLE_ASCII_V3) {
        // The very top outer border line
        sd->top_horizontal = style == TSTYLE_ASCII_V3 ? "-" : "=";
        sd->top_down = "+";

        // The very bottom outer border line
        sd->bottom_horizontal = sd->top_horizontal;
        sd->bottom_up = "+";

        // The four corners
        sd->top_left = style == TSTYLE_ASCII_V2 ? "+" : sd->top_horizontal;
        sd->top_right = style == TSTYLE_ASCII_V2 ? "+" : sd->top_horizontal;
        sd->bottom_left =
            style == TSTYLE_ASCII_V2 ? "+" : sd->bottom_horizontal;
        sd->bottom_right =
            style == TSTYLE_ASCII_V2 ? "+" : sd->bottom_horizontal;

        // The line below the header row, just before the data rows
        sd->top_middle_left =
            style == TSTYLE_ASCII_V2 ? "+" : sd->top_horizontal;
        sd->top_middle_right =
            style == TSTYLE_ASCII_V2 ? "+" : sd->top_horizontal;
        sd->top_middle_cross = "+";
        sd->top_middle_horizontal = sd->top_horizontal;

        // The interior horizontal line (also used for line below the title)
        sd->middle_horizontal = "-";

        // Interior vertical
        sd->middle_vertical = "|";

        // The vertical outer border
        sd->border_vertical = style == TSTYLE_ASCII_V2 ? "|" : " ";

        // The connection to the lefty and right border for interior lines
        sd->middle_left =
            style == TSTYLE_ASCII_V2 ? "+" : sd->middle_horizontal;
        sd->middle_right =
            style == TSTYLE_ASCII_V2 ? "+" : sd->middle_horizontal;

        // Connection for interior lines
        sd->middle_horizontal_up = "+";
        sd->middle_horizontal_down = "+";
        sd->middle_cross = "+";

    } 
}


static void 
style_body_simple(tblstyle_t style, style_t *sd) {
  if (style == TSTYLE_SIMPLE_V1 || style == TSTYLE_SIMPLE_V2 ||
               style == TSTYLE_SIMPLE_V3 || style == TSTYLE_SIMPLE_V4 ||
               style == TSTYLE_SIMPLE_V5 || style == TSTYLE_SIMPLE_V6) {
        if (style == TSTYLE_SIMPLE_V1 || style == TSTYLE_SIMPLE_V2 ||
            style == TSTYLE_SIMPLE_V3)
            sd->have_bottom_border = FALSE;

        sd->top_left = " ";
        sd->top_horizontal = " ";
        sd->top_down = " ";
        sd->top_right = " ";
        sd->top_middle_left = " ";
        sd->top_middle_right = " ";
        sd->top_middle_cross =
            style == TSTYLE_SIMPLE_V2 || style == TSTYLE_SIMPLE_V5   ? BDL_HB_X
            : style == TSTYLE_SIMPLE_V1 || style == TSTYLE_SIMPLE_V4 ? BDL_X
                                                                     : BDL_DH_X;
        ;
        sd->top_middle_horizontal =
            style == TSTYLE_SIMPLE_V2 || style == TSTYLE_SIMPLE_V5   ? BDL_HB_H
            : style == TSTYLE_SIMPLE_V1 || style == TSTYLE_SIMPLE_V4 ? BDL_H
                                                                     : BDL_DL_H;
        sd->middle_left = " ";
        sd->middle_horizontal = " ";
        sd->middle_right = " ";
        sd->middle_horizontal_up = " ";
        sd->middle_horizontal_down = " ";
        sd->middle_cross = " ";
        sd->middle_vertical = BDL_V;
        sd->border_vertical = " ";
        sd->bottom_left = " ";
        sd->bottom_horizontal = style == TSTYLE_SIMPLE_V4 ||
                                        style == TSTYLE_SIMPLE_V5 ||
                                        style == TSTYLE_SIMPLE_V6
                                    ? BDL_H
                                    : " ";
        sd->bottom_right = " ";
        sd->bottom_up = style == TSTYLE_SIMPLE_V1 ||
                                style == TSTYLE_SIMPLE_V2 ||
                                style == TSTYLE_SIMPLE_V3
                            ? BDL_V
                            : BDL_UH;
    }

}

static void 
style_body_double(tblstyle_t style, style_t *sd) {
  if (style == TSTYLE_DOUBLE_V1 || style == TSTYLE_DOUBLE_V2 ||
               style == TSTYLE_DOUBLE_V3 || style == TSTYLE_DOUBLE_V4) {
        // The very top outer border line
        sd->top_horizontal = BDL_DH_H;
        sd->top_down = BDL_DH_DH;

        // The very bottom outer border line
        sd->bottom_horizontal = BDL_DH_H;
        sd->bottom_up = BDL_DH_UH;

        // The four corners
        sd->top_left = (style == TSTYLE_DOUBLE_V2 || style == TSTYLE_DOUBLE_V4)
                           ? BDL_DL_DR
                       : style == TSTYLE_DOUBLE_V3 ? BDL_DH_DR
                                                   : sd->top_horizontal;
        sd->top_right = (style == TSTYLE_DOUBLE_V2 || style == TSTYLE_DOUBLE_V4)
                            ? BDL_DL_DL
                        : style == TSTYLE_DOUBLE_V3 ? BDL_DH_DL
                                                    : sd->top_horizontal;
        sd->bottom_left =
            (style == TSTYLE_DOUBLE_V2 || style == TSTYLE_DOUBLE_V4) ? BDL_DL_UR
            : style == TSTYLE_DOUBLE_V3                              ? BDL_DH_UR
                                        : sd->bottom_horizontal;
        sd->bottom_right =
            (style == TSTYLE_DOUBLE_V2 || style == TSTYLE_DOUBLE_V4) ? BDL_DL_UL
            : style == TSTYLE_DOUBLE_V3                              ? BDL_DH_UL
                                        : sd->bottom_horizontal;

        // The line below the header row, just before the data rows
        // This is set to be the same as the border style
        sd->top_middle_left = style == TSTYLE_DOUBLE_V2   ? BDL_DL_VR
                              : style == TSTYLE_DOUBLE_V3 ? BDL_DH_VR
                                                          : sd->top_horizontal;
        sd->top_middle_right = style == TSTYLE_DOUBLE_V2   ? BDL_DL_VL
                               : style == TSTYLE_DOUBLE_V3 ? BDL_DH_VL
                                                           : sd->top_horizontal;
        sd->top_middle_cross = BDL_DH_X;
        sd->top_middle_horizontal = sd->top_horizontal;

        // The vertical outer border
        sd->border_vertical =
            (style == TSTYLE_DOUBLE_V2 || style == TSTYLE_DOUBLE_V4) ? BDL_DL_V
            : style == TSTYLE_DOUBLE_V3                              ? BDL_V
                                                                     : " ";

        // The interior horizontal line (also used for line below the title)
        sd->middle_horizontal = BDL_H;

        // Interior vertical
        sd->middle_vertical = BDL_V;

        // The connection to the lefty and right border for interior lines
        sd->middle_left =
            (style == TSTYLE_DOUBLE_V2 || style == TSTYLE_DOUBLE_V4) ? BDL_DV_VR
            : style == TSTYLE_DOUBLE_V3                              ? BDL_VR
                                        : sd->middle_horizontal;
        sd->middle_right =
            (style == TSTYLE_DOUBLE_V2 || style == TSTYLE_DOUBLE_V4) ? BDL_DV_VL
            : style == TSTYLE_DOUBLE_V3                              ? BDL_VL
                                        : sd->middle_horizontal;

        // Connection for interior lines
        sd->middle_horizontal_up = BDL_UH;
        sd->middle_horizontal_down = BDL_DH;
        sd->middle_cross = BDL_X;

        if (style == TSTYLE_DOUBLE_V4) {
            // Just a thick border around no special treatment of top row
            sd->top_middle_left = sd->middle_left;
            sd->top_middle_right = sd->middle_right;
            sd->top_middle_cross = sd->middle_cross;
            sd->top_middle_horizontal = sd->middle_horizontal;
        }

    }
}

static void 
style_body_single(tblstyle_t style, style_t *sd) {
  if (style == TSTYLE_SINGLE_V1 || style == TSTYLE_SINGLE_V2) {
        // The very top outer border line
        sd->top_horizontal = BDL_H;
        sd->top_down = BDL_DH;

        // The very bottom outer border line
        sd->bottom_horizontal = sd->top_horizontal;
        sd->bottom_up = BDL_UH;

        // The four corners
        sd->top_left = style == TSTYLE_SINGLE_V2 ? BDL_DR : sd->top_horizontal;
        sd->top_right = style == TSTYLE_SINGLE_V2 ? BDL_DL : sd->top_horizontal;
        sd->bottom_left =
            style == TSTYLE_SINGLE_V2 ? BDL_UR : sd->top_horizontal;
        sd->bottom_right =
            style == TSTYLE_SINGLE_V2 ? BDL_UL : sd->top_horizontal;

        // The line below the header row, just before the data rows
        // This is set to be the same as the border style
        sd->top_middle_left =
            style == TSTYLE_SINGLE_V2 ? BDL_VR : sd->top_horizontal;
        sd->top_middle_right =
            style == TSTYLE_SINGLE_V2 ? BDL_VL : sd->top_horizontal;
        sd->top_middle_cross = BDL_X;
        sd->top_middle_horizontal = sd->top_horizontal;

        // The vertical outer border
        sd->border_vertical = style == TSTYLE_SINGLE_V2 ? BDL_V : " ";

        // The interior horizontal line (also used for line below the title)
        sd->middle_horizontal = BDL_H;

        // Interior vertical
        sd->middle_vertical = BDL_V;

        // The connection to the lefty and right border for interior lines
        sd->middle_left =
            style == TSTYLE_SINGLE_V2 ? BDL_VR : sd->middle_horizontal;
        sd->middle_right =
            style == TSTYLE_SINGLE_V2 ? BDL_VL : sd->middle_horizontal;

        // Connection for interior lines
        sd->middle_horizontal_up = BDL_UH;
        sd->middle_horizontal_down = BDL_DH;
        sd->middle_cross = BDL_X;

    } 
}

static void 
style_body_heavy(tblstyle_t style, style_t *sd) {
  if (style == TSTYLE_HEAVY_V1 || style == TSTYLE_HEAVY_V2 ||
               style == TSTYLE_HEAVY_V3) {
        // The very top outer border line
        sd->top_horizontal = BDL_HB_H;
        sd->top_down = BDL_HB_DH;

        // The very bottom outer border line
        sd->bottom_horizontal = sd->top_horizontal;
        sd->bottom_up = BDL_HB_UH;

        // The four corners
        sd->top_left =
            style == TSTYLE_HEAVY_V1 ? sd->top_horizontal : BDL_HB_DR;
        sd->top_right =
            style == TSTYLE_HEAVY_V1 ? sd->top_horizontal : BDL_HB_DL;
        sd->bottom_left =
            style == TSTYLE_HEAVY_V1 ? sd->top_horizontal : BDL_HB_UR;
        sd->bottom_right =
            style == TSTYLE_HEAVY_V1 ? sd->top_horizontal : BDL_HB_UL;

        // The line below the header row, just before the data rows
        // This is set to be the same as the border style
        sd->top_middle_left = style == TSTYLE_HEAVY_V1   ? sd->top_horizontal
                              : style == TSTYLE_HEAVY_V2 ? BDL_HL_VR
                                                         : BDL_HB_VR;
        sd->top_middle_right = style == TSTYLE_HEAVY_V1   ? sd->top_horizontal
                               : style == TSTYLE_HEAVY_V2 ? BDL_HL_VL
                                                          : BDL_HB_VL;
        sd->top_middle_cross = style == TSTYLE_HEAVY_V3 ? BDL_X : BDL_HB_X;
        sd->top_middle_horizontal =
            style == TSTYLE_HEAVY_V3 ? BDL_H : sd->top_horizontal;

        // The vertical outer border
        sd->border_vertical = style == TSTYLE_HEAVY_V1 ? " " : BDL_HB_V;

        // The interior horizontal line (also used for line below the title)
        sd->middle_horizontal = BDL_H;

        // Interior vertical
        sd->middle_vertical = BDL_V;

        // The connection to the lefty and right border for interior lines
        sd->middle_left =
            style == TSTYLE_HEAVY_V1 ? sd->middle_horizontal : BDL_HB_VR;
        sd->middle_right =
            style == TSTYLE_HEAVY_V1 ? sd->middle_horizontal : BDL_HB_VL;

        // Connection for interior lines
        sd->middle_horizontal_up = BDL_UH;
        sd->middle_horizontal_down = BDL_DH;
        sd->middle_cross = BDL_X;

    }
}

/**
 * Internal helper function to initialize a style struct
 * @param sd	Data block that is filled with the chars to use
 */ 
static void
init_style(style_t *sd) {
    sd->top_left = " ";
    sd->top_horizontal = " ";
    sd->top_down = " ";
    sd->top_right = " ";
    sd->top_middle_left = " ";
    sd->top_middle_right = " ";
    sd->top_middle_cross = " ";
    sd->top_middle_horizontal = " ";
    sd->middle_left = " ";
    sd->middle_horizontal = " ";
    sd->middle_right = " ";
    sd->middle_horizontal_up = " ";
    sd->middle_horizontal_down = " ";
    sd->middle_cross = " ";
    sd->middle_vertical = " ";
    sd->border_vertical = " ";
    sd->bottom_left = " ";
    sd->bottom_horizontal = " ";
    sd->bottom_right = " ";
    sd->bottom_up = " ";
    sd->have_bottom_border = TRUE;
}


/**
 * Internal helper function to set the table drawing characters to use
 * in a particular style
 * @param t	Table handle
 * @param style Style to use
 * @param sd	Data block that is filled with the chars to use
 */
void 
get_style(tblstyle_t style, style_t *sd, _Bool interior_vert) {
   
    init_style(sd);

    // Jumptable for each style
    (sjt[style])(style,sd);

    // Should the table have interior vertical divider between columns
    if (!interior_vert) {
        sd->top_down = sd->top_horizontal;
        sd->top_middle_cross = sd->top_middle_horizontal;
        sd->bottom_up = sd->bottom_horizontal;
        sd->middle_vertical = " ";
        sd->middle_horizontal_up = sd->middle_horizontal;
        sd->middle_horizontal_down = sd->middle_horizontal;
        sd->middle_cross = sd->middle_horizontal;
    }
}

/* EOF */
