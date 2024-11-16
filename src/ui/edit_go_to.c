#include <stdlib.h>
#include <swilib.h>
#include <string.h>
#include "ui.h"

static HEADER_DESC HEADER_D = {{0, 0, 0, 0}, NULL, (int)"Go to...", LGP_NULL};

static const SOFTKEY_DESC SOFTKEY_D[] = {
    {0x0018, 0x0000, (int)"Go to"},
    {0x0000, 0x0000, (int)""},
};

static const SOFTKEYSTAB SOFTKEYS_TAB = {
    SOFTKEY_D, 0,
};

int OnKey(GUI *gui, GUI_MSG *msg) {
    GUI *main_gui = EDIT_GetUserPointer(gui);
    UI_DATA *data = EDIT_GetUserPointer(main_gui);

    if (msg->keys == 0x18) {
        EDITCONTROL ec;
        ExtractEditControl(gui, 1, &ec);
        char number[8];
        ws_2str(ec.pWS, number, 7);
        int id = strtoul(number, NULL, 10);
        if (GetPITaddr(id)) {
            data->id = id;
            return 1;
        } else {
            MsgBoxError(1, (int)"Not found!");
        }
    }
    return 0;
}

void GHook(GUI *gui, int cmd) {
    if (cmd == TI_CMD_REDRAW) {
        SetSoftKey(gui, &SOFTKEY_D[0], SET_LEFT_SOFTKEY);
    }
}

static INPUTDIA_DESC INPUTDIA_D = {
    1,
    OnKey,
    GHook,
    NULL,
    0,
    &SOFTKEYS_TAB,
    {0, 0, 0, 0},
    FONT_SMALL,
    100,
    101,
    0,
    { 0 },
    { INPUTDIA_FLAGS_SWAP_SOFTKEYS },
};

int CreateInputTextDialog_GoTo(GUI *main_gui) {
    memcpy(&(HEADER_D.rc), GetHeaderRECT(), sizeof(RECT));
    memcpy(&(INPUTDIA_D.rc), GetMainAreaRECT(), sizeof(RECT));

    void *ma = malloc_adr();
    void *eq = AllocEQueue(ma, mfree_adr());

    EDITCONTROL ec;
    WSHDR *ws = AllocWS(127);
    PrepareEditControl(&ec);

    ConstructEditControl(&ec, ECT_NUMBER_TYPING, ECF_SET_CURSOR_END, ws, 4);
    AddEditControlToEditQend(eq, &ec, ma);
    FreeWS(ws);

    return CreateInputTextDialog(&INPUTDIA_D, &HEADER_D, eq, 1, main_gui);
}
