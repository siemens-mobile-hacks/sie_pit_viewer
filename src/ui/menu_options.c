#include <string.h>
#include <swilib.h>

#define MAX_ITEMS 1

static HEADER_DESC HEADER_D = {{0, 0, 0, 0}, NULL, (int)"Options", LGP_NULL};

static const MENUITEM_DESC MENU_ITEMS[MAX_ITEMS] = {
    {NULL, (int)"Go to...", LGP_NULL, 0, NULL,
        MENU_FLAG3, MENU_FLAG2},
};

void GoTo_Proc(GUI *gui) {
    ShowMSG(1, (int)"Go to...");
}

static const MENUPROCS_DESC MENU_PROCS[MAX_ITEMS] = {
    GoTo_Proc,
};

static const int SOFTKEYS[] = {1, 0};

static const SOFTKEY_DESC SOFTKEY_D[] = {
    {0x0018, 0x0000, (int)"Select"},
    {0x0000, 0x0000, (int)""},
};

static const SOFTKEYSTAB SOFTKEYS_TAB = {
    SOFTKEY_D, 2,
};

static const MENU_DESC MENU_D = {
    8,
    NULL,
    NULL,
    NULL,
    SOFTKEYS,
    &SOFTKEYS_TAB,
    MENU_FLAGS_ENABLE_TEXT_SCROLLING,
    NULL,
    MENU_ITEMS,
    MENU_PROCS,
    MAX_ITEMS,
};

int CreateOptionsMenu() {
    memcpy(&(HEADER_D.rc), GetOptionsHeaderRect(), sizeof(RECT));
    return CreateMenu(1, 0, &MENU_D, &HEADER_D, 0, 1, NULL, NULL);
}
