project(qcustomplot)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_definitions(-DQCUSTOMPLOT_COMPILE_LIBRARY)

find_package(Qt5Widgets)
find_package(Qt5PrintSupport)

file(GLOB ALL_SOURCES "*.cpp" "*.c")
file(GLOB ALL_INCLUDES "*.hpp" "*.h")

add_library(${PROJECT_NAME} SHARED ${ALL_INCLUDES} ${ALL_SOURCES})

target_link_libraries(${PROJECT_NAME} Qt5::Widgets Qt5::PrintSupport)

install(TARGETS ${PROJECT_NAME}
            LIBRARY DESTINATION ${ROOT_PATH}/bin)
