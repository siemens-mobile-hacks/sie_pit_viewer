#include <swilib.h>
#include <stdio.h>
#include <string.h>
#include <sys/param.h>
#include <sie/sie.h>

#define PIT_MAX 20000

typedef struct {
    int id;
} UI_DATA;

static UI_DATA DATA = {};

static HEADER_DESC HEADER_D = {{0, 0, 0, 0}, NULL, LGP_NULL, LGP_NULL};

static SOFTKEY_DESC SK[] = {
    {0x0018, 0x0000, (int)"Options"},
    {0x003D, 0x0000, (int)LGP_NULL},
};
static const int SOFTKEYS[] = {1, 0};

static SOFTKEYSTAB SKT = {
    SK, 2
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
    WSHDR *ws = AllocWS(64);
    wsprintf(ws, "PIT: %d", DATA.id);
    SetHeaderText(GetHeaderPointer(gui), ws, malloc_adr(), mfree_adr());
}

static void GHook(GUI *gui, int cmd) {
    if (cmd == TI_CMD_REDRAW) {
        SetHeader(gui);
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

static TVIEW_DESC TVIEW_D = {
    0,
    OnKey,
    GHook,
    NULL,
    SOFTKEYS,
    &SKT,
    {0, 0, 0, 0},
    FONT_SMALL,
    0x64,
    0x65,
    0,
    0
};

int CreateMainGUI() {
    memcpy(&(HEADER_D.rc), GetHeaderRECT(), sizeof(RECT));
    memcpy(&(TVIEW_D.rc), GetMainAreaRECT(), sizeof(RECT));

    void *gui = TViewGetGUI(malloc_adr(), mfree_adr());
    TViewSetDefinition(gui, &TVIEW_D);
    TViewSetText(gui, AllocWS(1), malloc_adr(), mfree_adr());
    SetHeaderToMenu(gui, &HEADER_D, malloc_adr());
    return CreateGUI(gui);
}
