cmake_minimum_required(VERSION 3.16)

project(SALogViewer VERSION 1.0 LANGUAGES CXX)
add_definitions(-D_VER="V1.2")
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
        ${CMAKE_CURRENT_SOURCE_DIR}/TreeModel/*.*
        ${CMAKE_CURRENT_SOURCE_DIR}/items_loader_from_db/*.*
        ${CMAKE_CURRENT_SOURCE_DIR}/items_loader_from_csv/*.*
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

target_sources(${PROJECT_NAME}
        PRIVATE
        ${SOURCE_FILES}
)

target_include_directories(${PROJECT_NAME}
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/DB_Driver
        ${CMAKE_CURRENT_SOURCE_DIR}/view_Settings
        ${CMAKE_CURRENT_SOURCE_DIR}/LogItem
        ${CMAKE_CURRENT_SOURCE_DIR}/TreeModel
        ${CMAKE_CURRENT_SOURCE_DIR}/items_loader_from_db
        ${CMAKE_CURRENT_SOURCE_DIR}/items_loader_from_csv
        ${CMAKE_CURRENT_SOURCE_DIR}/CSV
        ${CMAKE_CURRENT_SOURCE_DIR}/CustomTicker
        ${CMAKE_CURRENT_SOURCE_DIR}/qcustomplot
        )

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    enable_language("RC")
    set (WIN32_RESOURCES Resource/resource.rc)
endif()

IF(CMAKE_BUILD_TYPE MATCHES Release)
    message("Release build!")
    set_property(TARGET ${PROJECT_NAME} PROPERTY WIN32_EXECUTABLE true)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/saved_params.json ${CMAKE_CURRENT_BINARY_DIR}/saved_params.json COPYONLY)
    add_custom_command(
            TARGET ${PROJECT_NAME} PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/Resource/ ${CMAKE_CURRENT_BINARY_DIR}/Resource/
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/Resources/ ${CMAKE_CURRENT_BINARY_DIR}/Resources/
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/psql_libs/lib ${CMAKE_CURRENT_BINARY_DIR}/
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/qcustomplot/bin ${CMAKE_CURRENT_BINARY_DIR}/
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/deploy_libs ${CMAKE_CURRENT_BINARY_DIR}/
    )
    add_custom_command(
            TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND windeployqt "$<TARGET_FILE:${PROJECT_NAME}>"
            VERBATIM
    )
    add_custom_command(
            TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_CURRENT_BINARY_DIR}/CMakeCache.txt
            COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_CURRENT_BINARY_DIR}/cmake_install.cmake
            COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_CURRENT_BINARY_DIR}/Makefile
            COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles
            COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/qcustomplot
            COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_autogen
            COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/.cmake

    )
ENDIF()

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