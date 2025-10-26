/*
 * MouseJiggler - mainwindow.h
 * Main window implementation
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "settings.h"
#include <windows.h>
#include <stdbool.h>

/* Main window handle (global for tray icon access) */
extern HWND g_hwndMain;

/* Initialize and create main window
 * Returns: HWND of the created window, NULL on failure
 */
HWND MainWindow_Create(bool start_jiggling, const Settings *settings);

/* Window procedure for main window */
LRESULT CALLBACK MainWindow_WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

/* Set the jiggling state (called from UI or timer)
 * Returns: true if successful
 */
bool MainWindow_SetJiggling(HWND hwnd, bool enable);

/* Get current settings from main window controls */
Settings MainWindow_GetSettings(HWND hwnd);

/* Update the tray icon tooltip text based on current state */
void MainWindow_UpdateTrayText(HWND hwnd);

/* Minimize window to tray */
void MainWindow_MinimizeToTray(HWND hwnd);

/* Restore window from tray */
void MainWindow_RestoreFromTray(HWND hwnd);

/* Check if window is currently jiggling */
bool MainWindow_IsJiggling(HWND hwnd);

/* Get current jiggle period from window */
int MainWindow_GetJigglePeriod(HWND hwnd);

#endif /* MAINWINDOW_H */
