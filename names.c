#include "global.h"
enum { LESS, GREATER, EQUAL, PREFIX, EXTENSION };

static int compare(x, y)
     char *x;
     char *y;
{
  int len, result;
  int xl = strlen(x);
  int yl = strlen(y);
  int xp = x[xl - 1] == ' ';
  int yp = y[yl - 1] == ' ';
  if (xp) xl--;
  if (yp) yl--;
  len = xl < yl ? xl : yl;
  result = strncmp(x, y, len);
  if (result < 0) return GREATER;
  else if (result > 0) return LESS;
  else if (xl < yl) {
    if (xp) return EXTENSION;
    else return LESS;
  }
  else if (xl > yl) {
    if (yp) return PREFIX;
    else return GREATER;
  }
  else return EQUAL;
}
char *save_string(s)
     char *s;
{
  char *new = (char *) arena_getmem((strlen(s) + 1) * sizeof(char));
  strcpy(new, s);
  return new;
}
static int ambiguous_prefix();

Name *prefix_add(root, spelling)
     Name **root;
     char *spelling;
{
  Name *node = *root;
  while (node) {
    switch (compare(node->spelling, spelling)) {
    case GREATER:   root = &node->rlink;
                    break;
    case LESS:      root = &node->llink;
                    break;
    case EQUAL:     return node;
    case EXTENSION: node->spelling = save_string(spelling);
                    return node;
    case PREFIX:    {
                      if (ambiguous_prefix(node->llink, spelling) ||
                          ambiguous_prefix(node->rlink, spelling))
                        fprintf(stderr,
                                "%s: ambiguous prefix @<%s...@> (%s, line %d)\n",
                                command_name, spelling, source_name, source_line);
                    }
                    return node;
    }
    node = *root;
  }
  {
    node = (Name *) arena_getmem(sizeof(Name));
    node->spelling = save_string(spelling);
    node->mark = FALSE;
    node->llink = NULL;
    node->rlink = NULL;
    node->uses = NULL;
    node->defs = NULL;
    node->tab_flag = TRUE;
    node->indent_flag = TRUE;
    node->debug_flag = FALSE;
    *root = node;
    return node;
  }
}
static int ambiguous_prefix(node, spelling)
     Name *node;
     char *spelling;
{
  while (node) {
    switch (compare(node->spelling, spelling)) {
    case GREATER:   node = node->rlink;
                    break;
    case LESS:      node = node->llink;
                    break;
    case EQUAL:
    case EXTENSION:
    case PREFIX:    return TRUE;
    }
  }
  return FALSE;
}
static int robs_strcmp(x, y)
     char *x;
     char *y;
{
  char *xx = x;
  char *yy = y;
  int xc = toupper(*xx);
  int yc = toupper(*yy);
  while (xc == yc && xc) {
    xx++;
    yy++;
    xc = toupper(*xx);
    yc = toupper(*yy);
  }
  if (xc != yc) return xc - yc;
  xc = *x;
  yc = *y;
  while (xc == yc && xc) {
    x++;
    y++;
    xc = *x;
    yc = *y;
  }
  if (isupper(xc) && islower(yc))
    return xc * 2 - (toupper(yc) * 2 + 1);
  if (islower(xc) && isupper(yc))
    return toupper(xc) * 2 + 1 - yc * 2;
  return xc - yc;
}
Name *name_add(root, spelling)
     Name **root;
     char *spelling;
{
  Name *node = *root;
  while (node) {
    int result = robs_strcmp(node->spelling, spelling);
    if (result > 0)
      root = &node->llink;
    else if (result < 0)
      root = &node->rlink;
    else
      return node;
    node = *root;
  }
  {
    node = (Name *) arena_getmem(sizeof(Name));
    node->spelling = save_string(spelling);
    node->mark = FALSE;
    node->llink = NULL;
    node->rlink = NULL;
    node->uses = NULL;
    node->defs = NULL;
    node->tab_flag = TRUE;
    node->indent_flag = TRUE;
    node->debug_flag = FALSE;
    *root = node;
    return node;
  }
}
Name *collect_file_name()
{
  Name *new_name;
  char name[100];
  char *p = name;
  int start_line = source_line;
  int c = source_get();
  while (isspace(c))
    c = source_get();
  while (isgraph(c)) {
    *p++ = c;
    c = source_get();
  }
  if (p == name) {
    fprintf(stderr, "%s: expected file name (%s, %d)\n",
            command_name, source_name, start_line);
    exit(-1);
  }
  *p = '\0';
  new_name = name_add(&file_names, name);
  {
    while (1) {
      while (isspace(c))
        c = source_get();
      if (c == '-') {
        c = source_get();
        do {
          switch (c) {
            case 't': new_name->tab_flag = FALSE;
                      break;
            case 'd': new_name->debug_flag = TRUE;
                      break;
            case 'i': new_name->indent_flag = FALSE;
                      break;
            default : fprintf(stderr, "%s: unexpected per-file flag (%s, %d)\n",
                              command_name, source_name, source_line);
                      break;
          }
          c = source_get();
        } while (!isspace(c));
      }
      else break;
    }
  }
  if (c != '@' || source_get() != '{') {
    fprintf(stderr, "%s: expected @{ after file name (%s, %d)\n",
            command_name, source_name, start_line);
    exit(-1);
  }
  return new_name;
}
Name *collect_macro_name()
{
  char name[100];
  char *p = name;
  int start_line = source_line;
  int c = source_get();
  while (isspace(c))
    c = source_get();
  while (c != EOF) {
    switch (c) {
      case '@':  {
                   c = source_get();
                   switch (c) {
                     case '@': *p++ = c;
                               break;
                     case '{': {
                                 if (p > name && p[-1] == ' ')
                                   p--;
                                 if (p - name > 3 && p[-1] == '.' && p[-2] == '.' && p[-3] == '.') {
                                   p[-3] = ' ';
                                   p -= 2;
                                 }
                                 if (p == name || name[0] == ' ') {
                                   fprintf(stderr, "%s: empty scrap name (%s, %d)\n",
                                           command_name, source_name, source_line);
                                   exit(-1);
                                 }
                                 *p = '\0';
                                 return prefix_add(&macro_names, name);
                               }
                     default:  fprintf(stderr,
                                       "%s: unexpected @%c in macro name (%s, %d)\n",
                                       command_name, c, source_name, start_line);
                               exit(-1);
                   }
                 }
                 break;
      case '\t':
      case ' ':  *p++ = ' ';
                 do
                   c = source_get();
                 while (c == ' ' || c == '\t');
                 break;
      case '\n': {
                   do
                     c = source_get();
                   while (isspace(c));
                   if (c != '@' || source_get() != '{') {
                     fprintf(stderr, "%s: expected @{ after macro name (%s, %d)\n",
                             command_name, source_name, start_line);
                     exit(-1);
                   }
                   {
                     if (p > name && p[-1] == ' ')
                       p--;
                     if (p - name > 3 && p[-1] == '.' && p[-2] == '.' && p[-3] == '.') {
                       p[-3] = ' ';
                       p -= 2;
                     }
                     if (p == name || name[0] == ' ') {
                       fprintf(stderr, "%s: empty scrap name (%s, %d)\n",
                               command_name, source_name, source_line);
                       exit(-1);
                     }
                     *p = '\0';
                     return prefix_add(&macro_names, name);
                   }
                 }
      default:   *p++ = c;
                 c = source_get();
                 break;
    }
  }
  fprintf(stderr, "%s: expected macro name (%s, %d)\n",
          command_name, source_name, start_line);
  exit(-1);
  return NULL;  /* unreachable return to avoid warnings on some compilers */
}
Name *collect_scrap_name()
{
  char name[100];
  char *p = name;
  int c = source_get();
  while (c == ' ' || c == '\t')
    c = source_get();
  while (c != EOF) {
    switch (c) {
      case '@':  {
                   c = source_get();
                   switch (c) {
                     case '@': *p++ = c;
                               c = source_get();
                               break;
                     case '>': {
                                 if (p > name && p[-1] == ' ')
                                   p--;
                                 if (p - name > 3 && p[-1] == '.' && p[-2] == '.' && p[-3] == '.') {
                                   p[-3] = ' ';
                                   p -= 2;
                                 }
                                 if (p == name || name[0] == ' ') {
                                   fprintf(stderr, "%s: empty scrap name (%s, %d)\n",
                                           command_name, source_name, source_line);
                                   exit(-1);
                                 }
                                 *p = '\0';
                                 return prefix_add(&macro_names, name);
                               }
                     default:  fprintf(stderr,
                                       "%s: unexpected @%c in macro name (%s, %d)\n",
                                       command_name, c, source_name, source_line);
                               exit(-1);
                   }
                 }
                 break;
      case '\t':
      case ' ':  *p++ = ' ';
                 do
                   c = source_get();
                 while (c == ' ' || c == '\t');
                 break;
      default:   if (!isgraph(c)) {
                   fprintf(stderr,
                           "%s: unexpected character in macro name (%s, %d)\n",
                           command_name, source_name, source_line);
                   exit(-1);
                 }
                 *p++ = c;
                 c = source_get();
                 break;
    }
  }
  fprintf(stderr, "%s: unexpected end of file (%s, %d)\n",
          command_name, source_name, source_line);
  exit(-1);
  return NULL;  /* unreachable return to avoid warnings on some compilers */
}
static Scrap_Node *reverse();   /* a forward declaration */

void reverse_lists(names)
     Name *names;
{
  while (names) {
    reverse_lists(names->llink);
    names->defs = reverse(names->defs);
    names->uses = reverse(names->uses);
    names = names->rlink;
  }
}
static Scrap_Node *reverse(a)
     Scrap_Node *a;
{
  if (a) {
    Scrap_Node *b = a->next;
    a->next = NULL;
    while (b) {
      Scrap_Node *c = b->next;
      b->next = a;
      a = b;
      b = c;
    }
  }
  return a;
}
