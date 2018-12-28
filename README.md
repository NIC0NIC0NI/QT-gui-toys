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
