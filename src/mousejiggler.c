/*
 * MouseJiggler - mousejiggler.c
 * Main entry point and application initialization
 */

#include "mainwindow.h"
#include "cmdline.h"
#include "jiggler.h"
#include <windows.h>
#include <commctrl.h>
#include <stdio.h>

/* Initialize common controls */
#ifndef ICC_TRACKBAR_CLASS
#define ICC_TRACKBAR_CLASS 0x00000020
#endif

/* Single-instance mutex name */
#define MUTEX_NAME L"Global\\ArkaneSystems.MouseJiggler"

int wmain(int argc, wchar_t *argv[])
{
    HANDLE hMutex;
    HWND hwnd;
    MSG msg;
    CmdlineOptions options;

    /* Attach to parent console for help/version output */
    CmdLine_AttachParentConsole();

    /* Create or open the single-instance mutex */
    hMutex = CreateMutexW(NULL, FALSE, MUTEX_NAME);
    if (!hMutex) {
        wprintf(L"Failed to create mutex.\n");
        CmdLine_DetachConsole();
        return 1;
    }

    /* Check if another instance is already running */
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        wprintf(L"Mouse Jiggler is already running. Aborting.\n");
        CloseHandle(hMutex);
        CmdLine_DetachConsole();
        return 1;
    }

    /* Parse command-line arguments */
    options = CmdLine_Parse(argc, argv);

    /* Handle help or version options */
    if (options.show_help) {
        CmdLine_PrintHelp();
        CmdLine_DetachConsole();
        CloseHandle(hMutex);
        return 0;
    }

    if (options.show_version) {
        CmdLine_PrintVersion();
        CmdLine_DetachConsole();
        CloseHandle(hMutex);
        return 0;
    }

    /* If we should show help or version, don't run GUI */
    if (!options.should_run_gui) {
        CmdLine_DetachConsole();
        CloseHandle(hMutex);
        return 0;
    }

    /* Detach from console before starting GUI */
    CmdLine_DetachConsole();

    /* Initialize common controls */
    INITCOMMONCONTROLSEX icc;
    icc.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icc.dwICC = ICC_STANDARD_CLASSES | ICC_TRACKBAR_CLASS;
    InitCommonControlsEx(&icc);

    /* Initialize jiggler module */
    Jiggler_Initialize();

    /* Create main window */
    hwnd = MainWindow_Create(options.should_jiggle, &options.settings);
    if (!hwnd) {
        ReleaseMutex(hMutex);
        CloseHandle(hMutex);
        return 1;
    }

    /* Main message loop */
    while (GetMessageW(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    /* Cleanup */
    CloseHandle(hMutex);

    return (int)msg.wParam;
}

/* Entry point for Windows subsystem */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow)
{
    /* Call wmain with wide-character arguments */
    int argc;
    wchar_t **argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    int result = wmain(argc, argv);
    LocalFree(argv);
    return result;
}
