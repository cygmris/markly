find_package(Qt6 6.5 REQUIRED COMPONENTS Test)

set(MARKLY_TESTS
    test_configmgr
    test_commandlineoptions
    test_singleinstanceguard
    test_thememodel
    test_notebookcore
    test_notebookexplorer
    test_bufferview
    test_editorconfig
    test_search
    test_editorinput
)

foreach(test_name ${MARKLY_TESTS})
    qt_add_executable(${test_name} ${test_name}.cpp)
    target_link_libraries(${test_name} PRIVATE markly_core Qt6::Test)
    add_test(NAME ${test_name} COMMAND ${test_name})
    # Run headless.
    set_tests_properties(${test_name} PROPERTIES
        ENVIRONMENT "QT_QPA_PLATFORM=offscreen")
endforeach()
