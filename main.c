#include "global.h"
void main(argc, argv)
     int argc;
     char **argv;
{
  int arg = 1;
  command_name = argv[0];
  while (arg < argc) {
    char *s = argv[arg];
    if (*s++ == '-') {
      {
        char c = *s++;
        while (c) {
          switch (c) {
            case 'c': compare_flag = FALSE;
                      break;
            case 'n': number_flag = TRUE;
                      break;
            case 'o': output_flag = FALSE;
                      break;
            case 't': tex_flag = FALSE;
                      break;
            case 'v': verbose_flag = TRUE;
                      break;
            default:  fprintf(stderr, "%s: unexpected argument ignored.  ",
                              command_name);
                      fprintf(stderr, "Usage is: %s [-cnotv] file...\n",
                              command_name);
                      break;
          }
          c = *s++;
        }
      }
      arg++;
    }
    else break;
  }
  {
    if (arg >= argc) {
      fprintf(stderr, "%s: expected a file name.  ", command_name);
      fprintf(stderr, "Usage is: %s [-cnotv] file-name...\n", command_name);
      exit(-1);
    }
    do {
      {
        char source_name[100];
        char tex_name[100];
        char aux_name[100];
        {
          char *p = argv[arg];
          char *q = source_name;
          char *trim = q;
          char *dot = NULL;
          char c = *p++;
          while (c) {
            *q++ = c;
            if (c == '/') {
              trim = q;
              dot = NULL;
            }
            else if (c == '.')
              dot = q - 1;
            c = *p++;
          }
          *q = '\0';
          if (dot) {
            *dot = '\0';
            sprintf(tex_name, "%s.tex", trim);
            sprintf(aux_name, "%s.aux", trim);
            *dot = '.';
          }
          else {
            sprintf(tex_name, "%s.tex", trim);
            sprintf(aux_name, "%s.aux", trim);
            *q++ = '.';
            *q++ = 'w';
            *q = '\0';
          }
        }
        {
          pass1(source_name);
          if (tex_flag) {
            collect_numbers(aux_name);
            write_tex(source_name, tex_name);
          }
          if (output_flag)
            write_files(file_names);
          arena_free();
        }
      }
      arg++;
    } while (arg < argc);
  }
  exit(0);
}
