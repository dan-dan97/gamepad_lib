#include <gamepad_lib.hpp>
#include <sys/file.h>
#include <iostream>
#include <string>
#include <algorithm>
#include <cstdio>
#include <limits>
#include <errno.h>
#include <boost/thread/thread.hpp>

int Gamepad::initGamepad(unsigned int gamepadNumber)
{
    const int bufferSize = 1024;
    static std::string filesNames[maxGamepadsNumber];
    char buffer[bufferSize];
    FILE *commandFile;
    int gamepadsNumber = 0;
    unsigned long findRes1, findRes2;
    bool deviceNameIsGood, deviceEventsAreGood;

    if ((commandFile = popen("lsinput", "r")) != NULL) {
        while(gamepadsNumber < maxGamepadsNumber) {
            if(fgets(buffer, bufferSize, commandFile) == NULL)
                break;
            filesNames[gamepadsNumber] = buffer;
            filesNames[gamepadsNumber].erase(filesNames[gamepadsNumber].length() - 1, 1);
            deviceNameIsGood = 0;
            deviceEventsAreGood = 0;
            while(1){
                fgets(buffer, bufferSize, commandFile);
                if(buffer[0] == 10 && buffer[1] == 0) break;
                std::string str = buffer;
                findRes1 = str.find("name");
                findRes2 = str.find(':');
                if(findRes1 != std::string::npos && findRes2 != std::string::npos && findRes1 < findRes2)
                {
                    str.erase(0, str.find('\"') + 1);
                    str.erase(str.find('\"'), 2);
                    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
                    if(str.find("gamepad") != std::string::npos){
                        deviceNameIsGood = 1;
                        continue;
                    }
                }
                findRes1 = str.find("bits ev");
                findRes2 = str.find(':');
                if(findRes1 != std::string::npos && findRes2 != std::string::npos && findRes1 < findRes2)
                    if(str.find("EV_SYN") != std::string::npos && str.find("EV_KEY") != std::string::npos && str.find("EV_ABS") != std::string::npos){
                        deviceEventsAreGood = 1;
                        continue;
                    }
            }
            if(deviceNameIsGood && deviceEventsAreGood)gamepadsNumber++;
        }
        if((gamepadNumber >= gamepadsNumber && gamepadsNumber != 0))
        {
            if(!wasInitialized)std::cout << "Invalid gamepad number!" << std::endl;
            gamepadNumber = -1;
        }
        if(gamepadsNumber == 0)
        {
            if(!wasInitialized)std::cout << "Error reading gamepads list! Maybe you need root access or input-utils is not installed" << std::endl;
            gamepadNumber = -1;
        }
    }
    else {
        std::cout << "Error running lsinput" << std::endl;
        gamepadNumber = -1;
    }
    pclose(commandFile);

    return initGamepadByPath(gamepadNumber == -1 ? NULL : filesNames[gamepadNumber].c_str());
}

int Gamepad::initGamepadByPath(const char* eventHandler)
{
    for(int i = 0; i < maxKeysNumber; i++) {
        keyDownMap[i] = 0;
        keyPushMap[i] = 0;
    }
    for(int i = 0; i < maxAxisNumber; i++)
        axisStateMap[i] = 0;

    if(!eventHandler) return -1;

    file = open (eventHandler, O_RDONLY|O_NONBLOCK); //WTF?
    if (file == -1){
        std::cout << "Error opening gamepad file! Maybe you need unroot it" << std::endl;
        return -1;
    }

    if(!wasInitialized){
        wasInitialized = 1;
        pthread_create(&updatingGamepadStateThread, NULL, updatingGamepadState, (void *)this);
    }
    return 0;
}

void* Gamepad::updatingGamepadState(void *arg)
{
    Gamepad& gamepad = *(Gamepad*)arg;
    struct input_event events[maxEventsNumber];
    int bytesRead;
    bool deviceConnected = 1;

    while(true) {
        boost::posix_time::ptime iterationTime = boost::posix_time::microsec_clock::local_time();
        boost::posix_time::time_duration delay_duration;

        if(deviceConnected)
        {
            bytesRead = read(gamepad.file, (void *) &events, sizeof(events));
            if(errno == ENODEV){
                close(gamepad.file);
                for(int i = 0; i < maxKeysNumber; i++) {
                    gamepad.keyDownMap[i] = 0;
                    gamepad.keyPushMap[i] = 0;
                }
                for(int i = 0; i < maxKeysNumber; i++)
                    gamepad.axisStateMap[i] = 0;
                deviceConnected = 0;
                continue;
            }

            int eventsNumber = bytesRead / (int) sizeof(input_event);
            for (int i = 0; i < eventsNumber; i++){
                if (events[i].type == EV_KEY && events[i].code >= 0 && events[i].code < maxKeysNumber) {
                    if(gamepad.keyDownMap[events[i].code] == 0 && events[i].value == 1)
                        gamepad.keyPushMap[events[i].code] = 1;
                    gamepad.keyDownMap[events[i].code] = events[i].value;
                    if(gamepad.keyDownMap[events[i].code] == 1 && gamepad.keyPushMap[events[i].code] == 1)
                        gamepad.lastKeyPushTime[events[i].code] = boost::posix_time::microsec_clock::local_time();
                }
                if (events[i].type == EV_ABS && events[i].code >= 0 && events[i].code < maxAxisNumber) {
                    gamepad.axisStateMap[events[i].code] = events[i].value;
                }
            }
            delay_duration = boost::posix_time::milliseconds(1.0 / updatingGamepadStateFrequency * 1000);
        }
        else {
            if(!gamepad.initGamepad(0)) {
                deviceConnected = 1;
                delay_duration = boost::posix_time::milliseconds(1.0 / updatingGamepadStateFrequency * 1000);
            }
            else
                delay_duration = boost::posix_time::milliseconds(1.0 / updatingGamepadsListFrequency * 1000);
        }
        delay_duration -= (boost::posix_time::microsec_clock::local_time() - iterationTime);
        if(delay_duration.total_milliseconds() > 0)
            boost::this_thread::sleep(delay_duration);
    }
}

Gamepad::Gamepad(unsigned int gamepadNumber)
{
    wasInitialized = 0;
    initGamepad(gamepadNumber);
}

bool Gamepad::keyDown(int key)
{
    if(key >= 0 && key < maxKeysNumber)
        return keyDownMap[key];
    else return 0;
}

bool Gamepad::keyPush(int key)
{
    if(key >= 0 && key < maxKeysNumber) {
        bool res = keyPushMap[key];
        keyPushMap[key] = 0;
        if(res == 1 && (boost::posix_time::microsec_clock::local_time() - lastKeyPushTime[key]).total_milliseconds() > keyPushSavingDurationMillis)
            res = 0;
        return res;
    }
    else return 0;
}

int Gamepad::axisState(int axis)
{
    if(axis >= 0 && axis < maxAxisNumber)
        return axisStateMap[axis];
    else return 0;
}

Gamepad::~Gamepad()
{
    pthread_cancel(updatingGamepadStateThread);
    pthread_join(updatingGamepadStateThread, NULL);
    close(file);
