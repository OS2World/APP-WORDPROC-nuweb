#include "global.h"
int tex_flag = TRUE;
int output_flag = TRUE;
int compare_flag = TRUE;
int verbose_flag = FALSE;
int number_flag = FALSE;
char *command_name = NULL;
char *source_name = NULL;
int source_line = 0;
int already_warned = 0;
Name *file_names = NULL;
Name *macro_names = NULL;
Name *user_names = NULL;

