#ifndef _DATA_HANDLING_H_
#define _DATA_HANDLING_H_

#include <string>

class DataHandling
{
public:
    DataHandling(const std::string &basePath);
    virtual ~DataHandling(void);

    bool fileExists(void);
    bool fileCopy(void);

    void uploadLocalFiles(void);

private:
    bool uploadLocalFile(const std::string &fileName);

    uint64_t timestamp(void);
    std::string timestampString(void);

    std::string m_basePath;
};

#endif // _DATA_HANDLING_H_
