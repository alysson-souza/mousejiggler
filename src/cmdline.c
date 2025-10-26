/*
 * MouseJiggler - cmdline.c
 * Command-line argument parsing
 */

#include "cmdline.h"
#include "settings.h"
#include <windows.h>
#include <wchar.h>
#include <stdio.h>

#define VERSION_MAJOR 2
#define VERSION_MINOR 1
#define VERSION_RELEASE 0

void CmdLine_AttachParentConsole(void)
{
    AttachConsole(ATTACH_PARENT_PROCESS);
}

void CmdLine_DetachConsole(void)
{
    FreeConsole();
}

void CmdLine_PrintHelp(void)
{
    wprintf(L"Usage:\n");
    wprintf(L"  MouseJiggler [options]\n\n");
    wprintf(L"Options:\n");
    wprintf(L"  -j, --jiggle               Start with jiggling enabled.\n");
    wprintf(L"  -m, --minimized            Start minimized (sets persistent option). [default: False]\n");
    wprintf(L"  -z, --zen                  Start with zen (invisible) jiggling enabled (sets persistent option). [default: False]\n");
    wprintf(L"  -s, --seconds <seconds>    Set number of seconds for the jiggle interval (sets persistent option). [default: 60]\n");
    wprintf(L"  --version                  Show version information\n");
    wprintf(L"  -?, -h, --help             Show help and usage information\n");
}

void CmdLine_PrintVersion(void)
{
    wprintf(L"MouseJiggler %d.%d.%d\n", VERSION_MAJOR, VERSION_MINOR, VERSION_RELEASE);
    wprintf(L"Copyright © Alistair J. R. Young 2007-2025\n");
}

CmdlineOptions CmdLine_Parse(int argc, wchar_t *argv[])
{
    CmdlineOptions options;
    int i;

    /* Initialize with defaults */
    options.should_jiggle = false;
    options.show_help = false;
    options.show_version = false;
    options.should_run_gui = true;
    options.settings = Settings_Load();

    /* Parse arguments */
    for (i = 1; i < argc; i++) {
        wchar_t *arg = argv[i];

        /* Help options */
        if (wcscmp(arg, L"-h") == 0 ||
            wcscmp(arg, L"-?") == 0 ||
            wcscmp(arg, L"--help") == 0) {
            options.show_help = true;
            options.should_run_gui = false;
            return options;
        }

        /* Version option */
        if (wcscmp(arg, L"--version") == 0) {
            options.show_version = true;
            options.should_run_gui = false;
            return options;
        }

        /* Jiggle option */
        if (wcscmp(arg, L"-j") == 0 || wcscmp(arg, L"--jiggle") == 0) {
            options.should_jiggle = true;
            continue;
        }

        /* Minimized option */
        if (wcscmp(arg, L"-m") == 0 || wcscmp(arg, L"--minimized") == 0) {
            options.settings.minimize_on_startup = true;
            Settings_Save(&options.settings);
            continue;
        }

        /* Zen option */
        if (wcscmp(arg, L"-z") == 0 || wcscmp(arg, L"--zen") == 0) {
            options.settings.zen_jiggle = true;
            Settings_Save(&options.settings);
            continue;
        }

        /* Seconds option */
        if (wcscmp(arg, L"-s") == 0 || wcscmp(arg, L"--seconds") == 0) {
            if (i + 1 < argc) {
                int period = _wtoi(argv[++i]);
                if (Settings_IsValidPeriod(period)) {
                    options.settings.jiggle_period = period;
                    Settings_Save(&options.settings);
                } else {
                    wprintf(L"Error: Period must be between 1 and 10800 seconds.\n");
                    options.should_run_gui = false;
                    return options;
                }
            } else {
                wprintf(L"Error: -s/--seconds requires an argument.\n");
                options.should_run_gui = false;
                return options;
            }
            continue;
        }

        /* Unknown option */
        wprintf(L"Unknown option: %s\n", arg);
        wprintf(L"Use -h or --help for usage information.\n");
        options.should_run_gui = false;
        return options;
    }

    return options;
}
