#Set the minimum cmake version required for this project.
cmake_minimum_required(VERSION 3.5)

# set the name of the project name and version, here is the Git repo package "adas_apc"
project(adas_apc)

# Find own interface packages
# if(NOT ("${CPU_PLATFORM}" STREQUAL "AD685_R52")) 
# find_package(adas_apc_base_if REQUIRED
  # OPTIONAL_COMPONENTS
    # CACHE_API_IF_COMP
    # SYS_CRC_IF_COMP
    # SYS_INT_IF_COMP
    # SYS_MEM_IF_COMP
    # SYS_TIME_IF_COMP
# )
# if(NOT (adas_apc_base_if_FOUND))
  # message("--> adas_apc_base_if package not found")
# else()
  # message("--> adas_apc_base_if_DIR: ${adas_apc_base_if_PACKAGE_PATH}")
# endif()

# find_package(adas_apc_if REQUIRED
  # OPTIONAL_COMPONENTS
    # DMA_API_IF_COMP
    # QSF_IF_COMP
    # SBL_IF_COMP
    # STARTUP_IF_COMP
    # SYS_EXT_IF_COMP
    # SYS_KPI_IF_COMP
    # SYS_PAL_IF_COMP
# )
# if(NOT (adas_apc_if_FOUND))
  # message("--> adas_apc_if package not found")
# else()
  # message("--> adas_apc_if_DIR: ${adas_apc_if_PACKAGE_PATH}")
# endif()
# endif()

#Setting the Cmake compiler ON
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# List of available components in the package
set(AVAILABLE_COMPONENTS
  ${CACHE_API}
  ${DMA_API}
  ${QSF}
  ${SBL}
  ${STARTUP}
  ${SYS}
)

# List of available sub-components in the SYS component
set(SYS_SUBCOMPONENTS
  ${SYS_CRC}
  ${SYS_EXT}
  ${SYS_INTERRUPT}
  ${SYS_KPI}
  ${SYS_MEM}
  ${SYS_PAL}
  ${SYS_TIME}
)

if(NOT VECU_ENABLED AND NOT ${CMAKE_CROSSCOMPILING})
  # -- tests
  include(CTest)
  add_subdirectory(tests/)
else()
  add_subdirectory(src/)
endif()


# -- documentation
# add_subdirectory(doc/)

# -- AUTOSAR SWC arxmls
# add_subdirectory(AutosarBuilder/)

#To fetch yaml
# install(FILES src/adas_apc_component.yaml
    # COMPONENT QSF_COMP
    # DESTINATION ./
# )
