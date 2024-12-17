X_ADD_INCLUDE_LINK(TARGET lite
    SOURCE ${CMAKE_CURRENT_SOURCE_DIR}
    DESTINATION ${CMAKE_BINARY_DIR_INCLUDE}
    HEADER_LIST pridata.h factory.h factory/factory_v1.h factory/factory_v2.h qtlog.h
)