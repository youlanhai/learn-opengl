#ifndef COMMON_FILE_SYSTEM_H
#define COMMON_FILE_SYSTEM_H

#include "Singleton.h"
#include <string>
#include <vector>

class FileSystem : public Singleton<FileSystem>
{
public:
    typedef std::vector<std::string> Paths;

    FileSystem();
    ~FileSystem();

    std::string getFullPath(const std::string &fileName) const;

    bool readFile(std::string &output, const std::string &fileName, bool isBinary = false);

    void addSearchPath(const std::string &path);

    void setSearchPaths(const Paths &paths){ searchPaths_ = paths; };
    Paths& getSearchPaths(){ return searchPaths_; }
    const Paths& getSearchPaths() const { return searchPaths_; }

private:
    Paths   searchPaths_;
};

#endif //COMMON_FILE_SYSTEM_H
