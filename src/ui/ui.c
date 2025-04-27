#include <swilib.h>
#include <stdio.h>
#include <string.h>
#include <sys/param.h>
#include <sie/sie.h>
#include "ui.h"
#include "menu_options.h"

#define PIT_MAX 20000

static HEADER_DESC HEADER_D = {{0, 0, 0, 0}, NULL, LGP_NULL, LGP_NULL};

static SOFTKEY_DESC SOFTKEY_D[] = {
    {0x0018, 0x0000, (int)"Options"},
    {0x0018, 0x0000, (int)LGP_MENU_PIC},
    {0x0001, 0x0000, (int)"Exit"},
};

static SOFTKEYSTAB SOFTKEYS_TAB = {
    SOFTKEY_D, 0
};

static UI_DATA *DATA;

static void OnRedraw(GUI *gui) {
    RECT *header_rect = GetHeaderRECT();
    RECT *main_area_rect = GetMainAreaRECT();
    int x = ((main_area_rect->x2 - main_area_rect->x) - GetImgWidth(DATA->id)) / 2;
    int y = header_rect->y2 + ((main_area_rect->y2 - main_area_rect->y) - GetImgHeight(DATA->id)) / 2;
    DrawImg(x, y, DATA->id);
}

void FindPNGFiles(GUI *gui) {
    UI_DATA *data = EDIT_GetUserPointer(gui);

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
    data->files = files;
}

void FindLastID(GUI *gui) {
    UI_DATA *data = EDIT_GetUserPointer(gui);
    if (!data->files) {
        FindPNGFiles(gui);
    }

    int id = 0;
    SIE_FILE *p = data->files;
    while (p) {
        id = MAX(id, strtol(p->file_name, NULL, 10));
        p = p->next;
    }
    data->id = MAX(id, GetPITSize() - 1);
}

void FindNextID(GUI *gui) {
    UI_DATA *data = EDIT_GetUserPointer(gui);
    if (!data->files) {
        FindPNGFiles(gui);
    }

    int new_id = PIT_MAX;
    SIE_FILE *p = data->files;
    while (p) {
        int f_id = strtol(p->file_name, NULL, 10);
        if (f_id >= GetPITSize() && f_id > data->id) {
            new_id = MIN(new_id, f_id);
        }
        p = p->next;
    }
    data->id = (new_id == PIT_MAX) ? 0 : new_id;
}

void FindPrevID(GUI *gui) {
    UI_DATA *data = EDIT_GetUserPointer(gui);
    if (!data->files) {
        FindPNGFiles(gui);
    }

    int new_id = GetPITSize() - 1;
    SIE_FILE *p = data->files;
    while (p) {
        int f_id = strtol(p->file_name, NULL, 10);
        if (GetPITSize() - 1 < f_id && f_id < data->id) {
            new_id = MAX(new_id, f_id);
        }
        p = p->next;
    }
    data->id = new_id;
}

static int OnKey(GUI *gui, GUI_MSG *msg) {
    if (msg->keys == 0x01) {
        return 1;
    } else if (msg->keys == 0x18) {
        CreateMenu_Options(gui);
    }
    else if (msg->gbsmsg->msg == KEY_DOWN  || msg->gbsmsg->msg == LONG_PRESS) {
        UI_DATA *data = EDIT_GetUserPointer(gui);

        int step = (msg->gbsmsg->msg == KEY_DOWN) ? 1 : 10;
        switch (msg->gbsmsg->submess) {
            case LEFT_BUTTON:
                data->id -= step;
                if (data->id < 0) {
                    FindLastID(gui);
                } else if (!GetPITaddr(data->id)) {
                    FindPrevID(gui);
                }
                DirectRedrawGUI();
                break;
            case RIGHT_BUTTON:
                data->id += step;
                if (!GetPITaddr(data->id)) {
                    FindNextID(gui);
                }
                DirectRedrawGUI();
                break;
            case '1':
                PIT_ClearCache();
                DirectRedrawGUI();
            break;
        }
    }
    return -1;
}

void SetHeader(GUI *gui) {
    UI_DATA *data = EDIT_GetUserPointer(gui);

    WSHDR *ws = AllocWS(16);
    wsprintf(ws, "%dx%d", GetImgWidth(data->id), GetImgHeight(data->id));
    WSHDR *ws_extra = AllocWS(8);
    wsprintf(ws_extra, "%d", data->id);
    SetHeaderText(GetHeaderPointer(gui), ws, malloc_adr(), mfree_adr());
    SetHeaderExtraText(GetHeaderPointer(gui), ws_extra, malloc_adr(), mfree_adr());
}

static void GHook(GUI *gui, int cmd) {
    UI_DATA *data = EDIT_GetUserPointer(gui);

    if (cmd == TI_CMD_REDRAW) {
        SetHeader(gui);
        SetSoftKey(gui, &SOFTKEY_D[0], SET_LEFT_SOFTKEY);
        SetSoftKey(gui, &SOFTKEY_D[1], SET_MIDDLE_SOFTKEY);
#ifdef NEWSGOLD
        SetSoftKey(gui, &SOFTKEY_D[2], SET_RIGHT_SOFTKEY);
#endif
    }
    else if (cmd == TI_CMD_CREATE) {
        static GUI_METHODS gui_methods;
        void **m = GetDataOfItemByID(gui, 4);
        memcpy(&gui_methods, m[1], sizeof(GUI_METHODS));
        gui_methods.onRedraw = OnRedraw;
        m[1] = &gui_methods;
        DATA = data; // теряется указатель где-то блядь, в OnRedraw не найти :-(
    } else if (cmd == TI_CMD_DESTROY) {
        if (data->files) {
            Sie_FS_DestroyFiles(data->files);
        }
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

    UI_DATA *data = malloc(sizeof(UI_DATA));
    zeromem(data, sizeof(UI_DATA));
    return CreateInputTextDialog(&INPUTDIA_D, &HEADER_D, eq, 1, data);
}
