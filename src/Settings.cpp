#include "include/Settings.h"

#include <fstream>

const char *Settings::s_settingsPath = "/ets/DataRetreiveUsb.conf";

Settings::Settings(void)
{
    std::ifstream iFile(s_settingsPath);
    iFile >> m_json;
}

json &Settings::settings(void)
{
    static Settings settings;
    return settings.jsonSettings();
}
