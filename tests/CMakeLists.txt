macro(greenboy_add_test TESTNAME)
    # create an exectuable in which the tests will be stored
    add_executable(${TESTNAME} ${ARGN})
    # link the Google test infrastructure, mocking library, and a default main fuction to
    # the test executable.  Remove g_test_main if writing your own main function.
    
    target_link_libraries(${TESTNAME} Greenboy gtest gmock gtest_main)
    
    # gtest_discover_tests replaces gtest_add_tests,
    # see https://cmake.org/cmake/help/v3.10/module/GoogleTest.html for more options to pass to it
    gtest_discover_tests(${TESTNAME})
    set_target_properties(${TESTNAME} PROPERTIES FOLDER tests)
    list(APPEND TestLibraries ${TESTNAME})
endmacro()

greenboy_add_test(DataAccess      greenboy/data_access.cpp)
greenboy_add_test(FetchExecuteCPU greenboy/fetch_execute_cpu.cpp)
greenboy_add_test(Gameboy         greenboy/gameboy.cpp)

# do not include intergration tests in coverage
if(NOT ${GREENBOY_COVERAGE})
  greenboy_add_test(Instructions greenboy/instructions.cpp)
endif()

if(GREENBOY_COVERAGE)
  setup_target_for_coverage_lcov(
    NAME coverage 
    EXECUTABLE ctest 
    DEPENDENCIES 
      ${TestLibraries}
    LCOV_ARGS "--no-external"
    EXCLUDE "*/external/**"
  )
endif()
