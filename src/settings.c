/*
 * MouseJiggler - settings.c
 * Settings management with Windows Registry persistence
 */

#include "settings.h"
#include <windows.h>
#include <string.h>

#define REG_KEY_PATH L"Software\\ArkaneSystems\\MouseJiggler"

/* Default settings */
static const Settings DEFAULT_SETTINGS = {
    .minimize_on_startup = false,
    .zen_jiggle = false,
    .jiggle_period = 60  /* 60 seconds */
};

Settings Settings_GetDefaults(void)
{
    return DEFAULT_SETTINGS;
}

bool Settings_IsValidPeriod(int period)
{
    return period >= 1 && period <= 10800;
}

Settings Settings_Load(void)
{
    Settings settings = DEFAULT_SETTINGS;
    HKEY hKey;
    DWORD dwValue;
    DWORD dwSize;
    LONG result;

    /* Try to open the registry key */
    result = RegOpenKeyExW(HKEY_CURRENT_USER, REG_KEY_PATH, 0, KEY_READ, &hKey);
    if (result != ERROR_SUCCESS) {
        /* Key doesn't exist, return defaults */
        return settings;
    }

    /* Read MinimizeOnStartup */
    dwSize = sizeof(DWORD);
    if (RegQueryValueExW(hKey, L"MinimizeOnStartup", NULL, NULL, (LPBYTE)&dwValue, &dwSize) == ERROR_SUCCESS) {
        settings.minimize_on_startup = (dwValue != 0);
    }

    /* Read ZenJiggle */
    dwSize = sizeof(DWORD);
    if (RegQueryValueExW(hKey, L"ZenJiggle", NULL, NULL, (LPBYTE)&dwValue, &dwSize) == ERROR_SUCCESS) {
        settings.zen_jiggle = (dwValue != 0);
    }

    /* Read JigglePeriod */
    dwSize = sizeof(DWORD);
    if (RegQueryValueExW(hKey, L"JigglePeriod", NULL, NULL, (LPBYTE)&dwValue, &dwSize) == ERROR_SUCCESS) {
        if (Settings_IsValidPeriod((int)dwValue)) {
            settings.jiggle_period = (int)dwValue;
        }
    }

    RegCloseKey(hKey);
    return settings;
}

bool Settings_Save(const Settings *settings)
{
    HKEY hKey;
    DWORD dwValue;
    LONG result;
    DWORD dwDisposition;

    if (settings == NULL) {
        return false;
    }

    /* Create or open the registry key */
    result = RegCreateKeyExW(HKEY_CURRENT_USER, REG_KEY_PATH, 0, NULL,
                             REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, &dwDisposition);
    if (result != ERROR_SUCCESS) {
        return false;
    }

    /* Write MinimizeOnStartup */
    dwValue = settings->minimize_on_startup ? 1 : 0;
    result = RegSetValueExW(hKey, L"MinimizeOnStartup", 0, REG_DWORD, (LPBYTE)&dwValue, sizeof(DWORD));
    if (result != ERROR_SUCCESS) {
        RegCloseKey(hKey);
        return false;
    }

    /* Write ZenJiggle */
    dwValue = settings->zen_jiggle ? 1 : 0;
    result = RegSetValueExW(hKey, L"ZenJiggle", 0, REG_DWORD, (LPBYTE)&dwValue, sizeof(DWORD));
    if (result != ERROR_SUCCESS) {
        RegCloseKey(hKey);
        return false;
    }

    /* Write JigglePeriod */
    dwValue = (DWORD)settings->jiggle_period;
    result = RegSetValueExW(hKey, L"JigglePeriod", 0, REG_DWORD, (LPBYTE)&dwValue, sizeof(DWORD));

    RegCloseKey(hKey);
    return result == ERROR_SUCCESS;
}
