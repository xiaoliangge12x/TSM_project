#ifndef HSM_DATA_H_
#define HSM_DATA_H_

#include <stdbool.h>
#include <stdint.h>

static const uint8_t MAX_HSM_DEPTH = 16U;
enum {MAX_HSM_STATES = 20U};

typedef enum HsmRet (*dispatch)(uint8_t event);
// ------------------------------- HSM枚举 ----------------
enum Event {
    HSM_NO_SIG,
    HSM_ENTRY,
    HSM_EXIT,
    HSM_INITIAL,
    IHBC_USER_START = 10,
};

enum State {
    ROOT,     // topmost state
};

enum HsmRet {
    HSM_IGNORED,
    HSM_HANDLED,
    HSM_TRAN,
    HSM_SUPER,
};

// ------------------------------- HSM结构体 ---------------
typedef struct {
    uint8_t currentSt;   // 当前状态机状态
} Hsm;

#endif