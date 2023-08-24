add_executable(build_table
  build_table.cpp)
target_link_libraries(build_table
  PRIVATE Boost::headers)

add_executable(build_map
  build_map.cpp)
target_link_libraries(build_map
  PRIVATE Boost::python
  PRIVATE Boost::timer
  PRIVATE Boost::headers
  PRIVATE python"${PYTHON_VERSION}")
