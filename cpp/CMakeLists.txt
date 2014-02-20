# Some functions require a certain version of cmake
cmake_minimum_required(VERSION 2.8)
#include(FindPackageHandleStandardArgs)

# Create the project
set(PROJECT_NAME ElasticMaterials)
project(${PROJECT_NAME})

################################
# External packages
################################

# Find the packages
find_package(GLEW REQUIRED)
  # glfw3 requires pkconfig which contains the function pkg_search_module().
  # This way glfw3 can also be found.
find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)
  # Find the OpenGl Framework
find_library(OPENGL_FRAMEWORK OpenGL)
find_library(COCOA_FRAMEWORK Cocoa)

# Add glew and glfw to the include directories, now these can be included in the code 
include_directories(${GLEW_INCLUDE_DIR} ${GLFW_INCLUDE_DIRS})


# Our own include directory
set(COMMON_INCLUDES ${PROJECT_SOURCE_DIR}/include)
include_directories(${COMMON_INCLUDES})


# Options. Turn on with 'cmake -Dmyvarname=ON'.
option(test "Build all tests." OFF) # Makes boolean 'test' available.

set(CMAKE_CXX_FLAGS "-g")


################################
# Normal Libraries & Executables
################################
file(GLOB Library_SOURCES src/Connection.cpp src/connection2massIndices.cpp 
		src/MCS.cpp src/NumericalMethods.cpp src/Particle.cpp src/shader.cpp src/test.cpp)
file(GLOB Library_HEADERS include/Connection.h include/connection2massIndices.h 
    include/MCS.h include/NumericalMethods.h include/Particle.h include/shader.h include/test.h)
add_library(ElasticMaterials_lib ${Library_SOURCES} ${Library_HEADERS})
add_executable(ElasticMaterials src/main.cpp)
# Key idea: SEPARATE OUT your main() function into its own file so it can be its
# own executable. Separating out main() means you can add this library to be
# used elsewhere (In our case for testing).
set_target_properties(ElasticMaterials_lib PROPERTIES COMPILE_FLAGS "-std=c++11")
set_target_properties(ElasticMaterials PROPERTIES COMPILE_FLAGS "-std=c++11")

target_link_libraries(ElasticMaterials ElasticMaterials_lib ${GLEW_LIBRARY} ${GLFW_LIBRARIES} ${OPENGL_FRAMEWORK} ${COCOA_FRAMEWORK})





################################
# Testing
################################
if (test)
	
  if( MSVC ) 
    # VS2012 doesn't support correctly the tuples yet
    add_definitions( /D _VARIADIC_MAX=10 )
  endif( MSVC)

  # This adds another subdirectory, which has 'project(gtest)'.
  add_subdirectory(lib/gtest-1.7.0)

  enable_testing()

  # Include the gtest library. gtest_SOURCE_DIR is available due to
  # 'project(gtest)' above.
  include_directories(
    ${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

  ##############
  # Unit Tests
  ##############
  add_executable(runUnitTests test/test.cpp)
  set_target_properties(runUnitTests PROPERTIES COMPILE_FLAGS "-std=gnu++11")

  # Standard linking to gtest stuff.
  target_link_libraries(runUnitTests gtest gtest_main)

  # Extra linking for the project.
  target_link_libraries(runUnitTests ElasticMaterials_lib)

  # This is so you can do 'make test' to see all your tests run, instead of
  # manually running the executable runUnitTests to see those specific tests.
  add_test(NAME that-test-I-made COMMAND runUnitTests)

  # You can also omit NAME and COMMAND. The second argument could be some other
  # test executable.
  add_test(that-other-test-I-made runUnitTests)
endif()
