if(${USE_CUDA})
    set_source_files_properties(iobase_tests.cc PROPERTIES LANGUAGE CUDA)
endif()
add_executable(sparsebase_bases_iobase_tests.test iobase_tests.cc)
target_link_libraries(sparsebase_bases_iobase_tests.test  sparsebase)
target_link_libraries(sparsebase_bases_iobase_tests.test  gtest gtest_main)
add_test(NAME sparsebase_bases_iobase_tests.test COMMAND sparsebase_bases_iobase_tests.test)

if(${USE_CUDA})
    set_source_files_properties(reorder_base_tests.cc PROPERTIES LANGUAGE CUDA)
endif()
add_executable(sparsebase_bases_reorder_base_tests.test reorder_base_tests.cc)
target_link_libraries(sparsebase_bases_reorder_base_tests.test  sparsebase)
target_link_libraries(sparsebase_bases_reorder_base_tests.test  gtest gtest_main)
add_test(NAME sparsebase_bases_reorder_base_tests.test COMMAND sparsebase_bases_reorder_base_tests.test)

if(${USE_CUDA})
    set_source_files_properties(graph_feature_base_tests.cc PROPERTIES LANGUAGE CUDA)
endif()
add_executable(sparsebase_bases_graph_feature_base_tests.test graph_feature_base_tests.cc)
target_link_libraries(sparsebase_bases_graph_feature_base_tests.test  sparsebase)
target_link_libraries(sparsebase_bases_graph_feature_base_tests.test  gtest gtest_main)
add_test(NAME sparsebase_bases_graph_feature_base_tests.test COMMAND sparsebase_bases_graph_feature_base_tests.test)

