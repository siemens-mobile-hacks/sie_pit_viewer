#pragma once

#include <sie/sie.h>

#ifdef ELKA
    #define PIT_ICON_EMPTY 1336
#endif

typedef struct {
    int id;
    SIE_FILE *files;
} UI_DATA;

int CreateUI();
