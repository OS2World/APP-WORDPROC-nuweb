#include "global.h"
static FILE *source_file;  /* the current input file */
static int source_peek;
static int double_at;
static int include_depth;
struct {
  FILE *file;
  char *name;
  int line;
} stack[10];
int source_get()
{
  int c = source_peek;
  switch (c) {
    case EOF:  {
                 fclose(source_file);
                 if (include_depth) {
                   include_depth--;
                   source_file = stack[include_depth].file;
                   source_line = stack[include_depth].line;
                   source_name = stack[include_depth].name;
                   source_peek = getc(source_file);
                   c = source_get();
                 }
               }
               return c;
    case '@':  {
                 c = getc(source_file);
                 if (double_at) {
                   source_peek = c;
                   double_at = FALSE;
                   c = '@';
                 }
                 else
                   switch (c) {
                     case 'i': {
                                 char name[100];
                                 if (include_depth >= 10) {
                                   fprintf(stderr, "%s: include nesting too deep (%s, %d)\n",
                                           command_name, source_name, source_line);
                                   exit(-1);
                                 }
                                 {
                                     char *p = name;
                                     do 
                                       c = getc(source_file);
                                     while (c == ' ' || c == '\t');
                                     while (isgraph(c)) {
                                       *p++ = c;
                                       c = getc(source_file);
                                     }
                                     *p = '\0';
                                     if (c != '\n') {
                                       fprintf(stderr, "%s: unexpected characters after file name (%s, %d)\n",
                                               command_name, source_name, source_line);
                                       exit(-1);
                                     }
                                 }
                                 stack[include_depth].name = source_name;
                                 stack[include_depth].file = source_file;
                                 stack[include_depth].line = source_line + 1;
                                 include_depth++;
                                 source_line = 1;
                                 source_name = save_string(name);
                                 source_file = fopen(source_name, "r");
                                 if (!source_file) {
                                   fprintf(stderr, "%s: can't open include file %s\n",
                                    command_name, source_name);
                                   exit(-1);
                                 }
                                 source_peek = getc(source_file);
                                 c = source_get();
                               }
                               break;
                     case 'f': case 'm': case 'u':
                     case 'd': case 'o': case 'D': case 'O':
                     case '{': case '}': case '<': case '>': case '|':
                               source_peek = c;
                               c = '@';
                               break;
                     case '@': source_peek = c;
                               double_at = TRUE;
                               break;
                     default:  fprintf(stderr, "%s: bad @ sequence (%s, line %d)\n",
                                       command_name, source_name, source_line);
                               exit(-1);
                   }
               }
               return c;
    case '\n': source_line++;
    default:   source_peek = getc(source_file);
               return c;
  }
}
void source_open(name)
     char *name;
{
  source_file = fopen(name, "r");
  if (!source_file) {
    fprintf(stderr, "%s: couldn't open %s\n", command_name, name);
    exit(-1);
  }
  source_name = name;
  source_line = 1;
  source_peek = getc(source_file);
  double_at = FALSE;
  include_depth = 0;
}
