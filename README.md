# learn-opengl
learning opengl

# 如何运行
需要安装cmake2.8或更高版本。

## mac
1. 执行`thirdparty/build.py`，编译依赖文件
2. 创建并进入文件夹`build`，执行`cmake -GXcode ..`
3. 打开`LearnOpenGL.xcodeproj`
4. 编译。选择要观看的target，然后点击运行。

## windows
### 额外环境配置
+ Visual Studio 2013或更高版本
+ 需要安装Python 2.7，并将python.exe加入PATH环境变量
+ 具备OpenGL 2.0或更高版本的驱动

### 构建步骤
1. 从Visual Studio菜单中打开“开发者命令行”，执行`thirdparty/build.py`，用于构建依赖库
2. 创建文件夹`build`，用命令行进入`build`文件夹，执行`cmake -G"Visual Studio xxx" ..`。“xxx”为安装的Visual Studio版本
3. 打开build目录下的`LearnOpenGL.sln`
4. 编译。选择要执行的工程，在其右键菜单中选择“设为启动项”，然后可以执行了。

