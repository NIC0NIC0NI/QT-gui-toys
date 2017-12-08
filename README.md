# Pinyin-Bianjiqi

A text editor that converts "pin1yin1" to "pīnyīn", using Qt based GUI.

At first, I wanted to make a convenient tool for inputing Pinyin with tone marks. However, input methods have so many interactions with OS that it's difficult to debug and write cross-platform code. Thus this text editor is made as a alternative.

## Compile

```
mkdir build
cd build
qmake -o Makefile ../PinyinEditor.pro [options]
make
```

For Windows, if compiling with MSVC, replace `make` with `nmake`; if compiling with MingW-G++, replace `make` with `mingw32-make`.

### Tested platforms

* Windows 7, 64-bits (MSVC / MingW)
* Ubuntu 16, 64-bits (GCC / Clang)

## Potential issues

### Icon of the executable

Setting the icon is platform specific. In order not to upload too many binary files such as `.ico` and `icns`, the repository here does not change the icon of the executable.

### Smooth interaction

The converted text is displayed on the right. Whenver the input is edited, the output will be completelly recomputed after a short delay (currently 0.15s, you can edit the macro in `pinyin_editor.h`). The delay was supposed to reduce the times of computations while the user is typing fast, at the expense of smoothness. If better update mechanism is explored, the delay can be removed. 

### Cross-platform

Different desktop platforms should be tested, especially Mac OSX.

### Rich text support

Rich text is supported, however, only text with different fonts and colors are tested. Lists and tables are not tested with enough cases.
