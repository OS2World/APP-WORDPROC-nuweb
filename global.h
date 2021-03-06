#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE (!0)
#endif
typedef struct scrap_node {
  struct scrap_node *next;
  int scrap;
} Scrap_Node;
typedef struct name {
  char *spelling;
  struct name *llink;
  struct name *rlink;
  Scrap_Node *defs;
  Scrap_Node *uses;
  int mark;
  char tab_flag;
  char indent_flag;
  char debug_flag;
} Name;

extern int tex_flag;      /* if FALSE, don't emit the .tex file */
extern int output_flag;   /* if FALSE, don't emit the output files */
extern int compare_flag;  /* if FALSE, overwrite without comparison */
extern int verbose_flag;  /* if TRUE, write progress information */
extern int number_flag;   /* if TRUE, use a sequential numbering scheme */
extern char *command_name;
extern char *source_name;  /* name of the current file */
extern int source_line;    /* current line in the source file */
extern int already_warned;
extern Name *file_names;
extern Name *macro_names;
extern Name *user_names;

extern void pass1();
extern void write_tex();
extern void write_files();
extern void source_open(); /* pass in the name of the source file */
extern int source_get();   /* no args; returns the next char or EOF */
extern void init_scraps();
extern int collect_scrap();
extern int write_scraps();
extern void write_scrap_ref();
extern void write_single_scrap_ref();
extern void collect_numbers();
extern Name *collect_file_name();
extern Name *collect_macro_name();
extern Name *collect_scrap_name();
extern Name *name_add();
extern Name *prefix_add();
extern char *save_string();
extern void reverse_lists();
extern void search();
extern void *arena_getmem();
extern void arena_free();

