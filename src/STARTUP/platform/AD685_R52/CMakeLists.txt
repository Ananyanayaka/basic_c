#building of 2 libs in here. startup for sbl and appl
#use with ghs compiler options: -nostdlib -LPATH/libStartupSBL.a
enable_language(ASM)

add_library(StartupSBL STATIC)

set(SBL_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/SblBoot.S
)

target_include_directories(StartupSBL
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_sources(StartupSBL
    PRIVATE ${SBL_SRC}
)

install(TARGETS StartupSBL
    COMPONENT STARTUP_COMP
    DESTINATION lib
)


add_library(StartupAppl STATIC)
set(APPL_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/ApplBoot.S
)
target_include_directories(StartupAppl
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)
target_sources(StartupAppl
    PRIVATE ${APPL_SRC}
)

install(TARGETS StartupAppl
    COMPONENT STARTUP_COMP
    DESTINATION lib
)
