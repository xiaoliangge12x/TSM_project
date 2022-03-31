#include "ihbc/ihbc_hsm.h"

// 状态分发函数
HsmRet dispatchForDisable(const uint8_t event)
{
    switch (event) {
        case IHBC_WORK: {
            workInDisable();
            return HSM_HANDLED;
        }

        case IHBC_PASSIVE:
        case IHBC_STANDBY:
        case IHBC_LOW_BEAM:
        case IHBC_HIGH_BEAM: {
            g_hsm.currentSt = ENABLE;
            return HSM_TRAN;
        }

        case IHBC_FAILURE: {
            g_hsm.currentSt = FAILURE;
            return HSM_TRAN;
        }

        default: {
            g_hsm.currentSt = ROOT;
            return HSM_SUPER;
        }
    }
}

HsmRet dispatchForFailure(const uint8_t event)
{
    switch (event) {
        case IHBC_WORK: {
            workInFailure();    
            return HSM_HANDLED;         
        }

        case IHBC_DISABLE: {
            g_hsm.currentSt = DISABLE;
            return HSM_TRAN;
        }
        
        default: {
            g_hsm.currentSt = ROOT;
            return HSM_SUPER;
        }
    }
}

HsmRet dispatchForEnable(const uint8_t event)
{
    switch (event) {
        case IHBC_WORK: {
            workInEnable();    
            return HSM_HANDLED;         
        }

        case IHBC_DISABLE: {
            g_hsm.currentSt = DISABLE;
            return HSM_TRAN;
        }

        case IHBC_FAILURE: {
            g_hsm.currentSt = FAILURE;
            return HSM_TRAN;
        }

        case IHBC_PASSIVE: {
            g_hsm.currentSt = PASSIVE;
            return HSM_TRAN;
        }

        case IHBC_STANDBY:
        case IHBC_LOW_BEAM:
        case IHBC_HIGH_BEAM: {
            g_hsm.currentSt = STANDBY;
            return HSM_TRAN;
        }

        case HSM_INITIAL: {
            g_hsm.currentSt = PASSIVE;
            return HSM_TRAN;
        }

        default: {
            g_hsm.currentSt = ROOT;
            return HSM_SUPER;
        } 
    }
}

HsmRet dispatchForPassive(const uint8_t event)
{
    switch (event) {
        case IHBC_WORK: {
            workInPassive();    
            return HSM_HANDLED;         
        }

        default: {
            g_hsm.currentSt = ENABLE;
            return HSM_SUPER;
        } 
    }
}

HsmRet dispatchForStandby(const uint8_t event)
{
    switch (event) {
        case IHBC_WORK: {
            workInStandby();    
            return HSM_HANDLED;         
        }

        case IHBC_LOW_BEAM: {
            g_hsm.currentSt = LOWBEAM;
            return HSM_TRAN;
        }

        case IHBC_HIGH_BEAM: {
            g_hsm.currentSt = HIGHBEAM;
            return HSM_TRAN;
        }

        default: {
            g_hsm.currentSt = ENABLE;
            return HSM_SUPER;
        } 
    }
}

HsmRet dispatchForActive(const uint8_t event)
{
    switch (event) {
        case IHBC_WORK: {
            workInActive();    
            return HSM_HANDLED;         
        }
        
        case IHBC_LOW_BEAM: {
            g_hsm.currentSt = LOWBEAM;
            return HSM_TRAN;
        }

        case IHBC_HIGH_BEAM: {
            g_hsm.currentSt = HIGHBEAM;
            return HSM_TRAN;
        }

        case HSM_INITIAL: {
            g_hsm.currentSt = LOWBEAM;
            return HSM_TRAN;
        }

        default: {
            g_hsm.currentSt = ENABLE;
            return HSM_SUPER;
        } 
    }
}

HsmRet dispatchForLowBeamRunning(const uint8_t event)
{
    switch (event) {
        case IHBC_WORK: {
            workInLowBeamRunning();    
            return HSM_HANDLED;         
        }

        default: {
            g_hsm.currentSt = ACTIVE;
            return HSM_SUPER;
        } 
    }
}

HsmRet dispatchForHighBeamRunning(const uint8_t event)
{
    switch (event) {
        case IHBC_WORK: {
            workInHighBeamRuning();    
            return HSM_HANDLED;         
        }

        default: {
            g_hsm.currentSt = ACTIVE;
            return HSM_SUPER;
        } 
    }
}