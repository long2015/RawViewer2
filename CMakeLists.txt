cmake_minimum_required(VERSION 3.2)

project(RawViewer2)

set(CMAKE_AUTOMOC ON)
set(INCLUDE_DIR ${INCLUDE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
set(INCLUDE_DIR ${INCLUDE_DIR} ${CMAKE_SOURCE_DIR})


# set Qt path
if(WIN32)
    set(CMAKE_PREFIX_PATH "C:/QT/Qt5.8.0_win64/5.8/msvc2013_64/lib/cmake/Qt5Widgets")
endif()

# Qt5
find_package(Qt5Widgets REQUIRED)
set(INCLUDE_DIR ${INCLUDE_DIR} ${Qt5Widgets_INCLUDES})
set(INCLUDE_DIR ${CMAKE_BINARY_DIR})
set(MACROS_DEFINE ${Qt5Widgets_DEFINITIONS} -D__STDC_CONSTANT_MACROS)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")
#
file(GLOB SRC_FILES "src/*.cpp")
qt5_wrap_ui(UI_FILES ui/frame.ui)
qt5_wrap_ui(UI_FILES ui/mainwindow.ui)

set(RESOURCE ui/toolbar.qrc)
qt5_add_resources(RESOURCE_ADDED ${RESOURCE})

add_definitions(${MACROS_DEFINE})

include_directories(${INCLUDE_DIR})

add_executable(RawViewer2 ${SRC_FILES} ${UI_FILES} ui/myRC.rc ${RESOURCE_ADDED} src/EncodingImage.cpp src/EncodingImage.h)
target_link_libraries(RawViewer2 opencv_videoio opencv_imgcodecs opencv_core)

qt5_use_modules(RawViewer2 Widgets)

if(WIN32)
    set_target_properties(RawViewer2 PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")
endif()
