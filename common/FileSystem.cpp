#include "FileSystem.h"
#include "PathTool.h"
#include "LogTool.h"

IMPLEMENT_SINGLETON(FileSystem);

FileSystem::FileSystem()
{
    searchPaths_.push_back(getExePath());
}

FileSystem::~FileSystem()
{

}

std::string FileSystem::getFullPath(const std::string &fileName) const
{
    if(fileName.empty() || fileName.front() == '/')
    {
        return fileName;
    }

    for(const std::string &path : searchPaths_)
    {
        std::string fullPath = joinPath(path, fileName);
        if(isExist(fullPath))
        {
            return fullPath;
        }
    }
    return std::string();
}

bool FileSystem::readFile(std::string &output, const std::string &fileName, bool isBinary)
{
    std::string fullPath = getFullPath(fileName);
    if(fullPath.empty())
    {
        LOG_ERROR("Failed find file %s", fileName.c_str());
        return false;
    }

    FILE *pFile = fopen(fullPath.c_str(), isBinary ? "rb" : "r");
    if(nullptr == pFile)
    {
        return false;
    }

    fseek(pFile, 0, SEEK_END);
    long length = ftell(pFile);
    fseek(pFile, 0, SEEK_SET);

    output.resize(length);
    if(length > 0)
    {
        fread(&output[0], length, 1, pFile);
    }

    fclose(pFile);
    return true;
}

void FileSystem::addSearchPath(const std::string &path)
{
    for(std::string &pa : searchPaths_)
    {
        if(pa == path)
        {
            return;
        }
    }
    searchPaths_.push_back(path);
}

void FileSystem::dumpSearchPath()
{
    for(const std::string &path : searchPaths_)
    {
        LOG_DEBUG("SearchPath: %s", path.c_str());
    }
}
