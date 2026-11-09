cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)
project(pico_keyboard_demo C CXX ASM)
pico_sdk_init()

add_executable(pico_keyboard_demo main.c usb_descriptors.c)
target_include_directories(pico_keyboard_demo PRIVATE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(pico_keyboard_demo PRIVATE pico_stdlib tinyusb_device)
pico_enable_stdio_usb(pico_keyboard_demo 0)
pico_enable_stdio_uart(pico_keyboard_demo 0)
pico_add_extra_outputs(pico_keyboard_demo)
