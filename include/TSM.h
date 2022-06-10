#ifndef DATA_TYPE_H_
#define DATA_TYPE_H_

#include "Rte_Type.h"

typedef signed char sint8;
typedef unsigned char uint8;
typedef short sint16;
typedef unsigned short uint16;
typedef int sint32;
typedef unsigned int uint32;
typedef long long sint64;
typedef unsigned long long uint64;
typedef float float32;
typedef double float64;

typedef unsigned char boolean;
// typedef unsigned long size_t;

#ifndef NULL_PTR
#define NULL_PTR ((void*)0)
#endif

#ifndef true
#define true 1U
#endif

#ifndef false
#define false 0U
#endif

extern void MRM_TSM_MODULE_Init(void);
extern void MRM_Swc1_TSM(const Dt_RECORD_CtrlArb2TSM *rtu_DeCtrlArb2TSM, 
    const Dt_RECORD_CANGATE2TSM *rtu_DeCANGATE2TSM, 
    const Dt_RECORD_Diag2TSM *rtu_DeDiag2TSM,
    const Dt_RECORD_PLANLITE2TSM *rtu_DePlanlite2Tsm,
    Dt_RECORD_TSM2PLANLITE *rty_DeTsm2Planlite,
    Dt_RECORD_TSM2CtrlArb *rty_DeTSM2CtrlArb, 
    Dt_RECORD_TSM2Diag *rty_DeTSM2Diag,
    Dt_RECORD_TSM2CANGATE *rty_DeTSM2CANGATE);
#endif