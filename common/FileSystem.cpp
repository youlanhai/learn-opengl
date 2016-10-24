#include "FileSystem.h"
#include "PathTool.h"
#include "LogTool.h"

IMPLEMENT_SINGLETON(FileSystem);

FileSystem::FileSystem()
{
    writablePath_ = getExePath();
    searchPaths_.push_back(getExePath());
}

FileSystem::~FileSystem()
{

}

std::string FileSystem::getFullPath(const std::string &fileName) const
{
    if(fileName.empty() || isAbsolutePath(fileName))
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

bool FileSystem::saveFile(const char* data, size_t size, const std::string &fileName, bool isBinary)
{
    std::string fullPath = resolveWritablePath(fileName);
    FILE *pFile = fopen(fullPath.c_str(), isBinary ? "wb" : "w");
    if(nullptr == pFile)
    {
        return false;
    }
    
    fwrite(data, size, 1, pFile);
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

std::string FileSystem::resolveWritablePath(const std::string &path) const
{
    if(isAbsolutePath(path))
    {
        std::string fullPath = path;
        formatPath(fullPath);
        return fullPath;
    }
    else
    {
        return joinPath(writablePath_, path);
    } 
}
