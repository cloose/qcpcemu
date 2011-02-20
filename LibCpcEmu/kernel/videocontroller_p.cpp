class VideoControllerPrivate
{
public:
    VideoControllerPrivate()
        : currentRegister(0)
        , horizontalCounter(0)
    {
    }

    byte_t registers[18];
    quint8 currentRegister;

    quint8 horizontalCounter;
};
