# PROJECT_NAME = ${LWESP_OPTION_OS}
target_sources(${PROJECT_NAME}
    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/lwesp_sys_freertos.c
    ${CMAKE_CURRENT_LIST_DIR}/lwesp_ll_raspberry_pi_pico.c
    ${CMAKE_CURRENT_LIST_DIR}/lwesp_mem_rpi_pico.c
)
