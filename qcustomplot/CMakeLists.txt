cmake_minimum_required(VERSION 3.16)

project(qcustomplot LANGUAGES CXX)

find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets PrintSupport REQUIRED)

add_library(qcustomplot SHARED)
target_compile_definitions(qcustomplot PRIVATE QCUSTOMPLOT_COMPILE_LIBRARY)
target_include_directories(qcustomplot PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_sources(qcustomplot PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/qcustomplot.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/qcustomplot.h
        )
target_link_libraries(qcustomplot PUBLIC
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Gui
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::PrintSupport
        )