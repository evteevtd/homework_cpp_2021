project(mipt-lib)

add_subdirectory(log)
add_subdirectory(journal)
add_subdirectory(set)
add_library(lib_all INTERFACE)
target_link_libraries(lib_all INTERFACE lib_log lib_journal lib_set)
