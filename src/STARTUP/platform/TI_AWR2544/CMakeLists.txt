message("==> Start with STARTUP_AWR2544 library")

enable_language(ASM)

add_compile_definitions(SCTLR_VE_BIT=0)

add_library(_STARTUP STATIC)

set(COMMON_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../../common/)

set(FILE_SET
    ${CMAKE_CURRENT_SOURCE_DIR}/startup.c
    ${CMAKE_CURRENT_SOURCE_DIR}/r5_startup.S
    ${CMAKE_CURRENT_SOURCE_DIR}/boot.S
)

target_include_directories(_STARTUP
  PRIVATE ${COMMON_SOURCES}
)

target_sources(_STARTUP
    PRIVATE "${FILE_SET}"
)

target_link_libraries(_STARTUP
    PUBLIC
        adas_apc_if::STARTUP_IF
)

install(TARGETS _STARTUP
    COMPONENT STARTUP_COMP
    DESTINATION lib
)

set_target_properties(STARTUP_PLATFORM PROPERTIES
    RESOURCE "${FILE_SET}"
)
