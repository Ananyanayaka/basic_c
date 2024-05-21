set(FILE_SET
    ${CMAKE_CURRENT_SOURCE_DIR}/pal_qsf.c
    ${CMAKE_CURRENT_SOURCE_DIR}/pal_qsf.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Fls.c	
)

set_target_properties(QSF_PLATFORM PROPERTIES
    RESOURCE "${FILE_SET}"
)
