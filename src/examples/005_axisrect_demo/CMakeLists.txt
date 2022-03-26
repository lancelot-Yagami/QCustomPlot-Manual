project(005_axisrect_demo)

find_package(Qt5Widgets)

#set(-DQCUSTOMPLOT_USE_LIBRARY)

file(GLOB ALL_SOURCES "*.cpp" "*.c")
file(GLOB ALL_INCLUDES "*.hpp" "*.h")

add_executable(${PROJECT_NAME} ${ALL_INCLUDES} ${ALL_SOURCES})

target_link_libraries(${PROJECT_NAME} Qt5::Widgets qcustomplot)
