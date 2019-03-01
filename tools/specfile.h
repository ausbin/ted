// crafted with love by Austin Adams, 2k17

#include <stdio.h>

#define SPEC_LEN 8

typedef struct {
    int nonempty_cells;
    int size;
    int capacity;
    char **elements;
} spec_collection;

spec_collection *spec_collection_new();
int spec_collection_load(spec_collection *sc, FILE *fp);
int spec_collection_lookup(spec_collection *sc, char *prefix, char c);
char *spec_collection_get(spec_collection *sc, unsigned int i);
int spec_collection_nonempty_cells(spec_collection *sc);
