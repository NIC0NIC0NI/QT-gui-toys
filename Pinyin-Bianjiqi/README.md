# Pinyin-Bianjiqi

A text editor that converts "pin1yin1" to "pīnyīn".

At first, I wanted to make a convenient tool for inputing Pinyin with tone marks. However, input methods have so many interactions with OS that it's difficult to debug and write cross-platform code. Thus this text editor is made as a alternative.

## Potential issues

### Smooth interaction

The converted text is displayed on the right. Whenver the input is edited, the output will be completelly recomputed after a short delay (currently 0.15s, you can edit the macro in `pinyin_editor.h`). The delay was supposed to reduce the times of computations while the user is typing fast, at the expense of smoothness. If better update mechanism is explored, the delay can be removed. 

### Rich text support

Rich text is supported, however, only text with different fonts and colors are tested. Lists and tables are not tested with enough cases.
