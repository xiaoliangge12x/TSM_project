#include "ihbc/ihbc_hsm.h"

void workInDisable()
{
    LOG("Disable St.");
}

void workInFailure()
{
    LOG("Failure St.");
}

void workInEnable()
{
    LOG("Enable St.");
}

void workInPassive()
{
    LOG("Passive St.");
}

void workInStandby()
{
    LOG("Standby St.");
}

void workInActive()
{
    LOG("Active St.");
}

void workInLowBeamRunning()
{
    LOG("LowBeam Running St.");
}

void workInHighBeamRuning()
{
    LOG("HighBeam Running St.");
}