#include <swilib.h>
#include <stdio.h>
#include <string.h>
#include <sys/param.h>
#include <sie/sie.h>
#include "ui.h"
#include "menu_options.h"

static HEADER_DESC HEADER_D = {{0, 0, 0, 0}, NULL, LGP_NULL - 1, LGP_NULL - 1};

const int SOFTKEYS[] = {SET_LEFT_SOFTKEY, SET_MIDDLE_SOFTKEY, SET_RIGHT_SOFTKEY };

static SOFTKEY_DESC SOFTKEY_D[] = {
    {0x0018, 0x0000, (int)"PIT"},
    {0x0018, 0x0000, (int)"UCS2"},
    {0x0018, 0x0000, (int)"LIT"},
    {0x003D, 0x0000, (int)LGP_MENU_PIC},
    {0x0001, 0x0000, (int)"Exit"},
};

static SOFTKEYSTAB SOFTKEYS_TAB = {
    SOFTKEY_D, 0
};

static void OnRedraw(WIDGET *widget) {
    void *gui = GetTopGUI();
    UI_DATA *data = TViewGetUserPointer(gui);

    int icon = -1;
    IMGHDR *img = NULL;
    if (data->p_table == PIT) {
        icon = data->pit_id;
    } else if (data->p_table == UCS2) {
        icon = GetPicNByUnicodeSymbol(UCS2_ICON_0 + data->ucs2_id);
    } else {
        icon = GetLgpIcon(LGP_ICON_0 + data->lit_id);
    }
    if (icon >= 0) {
        img = GetPITaddr(icon);
    }
    if (img) {
        WSHDR ws;
        uint16_t wsbody[16];
        CreateLocalWS(&ws, wsbody, 16);

        RECT *header_rect = GetHeaderRECT();
        RECT *main_area_rect = GetMainAreaRECT();
        int x = ((main_area_rect->x2 - main_area_rect->x) - img->w) / 2;
        int x2 = 0;
        int y = header_rect->y2 + ((main_area_rect->y2 - main_area_rect->y) - img->h) / 2;
        int y2 = 0;

        DrawRectangle(x - 1, y - 1, x + img->w, y + img->h, RECT_DOT_OUTLINE,
                   GetPaletteAdrByColorIndex(PC_FOREGROUND), GetPaletteAdrByColorIndex(0x17));
        DrawIMGHDR(x, y, img);

        x = main_area_rect->x;
        y = main_area_rect->y + 1;
        x2 = main_area_rect->x2;
        y2 = y + GetFontYSIZE(FONT_SMALL);
        wsprintf(&ws, "ID: %d", icon);
        wstrcatprintf(&ws, "%c%dx%d%c", UTF16_ALIGN_RIGHT, img->w, img->h, UTF16_ALIGN_NONE);
        DrawString(&ws, x, y, x2, y2, FONT_SMALL, TEXT_OUTLINE,
                   GetPaletteAdrByColorIndex(PC_FOREGROUND), GetPaletteAdrByColorIndex(PC_BACKGROUND));
    }
}

void FindPNGFiles(GUI *gui) {
    UI_DATA *data = TViewGetUserPointer(gui);

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

void PIT_FindLastID(GUI *gui) {
    UI_DATA *data = TViewGetUserPointer(gui);
    if (!data->files) {
        FindPNGFiles(gui);
    }

    int id = 0;
    SIE_FILE *p = data->files;
    while (p) {
        id = MAX(id, strtol(p->file_name, NULL, 10));
        p = p->next;
    }
    data->pit_id = MAX(id, GetPITSize() - 1);
}

void PIT_FindNextID(GUI *gui) {
    UI_DATA *data = TViewGetUserPointer(gui);
    if (!data->files) {
        FindPNGFiles(gui);
    }

    int new_id = PIT_MAX;
    SIE_FILE *p = data->files;
    while (p) {
        int f_id = strtol(p->file_name, NULL, 10);
        if (f_id >= GetPITSize() && f_id > data->pit_id) {
            new_id = MIN(new_id, f_id);
        }
        p = p->next;
    }
    data->pit_id = (new_id == PIT_MAX) ? 0 : new_id;
}

void PIT_FindPrevID(GUI *gui) {
    UI_DATA *data = TViewGetUserPointer(gui);
    if (!data->files) {
        FindPNGFiles(gui);
    }

    int new_id = GetPITSize() - 1;
    SIE_FILE *p = data->files;
    while (p) {
        int f_id = strtol(p->file_name, NULL, 10);
        if (GetPITSize() - 1 < f_id && f_id < data->pit_id) {
            new_id = MAX(new_id, f_id);
        }
        p = p->next;
    }
    data->pit_id = new_id;
}

static int OnKey(GUI *gui, GUI_MSG *msg) {
    UI_DATA *data = TViewGetUserPointer(gui);

    if (msg->keys == 0x01) {
        return 1;
    } else if (msg->keys == 0x18) {
        data->p_table++;
        if (data->p_table > 2) {
            data->p_table = 0;
        }
        DirectRedrawGUI();
    } else if (msg->keys == 0x3D) {
        CreateMenu_Options(gui);
    }
    else if (msg->gbsmsg->msg == KEY_DOWN  || msg->gbsmsg->msg == LONG_PRESS) {
        int step = (msg->gbsmsg->msg == KEY_DOWN) ? 1 : 10;
        switch (msg->gbsmsg->submess) {
            case LEFT_BUTTON: case UP_BUTTON:
                if (data->p_table == PIT) {
                    data->pit_id -= step;
                    if (data->pit_id < 0) {
                        PIT_FindLastID(gui);
                    } else if (!GetPITaddr(data->pit_id)) {
                        PIT_FindPrevID(gui);
                    }
                } else if (data->p_table == UCS2) {
                    data->ucs2_id -= step;
                    if (data->ucs2_id < 0) {
                        data->ucs2_id = UCS2_TOTAL_COUNT - 1;
                    }
                } else if (data->p_table == LIT) {
                    data->lit_id -= step;
                    if (data->lit_id < 0) {
                        data->lit_id = LIT_TOTAL_COUNT - 1;
                    }
                }
                DirectRedrawGUI();
                break;
            case RIGHT_BUTTON: case DOWN_BUTTON:
                if (data->p_table == PIT) {
                    data->pit_id += step;
                    if (!GetPITaddr(data->pit_id)) {
                        PIT_FindNextID(gui);
                    }
                } else if (data->p_table == UCS2) {
                    data->ucs2_id += step;
                    if (data->ucs2_id >= UCS2_TOTAL_COUNT) {
                        data->ucs2_id = 0;
                    }
                } else {
                    data->lit_id += step;
                    if (data->lit_id >= LIT_TOTAL_COUNT) {
                        data->lit_id = 0;
                    }
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

void SetUIHeader(GUI *gui) {
    UI_DATA *data = TViewGetUserPointer(gui);
    WSHDR *ws_header = AllocWS(32);
    WSHDR *ws_extra_header = AllocWS(8);
    if (data->p_table == PIT) {
        wsprintf(ws_header, "PIT: 0x%X", data->pit_id);
        wsprintf(ws_extra_header, "%d", data->pit_id);
    } else if (data->p_table == UCS2) {
        wsprintf(ws_header, "UCS2: 0x%X", data->ucs2_id + UCS2_ICON_0);
        wsprintf(ws_extra_header, "%d", data->ucs2_id);
    } else if (data->p_table == LIT) {
        wsprintf(ws_header, "LIT: 0x%X", data->lit_id + LGP_ICON_0);
        wsprintf(ws_extra_header, "%d", data->lit_id);
    }
    void *ma = malloc_adr();
    void *mf = mfree_adr();
    void *header = GetHeaderPointer(gui);
    SetHeaderText(header, ws_header, ma, mf);
    SetHeaderExtraText(header, ws_extra_header, ma, mf);
}

static void GHook(GUI *gui, int cmd) {
    UI_DATA *data = TViewGetUserPointer(gui);

    if (cmd == UI_CMD_REDRAW) {
        const int lsk_id = (data->p_table < 2) ? data->p_table + 1 : 0;
        SetUIHeader(gui);
        SetMenuSoftKey(gui, &SOFTKEY_D[lsk_id], SET_LEFT_SOFTKEY);
        SetMenuSoftKey(gui, &SOFTKEY_D[3], SET_MIDDLE_SOFTKEY);
#ifdef NEWSGOLD
        SetMenuSoftKey(gui, &SOFTKEY_D[4], SET_RIGHT_SOFTKEY);
#endif
    }
    else if (cmd == UI_CMD_CREATE) {
        static GUI_METHODS gui_methods;
        WIDGET *m = GetDataOfItemByID(gui, 4);
        memcpy(&gui_methods, m->methods, sizeof(GUI_METHODS));
        gui_methods.onRedraw = OnRedraw;
        m->methods = &gui_methods;
    } else if (cmd == UI_CMD_DESTROY) {
        if (data->files) {
            Sie_FS_DestroyFiles(data->files);
        }
    } else if (cmd == UI_CMD_FOCUS) {
        DisableIDLETMR();
    }
}

static TVIEW_DESC TVIEW_D = {
    8,
    OnKey,
    GHook,
    NULL,
    SOFTKEYS,
    &SOFTKEYS_TAB,
    {0, 0, 0, 0},
    FONT_SMALL,
    0x64,
    0x65,
    0,
    0,
};

int CreateUI() {
    memcpy(&(HEADER_D.rc), GetHeaderRECT(), sizeof(RECT));
    memcpy(&(TVIEW_D.rc), GetMainAreaRECT(), sizeof(RECT));

    UI_DATA *data = malloc(sizeof(UI_DATA));
    zeromem(data, sizeof(UI_DATA));

    void *mfree = mfree_adr();
    void *malloc = malloc_adr();

    WSHDR *ws = AllocWS(1);
    void *gui = TViewGetGUI(malloc, mfree);
    TViewSetDefinition(gui, &TVIEW_D);

    SetHeader(gui, &HEADER_D, malloc);
    TViewSetText(gui, ws, malloc, mfree);
    TViewSetUserPointer(gui, data);

    return CreateGUI(gui);
}
