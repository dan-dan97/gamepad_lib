#include <unistd.h>
#include <iostream>
#include <gamepad_lib.hpp>

using std::cout;
using std::cin;
using std::endl;

int main (int argc, char *argv[])
{
    Gamepad gamepad; //First found gamepad

    //Use key codes from file "linux/input.h", they are different from ASCII codes

    while (!gamepad.keyPush(KEY_SELECT)){
        for(int i = 0; i < KEY_CNT; i++){
            if(gamepad.keyPush(i)) cout << "Pushed key " << i << endl;
	    }

        usleep(50000);
    }
    return 0;
}
