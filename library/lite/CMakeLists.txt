PROJECT(lite)

X_INSTALL_INCLUDE_LINK(TARGET ${PROJECT_NAME}
	SOURCE ${CMAKE_CURRENT_SOURCE_DIR}
	DESTION ${CMAKE_BINARY_DIR_INCLUDE}
	HEADER_FILES pridata.h factory.h factory/factory_v1.h factory/factory_v2.h qtlog.h tools.h
)
