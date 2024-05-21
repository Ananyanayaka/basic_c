message("==> Start with STARTUP targets")

#create target for exporting common sources 
add_library(STARTUP_COMMON INTERFACE)

#create target for exporting platform sources 
add_library(STARTUP_PLATFORM INTERFACE)

add_subdirectory(common)
if(NOT VECU_ENABLED)
    add_subdirectory(platform)
endif()

# export common files
# RESOURCE property is set in /common folder
install(
    TARGETS STARTUP_COMMON
    RESOURCE
    COMPONENT STARTUP_COMP
    DESTINATION src/startup_src
)

# export normal files
# RESOURCE property is set in each platform folder
install(
    TARGETS STARTUP_PLATFORM
    RESOURCE
    COMPONENT STARTUP_COMP
    DESTINATION src/startup_src
)
