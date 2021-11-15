add_executable(${NAME}
        main.cpp
        NetConnection.cpp
        TCPConnection.cpp
        TCPConnectionObserver.cpp
        MQTTCommand.cpp
        MQTTConnection.cpp
        MQTTConnObserver.cpp
        MQTTConnTask.cpp
        MQTTInterface.cpp
        MQTTRouter.cpp
        MQTTConnTask.cpp
        MQTTRouterPing.cpp
        MQTTPingTask.cpp
        MQTTDebug.cpp
        )

# Pull in our pico_stdlib which pulls in commonly used features
target_link_libraries(${NAME} pico_stdlib FreeRTOS coreMQTT lwesp)

# enable usb output, disable uart output
pico_enable_stdio_usb(${NAME} 1)
pico_enable_stdio_uart(${NAME} 0)

# create map/bin/hex file etc.
pico_add_extra_outputs(${NAME})