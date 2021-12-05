file(REMOVE_RECURSE
  "liblib_set.a"
  "liblib_set.pdb"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/lib_set.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
