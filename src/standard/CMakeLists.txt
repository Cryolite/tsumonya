add_executable(build_table
  build_table.cpp)
add_custom_target(
  src_standard_build_table_run ALL
  ASAN_OPTIONS=handle_abort=1 ./build_table "${CMAKE_SOURCE_DIR}/tsumonya/standard/table.hpp")

add_library(src_standard_hule_enumeration
  hule_enumeration.cpp)

#add_executable(build_map
#  build_map.cpp)
#add_dependencies(build_map
#  src_standard_build_table_run)
#target_link_libraries(build_map
#  PRIVATE Boost::python
#  PRIVATE Boost::timer
#  PRIVATE Boost::headers
#  PRIVATE python"${PYTHON_VERSION}")
