
# Set path to the stubbed dependencies of the adas_platforms package
SET(STUBS_FOR_ADAS_PLATFORMS ${CMAKE_CURRENT_SOURCE_DIR}/STUB/) 

# if(NOT ("${CPU_PLATFORM}" STREQUAL "AD685_R52")) 
  # find_package(adas_platforms REQUIRED)
  # if(NOT (adas_platforms_FOUND))
    # message("--> adas_platforms package not found")
  # else()
    # message("--> adas_platforms_DIR: ${adas_platforms_PACKAGE_PATH}")
  # endif()
# endif()

foreach(COMPONENT_NAME IN LISTS AVAILABLE_COMPONENTS)
  if(NOT "${COMPONENT_NAME}" STREQUAL "none")
    set(LIB_NAME "${COMPONENT_NAME}")
    add_subdirectory(${COMPONENT_NAME}/)
  else()
    #if none was selected, nothing has to be done
  endif()
endforeach(GENERIC_FOLDER)
