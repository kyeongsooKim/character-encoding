#include "debug.h"
#include "utf.h"
#include "wrappers.h"
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


const char *STR_UTF16BE  = "UTF16BE";
char *const STR_UTF16LE = "UTF16LE";
char const *STR_UTF8  = "UTF8";



int opterr;
int optopt; // when error happen,the related argument is saved here
int optind; // next argument index after handling some argument
char *optarg;

state_t *program_state;

void
parse_args(int argc, char *argv[])
{
  int i;
  char option;
  char *joined_argv;
  optarg = NULL;

  joined_argv = join_string_array(argc, argv);
  info("argc: %d argv: %s", argc, joined_argv);
  free(joined_argv);

  program_state = Calloc(1, sizeof(state_t));

  bool eFlag = false;
  bool hFlag = false;


  if (argc == 1)
  {
    fprintf(stderr, KRED "No arguments\n" KNRM);
    USAGE(argv[0]);
    exit(EXIT_FAILURE);
  }

  for (i = 0; optind < argc; ++i)
  {
    debug("%d opterr: %d", i, opterr);
    debug("%d optind: %d", i, optind);
    debug("%d optopt: %d", i, optopt);
    debug("%d argv[optind]: %s", i, argv[optind]);

    if ((option = getopt(argc, argv, "+e:h")) != -1) {
      switch (option) {
        case 'h':
          USAGE(argv[0]);
          hFlag = true;
          break;
        case 'e':
          info("Encoding Argument: %s", optarg);
          eFlag = true;
          if ((program_state->encoding_to = determine_format(optarg)) == 0)
            print_state();

          if (strcmp(optarg,"UTF8") != 0 && strcmp(optarg,"UTF16LE") != 0 && strcmp(optarg,"UTF16BE") != 0)
          {
              fprintf(stderr, KRED "Invalid encoding type is given\n" KNRM);
              USAGE(argv[0]);
              exit(EXIT_FAILURE);
          }
          break;
        case '?':
          if (optopt == 'e')
          {
            fprintf(stderr, KRED "Encoding type must be given after -e\n" KNRM);
            USAGE(argv[0]);
          }
          else if (optopt != 'h')
          {
            fprintf(stderr, KRED "-%c is not a supported argument\n" KNRM, optopt);
            USAGE(argv[0]);
          }
          exit(EXIT_FAILURE);
      }
    }
    elsif(argv[optind] != NULL)
    {
      if (program_state->in_file == NULL) {
        program_state->in_file = argv[optind];

      }
      elsif(program_state->out_file == NULL)
      {
        program_state->out_file = argv[optind];
      }
      else{ // unnecessary arguments after two file names.
        fprintf(stderr, KRED "unnecessary arguments after input file and output file\n" KNRM);
        USAGE(argv[0]);
        exit(EXIT_FAILURE);
      }
      optind++;
    }

  } //for


  if (eFlag == false && hFlag == false)
  {
    //fprintf(stderr, KRED "E flag is not given\n" KNRM);
    USAGE(argv[0]);
    exit(EXIT_FAILURE);
  }
  else if (eFlag == false && hFlag == true)
  {
    //fprintf(stderr, KRED "E flag is not given\n" KNRM);
    exit(EXIT_FAILURE);
  }

  if (((program_state->in_file == NULL) || (program_state->out_file == NULL)) && hFlag == false)
  {
    //fprintf(stderr, KRED "File name is not given\n" KNRM);
    USAGE(argv[0]);
    exit(EXIT_FAILURE);
  }
  else if (((program_state->in_file == NULL) || (program_state->out_file == NULL)) && hFlag == true)
  {
    //fprintf(stderr, KRED "File name is not given\n" KNRM);
    exit(EXIT_FAILURE);
  }

}

format_t
determine_format(char *argument)
{
  if (strcmp(argument, STR_UTF16LE) == 0)
    return UTF16LE;
  if (strcmp(argument, STR_UTF16BE) == 0)
    return UTF16BE;
  if (strcmp(argument, STR_UTF8) == 0)
    return UTF8;
  return 0;
}

char*
bom_to_string(format_t bom){
  switch(bom){
    case UTF8: return (char*)STR_UTF8;
    case UTF16BE: return (char*)STR_UTF16BE;
    case UTF16LE: return (char*)STR_UTF16LE;
  }
  return "UNKNOWN";
}

char*
join_string_array(int count, char *array[])
{

  char *ret;
  int i;
  int len = 0, str_len, cur_str_len;
  str_len = array_size(count, array);
  ret = Malloc(sizeof(char)* (str_len));

  for (i = 0; i < count; ++i) {
    cur_str_len = strlen(array[i]);
    memcpy(ret + len, array[i], cur_str_len);
    len += cur_str_len;
    memcpy(ret + len, " ", 1);
    len += 1;
  };

  return ret;
}

int
array_size(int count, char *array[])
{
  int i, sum = 1; /* NULL terminator */
  for (i = 0; i < count; ++i) {

    sum += strlen(array[i]);
    ++sum; /* For the spaces */
  }
  return sum+1;
}

void
print_state()
{
  if (program_state == NULL) {
    error("program_state is %p", (void*)program_state);
    exit(EXIT_FAILURE);
  }
  info("program_state {\n"
         "  format_t encoding_to = 0x%X;\n"
         "  format_t encoding_from = 0x%X;\n"
         "  char *in_file = '%s';\n"
         "  char *out_file = '%s';\n"
         "};\n",
         program_state->encoding_to, program_state->encoding_from,
         program_state->in_file, program_state->out_file);
}
