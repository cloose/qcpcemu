#include "keyboard.h"

#include <QDebug>
#include <QEvent>
#include <QKeyEvent>

const Keyboard::CpcKey Keyboard::cpcKeyList[CPC_LAST_KEY] =
{
    { CPC_CURSOR_UP,         0, 0x01, false },
    { CPC_CURSOR_RIGHT,      0, 0x02, false },
    { CPC_CURSOR_DOWN,       0, 0x04, false },
    { CPC_F9,                0, 0x08, false },
    { CPC_F6,                0, 0x10, false },
    { CPC_F3,                0, 0x20, false },
    { CPC_ENTER,             0, 0x40, false },
    { CPC_KEYPAD_PERIOD,     0, 0x80, false },
    { CPC_CURSOR_LEFT,       1, 0x01, false },
    { CPC_COPY,              1, 0x02, false },
    { CPC_F7,                1, 0x04, false },
    { CPC_F8,                1, 0x08, false },
    { CPC_F5,                1, 0x10, false },
    { CPC_F1,                1, 0x20, false },
    { CPC_F2,                1, 0x40, false },
    { CPC_F0,                1, 0x80, false },
    { CPC_CLR,               2, 0x01, false },
    { CPC_BRACKET_LEFT,      2, 0x02, false },  // [
    { CPC_BRACE_LEFT,        2, 0x02, true },   // {
    { CPC_RETURN,            2, 0x04, false },
    { CPC_BRACKET_RIGHT,     2, 0x08, false },  // ]
    { CPC_BRACE_RIGHT,       2, 0x08, true },   // }
    { CPC_F4,                2, 0x10, false },
    { CPC_SHIFT,             2, 0x20, false },
    { CPC_BACKSLASH,         2, 0x40, false },
    { CPC_GRAVE,             2, 0x40, true },   // `
    { CPC_CTRL,              2, 0x80, false },
    { CPC_CIRCUMFLEX,        3, 0x01, false },  // ^
    { CPC_POUND,             3, 0x01, true },
    { CPC_MINUS,             3, 0x02, false },  // -
    { CPC_EQUAL,             3, 0x02, true },   // =
    { CPC_AT,                3, 0x04, false },  // @
    { CPC_BAR,               3, 0x04, true },   // |
    { CPC_P,                 3, 0x08, false },
    { CPC_SEMICOLON,         3, 0x10, false },  // ;
    { CPC_PLUS,              3, 0x10, true },   // +
    { CPC_COLON,             3, 0x20, false },  // :
    { CPC_ASTERISK,          3, 0x20, true },   // *
    { CPC_SLASH,             3, 0x40, false },  // /
    { CPC_QUESTIONMARK,      3, 0x40, true },   // ?
    { CPC_PERIOD,            3, 0x80, false },  // .
    { CPC_GREATER,           3, 0x80, true },   // >
    { CPC_0,                 4, 0x01, false },
    { CPC_UNDERSCORE,        4, 0x01, true },   // _
    { CPC_9,                 4, 0x02, false },
    { CPC_PARENTHESIS_RIGHT, 4, 0x02, true },   // )
    { CPC_O,                 4, 0x04, false },
    { CPC_I,                 4, 0x08, false },
    { CPC_L,                 4, 0x10, false },
    { CPC_K,                 4, 0x20, false },
    { CPC_M,                 4, 0x40, false },
    { CPC_COMMA,             4, 0x80, false },  // ,
    { CPC_LESS,              4, 0x80, true },   // <
    { CPC_8,                 5, 0x01, false },
    { CPC_PARENTHESIS_LEFT,  5, 0x01, true },   // (
    { CPC_7,                 5, 0x02, false },
    { CPC_APOSTROPHE,        5, 0x02, true },   // '
    { CPC_U,                 5, 0x04, false },
    { CPC_Y,                 5, 0x08, false },
    { CPC_H,                 5, 0x10, false },
    { CPC_J,                 5, 0x20, false },
    { CPC_N,                 5, 0x40, false },
    { CPC_SPACE,             5, 0x80, false },
    { CPC_6,                 6, 0x01, false },
    { CPC_AMPERSAND,         6, 0x01, true },   // &
    { CPC_5,                 6, 0x02, false },
    { CPC_PERCENT,           6, 0x02, true },   // %
    { CPC_R,                 6, 0x04, false },
    { CPC_T,                 6, 0x08, false },
    { CPC_G,                 6, 0x10, false },
    { CPC_F,                 6, 0x20, false },
    { CPC_B,                 6, 0x40, false },
    { CPC_V,                 6, 0x80, false },
    { CPC_4,                 7, 0x01, false },
    { CPC_DOLLAR,            7, 0x01, true },   // $
    { CPC_3,                 7, 0x02, false },
    { CPC_NUMBERSIGN,        7, 0x02, true },   // #
    { CPC_E,                 7, 0x04, false },
    { CPC_W,                 7, 0x08, false },
    { CPC_S,                 7, 0x10, false },
    { CPC_D,                 7, 0x20, false },
    { CPC_C,                 7, 0x40, false },
    { CPC_X,                 7, 0x80, false },
    { CPC_1,                 8, 0x01, false },
    { CPC_EXCLAMATIONMARK,   8, 0x01, true },   // !
    { CPC_2,                 8, 0x02, false },
    { CPC_DOUBLEQUOTE,       8, 0x02, true },   // "
    { CPC_ESC,               8, 0x04, false },
    { CPC_Q,                 8, 0x08, false },
    { CPC_TAB,               8, 0x10, false },
    { CPC_A,                 8, 0x20, false },
    { CPC_CAPS_LOCK,         8, 0x40, false },
    { CPC_Z,                 8, 0x80, false },
    { CPC_JOY0_UP,           9, 0x01, false },
    { CPC_JOY0_DOWN,         9, 0x02, false },
    { CPC_JOY0_LEFT,         9, 0x04, false },
    { CPC_JOY0_RIGHT,        9, 0x08, false },
    { CPC_JOY0_FIRE_2,       9, 0x10, false },
    { CPC_JOY0_FIRE_1,       9, 0x20, false },
    { CPC_BACKSPACE,         9, 0x80, false }
};


Keyboard::Keyboard(QObject* parent)
    : QObject(parent)
    , m_currentRow(0)
{
    reset();
}

void Keyboard::reset()
{
    m_currentRow = 0;

    for (int i = 0; i < MAXTRIX_ROWS; ++i)
    {
        m_keymatrix[i] = 0xff;
    }

    keyMapping.insert(Qt::Key_Up, cpcKeyList[CPC_CURSOR_UP]);
    keyMapping.insert(Qt::Key_Right, cpcKeyList[CPC_CURSOR_RIGHT]);
    keyMapping.insert(Qt::Key_Down, cpcKeyList[CPC_CURSOR_DOWN]);
    keyMapping.insert(Qt::Key_F9, cpcKeyList[CPC_F9]);
    keyMapping.insert(Qt::Key_F6, cpcKeyList[CPC_F6]);
    keyMapping.insert(Qt::Key_F3, cpcKeyList[CPC_F3]);
    keyMapping.insert(Qt::Key_Enter, cpcKeyList[CPC_ENTER]);
    // CPC_KEYPAD_PERIOD
    keyMapping.insert(Qt::Key_Left, cpcKeyList[CPC_CURSOR_LEFT]);
    // CPC_COPY
    keyMapping.insert(Qt::Key_F7, cpcKeyList[CPC_F7]);
    keyMapping.insert(Qt::Key_F8, cpcKeyList[CPC_F8]);
    keyMapping.insert(Qt::Key_F5, cpcKeyList[CPC_F5]);
    keyMapping.insert(Qt::Key_F1, cpcKeyList[CPC_F1]);
    keyMapping.insert(Qt::Key_F2, cpcKeyList[CPC_F2]);
    // CPC_F0
    keyMapping.insert(Qt::Key_Delete, cpcKeyList[CPC_CLR]);
    keyMapping.insert(Qt::Key_BracketLeft, cpcKeyList[CPC_BRACKET_LEFT]);
    keyMapping.insert(Qt::Key_BraceLeft, cpcKeyList[CPC_BRACE_LEFT]);
    keyMapping.insert(Qt::Key_Return, cpcKeyList[CPC_RETURN]);
    keyMapping.insert(Qt::Key_BracketRight, cpcKeyList[CPC_BRACKET_RIGHT]);
    keyMapping.insert(Qt::Key_BraceRight, cpcKeyList[CPC_BRACE_RIGHT]);
    // ...
    keyMapping.insert(Qt::Key_Shift, cpcKeyList[CPC_SHIFT]);
    // ...
    keyMapping.insert(Qt::Key_Control, cpcKeyList[CPC_CTRL]);
    // ...
    keyMapping.insert(Qt::Key_At, cpcKeyList[CPC_AT]);
    keyMapping.insert(Qt::Key_Bar, cpcKeyList[CPC_BAR]);
    keyMapping.insert(Qt::Key_P, cpcKeyList[CPC_P]);
    keyMapping.insert(Qt::Key_Semicolon, cpcKeyList[CPC_SEMICOLON]);
    keyMapping.insert(Qt::Key_Plus, cpcKeyList[CPC_PLUS]);
    keyMapping.insert(Qt::Key_Colon, cpcKeyList[CPC_COLON]);
    keyMapping.insert(Qt::Key_Asterisk, cpcKeyList[CPC_ASTERISK]);
    keyMapping.insert(Qt::Key_Slash, cpcKeyList[CPC_SLASH]);
    keyMapping.insert(Qt::Key_Question, cpcKeyList[CPC_QUESTIONMARK]);
    keyMapping.insert(Qt::Key_Period, cpcKeyList[CPC_PERIOD]);
    keyMapping.insert(Qt::Key_Greater, cpcKeyList[CPC_GREATER]);
    keyMapping.insert(Qt::Key_0, cpcKeyList[CPC_0]);
    keyMapping.insert(Qt::Key_Underscore, cpcKeyList[CPC_UNDERSCORE]);
    keyMapping.insert(Qt::Key_9, cpcKeyList[CPC_9]);
    keyMapping.insert(Qt::Key_ParenRight, cpcKeyList[CPC_PARENTHESIS_RIGHT]);
    keyMapping.insert(Qt::Key_O, cpcKeyList[CPC_O]);
    keyMapping.insert(Qt::Key_I, cpcKeyList[CPC_I]);
    keyMapping.insert(Qt::Key_L, cpcKeyList[CPC_L]);
    keyMapping.insert(Qt::Key_K, cpcKeyList[CPC_K]);
    keyMapping.insert(Qt::Key_M, cpcKeyList[CPC_M]);
    keyMapping.insert(Qt::Key_Comma, cpcKeyList[CPC_COMMA]);
    keyMapping.insert(Qt::Key_Less, cpcKeyList[CPC_LESS]);
    keyMapping.insert(Qt::Key_8, cpcKeyList[CPC_8]);
    keyMapping.insert(Qt::Key_ParenLeft, cpcKeyList[CPC_PARENTHESIS_LEFT]);
    keyMapping.insert(Qt::Key_7, cpcKeyList[CPC_7]);
    keyMapping.insert(Qt::Key_Apostrophe, cpcKeyList[CPC_APOSTROPHE]);
    keyMapping.insert(Qt::Key_U, cpcKeyList[CPC_U]);
    keyMapping.insert(Qt::Key_Y, cpcKeyList[CPC_Y]);
    keyMapping.insert(Qt::Key_H, cpcKeyList[CPC_H]);
    keyMapping.insert(Qt::Key_J, cpcKeyList[CPC_J]);
    keyMapping.insert(Qt::Key_N, cpcKeyList[CPC_N]);
    keyMapping.insert(Qt::Key_Space, cpcKeyList[CPC_SPACE]);
    keyMapping.insert(Qt::Key_6, cpcKeyList[CPC_6]);
    keyMapping.insert(Qt::Key_Ampersand, cpcKeyList[CPC_AMPERSAND]);
    keyMapping.insert(Qt::Key_5, cpcKeyList[CPC_5]);
    keyMapping.insert(Qt::Key_Percent, cpcKeyList[CPC_PERCENT]);
    keyMapping.insert(Qt::Key_R, cpcKeyList[CPC_R]);
    keyMapping.insert(Qt::Key_T, cpcKeyList[CPC_T]);
    keyMapping.insert(Qt::Key_G, cpcKeyList[CPC_G]);
    keyMapping.insert(Qt::Key_F, cpcKeyList[CPC_F]);
    keyMapping.insert(Qt::Key_B, cpcKeyList[CPC_B]);
    keyMapping.insert(Qt::Key_V, cpcKeyList[CPC_V]);
    keyMapping.insert(Qt::Key_4, cpcKeyList[CPC_4]);
    keyMapping.insert(Qt::Key_Dollar, cpcKeyList[CPC_DOLLAR]);
    keyMapping.insert(Qt::Key_3, cpcKeyList[CPC_3]);
    keyMapping.insert(Qt::Key_NumberSign, cpcKeyList[CPC_NUMBERSIGN]);
    keyMapping.insert(Qt::Key_E, cpcKeyList[CPC_E]);
    keyMapping.insert(Qt::Key_W, cpcKeyList[CPC_W]);
    keyMapping.insert(Qt::Key_S, cpcKeyList[CPC_S]);
    keyMapping.insert(Qt::Key_D, cpcKeyList[CPC_D]);
    keyMapping.insert(Qt::Key_C, cpcKeyList[CPC_C]);
    keyMapping.insert(Qt::Key_X, cpcKeyList[CPC_X]);
    keyMapping.insert(Qt::Key_1, cpcKeyList[CPC_1]);
    keyMapping.insert(Qt::Key_Exclam, cpcKeyList[CPC_EXCLAMATIONMARK]);
    keyMapping.insert(Qt::Key_2, cpcKeyList[CPC_2]);
    keyMapping.insert(Qt::Key_QuoteDbl, cpcKeyList[CPC_DOUBLEQUOTE]);
    keyMapping.insert(Qt::Key_Escape, cpcKeyList[CPC_ESC]);
    keyMapping.insert(Qt::Key_Q, cpcKeyList[CPC_Q]);
    keyMapping.insert(Qt::Key_Tab, cpcKeyList[CPC_TAB]);
    keyMapping.insert(Qt::Key_A, cpcKeyList[CPC_A]);
    keyMapping.insert(Qt::Key_CapsLock, cpcKeyList[CPC_CAPS_LOCK]);
    keyMapping.insert(Qt::Key_Z, cpcKeyList[CPC_Z]);
    // ...
    keyMapping.insert(Qt::Key_Backspace, cpcKeyList[CPC_BACKSPACE]);
}

void Keyboard::setRow(byte_t row)
{
    m_currentRow = row;
}

byte_t Keyboard::pressedKey() const
{
    return m_keymatrix[m_currentRow];
}

bool Keyboard::eventFilter(QObject* watched, QEvent* event)
{
    bool handled = false;

    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        keyPressEvent(keyEvent->key(), keyEvent->modifiers());
        handled = true;
    }
    else if (event->type() == QEvent::KeyRelease)
    {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        keyReleaseEvent(keyEvent->key(), keyEvent->modifiers());
        handled = true;
    }
    else
    {
        handled = QObject::eventFilter(watched, event);
    }

    return handled;
}

void Keyboard::keyPressEvent(int key, Qt::KeyboardModifiers modifier)
{
    CpcKey cpcKey = keyMapping[key];

    if (key == Qt::Key_Return)
    {
        qDebug() << "ENTER PRESSED" << cpcKey.row << cpcKey.bit;
    }

    // activate the bit in the keyboard matrix
    m_keymatrix[cpcKey.row] &= ~cpcKey.bit;

    if (key >= Qt::Key_A && key <= Qt::Key_Z)
    {
        modifier & Qt::ShiftModifier ?  m_keymatrix[2] &= 223 : m_keymatrix[2] |= 32;
    }
    else
    {
        cpcKey.shiftPressed ?  m_keymatrix[2] &= 223 : m_keymatrix[2] |= 32;
    }
}

void Keyboard::keyReleaseEvent(int key, Qt::KeyboardModifiers modifier)
{
    CpcKey cpcKey = keyMapping[key];

    // deactivate the bit in the keyboard matrix
    m_keymatrix[cpcKey.row] |= cpcKey.bit;

    if (key >= Qt::Key_A && key <= Qt::Key_Z)
    {
        if (modifier & Qt::ShiftModifier) m_keymatrix[2] |= 32;
    }
    else
    {
        if (cpcKey.shiftPressed) m_keymatrix[2] |= 32;
    }
}

