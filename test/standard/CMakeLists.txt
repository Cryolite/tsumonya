add_executable(hash
  hash.cpp)
add_dependencies(hash
  src_standard_build_table_run)

add_custom_target(
  test_standard_hash_run ALL ASAN_OPTIONS=handle_abort=1 ./hash
  DEPENDS hash)
