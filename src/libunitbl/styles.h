/* =========================================================================
 * File:        styles.h
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


#ifndef STYLES_H
#define	STYLES_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdio.h>

// Shortcut names for the individual characters for easy of use

// Single line character
#define BDL_DR "\u250c"  // Down Right
#define BDL_DL "\u2510"  // Down Left
#define BDL_UR "\u2514"  // Up Right
#define BDL_UL "\u2518"  // Up Left
#define BDL_VR "\u251c"  // Vertical Right
#define BDL_VL "\u2524"  // Vertical Left
#define BDL_DH "\u252c"  // Down Horizontal
#define BDL_UH "\u2534"  // Up Horizontal
#define BDL_V "\u2502"   // Vertical
#define BDL_H "\u2500"   // Horizontal
#define BDL_X "\u253c"   // Cross

// Single line rounded corne
#define BDL_SR_DR "\u256d"  // Down Right
#define BDL_SR_DL "\u256e"  // Down Left
#define BDL_SR_UR "\u2570"  // Up Right
#define BDL_SR_UL "\u256f"  // Up Left
#define BDL_SR_VR BDL_VR    // Vertical Right
#define BDL_SR_VL BDL_VL    // Vertical Left
#define BDL_SR_DH BDL_DH    // Down Horizontal
#define BDL_SR_UH BDL_UH    // Up Horizontal
#define BDL_SR_V BDL_V      // Vertical
#define BDL_SR_H BDL_H      // Horizontal
#define BDL_SR_X BDL_X      // Cross

// Double line chars connected to double
#define BDL_DL_H "\u2550"
#define BDL_DL_V "\u2551"
#define BDL_DL_DR "\u2554"
#define BDL_DL_DL "\u2557"
#define BDL_DL_UR "\u255a"
#define BDL_DL_UL "\u255d"
#define BDL_DL_VR "\u2560"
#define BDL_DL_VL "\u2563"
#define BDL_DL_UH "\u2569"
#define BDL_DL_DH "\u2566"
#define BDL_DL_X "\u256c"

// Double horizontal chars connected to thin
#define BDL_DH_H BDL_DL_H
#define BDL_DH_V BDL_V
#define BDL_DH_DR "\u2552"
#define BDL_DH_DL "\u2555"
#define BDL_DH_UR "\u2558"
#define BDL_DH_UL "\u255b"
#define BDL_DH_VR "\u255e"
#define BDL_DH_VL "\u2561"
#define BDL_DH_DH "\u2564"
#define BDL_DH_UH "\u2567"
#define BDL_DH_X "\u256a"

// Double vertical chars connected to thin
#define BDL_DV_H BDL_H
#define BDL_DV_V BDL_DL_V
#define BDL_DV_DR "\u2553"  // Down Right
#define BDL_DV_DL "\u2556"  // Down Left
#define BDL_DV_UR "\u2559"  // Up Right
#define BDL_DV_UL "\u255c"  // Up Left
#define BDL_DV_VR "\u255f"
#define BDL_DV_VL "\u2562"
#define BDL_DV_DH "\u2565"
#define BDL_DV_UH "\u2568"
#define BDL_DV_X "\u256b"

// Heavy border lines
#define BDL_HL_H "\u2501"
#define BDL_HL_V "\u2503"
#define BDL_HL_DR "\u250f"  // Down Right
#define BDL_HL_DL "\u2513"  // Down Left
#define BDL_HL_UR "\u2517"  // Up Right
#define BDL_HL_UL "\u251b"  // Up Left
#define BDL_HL_VR "\u2523"
#define BDL_HL_VL "\u252b"
#define BDL_HL_DH "\u2533"
#define BDL_HL_UH "\u253b"

// Heavy border lines connected to thin interior
#define BDL_HB_H BDL_HL_H
#define BDL_HB_V BDL_HL_V
#define BDL_HB_DR BDL_HL_DR
#define BDL_HB_DL BDL_HL_DL
#define BDL_HB_UR BDL_HL_UR
#define BDL_HB_UL BDL_HL_UL
#define BDL_HB_VR "\u2520"
#define BDL_HB_VL "\u2528"
#define BDL_HB_DH "\u252f"
#define BDL_HB_UH "\u2537"
#define BDL_HB_X "\u253f"


#define NBR_TSTYLES 20

/**
 * Define tha symbols (chars) that go into table style
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

typedef void (*style_func_t)(tblstyle_t style, style_t *sd);

extern style_func_t sjt[];

void 
get_style(tblstyle_t style, style_t *sd, _Bool interior_vert);


#ifdef	__cplusplus
}
#endif

#endif	/* STYLES_H */
