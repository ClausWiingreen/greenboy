macro(greenboy_add_test TESTNAME)
    # create an exectuable in which the tests will be stored
    add_executable(${TESTNAME} ${ARGN})
    # link the Google test infrastructure, mocking library, and a default main fuction to
    # the test executable.  Remove g_test_main if writing your own main function.
    
    target_link_libraries(${TESTNAME} Greenboy gtest gmock gtest_main)
    
    # gtest_discover_tests replaces gtest_add_tests,
    # see https://cmake.org/cmake/help/v3.10/module/GoogleTest.html for more options to pass to it
    gtest_discover_tests(${TESTNAME}
    # set a working directory so your project root so that you can find test data via paths relative to the project root
    WORKING_DIRECTORY ${PROJECT_DIR}
    PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_DIR}"
    )
    set_target_properties(${TESTNAME} PROPERTIES FOLDER tests)
endmacro()

greenboy_add_test(Gameboy         greenboy/gameboy.cpp)
greenboy_add_test(FetchExecuteCPU greenboy/fetch_execute_cpu.cpp)

if(GREENBOY_COVERAGE)
    setup_target_for_coverage_lcov(
        NAME coverage 
        EXECUTABLE ctest 
        DEPENDENCIES Gameboy
        LCOV_ARGS 
          "--no-external"
        EXCLUDE 
            "*/external/**"
    )
endif()
