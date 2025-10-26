/*
 * MouseJiggler - settings.h
 * Settings management with Windows Registry persistence
 */

#ifndef SETTINGS_H
#define SETTINGS_H

#include <stdbool.h>

/* Settings structure */
typedef struct {
    bool minimize_on_startup;
    bool zen_jiggle;
    int jiggle_period;  /* seconds, 1-10800 */
} Settings;

/* Load settings from Windows Registry
 * Returns: Settings structure with default values if registry key doesn't exist
 */
Settings Settings_Load(void);

/* Save settings to Windows Registry
 * Returns: true if successful, false on error
 */
bool Settings_Save(const Settings *settings);

/* Get default settings */
Settings Settings_GetDefaults(void);

/* Validate jiggle period (1-10800 seconds)
 * Returns: true if valid, false otherwise
 */
bool Settings_IsValidPeriod(int period);

#endif /* SETTINGS_H */
