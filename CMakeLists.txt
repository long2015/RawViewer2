cmake_minimum_required(VERSION 3.2)

project(RawViewer2)

set(CMAKE_AUTOMOC ON)
set(INCLUDE_DIR ${INCLUDE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
set(INCLUDE_DIR ${INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

# Qt5
find_package(Qt5Widgets REQUIRED)
set(INCLUDE_DIR ${INCLUDE_DIR} ${Qt5Widgets_INCLUDES})
set(MACROS_DEFINE ${Qt5Widgets_DEFINITIONS} -D__STDC_CONSTANT_MACROS)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")
#
file(GLOB SRC_FILES "*.cpp")
qt5_wrap_ui(UI_FILES frame.ui)
qt5_wrap_ui(UI_FILES mainwindow.ui)


add_definitions(${MACROS_DEFINE})

include_directories(${INCLUDE_DIR})

add_executable(RawViewer2 ${SRC_FILES} ${UI_FILES} myRC.rc)
target_link_libraries(RawViewer2 swscale avformat avcodec avutil)

qt5_use_modules(RawViewer2 Widgets)
