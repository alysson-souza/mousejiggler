/*
 * MouseJiggler - jiggler.h
 * Core mouse jiggling functionality
 */

#ifndef JIGGLER_H
#define JIGGLER_H

#include <windows.h>
#include <stdbool.h>

/* Initialize the jiggler module */
void Jiggler_Initialize(void);

/* Perform a mouse jiggle with the given delta
 * delta: pixels to move (positive zig, negative zag, 0 for zen mode)
 * Returns: true if successful, false on error
 */
bool Jiggler_Jiggle(int delta);

/* Get the last Win32 error from a jiggle operation */
DWORD Jiggler_GetLastError(void);

#endif /* JIGGLER_H */
