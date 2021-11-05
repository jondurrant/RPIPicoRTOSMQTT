cmake_minimum_required(VERSION 3.12)

# Change your executable name to something creative!
set(NAME PicoMQTT) # <-- Name your project/executable here!

include(pico_sdk_import.cmake)

# Gooey boilerplate
project(${NAME} C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

set(PICO_CXX_ENABLE_EXCEPTIONS 1)
set(PICO_CXX_ENABLE_RTTI 1)

# Initialize the SDK
pico_sdk_init()

set(LWESP_OPTION_OS             "FreeRTOS")
set(LWESP_OPTION_PORT_PATH      ${CMAKE_CURRENT_LIST_DIR}/src/port/lwesp)
set(LWESP_OPTION_USER_CFG_FILE  ${CMAKE_CURRENT_LIST_DIR}/src/config/lwesp_opts.h)

set(FREERTOS_PORT_PATH      ${CMAKE_CURRENT_LIST_DIR}/src/port/FreeRTOS)
set(COREMQTT_PORT_PATH      ${CMAKE_CURRENT_LIST_DIR}/src/port/CoreMQTT)

add_subdirectory(src)


# Include required libraries
include("lib/FreeRTOS-Kernel.cmake")
include("lib/lwesp.cmake")
include("lib/FreeRTOS-coreMQTT.cmake")
#include("lib/dbg.cmake")




#Set up files for the release packages
install(CODE "execute_process(COMMAND $ENV{HOME}/bin/picoDeploy.sh ${CMAKE_CURRENT_BINARY_DIR}/src/${NAME}.elf)")

# Set up files for the release packages
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/src/${NAME}.uf2
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)

set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
set(CPACK_GENERATOR "ZIP" "TGZ")
include(CPack)
