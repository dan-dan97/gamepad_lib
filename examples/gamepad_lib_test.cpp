#include <gamepad_lib.hpp>
#include <boost/thread/thread.hpp>
#include <iostream>

int main (int argc, char *argv[])
{
    Gamepad gamepad; //First found gamepad

    //Use key codes from file "linux/input.h", they are different from ASCII codes

    while (!gamepad.keyPush(KEY_SELECT)){
        for(int i = 0; i < KEY_CNT; i++){
            if(gamepad.keyPush(i)) std::cout << "Pushed key " << i << std::endl;
	    }

        boost::this_thread::sleep(boost::posix_time::milliseconds(50));
    }
    return 0;
}
