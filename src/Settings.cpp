#include "include/Settings.h"

#include <fstream>

const char *Settings::s_settingsPath = "/ets/DataRetreiveUsb.conf";
//const char *Settings::s_settingsPath = "DataRetreiveUsb.conf";

Settings::Settings(void)
{
    std::ifstream iFile(s_settingsPath);
    iFile >> m_json;
}

Settings::~Settings(void)
{
}
