set(FILE_SET
    ${CMAKE_CURRENT_SOURCE_DIR}/IuSbl_part.c
    ${CMAKE_CURRENT_SOURCE_DIR}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/startup_r7.c
    ${CMAKE_CURRENT_SOURCE_DIR}/startup_r7_asm.S
)

set_target_properties(STARTUP_PLATFORM PROPERTIES
    RESOURCE "${FILE_SET}"
)
