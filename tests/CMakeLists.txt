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
    test_tags
    test_images
    test_history
    test_snippet
    test_export
    test_task
    test_locale
    test_spell
    test_viengine
    test_imagehost
    test_update
    test_pandoc
    test_merge
    test_hotkey
)

foreach(test_name ${MARKLY_TESTS})
    qt_add_executable(${test_name} ${test_name}.cpp)
    target_link_libraries(${test_name} PRIVATE markly_core Qt6::Test)
    add_test(NAME ${test_name} COMMAND ${test_name})
    # Run headless.
    set_tests_properties(${test_name} PROPERTIES
        ENVIRONMENT "QT_QPA_PLATFORM=offscreen")
endforeach()

# test_spell needs the shipped dictionary directory (#8c).
target_compile_definitions(test_spell PRIVATE
    SRC_DICT_DIR="${CMAKE_SOURCE_DIR}/src/data/dicts")

# Integration smoke tests on the built binary (#21).
add_test(NAME smoke_version COMMAND markly --version)
set_tests_properties(smoke_version PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
    PASS_REGULAR_EXPRESSION "Markly")

# Full QML shell init: MARKLY_SHOT grabs the shell after init. We assert the artifact
# is produced rather than the exit code, because QtWebEngine segfaults on offscreen
# teardown *after* the screenshot is written (known; not an init failure).
add_test(NAME smoke_startup COMMAND bash -c
    "rm -f '${CMAKE_BINARY_DIR}/smoke-startup.png'; '$<TARGET_FILE:markly>' || true; test -s '${CMAKE_BINARY_DIR}/smoke-startup.png'")
set_tests_properties(smoke_startup PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen;MARKLY_SHOT=${CMAKE_BINARY_DIR}/smoke-startup.png"
    TIMEOUT 30)
