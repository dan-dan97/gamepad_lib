# gamepad_lib
A library to use gamepad

# Linux Installation

1. Install input-utils to use command lsinput showing input devices list and libpthread-stubs0-dev that contains pthread
  * `sudo apt-get install input-utils libpthread-stubs0-dev`

2. Install udev rules to unroot all devices event* files and reload rules
  Navigate to *gamepad_lib* root directory
  * `cd .../gamepad_lib`
  * `sudo cp config/99-input-devices.rules /etc/udev/rules.d/`
  * `sudo udevadm control --reload-rules && udevadm trigger`

3. Compile
  Create build folder
  * `mkdir build && cd build`
  Run cmake
  * `cmake ../`
  Recompile *gamepad_lib*
  * `make clean && make && sudo make install`
  Now the library is installed to /usr/local/include and /usr/local/lib. Example is installed to /usr/local/bin.

4. Setup dynamic libaries
  * `sudo ldconfig`
