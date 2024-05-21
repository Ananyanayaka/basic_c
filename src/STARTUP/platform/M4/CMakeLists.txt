set(FILE_SET
    ${CMAKE_CURRENT_SOURCE_DIR}/startup.c
)

set_target_properties(STARTUP_PLATFORM PROPERTIES
    RESOURCE "${FILE_SET}"
)
