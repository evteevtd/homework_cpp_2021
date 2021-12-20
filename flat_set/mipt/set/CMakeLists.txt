project(set)

add_library(lib_set set.h)
target_link_libraries(lib_set lib_journal)
