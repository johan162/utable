// We want the full POSIX and C99 standard
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include <sys/param.h> // To get MIN/MAX

#include "libunitbl/unicode_tbl.h"
 

void ut1(void) {
  table_t *tbl = utable_create(7, 7);

  if (NULL == tbl) {
    printf("Cannot create table\n");
    exit(EXIT_SUCCESS);
  }
  char buff[64];

  for (size_t r = 0; r < tbl->nRow; r++) {
    for (size_t c = 0; c < tbl->nCol; c++) {
      snprintf(buff, sizeof (buff), " (%zu,%zu) ", r, c);
      utable_set_cell(tbl, r, c, buff);
    }
  }

  if (-1 ==  utable_set_table_halign(tbl,CENTERALIGN) )
    goto tbl_err;
  
  if (-1 == utable_set_cell_halign(tbl, 2, 2, RIGHTALIGN))
    goto tbl_err;

  if (-1 == utable_set_colwidth(tbl, 0, 20) ||
      -1 == utable_set_cell_colspan(tbl, 2, 2, 3) )
    goto tbl_err;

  utable_set_table_mincolwidth(tbl,12);
  utable_set_table_cellpadding(tbl, 2, 2);
  utable_set_interior(tbl,TRUE,TRUE);

  if (-1 == utable_set_cell(tbl, 0, 0, " Much longer text than column width "))
    goto tbl_err;

  utable_set_title(tbl, "Table title", TITLESTYLE_LINE);
  
  utable_stroke(tbl, STDOUT_FILENO, TSTYLE_DOUBLE_V4);

  return;

 tbl_err:
  printf("\n\n");
  printf("Failed!\n");
  exit(EXIT_FAILURE);

}

void
ut2(void) {

  /*
    char *data[] = {
    "Titel 1","Titel 2","Titel 3","Titel 4","Titel 5","Titel 6",
    "2012-12-12 12:12","3000000001","30.345678","17.676767","ÖVERJÄRVÅ TÅRNE 32, 134 34 LÅNGJÄRVI NUMÅENDE","olle",
    "2012-12-12 12:12","3000000001","30.345678","17.676767","ÖVERJÄRVÅ TÅRNE 32, 134 34 LÅNGJÄRVI NUMÅENDE","olle"
    };
  */

  char *data[] = {
		  "Titel 1",
		  "ÖVERJÄRVÅ TÅRNE 32, 134 34 LÅNGJÄRVI NUMÅENDE",
		  "ÖVERJÄRVÅ TÅRNE 32, 134 34 LÅNGJÄRVI NUMÅENDE",
  };


  //table_t *tbl = table_create_set(3, 6,data);
  table_t *tbl = utable_create_set(3, 1, data);


  if (NULL == tbl) {
    printf("Cannot create table\n");
    exit(EXIT_FAILURE);
  }

  utable_stroke(tbl, STDOUT_FILENO, TSTYLE_SINGLE_V1);
}

void
ut3(void) {


  char *data[] = {
		  "Title 1", "Title 2", "Title 3", "Title 4", "Title 5", "Title 6",
		  "2012-12-12 12:12", "3000000001", "30.345678", "17.676767", "ÖVERJÄRVÅ TÅRNE 32, 134 34 LÅNGJÄRVI NUMÅENDE", "olle",
		  "2012-12-12 12:12", "3000000001", "30.345678", "17.676767", "ÖVERJÄRVÅ TÅRNE 32, 134 34 LÅNGJÄRVI NUMÅENDE", "olle"
  };


  /*
    char *data[] = {
    "Kol 1", "Kol 2", 
    "ÖVERJÄRVÅ TÅRNE 32, 134 34 LÅNGJÄRVI NUMÅENDE","Olle",
    "ÖVERJÄRVÅ TÅRNE 32, 134 34 LÅNGJÄRVI NUMÅENDE","Kalle"
    };
  */

  table_t *tbl = utable_create_set(3, 6, data);
  //table_t *tbl = utable_create_set(3, 1,data);


  if (NULL == tbl) {
    printf("Cannot create table\n");
    exit(EXIT_FAILURE);
  }

  utable_set_title(tbl, "Table title", TITLESTYLE_LINE);

  utable_set_row_halign(tbl, 0, CENTERALIGN);
  utable_set_table_cellpadding(tbl,1,1);
  
  utable_set_title(tbl, "TSTYLE_ASCII_V0", TITLESTYLE_LINE);
  utable_stroke(tbl, STDOUT_FILENO, TSTYLE_ASCII_V0);
    
  printf("\n\n\n");
  utable_set_title(tbl, "TSTYLE_ASCII_V4", TITLESTYLE_LINE);
  utable_stroke(tbl, STDOUT_FILENO, TSTYLE_ASCII_V4);
    
  utable_set_interior(tbl, TRUE, FALSE);
    
  printf("\n\n\n");
  utable_set_title(tbl, "TSTYLE_ASCII_V0 + Vert interior", TITLESTYLE_LINE);
  utable_stroke(tbl, STDOUT_FILENO, TSTYLE_ASCII_V0);

  printf("\n\n\n");
  utable_set_title(tbl, "TSTYLE_ASCII_V4  + Vert interior", TITLESTYLE_LINE);
  utable_stroke(tbl, STDOUT_FILENO, TSTYLE_ASCII_V4);
   
    
  utable_set_interior(tbl, FALSE, FALSE);
  printf("\n\n\n");
  utable_set_title(tbl, "TSTYLE_ASCII_V1", TITLESTYLE_LINE);
  utable_stroke(tbl, STDOUT_FILENO, TSTYLE_ASCII_V1);

  printf("\n\n\n");
  utable_set_title(tbl, "TSTYLE_ASCII_V2", TITLESTYLE_LINE);
  utable_stroke(tbl, STDOUT_FILENO, TSTYLE_ASCII_V2);
    
    

  printf("\n\n\n");
  utable_set_title(tbl, "TSTYLE_ASCII_V3", TITLESTYLE_LINE);
  utable_stroke(tbl, STDOUT_FILENO, TSTYLE_ASCII_V3);

  printf("\n\n\n");
  utable_set_title(tbl, "TSTYLE_DOUBLE_V1", TITLESTYLE_LINE);
  utable_stroke(tbl, STDOUT_FILENO, TSTYLE_DOUBLE_V1);

  printf("\n\n\n");
  utable_set_title(tbl, "TSTYLE_DOUBLE_V2", TITLESTYLE_LINE);
  utable_stroke(tbl, STDOUT_FILENO, TSTYLE_DOUBLE_V2);

  printf("\n\n\n");
  utable_set_title(tbl, "TSTYLE_DOUBLE_V3", TITLESTYLE_LINE);
  utable_stroke(tbl, STDOUT_FILENO, TSTYLE_DOUBLE_V3);

  printf("\n\n");
  utable_set_title(tbl, "TSTYLE_DOUBLE_V4", TITLESTYLE_LINE);
  utable_stroke(tbl, STDOUT_FILENO, TSTYLE_DOUBLE_V4);

  printf("\n\n");
  utable_set_title(tbl, "TSTYLE_SINGLE_V1", TITLESTYLE_LINE);
  utable_stroke(tbl, STDOUT_FILENO, TSTYLE_SINGLE_V1);

  printf("\n\n");
  utable_set_title(tbl, "TSTYLE_SINGLE_V2", TITLESTYLE_LINE);
  utable_stroke(tbl, STDOUT_FILENO, TSTYLE_SINGLE_V2);
    
    
  printf("\n\n");
  utable_set_title(tbl, "TSTYLE_HEAVY_V1", TITLESTYLE_LINE);
  utable_stroke(tbl, STDOUT_FILENO, TSTYLE_HEAVY_V1);

  printf("\n\n");
  utable_set_title(tbl, "TSTYLE_HEAVY_V2", TITLESTYLE_LINE);
  utable_stroke(tbl, STDOUT_FILENO, TSTYLE_HEAVY_V2);    
    
  printf("\n\n");
  utable_set_title(tbl, "TSTYLE_HEAVY_V3", TITLESTYLE_LINE);
  utable_stroke(tbl, STDOUT_FILENO, TSTYLE_HEAVY_V3);    
    
  printf("\n\n");
  utable_set_title(tbl, "TSTYLE_SIMPLE_V1", TITLESTYLE_LINE);
  utable_stroke(tbl, STDOUT_FILENO, TSTYLE_SIMPLE_V1);    

  printf("\n\n");
  utable_set_title(tbl, "TSTYLE_SIMPLE_V2", TITLESTYLE_LINE);
  utable_stroke(tbl, STDOUT_FILENO, TSTYLE_SIMPLE_V2);    

  printf("\n\n");
  utable_set_title(tbl, "TSTYLE_SIMPLE_V3", TITLESTYLE_LINE);
  utable_stroke(tbl, STDOUT_FILENO, TSTYLE_SIMPLE_V3);    

  printf("\n\n");
  utable_set_title(tbl, "TSTYLE_SIMPLE_V4", TITLESTYLE_LINE);
  utable_stroke(tbl, STDOUT_FILENO, TSTYLE_SIMPLE_V4);    
    
  printf("\n\n");
  utable_set_title(tbl, "TSTYLE_SIMPLE_V5", TITLESTYLE_LINE);
  utable_stroke(tbl, STDOUT_FILENO, TSTYLE_SIMPLE_V5);
    
  printf("\n\n");
  utable_set_title(tbl, "TSTYLE_SIMPLE_V6", TITLESTYLE_LINE);
  utable_stroke(tbl, STDOUT_FILENO, TSTYLE_SIMPLE_V6);        

  printf("\n\n");
  utable_set_title(tbl, "TSTYLE_SIMPLE_V1 (+ Vert Interior)", TITLESTYLE_LINE);
  utable_set_interior(tbl, TRUE, FALSE);
  utable_stroke(tbl, STDOUT_FILENO, TSTYLE_SIMPLE_V1);
    
  printf("\n\n");
  utable_set_title(tbl, "TSTYLE_SIMPLE_V2 (+ Vert Interior)", TITLESTYLE_LINE);
  utable_set_interior(tbl, TRUE, FALSE);
  utable_stroke(tbl, STDOUT_FILENO, TSTYLE_SIMPLE_V2);
    
  printf("\n\n");
  utable_set_title(tbl, "TSTYLE_SIMPLE_V3 (+ Vert Interior)", TITLESTYLE_LINE);
  utable_set_interior(tbl, TRUE, FALSE);
  utable_stroke(tbl, STDOUT_FILENO, TSTYLE_SIMPLE_V3);

  printf("\n\n");
  utable_set_title(tbl, "TSTYLE_SIMPLE_V4 (+ Vert Interior)", TITLESTYLE_LINE);
  utable_set_interior(tbl, TRUE, FALSE);
  utable_stroke(tbl, STDOUT_FILENO, TSTYLE_SIMPLE_V4);

  printf("\n\n");
  utable_set_title(tbl, "TSTYLE_SIMPLE_V5 (+ Vert Interior)", TITLESTYLE_LINE);
  utable_set_interior(tbl, TRUE, FALSE);
  utable_stroke(tbl, STDOUT_FILENO, TSTYLE_SIMPLE_V5);

  printf("\n\n");
  utable_set_title(tbl, "TSTYLE_SIMPLE_V6 (+ Vert Interior)", TITLESTYLE_LINE);
  utable_set_interior(tbl, TRUE, FALSE);
  utable_stroke(tbl, STDOUT_FILENO, TSTYLE_SIMPLE_V6);
    
    
  printf("\n\n");
  utable_set_headerline(tbl,FALSE);
  utable_set_table_cellpadding(tbl,2,2);
  utable_set_title(tbl, "TSTYLE_SIMPLE_V3 (HEADER_LINE=FALSE, + Vert Interior)", TITLESTYLE_LINE);
  utable_stroke(tbl, STDOUT_FILENO, TSTYLE_SIMPLE_V3);    
    
    
  printf("\n\n");
  utable_set_interior(tbl, FALSE, FALSE);
  utable_set_headerline(tbl,FALSE);
  utable_set_table_cellpadding(tbl,2,2);
  utable_set_title(tbl, "TSTYLE_SIMPLE_V3 (HEADER_LINE=FALSE)", TITLESTYLE_LINE);
  utable_stroke(tbl, STDOUT_FILENO, TSTYLE_SIMPLE_V3);    
    
  printf("\n\n");
  
}

char *cell_cb(int r, int c, void *tag) {
  static char buff[80];
  snprintf(buff,80,"(%d, %d)",r,c);
  return buff;
}

void
ut4(void) {

  char *coltitles[] = {
		       "Title 1", "Title 2", "Title 3", "Title 4", "Title 5", "Title 6"
  };

  table_t *tbl = utable_create(5, 6);

  if (NULL == tbl) {
    printf("Cannot create table\n");
    exit(EXIT_FAILURE);
  }
  utable_set_coltitles(tbl,coltitles);
  //    utable_set_table_mincolwidth(tbl,15);
  utable_set_row_halign(tbl, 0, CENTERALIGN);
  //    utable_set_table_cellpadding(tbl,2,2);
  utable_set_interior(tbl, TRUE, FALSE);
    
  utable_set_title(tbl, "Table title", TITLESTYLE_LINE);

  utable_set_table_cellcallback(tbl,cell_cb);
  utable_stroke(tbl, STDOUT_FILENO, TSTYLE_ASCII_V4);  
    
}

// Some rudimentary unit-test
// gcc -std=c99 -DTABLE_UNIT_TEST unicode_tbl.c 

int
main(int argc, char **argv) {

  if( argc == 1 )
    ut4();
  else if ( argc == 2 ) {
    if( strcmp(argv[1],"ut1") == 0 )
      ut1();
    else if( strcmp(argv[1],"ut2") == 0 )
      ut2();
    else if( strcmp(argv[1],"ut3") == 0)
      ut3();
    else if( strcmp(argv[1],"ut4") == 0)
      ut4();
    else {
      char *errstr="Usage test_table \"ut<1|2|3|4>\"\n";
      size_t n = write(STDERR_FILENO,errstr,strlen(errstr));
      if( n == strlen(errstr) )
	n=0;
      fsync(STDERR_FILENO);
    }
  }
  else {
    char *errstr="Usage test_table \"ut<1|2|3|4>\"\n";
    size_t n = write(STDERR_FILENO,errstr,strlen(errstr));
    if( n == strlen(errstr) )
      n=0;
    fsync(STDERR_FILENO);
  }
  
  printf("\n\n");

  exit(EXIT_SUCCESS);
}




