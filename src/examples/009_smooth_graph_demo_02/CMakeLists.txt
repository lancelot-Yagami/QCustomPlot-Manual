get_filename_component(CURRENT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR} NAME)
project(${CURRENT_SOURCE_DIR})

find_package(Qt5Widgets)

file(GLOB ALL_SOURCES "*.cpp" "*.c")
file(GLOB ALL_INCLUDES "*.hpp" "*.h")

add_executable(${PROJECT_NAME} ${ALL_INCLUDES} ${ALL_SOURCES})

target_link_libraries(${PROJECT_NAME} Qt5::Widgets qcustomplot)
