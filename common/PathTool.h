#ifndef PATH_TOOL_H
#define PATH_TOOL_H

#include <cstdlib>
#include <string>
#include <vector>

#ifdef WIN32
#  define SLASH_CHAR '\\'
#  define INV_SLASH_CHAR '/'
#else
#  define SLASH_CHAR '/'
#  define INV_SLASH_CHAR '\\'
#endif

void formatSlash(std::string &path);
void formatPath(std::string &path);
void formatPathNoEndSlash(std::string &path);
void formatPathWithEndSlash(std::string &path);

void appendPathSlash(std::string &path);
void trimPathSlash(std::string &path);

std::string getFileExt(const std::string &filename);
void removeFileExt(std::string &name);

std::string getFilePath(const std::string &filename);
std::string getFileName(const std::string &filename);
std::string joinPath(const std::string &a, const std::string &b);
std::string getParentPath(const std::string &filename, int tier);

bool stringStartWith(const char *src, const char *prefix);
bool stringEndWith(const char *src, const char *postfix);

void normalizePath(std::string &path);

bool listDir(const std::string &path, std::vector<std::string> &files);
bool isFile(const std::string &path);
bool isDir(const std::string &path);
bool isExist(const std::string &path);
bool isAbsolutePath(const std::string &path);

std::string getExePath();
std::string getAppResPath();
std::string getAppModulePath();

#endif //PATH_TOOL_H
