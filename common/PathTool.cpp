#include "PathTool.h"
#include <sstream>
#include <algorithm>

#ifdef __APPLE__
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <mach-o/dyld.h>
#elif defined(WIN32)
#include <Windows.h>
#endif

void formatSlash(std::string &path)
{
    std::replace(path.begin(), path.end(), INV_SLASH_CHAR, SLASH_CHAR);
}

void formatPath(std::string &path)
{
    formatSlash(path);
    appendPathSlash(path);
}

void formatPathNoEndSlash(std::string &path)
{
    formatSlash(path);
    trimPathSlash(path);
}

void appendPathSlash(std::string &path)
{
    if(!path.empty() && path.back() != SLASH_CHAR)
    {
        path += SLASH_CHAR;
    }
}

void trimPathSlash(std::string &path)
{
    // ignore "/"
    if(path.size() > 1 && path.back() == SLASH_CHAR)
    {
        path.erase(path.end() - 1);
    }
}

std::string getFileExt(const std::string &filename)
{
    std::string path = filename;
    formatPathNoEndSlash(path);

    std::string::size_type pos = path.find_last_of('.');
    if(pos != std::string::npos)
    {
        std::string::size_type pos2 = path.find_last_of(SLASH_CHAR);
        if(pos2 != std::string::npos && pos > pos2)
            return path.substr(pos);
    }
    
    return std::string();
}

void removeFileExt(std::string &filename)
{
    std::string path = filename;
    formatPathNoEndSlash(path);

    std::string::size_type pos = path.find_last_of('.');
    if(pos != std::string::npos)
    {
        std::string::size_type pos2 = path.find_last_of(SLASH_CHAR);
        if(pos2 != std::string::npos && pos > pos2)
        {
            filename.erase(pos);
        }
    }
}

std::string getFilePath(const std::string &filename)
{
    std::string path = filename;
    formatPathNoEndSlash(path);
    
    std::string::size_type pos = path.find_last_of(SLASH_CHAR);
    if(pos != std::string::npos)
    {
        return path.substr(0, pos);
    }
    
    return std::string();
}

std::string getFileName(const std::string &filename)
{
    std::string path = filename;
    formatPathNoEndSlash(path);
    
    std::string::size_type pos = path.find_last_of(SLASH_CHAR);
    if(pos != std::string::npos)
    {
        return path.substr(pos + 1);
    }
    
    return path;
}

std::string joinPath(const std::string &a, const std::string &b)
{
    std::string pa = a;
    formatPathNoEndSlash(pa);

    std::string pb = b;
    formatPathNoEndSlash(pb);

    if(!b.empty() && b[0] != SLASH_CHAR)
    {
        pa += SLASH_CHAR;
    }
    pa += pb;
    return pa;
}

std::string getParentPath(const std::string &filename, int tier)
{
    std::string path = filename;
    formatPathNoEndSlash(path);

    if(tier < 1 || path.empty())
    {
        return path;
    }
    
    int i = int(path.size()) - 1;
    for( ; i >= 0 && tier > 0; --i)
    {
        if(path[i] == SLASH_CHAR)
        {
            --tier;
        }
    }
    
    return path.substr(0, i);
}

bool stringStartWith(const char *src, const char *prefix)
{
    while(*src != 0 && *prefix !=0 && *src == *prefix)
    {
        ++src;
        ++prefix;
    }
    
    return *prefix == 0;
}

bool stringEndWith(const char *src, const char *postfix)
{
    size_t srcLen = strlen(src);
    size_t dstLen = strlen(postfix);
    
    if(srcLen >= dstLen)
    {
        return stringStartWith(src + srcLen - dstLen, postfix);
    }
    return false;
}

bool listDir(const std::string &path, std::vector<std::string> &files)
{
    std::string filePath = path;
    formatPathNoEndSlash(filePath);

#ifdef __APPLE__
    DIR *dp;
    struct dirent *entry;
    if((dp = opendir(filePath.c_str())) == NULL)
    {
        return false;
    }
    
    while((entry = readdir(dp)) != NULL)
    {
        if(strcmp(entry->d_name, ".") == 0 ||
           strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }
        
        files.push_back(std::string(entry->d_name, entry->d_namlen));
    }
    closedir(dp);
    return true;
#else
    return false;
#endif
}

bool isFile(const std::string &path)
{
    bool ret = false;
#ifdef __APPLE__
    struct stat statbuf;
    if(0 == lstat(path.c_str(), &statbuf))
    {
        ret = (S_ISREG(statbuf.st_mode));
    }
#endif
    return ret;
}

bool isDir(const std::string &path)
{
    bool ret = false;
#ifdef __APPLE__
    struct stat statbuf;
    if(0 == lstat(path.c_str(), &statbuf))
    {
        ret = (S_ISDIR(statbuf.st_mode));
    }
#endif
    return ret;
}

bool isExist(const std::string &path)
{
    bool ret = false;
#ifdef __APPLE__
    ret = (access(path.c_str(), F_OK) == 0);
#endif
    return ret;
}


void normalizePath(std::string &path)
{
    if(path.empty())
    {
        return;
    }
    
    formatPathNoEndSlash(path);
    
    std::string ret;
    ret.reserve(path.size());
    
    std::vector<size_t> dirs;
    char ch;
    bool startWithSlash = path[0] != SLASH_CHAR;
    for(size_t i = 0; i < path.size(); ++i)
    {
        ch = path[i];
        
        if(ch == SLASH_CHAR)
        {
            if(!startWithSlash) // may be "//" or "/./"
            {
                ret += SLASH_CHAR;
                dirs.push_back(i);
            }
            startWithSlash = true;
            continue;
        }
        
        if(startWithSlash && ch == '.')
        {
            if(i + 1 >= path.size()) // '/.'
            {
                continue;
            }
            else if(path[i + 1] == SLASH_CHAR) // '/./'
            {
                continue;
            }
            else if(path[i + 1] == '.') // '/..'
            {
                if(i + 2 >= path.size() || path[i + 2] == SLASH_CHAR) 
                {
                    //this is a valid path '..'
                    //so we need jump to parent path.
                    if(dirs.size() > 1)
                    {
                        dirs.pop_back();
                    }
                    
                    if(dirs.size() > 1)
                    {
                        ret.erase(dirs.back() + 1);
                    }
                    i += 2;
                    continue;
                }
            }
        }
        
        ret += ch;
        startWithSlash = false;
    }
    
    path = ret;
}

std::string number2str(int v)
{
    std::ostringstream ss;
    ss << v;
    return ss.str();
}

static std::string s_exePath;

std::string getExePath()
{
    if(s_exePath.empty())
    {
        char buffer[1024];
        uint32_t length = sizeof(buffer);
#ifdef __APPLE__
        if(0 == _NSGetExecutablePath(buffer, &length))
        {
            s_exePath = getFilePath(buffer);
        }
#elif defined(WIN32)
        GetModuleFileNameA(0, buffer, length);
        s_exePath = getFilePath(buffer);
#endif
    }
    return s_exePath;
}

std::string getAppResPath()
{
#ifdef __APPLE__
    std::string path = getExePath();
    return joinPath(getFilePath(path), "Resources");
#else
    return getAppModulePath();
#endif
}

std::string getAppModulePath()
{
#ifdef __APPLE__
    // retpath/xxx.app/Contents/MacOS
    return getParentPath(getExePath(), 3);
#else
    return getExePath();
#endif
}
