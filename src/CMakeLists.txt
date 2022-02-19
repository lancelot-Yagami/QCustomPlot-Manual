cmake_minimum_required(VERSION 3.5)
project(qcustomplot-manual)

set(ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/..)
set(EXECUTABLE_OUTPUT_PATH ${ROOT_PATH}/bin)
set(LIBRARY_OUTPUT_PATH ${ROOT_PATH}/bin)

add_subdirectory(qcustomplot)
add_subdirectory(examples)
