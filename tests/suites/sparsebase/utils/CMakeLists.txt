if(${USE_CUDA})
    set_source_files_properties(function_matcher_mixin_tests.cc PROPERTIES LANGUAGE CUDA)
endif()
add_executable(sparsebase_utils_function_matcher_mixin_tests.test function_matcher_mixin_tests.cc)
target_link_libraries(sparsebase_utils_function_matcher_mixin_tests.test sparsebase)
target_link_libraries(sparsebase_utils_function_matcher_mixin_tests.test gtest gtest_main)
add_test(NAME sparsebase_utils_function_matcher_mixin_tests.test COMMAND sparsebase_utils_function_matcher_mixin_tests.test)

if(${USE_CUDA})
    set_source_files_properties(utils_tests.cc PROPERTIES LANGUAGE CUDA)
endif()
add_executable(sparsebase_utils_utils_tests.test utils_tests.cc)
target_link_libraries(sparsebase_utils_utils_tests.test sparsebase)
target_link_libraries(sparsebase_utils_utils_tests.test gtest gtest_main)
add_test(NAME sparsebase_utils_utils_tests.test COMMAND sparsebase_utils_utils_tests.test)
