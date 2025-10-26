/*
 * MouseJiggler - cmdline.h
 * Command-line argument parsing
 */

#ifndef CMDLINE_H
#define CMDLINE_H

#include "settings.h"
#include <stdbool.h>

/* Command-line options */
typedef struct {
    bool should_jiggle;
    Settings settings;
    bool show_help;
    bool show_version;
    bool should_run_gui;  /* false if showing help/version */
} CmdlineOptions;

/* Parse command-line arguments
 * Returns: CmdlineOptions structure with parsed values
 */
CmdlineOptions CmdLine_Parse(int argc, wchar_t *argv[]);

/* Print help message to console */
void CmdLine_PrintHelp(void);

/* Print version information */
void CmdLine_PrintVersion(void);

/* Attach to parent process console for output */
void CmdLine_AttachParentConsole(void);

/* Detach from console */
void CmdLine_DetachConsole(void);

#endif /* CMDLINE_H */
