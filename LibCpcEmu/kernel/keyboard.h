#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "LibCpcEmu_global.h"
#include <QtCore\qobject.h>

#include <QtCore\qmap.h>
#include "types.h"


enum
{
    CPC_CURSOR_UP,
    CPC_CURSOR_RIGHT,
    CPC_CURSOR_DOWN,
    CPC_F9,
    CPC_F6,
    CPC_F3,
    CPC_ENTER,
    CPC_KEYPAD_PERIOD,
    CPC_CURSOR_LEFT,
    CPC_COPY,
    CPC_F7,
    CPC_F8,
    CPC_F5,
    CPC_F1,
    CPC_F2,
    CPC_F0,
    CPC_CLR,
    CPC_BRACKET_LEFT,
    CPC_BRACE_LEFT,
    CPC_RETURN,
    CPC_BRACKET_RIGHT,
    CPC_BRACE_RIGHT,
    CPC_F4,
    CPC_SHIFT,
    CPC_BACKSLASH,
    CPC_GRAVE,
    CPC_CTRL,
    CPC_CIRCUMFLEX,
    CPC_POUND,
    CPC_MINUS,
    CPC_EQUAL,
    CPC_AT,
    CPC_BAR,
    CPC_P,
    CPC_SEMICOLON,
    CPC_PLUS,
    CPC_COLON,
    CPC_ASTERISK,
    CPC_SLASH,
    CPC_QUESTIONMARK,
    CPC_PERIOD,
    CPC_GREATER,
    CPC_0,
    CPC_UNDERSCORE,
    CPC_9,
    CPC_PARENTHESIS_RIGHT,
    CPC_O,
    CPC_I,
    CPC_L,
    CPC_K,
    CPC_M,
    CPC_COMMA,
    CPC_LESS,
    CPC_8,
    CPC_PARENTHESIS_LEFT,
    CPC_7,
    CPC_APOSTROPHE,
    CPC_U,
    CPC_Y,
    CPC_H,
    CPC_J,
    CPC_N,
    CPC_SPACE,
    CPC_6,
    CPC_AMPERSAND,
    CPC_5,
    CPC_PERCENT,
    CPC_R,
    CPC_T,
    CPC_G,
    CPC_F,
    CPC_B,
    CPC_V,
    CPC_4,
    CPC_DOLLAR,
    CPC_3,
    CPC_NUMBERSIGN,
    CPC_E,
    CPC_W,
    CPC_S,
    CPC_D,
    CPC_C,
    CPC_X,
    CPC_1,
    CPC_EXCLAMATIONMARK,
    CPC_2,
    CPC_DOUBLEQUOTE,
    CPC_ESC,
    CPC_Q,
    CPC_TAB,
    CPC_A,
    CPC_CAPS_LOCK,
    CPC_Z,
    CPC_JOY0_UP,
    CPC_JOY0_DOWN,
    CPC_JOY0_LEFT,
    CPC_JOY0_RIGHT,
    CPC_JOY0_FIRE_2,
    CPC_JOY0_FIRE_1,
    CPC_BACKSPACE,
    CPC_LAST_KEY
};


class LIBCPCEMUSHARED_EXPORT Keyboard : public QObject
{
    Q_OBJECT

public:
    explicit Keyboard(QObject* parent = 0);

    void reset();

    void setRow(byte_t row);
    byte_t pressedKey() const;

    virtual bool eventFilter(QObject* watched, QEvent* event);

private:
    struct CpcKey
    {
        byte_t keyNum;
        byte_t row;
        byte_t bit;
        bool shiftPressed;
    };

    void keyPressEvent(int key, Qt::KeyboardModifiers modifier);
    void keyReleaseEvent(int key, Qt::KeyboardModifiers modifier);

    static const byte_t MAXTRIX_ROWS = 10;

    byte_t m_currentRow;
    byte_t m_keymatrix[MAXTRIX_ROWS];
    QMap<int, CpcKey> keyMapping;

    static const CpcKey cpcKeyList[CPC_LAST_KEY];
};

#endif // KEYBOARD_H
