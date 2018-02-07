#ifndef GAMEPAD_LIB_HPP
#define GAMEPAD_LIB_HPP

#include <linux/input.h>
#include <boost/date_time/posix_time/posix_time.hpp>

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

    int initGamepad(const char* eventHandler);
    int initGamepad(int gamepadNumber);
    static void* updatingGamepadState(void *arg);

public:

    Gamepad(const char* eventHandler);
    Gamepad(int gamepadNumber = 0);
    bool keyDown(int key);
    bool keyPush(int key);
    int axisState(int axis);
    ~Gamepad();
};

//If you don`t enable echo after its disabling and the program finishes to work, echo will be switched off in the console, so you need to restart console to switch on echo again or switch it by the command "stty echo"
void echoEnable(bool enable);

void clearInputBuffer();

#endif //GAMEPAD_LIB_HPP
