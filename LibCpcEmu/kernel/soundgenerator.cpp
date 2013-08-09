#include "soundgenerator.h"

#include <cmath>

#include <QDebug>
#include <QIODevice>

static QList<QList<byte_t> > ENVELOPE_SHAPES;

static const int MAX_FINAL_SAMPLE_VALUE = 255;

SoundGenerator::SoundGenerator(QObject *parent)
    : QObject(parent)
    , m_clockCount(0)
    , m_function(Inactive)
    , m_device(0)
    , m_timer(new QTimer(this))
{
    m_channelA.fineTune = 0;
    m_channelA.coarseTune = 0;
    m_channelA.amplitudeLevel = 0;
    m_channelA.mixValue = 0;
    m_channelA.toneOutput = 0;
    m_channelA.counter = 0;
    m_channelB.fineTune = 0;
    m_channelB.coarseTune = 0;
    m_channelB.amplitudeLevel = 0;
    m_channelB.mixValue = 0;
    m_channelB.toneOutput = 0;
    m_channelB.counter = 0;
    m_channelC.fineTune = 0;
    m_channelC.coarseTune = 0;
    m_channelC.amplitudeLevel = 0;
    m_channelC.mixValue = 0;
    m_channelC.toneOutput = 0;
    m_channelC.counter = 0;
    m_envelope.fineTune = 0;
    m_envelope.coarseTune = 0;
    m_envelope.counter = 0;
    m_envelope.shapePosition = 0;
    m_envelope.shouldHold = true;
    m_mixerA.isChannelNoiseOn = false;
    m_mixerA.isChannelToneOn = false;
    m_mixerB.isChannelNoiseOn = false;
    m_mixerB.isChannelToneOn = false;
    m_mixerC.isChannelNoiseOn = false;
    m_mixerC.isChannelToneOn = false;
    m_noise.period = 0;
    m_noise.rng = 1;
    m_noise.noiseOutput = 0xff;
    m_noise.counter = 0;

    QList<byte_t> shape;
    shape << 15 << 14 << 13 << 12 << 11 << 10 << 9 << 8 << 7 << 6 << 5 << 4 << 3 << 2 << 1 << 0;
    ENVELOPE_SHAPES.append(shape);
    ENVELOPE_SHAPES.append(shape);
    ENVELOPE_SHAPES.append(shape);
    ENVELOPE_SHAPES.append(shape);
    shape.clear();
    shape << 0 << 1 << 2 << 3 << 4 << 5 << 6 << 7 << 8 << 9 << 10 << 11 << 12 << 13 << 14 << 15 << 0;
    ENVELOPE_SHAPES.append(shape);
    ENVELOPE_SHAPES.append(shape);
    ENVELOPE_SHAPES.append(shape);
    ENVELOPE_SHAPES.append(shape);
    shape.clear();
    shape << 15 << 14 << 13 << 12 << 11 << 10 << 9 << 8 << 7 << 6 << 5 << 4 << 3 << 2 << 1 << 0;
    ENVELOPE_SHAPES.append(shape);
    ENVELOPE_SHAPES.append(shape);
    shape.clear();
    shape << 15 << 14 << 13 << 12 << 11 << 10 << 9 << 8 << 7 << 6 << 5 << 4 << 3 << 2 << 1 << 0
          << 0 << 1 << 2 << 3 << 4 << 5 << 6 << 7 << 8 << 9 << 10 << 11 << 12 << 13 << 14 << 15;
    ENVELOPE_SHAPES.append(shape);
    shape.clear();
    shape << 15 << 14 << 13 << 12 << 11 << 10 << 9 << 8 << 7 << 6 << 5 << 4 << 3 << 2 << 1 << 0 << 15;
    ENVELOPE_SHAPES.append(shape);
    shape.clear();
    shape << 0 << 1 << 2 << 3 << 4 << 5 << 6 << 7 << 8 << 9 << 10 << 11 << 12 << 13 << 14 << 15;
    ENVELOPE_SHAPES.append(shape);
    ENVELOPE_SHAPES.append(shape);
    shape.clear();
    shape << 0 << 1 << 2 << 3 << 4 << 5 << 6 << 7 << 8 << 9 << 10 << 11 << 12 << 13 << 14 << 15
          << 15 << 14 << 13 << 12 << 11 << 10 << 9 << 8 << 7 << 6 << 5 << 4 << 3 << 2 << 1 << 0;
    ENVELOPE_SHAPES.append(shape);
    shape.clear();
    shape << 0 << 1 << 2 << 3 << 4 << 5 << 6 << 7 << 8 << 9 << 10 << 11 << 12 << 13 << 14 << 15 << 0;
    ENVELOPE_SHAPES.append(shape);

    m_envelope.currentShape = ENVELOPE_SHAPES[0];

    m_audioBuffer.resize(882000);
    m_bufferPos = 0;
    m_bufferPtr = reinterpret_cast<qint8 *>(m_audioBuffer.data());

    connect(m_timer, SIGNAL(timeout()), SLOT(timerExpired()));
    m_timer->start(20);
}

void SoundGenerator::run()
{
    static int nextCC = 0;
    m_clockCount++;

    // every 16 clock cycles => 62,5 kHz
    if ((m_clockCount & 0x0f) == 0)
    {
        updateEnvelope();
        updateNoise();

        updateChannel(m_channelA, m_mixerA);
        updateChannel(m_channelB, m_mixerB);
        updateChannel(m_channelC, m_mixerC);

        *m_bufferPtr++ = (m_channelC.mixValue & 0xff) - 128;
        m_bufferPos += 1;
        if (m_bufferPos >= m_audioBuffer.length())
        {
           m_bufferPtr = reinterpret_cast<qint8 *>(m_audioBuffer.data());
           m_bufferPos = 0;
        }

        *m_bufferPtr++ = (m_channelA.mixValue & 0xff) - 128;
        m_bufferPos += 1;
        if (m_bufferPos >= m_audioBuffer.length())
        {
           m_bufferPtr = reinterpret_cast<qint8 *>(m_audioBuffer.data());
           m_bufferPos = 0;
        }

        m_channelA.mixValue = 0;
        m_channelB.mixValue = 0;
        m_channelC.mixValue = 0;
    }
}

void SoundGenerator::setAudioDevice(QIODevice *device)
{
    m_device = device;
}

void SoundGenerator::selectFunction(byte_t value)
{
    byte_t bits = ((value & 0xc0) >> 6);
    m_function = (Function)bits;
}

void SoundGenerator::write(byte_t value)
{
    switch (m_function)
    {
        case WriteRegister:
            writeToRegister(value);
            break;

        case SelectRegister:
            m_currentRegister = value & 0x0f;
            break;
    }
}

void SoundGenerator::timerExpired()
{
    if (m_device)
    {
        // deep copy of audio buffer
        QByteArray audioBuffer(m_audioBuffer.constData(), m_bufferPos);

        m_device->write(audioBuffer);

        m_bufferPos = 0;
        m_bufferPtr = reinterpret_cast<qint8 *>(m_audioBuffer.data());
    }
}

void SoundGenerator::writeToRegister(byte_t value)
{
    switch (m_currentRegister)
    {
        case 0x00:
            m_channelA.fineTune = value;
            break;

        case 0x01:
            m_channelA.coarseTune = value;
            break;

        case 0x02:
            m_channelB.fineTune = value;
            break;

        case 0x03:
            m_channelB.coarseTune = value;
            break;

        case 0x04:
            m_channelC.fineTune = value;
            break;

        case 0x05:
            m_channelC.coarseTune = value;
            break;

        case 0x06:
            m_noise.period = value;
            break;

        case 0x07:
            m_mixerA.isChannelToneOn = ((value & 0x01) == 0);
            m_mixerB.isChannelToneOn = ((value & 0x02) == 0);
            m_mixerC.isChannelToneOn = ((value & 0x04) == 0);

            m_mixerA.isChannelNoiseOn = ((value & 0x08) == 0);
            m_mixerB.isChannelNoiseOn = ((value & 0x10) == 0);
            m_mixerC.isChannelNoiseOn = ((value & 0x20) == 0);
            break;

        case 0x08:
            m_channelA.amplitudeLevel = value & 0x0f;
            m_mixerA.useEnvelopeVolume = ((value & 0x10) != 0);
            break;

        case 0x09:
            m_channelB.amplitudeLevel = value & 0x0f;
            m_mixerB.useEnvelopeVolume = ((value & 0x10) != 0);
            break;

        case 0x0a:
            m_channelC.amplitudeLevel = value & 0x0f;
            m_mixerC.useEnvelopeVolume = ((value & 0x10) != 0);
            break;

        case 0x0b:
            m_envelope.fineTune = value;
            break;

        case 0x0c:
            m_envelope.coarseTune = value;
            break;

        case 0x0d:
            m_envelope.shapeControl = value;
            m_envelope.currentShape = ENVELOPE_SHAPES[value];
            m_envelope.shapePosition = 0;
            m_envelope.shouldHold = ( ( value & 0x01 ) != 0 ) || value < 8;
            break;
    }
}

void SoundGenerator::updateNoise()
{
    m_noise.counter++;
    if (m_noise.counter >= m_noise.period)
    {
        if (((m_noise.rng + 1) & 2) != 0)
        {
            m_noise.noiseOutput ^= 0xff;
        }

        if ((m_noise.rng & 1) != 0)
        {
            m_noise.rng ^= 0x24000;
        }

        m_noise.rng >>= 1;
        m_noise.counter = 0;
    }
}

void SoundGenerator::updateEnvelope()
{
    word_t period = (m_envelope.coarseTune << 8) + m_envelope.fineTune;

    m_envelope.counter++;
    if (m_envelope.counter >= period)
    {
        m_envelope.counter = 0;
        m_envelope.volumeLevel = m_envelope.currentShape[m_envelope.shapePosition];

        m_envelope.shapePosition++;
        if (m_envelope.shapePosition == m_envelope.currentShape.length())
        {
            if (m_envelope.shouldHold)
            {
                m_envelope.shapePosition--;
            }
            else
            {
                m_envelope.shapePosition = 0;
            }
        }
    }
}

void SoundGenerator::updateChannel(Channel& channel, Mixer mixer)
{
    word_t period = ((channel.coarseTune & 0x0f) << 8) + channel.fineTune;

    channel.counter++;
    if (channel.counter >= period)
    {
        channel.toneOutput ^= 0xff;
        channel.counter = 0;
    }

    bool output = false;

    if (mixer.isChannelNoiseOn)
    {
        if (m_noise.noiseOutput != 0)
        {
            output = true;
        }
    }
    if (mixer.isChannelToneOn)
    {
        if (channel.toneOutput != 0)
        {
            output = true;
        }
    }

    byte_t amplitude = byte_t((double)MAX_FINAL_SAMPLE_VALUE / pow(sqrt((double)2), 15-channel.amplitudeLevel));

    if (output)
    {
        channel.mixValue += mixer.useEnvelopeVolume ? m_envelope.volumeLevel : amplitude;
    }
    else
    {
        if (channel.mixValue > 0)
        {
            channel.mixValue -= mixer.useEnvelopeVolume ? m_envelope.volumeLevel : amplitude;
        }
    }
}
