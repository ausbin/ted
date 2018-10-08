// crafted with love by Austin Adams, 2k17

#include <stdlib.h>
#include <string.h>
#include "specfile.h"

spec_collection *spec_collection_new() {
    spec_collection *sc = malloc(sizeof (spec_collection));

    if (sc == NULL) {
        return NULL;
    }

    sc->nonempty_cells = 0;
    sc->size = 0;
    sc->capacity = SPEC_LEN;
    sc->elements = malloc(sc->capacity * sizeof (char *));

    if (sc->elements == NULL) {
        return NULL;
    }

    return sc;
}

void spec_collection_free(spec_collection *sc) {
    for (int i = 0; i < sc->size; i++) {
        if (sc->elements[i] != NULL) {
            free(sc->elements[i]);
        }
    }
    free(sc->elements);
    free(sc);
}

int spec_collection_push(spec_collection *sc, char *tile_name) {
    if (sc->size == sc->capacity) {
        sc->capacity *= 2;
        sc->elements = realloc(sc->elements, sc->capacity * sizeof (char *));

        if (sc->elements == NULL) {
            return 1;
        }
    }

    sc->elements[sc->size++] = tile_name;
    return 0;
}

int spec_collection_load(spec_collection *sc, FILE *fp) {
    char *line;
    size_t n;
    ssize_t len;

    // getline() dereferences pointers passed in(!), so these need to be
    // initialized
    line = NULL;
    n = 0;

    while ((len = getline(&line, &n, fp)) != -1) {
        char *tile_name;

        if (line[0] == '.') {
            tile_name = NULL;
        } else {
            // Found a nonempty cell
            sc->nonempty_cells++;

            // Exclude the newline
            size_t line_size = strlen(line) - 1;
            // Include a spot for the null terminator
            tile_name = malloc(line_size + 1);

            // XXX Don't leak previously-allocated tile names
            if (tile_name == NULL) {
                return 1;
            }

            memcpy(tile_name, line, line_size);
            tile_name[line_size] = '\0';
        }

        if (spec_collection_push(sc, tile_name)) {
            return 1;
        }
    }

    return 0;
}

int spec_collection_lookup(spec_collection *sc, char *prefix, char c) {
    int prefix_len = strlen(prefix);

    int empty_cells_found = 0;
    for (int i = 0; i < sc->size; i++) {
        if (sc->elements[i] == NULL) {
            empty_cells_found++;
        } else if (!strncmp(prefix, sc->elements[i], prefix_len) &&
                sc->elements[i][prefix_len] == c) {
            return i - empty_cells_found;
        }
    }

    return -1;
}

char *spec_collection_get(spec_collection *sc, unsigned int i) {
    if (i >= sc->size) {
        return NULL;
    } else {
        return sc->elements[i];
    }
}

int spec_collection_nonempty_cells(spec_collection *sc) {
    return sc->nonempty_cells;
}
