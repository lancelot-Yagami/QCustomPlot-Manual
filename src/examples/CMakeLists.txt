set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(-DQCUSTOMPLOT_USE_LIBRARY)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../qcustomplot)
link_directories(${ROOT_PATH}/bin)

add_subdirectory(001_start)
add_subdirectory(003_bar_chart_demo)
