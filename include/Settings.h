#ifndef _SETTING_H_
#define _SETTING_H_

#include <string>
#include <unordered_map>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class Settings
{
public:
    Settings(void);
    virtual ~Settings(void);

    inline json jsonSettings(void) { return m_json; }

private:
    json m_json;

    static const char *s_settingsPath;
};

#endif // _SETTING_H_
