if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPE)
  message(STATUS "Setting build type to 'RelWithDebInfo' as none was specified.")
  set(
    CMAKE_BUILD_TYPE RelWithDebInfo 
    CACHE 
    STRING "Choose the type of build." 
    FORCE
  )
  set_property(
    CACHE CMAKE_BUILD_TYPE PROPERTY 
    STRINGS 
      "Debug" 
      "Release" 
      "MinSizeRel"
      "RelWithDebInfo"
  )
endif()

find_program(CCACHE ccache)
if(CCACHE)
  message(STATUS "Using CCache")
  set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE})
else()
  message(STATUS "CCache not found, cannot auto enable")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(ENABLE_IPO "Enable Interprocedual optimization, aka Link Time Optimization (LTO)" OFF)

if(ENABLE_IPO)
  include(CheckIPOSupported)
  check_ipo_supported(RESULT result OUTPUT output)
  if(result)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
  else()
    message(SEND_ERROR "IPO was requested but not supported: ${output}")
  endif()
endif()
