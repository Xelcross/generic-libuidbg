PROJECT(xutility LANGUAGES CXX)

SET(CMAKE_AUTOUIC ON)
SET(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTORCC ON)

FIND_PACKAGE(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Gui Widgets Network)
FIND_PACKAGE(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Gui Widgets Network Sql)

SET(HEADER_LIST
	command/commandcenter.h
	command/abstractcommand.h
	eventfilter/eventfilter_shell.h
	objects/shellobject.h
	widgets/commandwidget.h
	widgets/progresswidget.h
	widgets/stepwidgetcontainer.h
	itemdelegate/checkbox_styleditemdelegate.h
	headerview/checkbox_headerview.h
	utility/tools.h
	http/httpclient.h
	utility/eventreactor.h
	utility.h
	xutility_global.h
)

SET(SOURCE_LIST
	command/commandcenter.cpp
	command/abstractcommand.cpp
	eventfilter/eventfilter_shell.cpp 
	objects/shellobject.cpp
	widgets/commandwidget.cpp
	widgets/progresswidget.cpp
	widgets/stepwidgetcontainer.cpp
	itemdelegate/checkbox_styleditemdelegate.cpp
	headerview/checkbox_headerview.cpp
	utility/tools.cpp
	http/httpclient.cpp
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${HEADER_LIST} ${SOURCE_LIST})

SOURCE_GROUP(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${HEADER_LIST} ${SOURCE_LIST})

SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "d")

TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} BEFORE PRIVATE
	${CMAKE_BINARY_DIR_INCLUDE}/lite
)

TARGET_LINK_DIRECTORIES(${PROJECT_NAME} BEFORE PRIVATE
)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} PRIVATE 
	Qt${QT_VERSION_MAJOR}::Gui
	Qt${QT_VERSION_MAJOR}::Widgets
	Qt${QT_VERSION_MAJOR}::Network
	Qt${QT_VERSION_MAJOR}::Sql
)

TARGET_COMPILE_DEFINITIONS(${PROJECT_NAME} PRIVATE XUTILITY_LIB)

X_INSTALL_INCLUDE_LINK(TARGET ${PROJECT_NAME}
	SOURCE ${CMAKE_CURRENT_SOURCE_DIR}
	DESTION ${CMAKE_BINARY_DIR_INCLUDE}
	HEADER_FILES ${HEADER_LIST}
)