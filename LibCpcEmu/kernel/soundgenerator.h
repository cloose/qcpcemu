#ifndef SOUNDGENERATOR_H
#define SOUNDGENERATOR_H

#include "types.h"
#include <QtCore/qlist.h>
#include <QtCore/qtimer.h>

class QIODevice;


class SoundGenerator : QObject
{
    Q_OBJECT

    struct Mixer
    {
        bool isChannelToneOn;
        bool isChannelNoiseOn;
        bool useEnvelopeVolume;
    };

    struct Channel
    {
        byte_t fineTune;
        byte_t coarseTune;
        byte_t amplitudeLevel;
        byte_t mixValue;
        int toneOutput;
        word_t counter;
    };

    struct Noise
    {
        byte_t period;
        int rng;
        int noiseOutput;
        word_t counter;
    };

    struct Envelope
    {
        byte_t fineTune;
        byte_t coarseTune;
        byte_t shapeControl;
        QList<byte_t> currentShape;
        byte_t shapePosition;
        byte_t volumeLevel;
        word_t counter;
        bool shouldHold;
    };

    enum Function
    {
        Inactive = 0,
        ReadRegister,
        WriteRegister,
        SelectRegister
    };

public:
    explicit SoundGenerator(QObject* parent = 0);

    void run();

    void setAudioDevice(QIODevice* device);

    void selectFunction(byte_t value);
    void write(byte_t value);

private slots:
    void timerExpired();

private:
    void writeToRegister(byte_t value);
    void updateNoise();
    void updateEnvelope();
    void updateChannel(Channel &channel, Mixer mixer);

    unsigned long m_clockCount;
    Function m_function;
    byte_t m_currentRegister;
    Channel m_channelA;
    Mixer m_mixerA;
    Channel m_channelB;
    Mixer m_mixerB;
    Channel m_channelC;
    Mixer m_mixerC;
    Envelope m_envelope;
    Noise m_noise;

    QIODevice* m_device;
    QByteArray m_audioBuffer;
    qint32 m_bufferPos;
    qint8 *m_bufferPtr;
    QTimer *m_timer;
};

#endif // SOUNDGENERATOR_H
