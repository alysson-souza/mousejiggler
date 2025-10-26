/*
 * MouseJiggler - trayicon.c
 * System tray icon management
 */

#include "trayicon.h"
#include "mainwindow.h"
#include "resource.h"
#include <windows.h>
#include <shellapi.h>
#include <string.h>

#define TRAY_ID 1

bool TrayIcon_Add(HWND hwnd)
{
    NOTIFYICONDATAW nid;

    ZeroMemory(&nid, sizeof(NOTIFYICONDATAW));
    nid.cbSize = sizeof(NOTIFYICONDATAW);
    nid.hWnd = hwnd;
    nid.uID = TRAY_ID;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = WM_APP + 1;  /* Custom message for tray */
    nid.hIcon = LoadIconW(NULL, MAKEINTRESOURCEW(IDI_APPLICATION));
    wcscpy_s(nid.szTip, ARRAYSIZE(nid.szTip), L"Mouse Jiggler");

    return Shell_NotifyIconW(NIM_ADD, &nid);
}

void TrayIcon_UpdateText(HWND hwnd, const wchar_t *text)
{
    NOTIFYICONDATAW nid;

    if (!text || wcslen(text) == 0) return;

    ZeroMemory(&nid, sizeof(NOTIFYICONDATAW));
    nid.cbSize = sizeof(NOTIFYICONDATAW);
    nid.hWnd = hwnd;
    nid.uID = TRAY_ID;
    nid.uFlags = NIF_TIP;
    wcscpy_s(nid.szTip, ARRAYSIZE(nid.szTip), text);

    Shell_NotifyIconW(NIM_MODIFY, &nid);
}

void TrayIcon_Remove(HWND hwnd)
{
    NOTIFYICONDATAW nid;

    ZeroMemory(&nid, sizeof(NOTIFYICONDATAW));
    nid.cbSize = sizeof(NOTIFYICONDATAW);
    nid.hWnd = hwnd;
    nid.uID = TRAY_ID;

    Shell_NotifyIconW(NIM_DELETE, &nid);
}

int TrayIcon_ShowContextMenu(HWND hwnd, const POINT pt)
{
    HMENU hMenu;
    HMENU hContextMenu;
    int cmd;

    hContextMenu = CreatePopupMenu();
    if (!hContextMenu) return 0;

    AppendMenuW(hContextMenu, MF_STRING, IDM_TRAY_OPEN, L"Open");
    AppendMenuW(hContextMenu, MF_SEPARATOR, 0, NULL);

    bool is_jiggling = MainWindow_IsJiggling(hwnd);
    if (is_jiggling) {
        AppendMenuW(hContextMenu, MF_STRING, IDM_TRAY_STOP, L"Stop Jiggling");
    } else {
        AppendMenuW(hContextMenu, MF_STRING, IDM_TRAY_START, L"Start Jiggling");
    }

    AppendMenuW(hContextMenu, MF_SEPARATOR, 0, NULL);
    AppendMenuW(hContextMenu, MF_STRING, IDM_TRAY_EXIT, L"Exit");

    /* Show menu at cursor position */
    SetForegroundWindow(hwnd);
    cmd = TrackPopupMenuEx(hContextMenu, TPM_RETURNCMD | TPM_LEFTALIGN,
                           pt.x, pt.y, hwnd, NULL);
    DestroyMenu(hContextMenu);

    return cmd;
}

LRESULT TrayIcon_HandleContextMenu(HWND hwnd, LPARAM lParam)
{
    switch (LOWORD(lParam)) {
    case WM_RBUTTONUP: {
        POINT pt;
        GetCursorPos(&pt);
        int cmd = TrayIcon_ShowContextMenu(hwnd, pt);

        switch (cmd) {
        case IDM_TRAY_OPEN:
            MainWindow_RestoreFromTray(hwnd);
            break;
        case IDM_TRAY_START:
            MainWindow_SetJiggling(hwnd, true);
            break;
        case IDM_TRAY_STOP:
            MainWindow_SetJiggling(hwnd, false);
            break;
        case IDM_TRAY_EXIT:
            PostQuitMessage(0);
            break;
        }
        break;
    }
    case WM_LBUTTONDBLCLK:
        MainWindow_RestoreFromTray(hwnd);
        break;
    }
    return 0;
}
