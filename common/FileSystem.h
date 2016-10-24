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
    bool saveFile(const char* data, size_t size, const std::string &fileName, bool isBinary = false);

    void addSearchPath(const std::string &path);

    void setSearchPaths(const Paths &paths){ searchPaths_ = paths; };
    Paths& getSearchPaths(){ return searchPaths_; }
    const Paths& getSearchPaths() const { return searchPaths_; }

    void dumpSearchPath();
    
    void setWritablePath(const std::string &path){ writablePath_ = path; }
    const std::string& getWritablePath() const { return writablePath_; }
    
    std::string resolveWritablePath(const std::string &path) const;

private:
    Paths   searchPaths_;
    std::string writablePath_;
};

#endif //COMMON_FILE_SYSTEM_H
