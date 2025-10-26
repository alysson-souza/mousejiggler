/*
 * MouseJiggler - trayicon.h
 * System tray icon management
 */

#ifndef TRAYICON_H
#define TRAYICON_H

#include <windows.h>
#include <stdbool.h>

/* Initialize tray icon for the given window
 * Returns: true if successful
 */
bool TrayIcon_Add(HWND hwnd);

/* Update tray icon tooltip text */
void TrayIcon_UpdateText(HWND hwnd, const wchar_t *text);

/* Remove tray icon */
void TrayIcon_Remove(HWND hwnd);

/* Handle tray icon context menu message
 * Returns: LRESULT from TrackPopupMenuEx
 */
LRESULT TrayIcon_HandleContextMenu(HWND hwnd, LPARAM lParam);

/* Show the tray context menu at the given position
 * Returns: the command selected from the menu
 */
int TrayIcon_ShowContextMenu(HWND hwnd, const POINT pt);

#endif /* TRAYICON_H */
