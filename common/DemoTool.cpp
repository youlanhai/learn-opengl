#include "DemoTool.h"
#include "PathTool.h"

std::string findResPath()
{
    std::string rootPath = getExePath();
    std::string resPath;
    while(!rootPath.empty() && !isDir(resPath = joinPath(rootPath, "res")))
    {
        rootPath = getFilePath(rootPath);
    }
    return resPath;
}
