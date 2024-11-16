#include <swilib.h>
#include <stdio.h>
#include <string.h>
#include <sys/param.h>
#include <sie/sie.h>
#include "ui.h"
#include "menu_options.h"

#define PIT_MAX 20000

static UI_DATA DATA = {};

static HEADER_DESC HEADER_D = {{0, 0, 0, 0}, NULL, LGP_NULL, LGP_NULL};

static SOFTKEY_DESC SOFTKEY_D[] = {
    {0x0018, 0x0000, (int)"Options"},
    {0x0018, 0x0000, (int)LGP_MENU_PIC},
};

static SOFTKEYSTAB SOFTKEYS_TAB = {
    SOFTKEY_D, 0
};

static void OnRedraw(GUI *gui) {
    RECT *rect = GetMainAreaRECT();
    int x = ((rect->x2 - rect->x) - GetImgWidth(DATA.id)) / 2;
    int y = HeaderH() + ((rect->y2 - rect->y) - GetImgHeight(DATA.id)) / 2;
    DrawImg(x, y, DATA.id);
}

SIE_FILE *FindPNGFiles() {
    SIE_FILE *files = NULL;
    char *path = "0:\\zbin\\img\\*.png";
    const unsigned char disks[] = { '0', '4', '1', '2' };
    for (int i = 0; i < 4; i++) {
        path[0] = disks[i];
        files = Sie_FS_FindFiles(path);
        if (files) {
            break;
        }
    }
    return files;
}

int FindLastID() {
    int id = 0;
    SIE_FILE *p = FindPNGFiles();
    while (p) {
        id = MAX(id, strtol(p->file_name, NULL, 10));
        p = p->next;
    }
    Sie_FS_DestroyFiles(p);
    return MAX(id, GetPITSize() - 1);
}

int FindNextID(int id) {
    int new_id = PIT_MAX;
    SIE_FILE *p = FindPNGFiles();
    while (p) {
        int f_id = strtol(p->file_name, NULL, 10);
        if (f_id >= GetPITSize() && f_id > id) {
            new_id = MIN(new_id, f_id);
        }
        p = p->next;
    }
    Sie_FS_DestroyFiles(p);
    return (new_id == PIT_MAX) ? 0 : new_id;
}

int FindPrevID(int id) {
    int new_id = GetPITSize() - 1;
    SIE_FILE *p = FindPNGFiles();
    while (p) {
        int f_id = strtol(p->file_name, NULL, 10);
        if (GetPITSize() - 1 < f_id && f_id < id) {
            new_id = MAX(new_id, f_id);
        }
        p = p->next;
    }
    Sie_FS_DestroyFiles(p);
    return new_id;
}

static int OnKey(GUI *gui, GUI_MSG *msg) {
    if (msg->keys == 0x01) {
        return 1;
    } else if (msg->keys == 0x18) {
        CreateMenu_Options(gui);
    }
    else if (msg->gbsmsg->msg == KEY_DOWN  || msg->gbsmsg->msg == LONG_PRESS) {
        int step = (msg->gbsmsg->msg == KEY_DOWN) ? 1 : 10;
        switch (msg->gbsmsg->submess) {
            case LEFT_BUTTON:
                DATA.id -= step;
                if (DATA.id < 0) {
                    DATA.id = FindLastID();
                } else if (!GetPITaddr(DATA.id)) {
                    DATA.id = FindPrevID(DATA.id);
                }
                DirectRedrawGUI();
                break;
            case RIGHT_BUTTON:
                DATA.id += step;
                if (!GetPITaddr(DATA.id)) {
                    DATA.id = FindNextID(DATA.id);
                }
                DirectRedrawGUI();
                break;
        }
    }
    return -1;
}

void SetHeader(GUI *gui) {
    WSHDR *ws = AllocWS(16);
    wsprintf(ws, "%dx%d", GetImgWidth(DATA.id), GetImgHeight(DATA.id));
    WSHDR *ws_extra = AllocWS(8);
    wsprintf(ws_extra, "%d", DATA.id);
    SetHeaderText(GetHeaderPointer(gui), ws, malloc_adr(), mfree_adr());
    SetHeaderExtraText(GetHeaderPointer(gui), ws_extra, malloc_adr(), mfree_adr());
}

static void GHook(GUI *gui, int cmd) {
    if (cmd == TI_CMD_REDRAW) {
        SetHeader(gui);
        SetSoftKey(gui, &SOFTKEY_D[0], 1);
        SetSoftKey(gui, &SOFTKEY_D[1], 2);
    }
    else if (cmd == TI_CMD_CREATE) {
        static GUI_METHODS gui_methods;
        void **m = GetDataOfItemByID(gui, 4);
        memcpy(&gui_methods, m[1], sizeof(GUI_METHODS));
        gui_methods.onRedraw = OnRedraw;
        m[1] = &gui_methods;
    } else if (cmd == TI_CMD_DESTROY) {
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

int CreateUI() {
    memcpy(&(HEADER_D.rc), GetHeaderRECT(), sizeof(RECT));
    memcpy(&(INPUTDIA_D.rc), GetMainAreaRECT(), sizeof(RECT));

    void *ma = malloc_adr();
    void *eq = AllocEQueue(ma, mfree_adr());

    EDITCONTROL ec;
    WSHDR *ws = AllocWS(8);
    PrepareEditControl(&ec);

    ConstructEditControl(&ec, ECT_HEADER, ECF_SET_CURSOR_END, ws, 8);
    AddEditControlToEditQend(eq, &ec, ma);
    FreeWS(ws);

    return CreateInputTextDialog(&INPUTDIA_D, &HEADER_D, eq, 1, &DATA);
}
