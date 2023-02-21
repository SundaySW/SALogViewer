cmake_minimum_required(VERSION 3.16)

project(qcustomplot LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CMAKE_COMPILE_PDB_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/pdb)

add_library(qcustomplot SHARED)
target_compile_definitions(qcustomplot PRIVATE
        QCUSTOMPLOT_COMPILE_LIBRARY
        QCUSTOMPLOT_USE_OPENGL
        QCUSTOMPLOT_USE_LIBRARY
)

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
        Qt${QT_VERSION_MAJOR}::OpenGL
        ${OPENGL_LIBRARIES}
)