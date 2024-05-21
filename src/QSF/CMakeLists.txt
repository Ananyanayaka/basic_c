message("==> Start with QSF targets")

#create target for exporting common sources 
add_library(QSF_COMMON INTERFACE)

#create target for exporting ADAPTER sources 
add_library(QSF_ADAPTER INTERFACE)

#create target for exporting platform sources 
add_library(QSF_PLATFORM INTERFACE)

if(NOT "${CPU_PLATFORM}" STREQUAL "RCARV3H")
add_subdirectory(common)
endif()
if(NOT VECU_ENABLED)
    add_subdirectory(platform)
endif()

# export common files
# RESOURCE property is set in /common folder
install(
    TARGETS QSF_COMMON
    RESOURCE
    COMPONENT QSF_COMP
    DESTINATION src/qsf_src
)

# export ADAPTER files
# RESOURCE property is set in /common folder
install(
    TARGETS QSF_ADAPTER
    RESOURCE
    COMPONENT QSF_ADAPTER_COMP
    DESTINATION src/qsf_src
)

# export normal files
# RESOURCE property is set in each platform folder
install(
    TARGETS QSF_PLATFORM
    RESOURCE
    COMPONENT QSF_COMP
    DESTINATION src/qsf_src
)

