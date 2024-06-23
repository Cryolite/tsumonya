add_executable(hash
  hash.cpp)
add_dependencies(hash
  src_standard_build_table_run)
target_link_libraries(hash
  src_standard_hule_enumeration)

add_custom_target(
  test_standard_hash_run ALL ASAN_OPTIONS=handle_abort=1 ./hash
  DEPENDS hash)
