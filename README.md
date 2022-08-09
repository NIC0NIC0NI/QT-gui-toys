# QT-gui-toys

Some toys with QT-based GUI.

## Compile

```
cd ${PROJECT_DIRECTORY}
mkdir build
cd build
qmake -o Makefile ../${PROJECT_NAME}.pro [options]
make
```

For Windows, if compiling with MSVC, replace `make` with `nmake`; if compiling with MingW-G++, replace `make` with `mingw32-make`.

### Tested platforms

* Windows 7, 64-bits (MSVC / MingW)
* Ubuntu 16, 64-bits (GCC / Clang)

## Potential issues

### Icon of the executable

Setting the icon is platform specific. In order not to upload too many binary files such as `.ico` and `icns`, the repository here does not change the icon of the executable.

### Cross-platform

Different desktop platforms should be tested, especially Mac OSX.

## 中文版

使用QT本地化工具生成中文翻译文件`lang/cn.qm`

```
cd ${项目目录}
cd build
lrelease ../${项目名}.pro
```

并在程序启动时，在命令行参数中指定翻译文件的路径

```
./${可执行文件} -l ${路径}/cn.qm
```

UI中使用的一些QT预先设计好的对话框没有中文翻译。