cmake_minimum_required(VERSION 2.8)
project(gamepad_lib)

find_package(Boost COMPONENTS date_time thread REQUIRED)

include_directories(
    include
    ${Boost_INCLUDE_DIR}
)

set(GAMEPAD_LIB_HEADERS include/gamepad_lib.hpp)
set(GAMEPAD_LIB_SOURCES src/gamepad_lib.cpp)

add_library(gamepad_lib SHARED ${GAMEPAD_LIB_HEADERS} ${GAMEPAD_LIB_SOURCES})
target_link_libraries(gamepad_lib pthread ${Boost_LIBRARIES})

add_executable(gamepad_lib_test examples/gamepad_lib_test.cpp)
target_link_libraries(gamepad_lib_test gamepad_lib)

install(FILES ${GAMEPAD_LIB_HEADERS} DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
install(TARGETS gamepad_lib DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(TARGETS gamepad_lib_test DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
