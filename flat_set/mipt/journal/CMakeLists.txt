project(journal)

add_library(lib_journal journal.h)
target_link_libraries(lib_journal lib_log)
