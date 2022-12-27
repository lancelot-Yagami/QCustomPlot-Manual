set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_definitions(-DQCUSTOMPLOT_USE_LIBRARY)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../qcustomplot)
link_directories(${ROOT_PATH}/bin)

add_subdirectory(001_start)
add_subdirectory(003_bar_chart_demo)
add_subdirectory(004_layout_demo)
add_subdirectory(005_axisrect_demo)
add_subdirectory(006_dynamic_data_demo)
add_subdirectory(007_lemniscate_demo)
add_subdirectory(008_smooth_graph_demo_01)
add_subdirectory(009_smooth_graph_demo_02)
add_subdirectory(012_stretch_item_demo)

