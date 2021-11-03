# Add ${CMAKE_CURRENT_LIST_DIR}rary cpp files

add_library(lwesp STATIC)

# Make sure OS name and OS port folder are specified at the same time
if((NOT DEFINED LWESP_OPTION_OS) OR (NOT DEFINED LWESP_OPTION_PORT_PATH))
    message(FATAL_ERROR "Error: LWESP_OPTION_OS or LWESP_OPTION_PORT_PATH is not defined")
endif()

# Include port path for header file lwesp_sys_port.h
target_include_directories(lwesp PUBLIC ${LWESP_OPTION_PORT_PATH}
	${CMAKE_CURRENT_LIST_DIR}/lwesp/lwesp/src/include
)

# Building this library requires OS library
target_link_libraries(lwesp PUBLIC ${LWESP_OPTION_OS})

# Pick up port source folder out of this directory
add_subdirectory(${LWESP_OPTION_PORT_PATH} lwesp_custom_port)


# A place for generated/copied include files (no need to change)
set(CUSTOM_INC_DIR ${CMAKE_CURRENT_BINARY_DIR}/custom_inc)

# If the user provided configuration file, copy it to the custom directory
if (NOT DEFINED LWESP_OPTION_USER_CFG_FILE)
    message(STATUS "Using default configurations defined in lwesp_opts.h")
    target_compile_definitions(lwesp PUBLIC "LWESP_IGNORE_USER_OPTS")
else()
    message(STATUS "Using custom lwesp_opts.h file from ${LWESP_OPTION_USER_CFG_FILE}")
    configure_file(${LWESP_OPTION_USER_CFG_FILE} ${CUSTOM_INC_DIR}/lwesp_opts.h COPYONLY)
    target_include_directories(lwesp PUBLIC ${CUSTOM_INC_DIR})
endif()



# All src files for module
target_sources(lwesp PUBLIC
	${CMAKE_CURRENT_LIST_DIR}/lwesp/lwesp/src/api/lwesp_netconn.c
	#${CMAKE_CURRENT_LIST_DIR}/lwesp/lwesp/src/apps/cayenne/lwesp_cayenne.c
	#${CMAKE_CURRENT_LIST_DIR}/lwesp/lwesp/src/apps/cayenne/lwesp_cayenne_evt.c
	#${CMAKE_CURRENT_LIST_DIR}/lwesp/lwesp/src/apps/http_server/lwesp_http_server.c
	#${CMAKE_CURRENT_LIST_DIR}/lwesp/lwesp/src/apps/http_server/lwesp_http_server_fs.c
	#${CMAKE_CURRENT_LIST_DIR}/lwesp/lwesp/src/apps/http_server/lwesp_http_server_fs_fat.c
	#${CMAKE_CURRENT_LIST_DIR}/lwesp/lwesp/src/apps/http_server/lwesp_http_server_fs_win32.c
	#${CMAKE_CURRENT_LIST_DIR}/lwesp/lwesp/src/apps/mqtt/lwesp_mqtt_client.c
	#${CMAKE_CURRENT_LIST_DIR}/lwesp/lwesp/src/apps/mqtt/lwesp_mqtt_client_api.c
	#${CMAKE_CURRENT_LIST_DIR}/lwesp/lwesp/src/apps/mqtt/lwesp_mqtt_client_evt.c
	${CMAKE_CURRENT_LIST_DIR}/lwesp/lwesp/src/cli/cli.c
	${CMAKE_CURRENT_LIST_DIR}/lwesp/lwesp/src/cli/cli_input.c
	${CMAKE_CURRENT_LIST_DIR}/lwesp/lwesp/src/lwesp/lwesp.c
	${CMAKE_CURRENT_LIST_DIR}/lwesp/lwesp/src/lwesp/lwesp_ap.c
	${CMAKE_CURRENT_LIST_DIR}/lwesp/lwesp/src/lwesp/lwesp_buff.c
	${CMAKE_CURRENT_LIST_DIR}/lwesp/lwesp/src/lwesp/lwesp_cli.c
	${CMAKE_CURRENT_LIST_DIR}/lwesp/lwesp/src/lwesp/lwesp_conn.c
	${CMAKE_CURRENT_LIST_DIR}/lwesp/lwesp/src/lwesp/lwesp_debug.c
	${CMAKE_CURRENT_LIST_DIR}/lwesp/lwesp/src/lwesp/lwesp_dhcp.c
	${CMAKE_CURRENT_LIST_DIR}/lwesp/lwesp/src/lwesp/lwesp_dns.c
	${CMAKE_CURRENT_LIST_DIR}/lwesp/lwesp/src/lwesp/lwesp_evt.c
	${CMAKE_CURRENT_LIST_DIR}/lwesp/lwesp/src/lwesp/lwesp_hostname.c
	${CMAKE_CURRENT_LIST_DIR}/lwesp/lwesp/src/lwesp/lwesp_input.c
	${CMAKE_CURRENT_LIST_DIR}/lwesp/lwesp/src/lwesp/lwesp_int.c
	${CMAKE_CURRENT_LIST_DIR}/lwesp/lwesp/src/lwesp/lwesp_mdns.c
	${CMAKE_CURRENT_LIST_DIR}/lwesp/lwesp/src/lwesp/lwesp_mem.c
	${CMAKE_CURRENT_LIST_DIR}/lwesp/lwesp/src/lwesp/lwesp_parser.c
	${CMAKE_CURRENT_LIST_DIR}/lwesp/lwesp/src/lwesp/lwesp_pbuf.c
	${CMAKE_CURRENT_LIST_DIR}/lwesp/lwesp/src/lwesp/lwesp_ping.c
	${CMAKE_CURRENT_LIST_DIR}/lwesp/lwesp/src/lwesp/lwesp_smart.c
	${CMAKE_CURRENT_LIST_DIR}/lwesp/lwesp/src/lwesp/lwesp_sntp.c
	${CMAKE_CURRENT_LIST_DIR}/lwesp/lwesp/src/lwesp/lwesp_sta.c
	${CMAKE_CURRENT_LIST_DIR}/lwesp/lwesp/src/lwesp/lwesp_threads.c
	${CMAKE_CURRENT_LIST_DIR}/lwesp/lwesp/src/lwesp/lwesp_timeout.c
	${CMAKE_CURRENT_LIST_DIR}/lwesp/lwesp/src/lwesp/lwesp_unicode.c
	${CMAKE_CURRENT_LIST_DIR}/lwesp/lwesp/src/lwesp/lwesp_utils.c
	${CMAKE_CURRENT_LIST_DIR}/lwesp/lwesp/src/lwesp/lwesp_webserver.c
	${CMAKE_CURRENT_LIST_DIR}/lwesp/lwesp/src/lwesp/lwesp_wps.c
)

target_include_directories(lwesp PUBLIC 
	${CMAKE_CURRENT_LIST_DIR}/FreeRTOS-Kernel/include 
	${CMAKE_CURRENT_LIST_DIR}/FreeRTOS-Kernel/portable/GCC/ARM_CM0
	src
)


# Add the standard ${CMAKE_CURRENT_LIST_DIR}rary to the build
target_link_libraries(FreeRTOS pico_stdlib)