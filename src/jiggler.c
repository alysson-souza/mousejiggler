/*
 * MouseJiggler - jiggler.c
 * Core mouse jiggling functionality using Win32 SendInput API
 */

#include "jiggler.h"
#include <windows.h>

static DWORD last_error = 0;

void Jiggler_Initialize(void)
{
    /* Initialize jiggler module - currently nothing to do */
    last_error = 0;
}

bool Jiggler_Jiggle(int delta)
{
    INPUT inp;
    UINT result;

    /* Clear previous error */
    SetLastError(0);

    /* Initialize INPUT structure for mouse movement */
    ZeroMemory(&inp, sizeof(INPUT));
    inp.type = INPUT_MOUSE;
    inp.mi.dx = delta;
    inp.mi.dy = delta;
    inp.mi.mouseData = 0;
    inp.mi.dwFlags = MOUSEEVENTF_MOVE;
    inp.mi.time = 0;
    inp.mi.dwExtraInfo = 0;

    /* Send the input event */
    result = SendInput(1, &inp, sizeof(INPUT));

    if (result == 1) {
        last_error = 0;
        return true;
    }

    /* Save error for debugging */
    last_error = GetLastError();
    return false;
}

DWORD Jiggler_GetLastError(void)
{
    return last_error;
}
