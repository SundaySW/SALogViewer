cmake_minimum_required(VERSION 3.16)

project(SALogViewer VERSION 1.0 LANGUAGES CXX)
add_definitions(-D_VER="V1.0")
add_definitions(-D_BUILD_TYPE_="${CMAKE_BUILD_TYPE}")

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS PrintSupport)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Sql)
# For OPENGl
find_package(OpenGL REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets OpenGL)
# end For OPENGl

set(TS_FILES ProtosLogViewer_en_US.ts)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/DB_Driver/*.*
        ${CMAKE_CURRENT_SOURCE_DIR}/view_Settings/*.*
        ${CMAKE_CURRENT_SOURCE_DIR}/LogItem/*.*
        ${CMAKE_CURRENT_SOURCE_DIR}/view_treeview/*.*
        ${CMAKE_CURRENT_SOURCE_DIR}/view_load_items_from_db/*.*
        ${CMAKE_CURRENT_SOURCE_DIR}/view_load_items_from_csv/*.*
        ${CMAKE_CURRENT_SOURCE_DIR}/CSV/*.*
        ${CMAKE_CURRENT_SOURCE_DIR}/CustomTicker/*.*
        )

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.ui
        mainwindow.h
        LogViewer.cpp
        Resources/files.qrc
        ${TS_FILES}
        ${SOURCES}
        )

add_subdirectory(qcustomplot)

add_executable(${PROJECT_NAME}
        ${PROJECT_SOURCES}
        )

#if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
#    qt_add_executable(${PROJECT_NAME}
#        MANUAL_FINALIZATION
#        ${PROJECT_SOURCES}
#    )

# Define target properties for Android with Qt 6 as:
#    set_property(TARGET ProtosLogViewer APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation

#    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
#else()
#    if(ANDROID)
#        add_library(${PROJECT_NAME} SHARED
#            ${PROJECT_SOURCES}
#        )
## Define properties for Android with Qt 5 after find_package() calls as:
##    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    else()
#        add_executable(${PROJECT_NAME}
#            ${PROJECT_SOURCES}
#        )
#    endif()
#
#    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
#endif()

#add_custom_command(
#        TARGET ${PROJECT_NAME} PRE_BUILD
#        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/psql_libs/lib ${CMAKE_CURRENT_BINARY_DIR}/
#)

#target_include_directories(${PROJECT_NAME}
#        PRIVATE
#        ${CMAKE_CURRENT_SOURCE_DIR}/DB_Driver
#        ${CMAKE_CURRENT_SOURCE_DIR}/view_treeview
#        ${CMAKE_CURRENT_SOURCE_DIR}/view_Settings
#        ${CMAKE_CURRENT_SOURCE_DIR}/view_load_items_from_db
#        ${CMAKE_CURRENT_SOURCE_DIR}/view_load_items_from_csv/
#        ${CMAKE_CURRENT_SOURCE_DIR}/Resources
#        ${CMAKE_CURRENT_SOURCE_DIR}/LogItem
#)

target_sources(${PROJECT_NAME}
        PRIVATE
        ${SOURCE_FILES}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::PrintSupport
        Qt${QT_VERSION_MAJOR}::Sql
        Qt${QT_VERSION_MAJOR}::Widgets qcustomplot
        Qt${QT_VERSION_MAJOR}::OpenGL
        ${OPENGL_LIBRARIES}
        )

target_compile_definitions(${PROJECT_NAME} PRIVATE
        QCUSTOMPLOT_USE_OPENGL
        QCUSTOMPLOT_USE_LIBRARY
        )

add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND windeployqt "$<TARGET_FILE:${PROJECT_NAME}>"
        VERBATIM
)

#set_target_properties(${PROJECT_NAME} PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
#    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
#    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
#    MACOSX_BUNDLE TRUE
#    WIN32_EXECUTABLE TRUE
#)

#if(QT_VERSION_MAJOR EQUAL 6)
#    qt_finalize_executable(ProtosLogViewer)
#endif()

#target_compile_options(${PROJECT_NAME}
#        PRIVATE
#        -Wall
#        -Wextra
#        -Wpedantic
##        -Werror
#        -Wduplicated-cond
#        -Wduplicated-branches
#        -Wlogical-op
#        -Wnull-dereference
#        -Wshadow
#        -Wconversion
#        -Wsign-conversion
#        )