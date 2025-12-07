#pragma once

#include <sie/sie.h>

#ifdef NEWSGOLD
    #ifdef ELKA
        #define PIT_ICON_EMPTY 1336
    #else
        #define PIT_ICON_EMPTY 1380
    #endif
#else
    #define PIT_ICON_EMPTY 226
#endif

#define TABLE_PIT  0x00
#define TABLE_UCS2 0x01

typedef struct {
    int p_table;
    int pit_id;
    int ucs2_id;
    SIE_FILE *files;
} UI_DATA;

int CreateUI();
