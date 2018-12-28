
#include <QTextCursor>
#include <QChar>
#include <QList>

#include "common.h"
#include "static_map.h"

static const QChar TABLE_1[12][5] = {
    { 'a', 0x101, 0xE1, 0x1CE, 0xE0 },
    { 'o', 0x14D, 0xF3, 0x1D2, 0xF2 },
    { 'e', 0x113, 0xE9, 0x11B, 0xE8 },
    { 'i', 0x12B, 0xED, 0x1D0, 0xEC },
    { 'u', 0x16B, 0xFA, 0x1D4, 0xF9 },
    { 0xFC, 0x1D6, 0x1D8, 0x1DA, 0x1DC },
    { 'A', 0x100, 0xC1, 0x1CD, 0xC0 },
    { 'O', 0x14C, 0xD3, 0x1D1, 0xD2 },
    { 'E', 0x112, 0xC9, 0x11A, 0xC8 },
    { 'I', 0x12A, 0xCD, 0x1CF, 0xCC },
    { 'U', 0x16A, 0xDA, 0x1D3, 0xD9 },
    { 0xDC, 0x1D5, 0x1D7, 0x1D9, 0x1DB }
};

static const quint8 NONE = 30;
static const quint8 FIRST = 60;
static const quint8 SECOND = 100;

static const quint8 TABLE_2[6][6] = {
    { NONE, FIRST, NONE, FIRST, NONE, NONE },  //a
    { NONE, NONE, NONE, NONE, FIRST, NONE },   //o
    { NONE, NONE, NONE, FIRST, NONE, NONE },   //e
    { NONE, NONE, SECOND, NONE, SECOND, NONE },//i
    { NONE, NONE, SECOND, SECOND, NONE, NONE }, //u
    { NONE, NONE, NONE, NONE, NONE, NONE },    //v
};

static const container::static_map<QChar, size_t, 14> VOWEL_NUMBER{
    {'a',  0}, {'o',  1}, {'e',  2}, {'i',  3}, {'u',  4}, {'v',  5}, {0xFC, 5},
    {'A',  6}, {'O',  7}, {'E',  8}, {'I',  9}, {'U',  10}, {'V',  11}, {0xDC, 11},
};

inline size_t vowelNumber(QChar c) {
    return VOWEL_NUMBER.find(c)->value();
}

inline bool isVowel(QChar c) {
    return VOWEL_NUMBER.find(c) != VOWEL_NUMBER.end();
}

inline int8_t doubleVowelCombinations(QChar a, QChar b) {
    return TABLE_2[vowelNumber(a) % 6][vowelNumber(b) % 6];
}

inline QChar tonedChar(QChar base, QChar tone) {
    return TABLE_1[vowelNumber(base)][tone.digitValue()];
}



struct Replace {
    int pos;
    QChar character;
    Replace(int p, QChar c) :pos(p), character(c) {}
};

class ChangeList {
    QList<Replace> replaces;
    QList<int> removes;
public:
    void replace(int pos, QChar c) {
        Replace r(pos, c);
        this->replaces.append(r);
    }

    void remove(int pos) {
        this->removes.append(pos);
    }

    void commit(QTextDocument * doc) {
        QTextCursor cursor(doc);
        for (const auto& replace : replaces) {
            cursor.setPosition(replace.pos); 
            cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
            auto format = cursor.charFormat();
            cursor.deleteChar();
            cursor.insertText(QString(replace.character));
            cursor.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor);
            cursor.setCharFormat(format);
        }
        for (auto i = removes.crbegin(), end = removes.crend(); i != end; ++i) {
            cursor.setPosition(*i);
            cursor.deleteChar();
        }
    }
};

class PinyinConverter {
private:
    QChar temp[3];
    size_t n;
    ChangeList changes;
    int currentNum;

    void consumeEmpty(QChar c) {
        if (isVowel(c)) {
            this->n = 1;
            this->temp[0] = c;
        }
    }

    void consumeOne(QChar c) {
        if (c == 'N' || c == 'n' || c == 'R' || c == 'r') {
            this->n = 2;
            this->temp[1] = c;
        }
        else if (isVowel(c) && doubleVowelCombinations(this->temp[0], c) != NONE) {
            this->n = 2;
            this->temp[1] = c;
        }
        else if (c >= '0' && c <= '4') {
            this->commitChange(tonedChar(this->temp[0], c), -1);
            this->n = 0;
        }
        else {
            this->n = 0;
            this->consumeEmpty(c);
        }
    }

    void consumeTwo(QChar c) {
        if ((c == 'g' || c == 'G') && (this->temp[1] == 'n' || this->temp[1] == 'N')) {
            this->n = this->n + 1;
            this->temp[2] = c;
        }
        else if (c >= '0' && c <= '4') {
            if (this->temp[1] == 'r' || this->temp[1] == 'R' || this->temp[1] == 'n' || this->temp[1] == 'N') {
                this->commitChange(tonedChar(this->temp[0], c), -2);
            }
            else {
                if (doubleVowelCombinations(this->temp[0], this->temp[1]) == FIRST) {
                    this->commitChange(tonedChar(this->temp[0], c), -2);
                }
                else {
                    this->commitChange(tonedChar(this->temp[1], c), -1);
                }
            }
            this->n = 0;
        }
        else {
            this->n = 0;
            this->consumeEmpty(c);
        }
    }

    void consumeThree(QChar c) {
        if (c >= '0' && c <= '4') {
            this->commitChange(tonedChar(this->temp[0], c), -3);
            this->n = 0;
        }
        else {
            this->n = 0;
            this->consumeEmpty(c);
        }
    }

    void commitChange(QChar c, int pos) {
        this->changes.replace(this->currentNum + pos, c);
        this->changes.remove(this->currentNum);
    }

public:
    PinyinConverter() : n(0), currentNum(0){}

    void consume(QChar c) {
        if (this->n == 0) {
            this->consumeEmpty(c);
        }
        else if (this->n == 1) {
            this->consumeOne(c);
        }
        else if (this->n == 2) {
            this->consumeTwo(c);
        }
        else if (this->n == 3) {
            this->consumeThree(c);
        }
        this->currentNum += 1;
    }
    ChangeList* getChanges() {
        return &changes;
    }
};




void convertPinyin(QTextDocument * doc) {
    PinyinConverter converter;
    for (auto c : doc->toPlainText()) {
        converter.consume(c);
    }
    converter.getChanges()->commit(doc);
}
