#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "LibCpcEmu_global.h"
#include <QtCore\qobject.h>

#include <QtCore\qmap.h>
#include "types.h"



class LIBCPCEMUSHARED_EXPORT Keyboard : public QObject
{
    Q_OBJECT

public:
    struct CpcKey
    {
        byte_t keyNum;
        byte_t row;
        byte_t bit;
        bool shiftPressed;
    };

    explicit Keyboard(QObject* parent = 0);

    void reset();

    void setRow(byte_t row);
    byte_t pressedKey() const;

    virtual bool eventFilter(QObject* watched, QEvent* event);

private:
    void keyPressEvent(int key, Qt::KeyboardModifiers modifier);
    void keyReleaseEvent(int key, Qt::KeyboardModifiers modifier);

    static const byte_t MAXTRIX_ROWS = 10;

    byte_t m_currentRow;
    byte_t m_keymatrix[MAXTRIX_ROWS];
    QMap<int, CpcKey> keyMapping;
};

#endif // KEYBOARD_H
