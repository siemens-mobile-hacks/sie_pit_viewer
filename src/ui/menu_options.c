#include <swilib.h>
#include <string.h>
#include "ui.h"
#include "edit_go_to.h"

#define MAX_ITEMS 1

static HEADER_DESC HEADER_D = {{0, 0, 0, 0}, NULL, (int)"Options", LGP_NULL};

int ICON_EMPTY[] = {PIT_ICON_EMPTY};

static const MENUITEM_DESC MENU_ITEMS[MAX_ITEMS] = {
    {ICON_EMPTY, (int)"Go to...", LGP_NULL, 0, NULL,MENU_FLAG3, MENU_FLAG2},
};

void GoTo_Proc(GUI *gui) {
    GUI *main_gui = MenuGetUserPointer(gui);

    GeneralFuncF1(1);
    CreateInputTextDialog_GoTo(main_gui);
}

static const MENUPROCS_DESC MENU_PROCS[MAX_ITEMS] = {
    GoTo_Proc,
};

static const int SOFTKEYS[] = {
#ifdef NEWSGOLD
    SET_LEFT_SOFTKEY, SET_MIDDLE_SOFTKEY, SET_RIGHT_SOFTKEY
#else
    SET_LEFT_SOFTKEY, SET_RIGHT_SOFTKEY
#endif
};

static const SOFTKEY_DESC SOFTKEY_D[] = {
    {0x0018, 0x0000, (int)"Select"},
#ifdef NEWSGOLD
    {0x0000, 0x0000, (int)LGP_DOIT_PIC},
#else
    {0x0000, 0x0000, (int)""},
#endif
    {0x0001, 0x0001, (int)"Back"},
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
    MENU_FLAGS_ENABLE_TEXT_SCROLLING | MENU_FLAGS_ENABLE_ICONS,
    NULL,
    MENU_ITEMS,
    MENU_PROCS,
    MAX_ITEMS,
};

int CreateMenu_Options(GUI *main_gui) {
    memcpy(&(HEADER_D.rc), GetOptionsHeaderRect(), sizeof(RECT));
    return CreateMenu(1, 0, &MENU_D, &HEADER_D, 0, 1, main_gui, NULL);
}
