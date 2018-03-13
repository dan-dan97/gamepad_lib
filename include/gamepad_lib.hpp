#pragma once

#include <linux/input.h>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <pthread.h>

class Gamepad
{
private:

    static const int updatingGamepadStateFrequency = 200;
    static const int updatingGamepadsListFrequency = 2;
    static const int maxKeysNumber = KEY_CNT;
    static const int maxAxisNumber = ABS_CNT;
    static const int maxGamepadsNumber = 64;
    static const int maxEventsNumber = 1024;
    static const int keyPushSavingDurationMillis = 1000;

    int file;
    bool wasInitialized;
    bool keyDownMap[maxKeysNumber];
    bool keyPushMap[maxKeysNumber];
    int axisStateMap[maxAxisNumber];
    boost::posix_time::ptime lastKeyPushTime[maxKeysNumber];
    pthread_t updatingGamepadStateThread;

    int initGamepad(unsigned int gamepadNumber);
    int initGamepadByPath(const char* eventHandler);
    static void* updatingGamepadState(void *arg);

public:

    Gamepad(unsigned int gamepadNumber = 0);
    bool keyDown(int key);
    bool keyPush(int key);
    int axisState(int axis);
    ~Gamepad();
};
