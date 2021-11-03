# Add ${CMAKE_CURRENT_LIST_DIR}rary cpp files

add_library(FreeRTOS STATIC)

target_sources(FreeRTOS PUBLIC
	${CMAKE_CURRENT_LIST_DIR}/FreeRTOS-Kernel/croutine.c
	${CMAKE_CURRENT_LIST_DIR}/FreeRTOS-Kernel/event_groups.c
	${CMAKE_CURRENT_LIST_DIR}/FreeRTOS-Kernel/list.c
	${CMAKE_CURRENT_LIST_DIR}/FreeRTOS-Kernel/queue.c
	${CMAKE_CURRENT_LIST_DIR}/FreeRTOS-Kernel/stream_buffer.c
	${CMAKE_CURRENT_LIST_DIR}/FreeRTOS-Kernel/tasks.c
	${CMAKE_CURRENT_LIST_DIR}/FreeRTOS-Kernel/timers.c
	${CMAKE_CURRENT_LIST_DIR}/FreeRTOS-Kernel/portable/GCC/ARM_CM0/port.c
	${CMAKE_CURRENT_LIST_DIR}/FreeRTOS-Kernel/portable/MemMang/heap_4.c
)

target_include_directories(FreeRTOS PUBLIC 
	${CMAKE_CURRENT_LIST_DIR}/FreeRTOS-Kernel/include 
	${CMAKE_CURRENT_LIST_DIR}/FreeRTOS-Kernel/portable/GCC/ARM_CM0
	${FREERTOS_PORT_PATH}
)

add_subdirectory(${FREERTOS_PORT_PATH})


# Add the standard ${CMAKE_CURRENT_LIST_DIR}rary to the build
target_link_libraries(FreeRTOS pico_stdlib)