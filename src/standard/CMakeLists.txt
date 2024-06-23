add_executable(build_table
  build_table.cpp)

add_custom_target(
  src_standard_build_table_run ALL ASAN_OPTIONS=handle_abort=1 ./build_table "${CMAKE_SOURCE_DIR}/tsumonya/standard/table.hpp")

#add_executable(build_map
#  "${CMAKE_SOURCE_DIR}/tsumonya/standard/table.hpp"
#  build_map.cpp)
#target_link_libraries(build_map
#  PRIVATE Boost::python
#  PRIVATE Boost::timer
#  PRIVATE Boost::headers
#  PRIVATE python"${PYTHON_VERSION}")
