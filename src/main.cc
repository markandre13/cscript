#include "lex.hh"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

static bool trace = true;

int
main(int argc, char **argv)
{
  int i;
  for(i=1; i<argc; ++i) {
    if (strcmp(argv[i], "--trace")==0)
      trace = true;
    else
      break;
  }
  if (i==argc) {
    fprintf(stderr, "no input files\n");
    exit(EXIT_FAILURE);
  }

  auto in = fopen(argv[i], "r");
  if (!in) {
    perror(argv[i]);
    exit(EXIT_FAILURE);
  }


  auto root = compile(in);
  if (root)
    node_print(stdout, root);
  else
    printf("empty file?\n");

  fclose(in);
  return EXIT_SUCCESS;
}
