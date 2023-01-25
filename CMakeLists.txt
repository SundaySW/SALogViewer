cmake_minimum_required(VERSION 3.16)

project(ProtosLogViewer VERSION 1.0 LANGUAGES CXX)
add_definitions(-D_VER="V1.3")
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

set(TS_FILES ProtosLogViewer_en_US.ts)

add_subdirectory(qcustomplot)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/DB_Driver/*.*
        ${CMAKE_CURRENT_SOURCE_DIR}/view_Settings/*.*
        ${CMAKE_CURRENT_SOURCE_DIR}/LogItem/*.*
        ${CMAKE_CURRENT_SOURCE_DIR}/view_treeview/*.*
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

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(${PROJECT_NAME}
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET ProtosLogViewer APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation

    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    if(ANDROID)
        add_library(${PROJECT_NAME} SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(${PROJECT_NAME}
            ${PROJECT_SOURCES}
        )
    endif()

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

#add_custom_command(
#        TARGET ${PROJECT_NAME} PRE_BUILD
#        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/psql_libs/lib ${CMAKE_CURRENT_BINARY_DIR}/
#)

target_link_libraries(${PROJECT_NAME} PRIVATE   Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(${PROJECT_NAME} PRIVATE   Qt${QT_VERSION_MAJOR}::PrintSupport)
target_link_libraries(${PROJECT_NAME} PRIVATE   Qt${QT_VERSION_MAJOR}::Sql)
target_compile_definitions(${PROJECT_NAME} PRIVATE QCUSTOMPLOT_USE_LIBRARY)
target_link_libraries(${PROJECT_NAME} PUBLIC   Qt${QT_VERSION_MAJOR}::Widgets qcustomplot)

set_target_properties(${PROJECT_NAME} PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(ProtosLogViewer)
endif()
