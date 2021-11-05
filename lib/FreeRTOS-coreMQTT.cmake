# Add ${CMAKE_CURRENT_LIST_DIR}rary cpp files

add_library(coreMQTT STATIC)
include("lib/coreMQTT/mqttFilePaths.cmake")

target_sources(coreMQTT PUBLIC
	${MQTT_SOURCES}
	${MQTT_SERIALIZER_SOURCES}
)

target_include_directories(coreMQTT PUBLIC 
	${MQTT_INCLUDE_PUBLIC_DIRS}
	${COREMQTT_PORT_PATH}
)


# Add the standard ${CMAKE_CURRENT_LIST_DIR}rary to the build
target_link_libraries(coreMQTT FreeRTOS lwesp pico_stdlib)