set(FILE_SET
    ${CMAKE_CURRENT_SOURCE_DIR}/Startup_Memmap.h
)

set_target_properties(STARTUP_COMMON PROPERTIES
    RESOURCE "${FILE_SET}"
)
