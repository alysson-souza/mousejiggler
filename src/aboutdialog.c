/*
 * MouseJiggler - aboutdialog.c
 * About dialog implementation
 */

#include "aboutdialog.h"
#include "resource.h"
#include <windows.h>
#include <string.h>

void AboutDialog_Show(HWND hwnd)
{
    HWND hAboutDlg;
    HINSTANCE hInst = GetModuleHandleW(NULL);
    int screen_width, screen_height, dlg_width, dlg_height, dlg_x, dlg_y;
    int y_offset, x_offset, width;
    HWND hOkButton;
    MSG msg;

    screen_width = GetSystemMetrics(SM_CXSCREEN);
    screen_height = GetSystemMetrics(SM_CYSCREEN);
    dlg_width = 400;
    dlg_height = 300;
    dlg_x = (screen_width - dlg_width) / 2;
    dlg_y = (screen_height - dlg_height) / 2;

    /* Disable parent window */
    EnableWindow(hwnd, FALSE);

    /* Create dialog window */
    hAboutDlg = CreateWindowExW(
        WS_EX_DLGMODALFRAME,
        L"STATIC",
        L"About Mouse Jiggler",
        WS_POPUP | WS_CAPTION | WS_SYSMENU,
        dlg_x, dlg_y, dlg_width, dlg_height,
        hwnd, NULL, hInst, NULL);

    if (!hAboutDlg) {
        EnableWindow(hwnd, TRUE);
        return;
    }

    y_offset = 20;
    x_offset = 20;
    width = dlg_width - 40;

    /* Product name */
    CreateWindowExW(0, L"STATIC", L"Mouse Jiggler",
                    SS_LEFT | WS_CHILD | WS_VISIBLE,
                    x_offset, y_offset, width, 25,
                    hAboutDlg, NULL, hInst, NULL);
    y_offset += 35;

    /* Version */
    CreateWindowExW(0, L"STATIC", L"Version 2.1.0",
                    SS_LEFT | WS_CHILD | WS_VISIBLE,
                    x_offset, y_offset, width, 20,
                    hAboutDlg, NULL, hInst, NULL);
    y_offset += 30;

    /* Copyright */
    CreateWindowExW(0, L"STATIC", L"Copyright © Alistair J. R. Young 2007-2025",
                    SS_LEFT | WS_CHILD | WS_VISIBLE,
                    x_offset, y_offset, width, 20,
                    hAboutDlg, NULL, hInst, NULL);
    y_offset += 30;

    /* Company */
    CreateWindowExW(0, L"STATIC", L"Arkane Systems",
                    SS_LEFT | WS_CHILD | WS_VISIBLE,
                    x_offset, y_offset, width, 20,
                    hAboutDlg, NULL, hInst, NULL);
    y_offset += 35;

    /* Description */
    CreateWindowExW(0, L"EDIT",
                    L"A utility to continuously jiggle the mouse pointer to prevent screen saver activation.",
                    ES_LEFT | ES_MULTILINE | ES_READONLY | WS_CHILD | WS_VISIBLE,
                    x_offset, y_offset, width, 70,
                    hAboutDlg, NULL, hInst, NULL);
    y_offset += 80;

    /* OK button */
    hOkButton = CreateWindowExW(0, L"BUTTON", L"OK",
                                BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE | WS_TABSTOP,
                                dlg_width / 2 - 40, y_offset, 80, 25,
                                hAboutDlg, (HMENU)IDC_BUTTON_OK_ABOUT, hInst, NULL);

    /* Show and focus */
    ShowWindow(hAboutDlg, SW_SHOW);
    SetFocus(hOkButton);

    /* Modal message loop - only process messages for this dialog */
    while (GetMessageW(&msg, NULL, 0, 0)) {
        /* Only handle messages for the about dialog */
        if (IsWindow(hAboutDlg) && (msg.hwnd == hAboutDlg || IsChild(hAboutDlg, msg.hwnd))) {
            if (msg.message == WM_COMMAND && LOWORD(msg.wParam) == IDC_BUTTON_OK_ABOUT) {
                break;
            }
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        } else if (msg.message == WM_QUIT) {
            /* Re-queue quit message for main loop */
            PostQuitMessage(0);
            break;
        } else {
            /* Pass other messages to main window */
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }

    /* Cleanup */
    if (IsWindow(hAboutDlg)) {
        DestroyWindow(hAboutDlg);
    }
    EnableWindow(hwnd, TRUE);
    SetFocus(hwnd);
}
