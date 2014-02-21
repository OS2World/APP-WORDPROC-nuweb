#include "global.h"
static void copy_scrap();               /* formats the body of a scrap */
static void print_scrap_numbers();      /* formats a list of scrap numbers */
static void format_entry();             /* formats an index entry */
static void format_user_entry();
void write_tex(file_name, tex_name)
     char *file_name;
     char *tex_name;
{
  FILE *tex_file = fopen(tex_name, "w");
  if (tex_file) {
    if (verbose_flag)
      fprintf(stderr, "writing %s\n", tex_name);
    source_open(file_name);
    {
      int scraps = 1;
      int c = source_get();
      while (c != EOF) {
        if (c == '@')
          {
            int big_definition = FALSE;
            c = source_get();
            switch (c) {
              case 'O': big_definition = TRUE;
              case 'o': {
                          Name *name = collect_file_name();
                          {
                            fputs("\\begin{flushleft} \\small", tex_file);
                            if (!big_definition)
                              fputs("\n\\begin{minipage}{\\linewidth}", tex_file);
                            fprintf(tex_file, " \\label{scrap%d}\n", scraps);
                          }
                          fprintf(tex_file, "\\verb@\"%s\"@ {\\footnotesize ", name->spelling);
                          write_single_scrap_ref(tex_file, scraps++);
                          fputs(" }$\\equiv$\n", tex_file);
                          {
                            fputs("\\vspace{-1ex}\n\\begin{list}{}{} \\item\n", tex_file);
                            copy_scrap(tex_file);
                            fputs("$\\Diamond$\n\\end{list}\n", tex_file);
                          }
                          {
                            if (name->defs->next) {
                              fputs("\\vspace{-1ex}\n", tex_file);
                              fputs("\\footnotesize\\addtolength{\\baselineskip}{-1ex}\n", tex_file);
                              fputs("\\begin{list}{}{\\setlength{\\itemsep}{-\\parsep}", tex_file);
                              fputs("\\setlength{\\itemindent}{-\\leftmargin}}\n", tex_file);
                              fputs("\\item File defined by scraps ", tex_file);
                              print_scrap_numbers(tex_file, name->defs);
                              fputs("\\end{list}\n", tex_file);
                            }
                            else
                              fputs("\\vspace{-2ex}\n", tex_file);
                          }
                          {
                            if (!big_definition)
                              fputs("\\end{minipage}\\\\[4ex]\n", tex_file);
                            fputs("\\end{flushleft}\n", tex_file);
                            do
                              c = source_get();
                            while (isspace(c));
                          }
                        }
                        break;
              case 'D': big_definition = TRUE;
              case 'd': {
                          Name *name = collect_macro_name();
                          {
                            fputs("\\begin{flushleft} \\small", tex_file);
                            if (!big_definition)
                              fputs("\n\\begin{minipage}{\\linewidth}", tex_file);
                            fprintf(tex_file, " \\label{scrap%d}\n", scraps);
                          }
                          fprintf(tex_file, "$\\langle$%s {\\footnotesize ", name->spelling);
                          write_single_scrap_ref(tex_file, scraps++);
                          fputs("}$\\rangle\\equiv$\n", tex_file);
                          {
                            fputs("\\vspace{-1ex}\n\\begin{list}{}{} \\item\n", tex_file);
                            copy_scrap(tex_file);
                            fputs("$\\Diamond$\n\\end{list}\n", tex_file);
                          }
                          {
                            fputs("\\vspace{-1ex}\n", tex_file);
                            fputs("\\footnotesize\\addtolength{\\baselineskip}{-1ex}\n", tex_file);
                            fputs("\\begin{list}{}{\\setlength{\\itemsep}{-\\parsep}", tex_file);
                            fputs("\\setlength{\\itemindent}{-\\leftmargin}}\n", tex_file);
                            if (name->defs->next) {
                              fputs("\\item Macro defined by scraps ", tex_file);
                              print_scrap_numbers(tex_file, name->defs);
                            }
                          }
                          {
                            if (name->uses) {
                              if (name->uses->next) {
                                fputs("\\item Macro referenced in scraps ", tex_file);
                                print_scrap_numbers(tex_file, name->uses);
                              }
                              else {
                                fputs("\\item Macro referenced in scrap ", tex_file);
                                write_single_scrap_ref(tex_file, name->uses->scrap);
                                fputs(".\n", tex_file);
                              }
                            }
                            else {
                              fputs("\\item Macro never referenced.\n", tex_file);
                              fprintf(stderr, "%s: <%s> never referenced.\n",
                                      command_name, name->spelling);
                            }
                            fputs("\\end{list}\n", tex_file);
                          }
                          {
                            if (!big_definition)
                              fputs("\\end{minipage}\\\\[4ex]\n", tex_file);
                            fputs("\\end{flushleft}\n", tex_file);
                            do
                              c = source_get();
                            while (isspace(c));
                          }
                        }
                        break;
              case 'f': {
                          if (file_names) {
                            fputs("\n{\\small\\begin{list}{}{\\setlength{\\itemsep}{-\\parsep}",
                                  tex_file);
                            fputs("\\setlength{\\itemindent}{-\\leftmargin}}\n", tex_file);
                            format_entry(file_names, tex_file, TRUE);
                            fputs("\\end{list}}", tex_file);
                          }
                          c = source_get();
                        }
                        break;
              case 'm': {
                          if (macro_names) {
                            fputs("\n{\\small\\begin{list}{}{\\setlength{\\itemsep}{-\\parsep}",
                                  tex_file);
                            fputs("\\setlength{\\itemindent}{-\\leftmargin}}\n", tex_file);
                            format_entry(macro_names, tex_file, FALSE);
                            fputs("\\end{list}}", tex_file);
                          }
                          c = source_get();
                        }
                        break;
              case 'u': {
                          if (user_names) {
                            fputs("\n{\\small\\begin{list}{}{\\setlength{\\itemsep}{-\\parsep}",
                                  tex_file);
                            fputs("\\setlength{\\itemindent}{-\\leftmargin}}\n", tex_file);
                            format_user_entry(user_names, tex_file);
                            fputs("\\end{list}}", tex_file);
                          }
                          c = source_get();
                        }
                        break;
              case '@': putc(c, tex_file);
              default:  c = source_get();
                        break;
            }
          }
        else {
          putc(c, tex_file);
          c = source_get();
        }
      }
    }
    fclose(tex_file);
  }
  else
    fprintf(stderr, "%s: can't open %s\n", command_name, tex_name);
}
static void print_scrap_numbers(tex_file, scraps)
     FILE *tex_file;
     Scrap_Node *scraps;
{
  int page;
  write_scrap_ref(tex_file, scraps->scrap, TRUE, &page);
  scraps = scraps->next;
  while (scraps) {
    write_scrap_ref(tex_file, scraps->scrap, FALSE, &page);
    scraps = scraps->next;
  }
  fputs(".\n", tex_file);
}
static void copy_scrap(file)
     FILE *file;
{
  int indent = 0;
  int c = source_get();
  fputs("\\mbox{}\\verb@", file);
  while (1) {
    switch (c) {
      case '@':  {
                   c = source_get();
                   switch (c) {
                     case '@': fputs("@{\\tt @}\\verb@", file);
                               break;
                     case '|': {
                                 do {
                                   do
                                     c = source_get();
                                   while (c != '@');
                                   c = source_get();
                                 } while (c != '}');
                               }
                     case '}': putc('@', file);
                               return;
                     case '<': {
                                 Name *name = collect_scrap_name();
                                 fprintf(file, "@$\\langle$%s {\\footnotesize ", name->spelling);
                                 if (name->defs)
                                   {
                                     Scrap_Node *p = name->defs;
                                     write_single_scrap_ref(file, p->scrap);
                                     p = p->next;
                                     if (p)
                                       fputs(", \\ldots\\ ", file);
                                   }
                                 else {
                                   putc('?', file);
                                   fprintf(stderr, "%s: scrap never defined <%s>\n",
                                           command_name, name->spelling);
                                 }
                                 fputs("}$\\rangle$\\verb@", file);
                               }
                               break;
                     default:  /* ignore these since pass1 will have warned about them */
                               break;
                   }
                 }
                 break;
      case '\n': fputs("@\\\\\n\\mbox{}\\verb@", file);
                 indent = 0;
                 break;
      case '\t': {
                   int delta = 8 - (indent % 8);
                   indent += delta;
                   while (delta > 0) {
                     putc(' ', file);
                     delta--;
                   }
                 }
                 break;
      default:   putc(c, file);
                 indent++;
                 break;
    }
    c = source_get();
  }
}
static void format_entry(name, tex_file, file_flag)
     Name *name;
     FILE *tex_file;
     int file_flag;
{
  while (name) {
    format_entry(name->llink, tex_file, file_flag);
    {
      fputs("\\item ", tex_file);
      if (file_flag) {
        fprintf(tex_file, "\\verb@\"%s\"@ ", name->spelling);
        {
          Scrap_Node *p = name->defs;
          fputs("{\\footnotesize Defined by scrap", tex_file);
          if (p->next) {
            fputs("s ", tex_file);
            print_scrap_numbers(tex_file, p);
          }
          else {
            putc(' ', tex_file);
            write_single_scrap_ref(tex_file, p->scrap);
            putc('.', tex_file);
          }
          putc('}', tex_file);
        }
      }
      else {
        fprintf(tex_file, "$\\langle$%s {\\footnotesize ", name->spelling);
        {
          Scrap_Node *p = name->defs;
          if (p) {
            int page;
            write_scrap_ref(tex_file, p->scrap, TRUE, &page);
            p = p->next;
            while (p) {
              write_scrap_ref(tex_file, p->scrap, FALSE, &page);
              p = p->next;
            }
          }
          else
            putc('?', tex_file);
        }
        fputs("}$\\rangle$ ", tex_file);
        {
          Scrap_Node *p = name->uses;
          fputs("{\\footnotesize ", tex_file);
          if (p) {
            fputs("Referenced in scrap", tex_file);
            if (p->next) {
              fputs("s ", tex_file);
              print_scrap_numbers(tex_file, p);
            }
            else {
              putc(' ', tex_file);
              write_single_scrap_ref(tex_file, p->scrap);
              putc('.', tex_file);
            }
          }
          else
            fputs("Not referenced.", tex_file);
          putc('}', tex_file);
        }
      }
      putc('\n', tex_file);
    }
    name = name->rlink;
  }
}
static void format_user_entry(name, tex_file)
     Name *name;
     FILE *tex_file;
{
  while (name) {
    format_user_entry(name->llink, tex_file);
    {
      Scrap_Node *uses = name->uses;
      if (uses) {
        int page;
        Scrap_Node *defs = name->defs;
        fprintf(tex_file, "\\item \\verb@%s@: ", name->spelling);
        if (uses->scrap < defs->scrap) {
          write_scrap_ref(tex_file, uses->scrap, TRUE, &page);
          uses = uses->next;
        }
        else {
          if (defs->scrap == uses->scrap)
            uses = uses->next;
          fputs("\\underline{", tex_file);
          write_single_scrap_ref(tex_file, defs->scrap);
          putc('}', tex_file);
          page = -2;
          defs = defs->next;
        }
        while (uses || defs) {
          if (uses && (!defs || uses->scrap < defs->scrap)) {
            write_scrap_ref(tex_file, uses->scrap, FALSE, &page);
            uses = uses->next;
          }
          else {
            if (uses && defs->scrap == uses->scrap)
              uses = uses->next;
            fputs(", \\underline{", tex_file);
            write_single_scrap_ref(tex_file, defs->scrap);
            putc('}', tex_file);
            page = -2;
            defs = defs->next;
          }
        }
        fputs(".\n", tex_file);
      }
    }
    name = name->rlink;
  }
}
