#pragma once

#include <sie/sie.h>

#ifdef NEWSGOLD
    #define PIT_MAX          20000
    #define UCS2_ICON_0      0xE100
    #define UCS2_TOTAL_COUNT 0xD1
    #define LGP_ICON_0       0x7FFFC0D3
    #define LIT_TOTAL_COUNT  0x53
    #ifdef ELKA
        #define MENU_ICON_EMPTY 1336
    #else
        #define MENU_ICON_EMPTY 1380
    #endif
#else
    #define MENU_ICON_EMPTY 226
#endif

enum IconsTable {
    PIT,
    UCS2,
    LIT,
};

typedef struct {
    int p_table;
    int pit_id;
    int ucs2_id;
    int lit_id;
    SIE_FILE *files;
} UI_DATA;

int CreateUI();
