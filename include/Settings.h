#ifndef _SETTING_H_
#define _SETTING_H_

#include <string>
#include <unordered_map>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class Settings
{
public:
    Settings(const Settings &) = delete;
    void operator=(const Settings &) = delete;

    static json &settings(void);

private:
    Settings(void);

    inline json &jsonSettings(void) { return m_json; };

    json m_json;

    static const char *s_settingsPath;
};

#endif // _SETTING_H_
