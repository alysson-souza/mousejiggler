/*
 * MouseJiggler - mainwindow.c
 * Main window implementation with UI controls
 */

#include "mainwindow.h"
#include "trayicon.h"
#include "aboutdialog.h"
#include "jiggler.h"
#include "settings.h"
#include "resource.h"
#include <windows.h>
#include <commctrl.h>
#include <stdio.h>

/* Main window handle (for tray icon access) */
HWND g_hwndMain = NULL;

/* Per-window state structure */
typedef struct {
    HWND hwndCheckJiggling;
    HWND hwndCheckSettings;
    HWND hwndCheckZen;
    HWND hwndCheckMinimize;
    HWND hwndSliderPeriod;
    HWND hwndLabelPeriod;
    HWND hwndButtonAbout;
    HWND hwndButtonMinimize;

    HWND hwndPanelBase;
    HWND hwndPanelSettings;

    UINT_PTR timerID;
    bool is_jiggling;
    bool zen_mode;
    int jiggle_period;
    bool zig;  /* zig/zag state */
} MainWindowState;

/* Register window class */
static ATOM MainWindow_RegisterClass(void)
{
    static bool registered = false;
    WNDCLASSEXW wc = {0};
    ATOM result;

    if (registered) {
        return 1;  /* Already registered */
    }

    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = MainWindow_WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = sizeof(MainWindowState *);
    wc.hInstance = GetModuleHandleW(NULL);
    wc.hIcon = LoadIconW(NULL, MAKEINTRESOURCEW(IDI_APPLICATION));
    wc.hCursor = LoadCursorW(NULL, MAKEINTRESOURCEW(IDC_ARROW));
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = MAIN_WINDOW_CLASS;
    wc.hIconSm = LoadIconW(NULL, MAKEINTRESOURCEW(IDI_APPLICATION));

    result = RegisterClassExW(&wc);
    if (result) {
        registered = true;
    }
    return result;
}

/* Create and initialize controls */
static MainWindowState *MainWindow_CreateControls(HWND hwnd, const Settings *settings)
{
    MainWindowState *state;
    HINSTANCE hInst = GetModuleHandleW(NULL);
    int y_base = 10;
    int y_settings = 55;

    state = (MainWindowState *)malloc(sizeof(MainWindowState));
    if (!state) return NULL;

    ZeroMemory(state, sizeof(MainWindowState));
    state->jiggle_period = settings->jiggle_period;
    state->zen_mode = settings->zen_jiggle;
    state->zig = true;

    /* Create base panel */
    state->hwndPanelBase = CreateWindowExW(0, L"STATIC", NULL,
                                           SS_LEFT | WS_CHILD | WS_VISIBLE,
                                           9, y_base, 330, 37,
                                           hwnd, NULL, hInst, NULL);

    /* Create "Jiggling?" checkbox */
    state->hwndCheckJiggling = CreateWindowExW(0, L"BUTTON", L"Jiggling?",
                                               BS_AUTOCHECKBOX | WS_CHILD | WS_VISIBLE,
                                               11, y_base + 7, 90, 24,
                                               hwnd, (HMENU)IDC_CHECK_JIGGLING, hInst, NULL);
    SendMessageW(state->hwndCheckJiggling, BM_SETCHECK, BST_UNCHECKED, 0);

    /* Create "Settings..." checkbox */
    state->hwndCheckSettings = CreateWindowExW(0, L"BUTTON", L"Settings...",
                                               BS_AUTOCHECKBOX | WS_CHILD | WS_VISIBLE,
                                               101, y_base + 7, 88, 25,
                                               hwnd, (HMENU)IDC_CHECK_SETTINGS, hInst, NULL);
    SendMessageW(state->hwndCheckSettings, BM_SETCHECK, BST_UNCHECKED, 0);

    /* Create "?" button */
    state->hwndButtonAbout = CreateWindowExW(0, L"BUTTON", L"?",
                                             BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE,
                                             226, y_base + 3, 46, 31,
                                             hwnd, (HMENU)IDC_BUTTON_ABOUT, hInst, NULL);

    /* Create minimize button */
    state->hwndButtonMinimize = CreateWindowExW(0, L"BUTTON", L"\xFEA7",  /* down arrow */
                                                BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE,
                                                279, y_base + 3, 46, 31,
                                                hwnd, (HMENU)IDC_BUTTON_MINIMIZE, hInst, NULL);

    /* Create settings panel */
    state->hwndPanelSettings = CreateWindowExW(WS_EX_TRANSPARENT, L"STATIC", NULL,
                                               WS_CHILD, /* starts hidden */
                                               9, y_settings, 330, 143,
                                               hwnd, NULL, hInst, NULL);

    /* Create "Zen jiggle?" checkbox */
    state->hwndCheckZen = CreateWindowExW(0, L"BUTTON", L"Zen jiggle?",
                                          BS_AUTOCHECKBOX | WS_CHILD | WS_VISIBLE,
                                          11, y_settings + 15, 105, 24,
                                          hwnd, (HMENU)IDC_CHECK_ZEN, hInst, NULL);
    SendMessageW(state->hwndCheckZen, BM_SETCHECK,
                 settings->zen_jiggle ? BST_CHECKED : BST_UNCHECKED, 0);

    /* Create "Minimize on start?" checkbox */
    state->hwndCheckMinimize = CreateWindowExW(0, L"BUTTON", L"Minimize on start?",
                                               BS_AUTOCHECKBOX | WS_CHILD | WS_VISIBLE,
                                               11, y_settings + 49, 153, 24,
                                               hwnd, (HMENU)IDC_CHECK_MINIMIZE, hInst, NULL);
    SendMessageW(state->hwndCheckMinimize, BM_SETCHECK,
                 settings->minimize_on_startup ? BST_CHECKED : BST_UNCHECKED, 0);

    /* Create trackbar for period */
    state->hwndSliderPeriod = CreateWindowExW(0, TRACKBAR_CLASSW, NULL,
                                              WS_CHILD | WS_VISIBLE | TBS_HORZ,
                                              5, y_settings + 83, 321, 56,
                                              hwnd, (HMENU)IDC_SLIDER_PERIOD, hInst, NULL);
    SendMessageW(state->hwndSliderPeriod, TBM_SETRANGE, TRUE, MAKELPARAM(1, 10800));
    SendMessageW(state->hwndSliderPeriod, TBM_SETPOS, TRUE, settings->jiggle_period);
    SendMessageW(state->hwndSliderPeriod, TBM_SETTICFREQ, 2, 0);

    /* Create period label */
    {
        wchar_t period_text[32];
        swprintf_s(period_text, ARRAYSIZE(period_text), L"%d s", settings->jiggle_period);
        state->hwndLabelPeriod = CreateWindowExW(0, L"STATIC", period_text,
                                             SS_LEFT | WS_CHILD | WS_VISIBLE,
                                             279, y_settings + 55, 27, 20,
                                             hwnd, (HMENU)IDC_LABEL_PERIOD, hInst, NULL);
    }

    return state;
}

/* Handle timer tick - perform jiggling */
static void MainWindow_OnTimer(HWND hwnd, MainWindowState *state)
{
    if (!state->is_jiggling) return;

    if (state->zen_mode) {
        /* Zen mode: virtual jiggle (0 delta) */
        Jiggler_Jiggle(0);
    } else {
        /* Normal mode: zig-zag pattern */
        if (state->zig) {
            Jiggler_Jiggle(4);
        } else {
            Jiggler_Jiggle(-4);
        }
        state->zig = !state->zig;
    }
}

/* Cleanup controls and state */
static void MainWindow_Cleanup(HWND hwnd, MainWindowState *state)
{
    if (!state) return;

    if (state->timerID) {
        KillTimer(hwnd, state->timerID);
    }

    free(state);
}

/* Message handler */
LRESULT CALLBACK MainWindow_WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    MainWindowState *state;

    /* Get state pointer from window extra data
     * Note: on WM_CREATE, we create controls and set the state
     */
    state = (MainWindowState *)GetWindowLongPtrW(hwnd, 0);

    switch (msg) {
    case WM_CREATE: {
        CREATESTRUCT *pCreate = (CREATESTRUCT *)lParam;
        Settings *settings = (Settings *)pCreate->lpCreateParams;
        state = MainWindow_CreateControls(hwnd, settings);
        if (!state) return -1;
        SetWindowLongPtrW(hwnd, 0, (LONG_PTR)state);
        if (!TrayIcon_Add(hwnd)) {
            /* Continue even if tray icon fails */
        }
        return 0;
    }

    case WM_COMMAND: {
        int ctrl_id = LOWORD(wParam);
        int notification = HIWORD(wParam);

        if (!state) return 0;

        if (ctrl_id == IDC_CHECK_JIGGLING && notification == BN_CLICKED) {
            bool checked = SendMessageW(state->hwndCheckJiggling, BM_GETCHECK, 0, 0) == BST_CHECKED;
            MainWindow_SetJiggling(hwnd, checked);
        }
        else if (ctrl_id == IDC_CHECK_SETTINGS && notification == BN_CLICKED) {
            bool checked = SendMessageW(state->hwndCheckSettings, BM_GETCHECK, 0, 0) == BST_CHECKED;
            ShowWindow(state->hwndPanelSettings, checked ? SW_SHOW : SW_HIDE);
            InvalidateRect(hwnd, NULL, TRUE);
        }
        else if (ctrl_id == IDC_CHECK_ZEN && notification == BN_CLICKED) {
            state->zen_mode = SendMessageW(state->hwndCheckZen, BM_GETCHECK, 0, 0) == BST_CHECKED;
            Settings settings = MainWindow_GetSettings(hwnd);
            Settings_Save(&settings);
        }
        else if (ctrl_id == IDC_CHECK_MINIMIZE && notification == BN_CLICKED) {
            Settings settings = MainWindow_GetSettings(hwnd);
            Settings_Save(&settings);
        }
        else if (ctrl_id == IDC_BUTTON_ABOUT && notification == BN_CLICKED) {
            AboutDialog_Show(hwnd);
        }
        else if (ctrl_id == IDC_BUTTON_MINIMIZE && notification == BN_CLICKED) {
            MainWindow_MinimizeToTray(hwnd);
        }
        return 0;
    }

    case WM_HSCROLL: {
        if (!state) return 0;
        if ((HWND)lParam == state->hwndSliderPeriod) {
            int new_period = (int)SendMessageW(state->hwndSliderPeriod, TBM_GETPOS, 0, 0);
            state->jiggle_period = new_period;

            {
                wchar_t period_text[32];
                swprintf_s(period_text, ARRAYSIZE(period_text), L"%d s", new_period);
                SetWindowTextW(state->hwndLabelPeriod, period_text);
            }

            if (state->is_jiggling && state->timerID) {
                KillTimer(hwnd, state->timerID);
                state->timerID = SetTimer(hwnd, 1, new_period * 1000, NULL);
            }

            Settings settings = MainWindow_GetSettings(hwnd);
            Settings_Save(&settings);
        }
        return 0;
    }

    case WM_TIMER:
        MainWindow_OnTimer(hwnd, state);
        return 0;

    case WM_APP + 1:
        /* Handle tray icon messages (right-click menu, double-click) */
        if (state) {
            TrayIcon_HandleContextMenu(hwnd, lParam);
        }
        return 0;

    case WM_CLOSE:
        PostQuitMessage(0);
        return 0;

    case WM_DESTROY:
        TrayIcon_Remove(hwnd);
        MainWindow_Cleanup(hwnd, state);
        return 0;

    case WM_SHOWWINDOW:
        if (wParam) {
            /* Window is being shown */
            MainWindow_UpdateTrayText(hwnd);
        }
        return DefWindowProcW(hwnd, msg, wParam, lParam);

    default:
        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
}

HWND MainWindow_Create(bool start_jiggling, const Settings *settings)
{
    HWND hwnd;
    MainWindowState *state;

    /* Register window class */
    if (!MainWindow_RegisterClass()) {
        return NULL;
    }

    /* Create the window */
    hwnd = CreateWindowExW(0, MAIN_WINDOW_CLASS, L"Mouse Jiggler",
                           WS_OVERLAPPEDWINDOW & ~(WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_THICKFRAME),
                           CW_USEDEFAULT, CW_USEDEFAULT, 347, 213,
                           NULL, NULL, GetModuleHandleW(NULL), (void *)settings);

    if (!hwnd) {
        return NULL;
    }

    g_hwndMain = hwnd;
    state = (MainWindowState *)GetWindowLongPtrW(hwnd, 0);

    if (start_jiggling && state) {
        SendMessageW(state->hwndCheckJiggling, BM_SETCHECK, BST_CHECKED, 0);
        MainWindow_SetJiggling(hwnd, true);
    }

    if (settings && settings->minimize_on_startup) {
        MainWindow_MinimizeToTray(hwnd);
    } else {
        ShowWindow(hwnd, SW_SHOW);
        UpdateWindow(hwnd);
    }

    return hwnd;
}

bool MainWindow_SetJiggling(HWND hwnd, bool enable)
{
    MainWindowState *state = (MainWindowState *)GetWindowLongPtrW(hwnd, 0);
    if (!state) return false;

    state->is_jiggling = enable;

    if (enable) {
        /* Start timer */
        if (state->timerID) {
            KillTimer(hwnd, state->timerID);
        }
        state->timerID = SetTimer(hwnd, 1, state->jiggle_period * 1000, NULL);
        if (!state->timerID) {
            /* Timer failed, don't mark as jiggling */
            state->is_jiggling = false;
            return false;
        }
        SendMessageW(state->hwndCheckJiggling, BM_SETCHECK, BST_CHECKED, 0);
    } else {
        /* Stop timer */
        if (state->timerID) {
            KillTimer(hwnd, state->timerID);
            state->timerID = 0;
        }
        SendMessageW(state->hwndCheckJiggling, BM_SETCHECK, BST_UNCHECKED, 0);
    }

    MainWindow_UpdateTrayText(hwnd);
    return true;
}

Settings MainWindow_GetSettings(HWND hwnd)
{
    MainWindowState *state = (MainWindowState *)GetWindowLongPtrW(hwnd, 0);
    Settings settings = {0};

    if (state) {
        settings.minimize_on_startup =
            SendMessageW(state->hwndCheckMinimize, BM_GETCHECK, 0, 0) == BST_CHECKED;
        settings.zen_jiggle =
            SendMessageW(state->hwndCheckZen, BM_GETCHECK, 0, 0) == BST_CHECKED;
        settings.jiggle_period = state->jiggle_period;
    }

    return settings;
}

void MainWindow_UpdateTrayText(HWND hwnd)
{
    MainWindowState *state = (MainWindowState *)GetWindowLongPtrW(hwnd, 0);
    if (!state) return;

    wchar_t text[256];
    if (state->is_jiggling) {
        wchar_t *mode = state->zen_mode ? L"with" : L"without";
        swprintf_s(text, ARRAYSIZE(text), L"Jiggling mouse every %d s, %s Zen.",
                   state->jiggle_period, mode);
    } else {
        wcscpy_s(text, ARRAYSIZE(text), L"Not jiggling the mouse.");
    }

    TrayIcon_UpdateText(hwnd, text);
}

void MainWindow_MinimizeToTray(HWND hwnd)
{
    ShowWindow(hwnd, SW_HIDE);
    SetWindowLongW(hwnd, GWL_EXSTYLE, GetWindowLongW(hwnd, GWL_EXSTYLE) | WS_EX_TOOLWINDOW);
}

void MainWindow_RestoreFromTray(HWND hwnd)
{
    SetWindowLongW(hwnd, GWL_EXSTYLE, GetWindowLongW(hwnd, GWL_EXSTYLE) & ~WS_EX_TOOLWINDOW);
    ShowWindow(hwnd, SW_SHOW);
    SetForegroundWindow(hwnd);
}

bool MainWindow_IsJiggling(HWND hwnd)
{
    MainWindowState *state = (MainWindowState *)GetWindowLongPtrW(hwnd, 0);
    return state ? state->is_jiggling : false;
}

int MainWindow_GetJigglePeriod(HWND hwnd)
{
    MainWindowState *state = (MainWindowState *)GetWindowLongPtrW(hwnd, 0);
    return state ? state->jiggle_period : 60;
}
