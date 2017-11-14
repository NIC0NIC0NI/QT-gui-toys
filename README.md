# Pinyin-editor
A text editor that converts "pin1yin1" to "pīnyīn", using Qt based GUI.

At first, I wanted to make a convenient tool for inputing Pinyin with tone marks. However, input methods have so many interactions with OS that it's difficult to debug and write cross-platform code. Thus this text editor is made. 

Window 7 64bit with MSVC is tested. The user interface is based on Qt, so it should be able to compile and run on other platforms.

## Compile

```
mkdir build
cd build
qmake -o Makefile ../PinyinEditor.pro [options]
make
```

In `[options]`, you can use `-spec` to choose the compiler.

For Windows, if compiling with MSVC, replace `make` with `nmake`; if compiling with MingW-G++, replace `make` with `mingw32-make`.

## Potential issues

### Smooth interaction

The converted text is displayed on the right. Whenver the input is edited, the output will be completelly recomputed after a short delay (currently 0.15s, you can edit the macro in `pinyin_editor.h`). The delay was supposed to reduce the times of computations while the user is typing fast, at the expense of smoothness. If better update mechanism is explored, the delay can be removed. 

### Cross-platform

Compilation on different platforms should be tested.

### Rich text support

Rich text is supported, however, only text with different fonts and colors are tested. Lists and tables are not tested with enough cases.