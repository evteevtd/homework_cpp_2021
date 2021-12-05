file(REMOVE_RECURSE
  "liblib_journal.a"
  "liblib_journal.pdb"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/lib_journal.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
