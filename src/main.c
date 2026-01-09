#include <swilib.h>
#include "ui/ui.h"

typedef struct {
    CSM_RAM csm;
    int gui_id;
} MAIN_CSM;

const int minus11 = -11;
unsigned short maincsm_name_body[140];

static void CSM_OnCreate(CSM_RAM *data) {
    MAIN_CSM *csm = (MAIN_CSM*)data;
    csm->gui_id = CreateUI();
}

static void CSM_OnClose(CSM_RAM *csm) {
    SUBPROC(kill_elf);
}

static int CSM_OnMessage(CSM_RAM *data, GBS_MSG *msg) {
    MAIN_CSM *csm = (MAIN_CSM*)data;
    if ((msg->msg == MSG_GUI_DESTROYED) && ((int)msg->data0 == csm->gui_id)) {
        csm->csm.state = CSM_STATE_CLOSED;
    }
    return 1;
}

static const struct {
    CSM_DESC maincsm;
    WSHDR maincsm_name;
} MAINCSM = {
        {
                CSM_OnMessage,
                CSM_OnCreate,
#ifdef NEWSGOLD
                0,
                0,
                0,
                0,
#endif
                CSM_OnClose,
                sizeof(MAIN_CSM),
                1,
                &minus11
        },
        {
                maincsm_name_body,
                NAMECSM_MAGIC1,
                NAMECSM_MAGIC2,
                0x0,
                139,
                0
        }
};

void UpdateCSMname(void) {
    wsprintf((WSHDR *)&MAINCSM.maincsm_name, "%t", "SiePITViewer");
}

int main() {
    MAIN_CSM main_csm;
    LockSched();
    UpdateCSMname();
    CreateCSM(&MAINCSM.maincsm, &main_csm, 0);
    UnlockSched();
    return 0;
}
