
function(add_mipt_test what_do_we_test where_is_the_test)
    find_package(GTest REQUIRED CONFIG)
    include(GoogleTest)

    # Your code goes here...
    include_directories("/usr/local/include/")
    add_executable(main_test ${where_is_the_test})
    target_include_directories(main_test PUBLIC .)
    target_link_libraries(main_test PUBLIC ${what_do_we_test} GTest::gtest GTest::gtest_main)
    target_link_libraries(main_test PUBLIC GTest::gmock GTest::gmock_main)
    include(GoogleTest)
    gtest_discover_tests(main_test)
    return(main_test)

endfunction()

