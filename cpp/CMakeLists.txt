# Some functions require a certain version of cmake
cmake_minimum_required(VERSION 2.8)

# Create the project
set(PROJECT_NAME ElasticMaterials)
project(${PROJECT_NAME})

# Add the external module path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/Modules)

# Find external packages
find_package(OPENGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLFW3 REQUIRED)

# Find external libraries
find_library(OPENGL_FRAMEWORK OpenGL)
find_library(COCOA_FRAMEWORK Cocoa)

# Add glew and glfw to the include directories 
include_directories(${GLEW_INCLUDE_DIR})
include_directories(${GLFW_INCLUDE_DIRS})

# Our own include directory
set(COMMON_INCLUDES ${PROJECT_SOURCE_DIR}/include)
include_directories(${COMMON_INCLUDES})

# Options. Turn on with 'cmake -Dmyvarname=ON'.
option(test "Build all tests." OFF) # Makes boolean 'test' available.

set(CMAKE_CXX_FLAGS "-g")

# Creating our own library
file(GLOB Library_SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)
# We do not want main.cpp in the library
file(GLOB to_remove ${PROJECT_SOURCE_DIR}/src/main.cpp)
list(REMOVE_ITEM Library_SOURCES ${to_remove})
file(GLOB Library_HEADERS ${PROJECT_SOURCE_DIR}/include/*.h)

add_library(ElasticMaterials_lib ${Library_SOURCES} ${Library_HEADERS})

# Creating our executable
add_executable(ElasticMaterials src/main.cpp)
# Key idea: SEPARATE OUT your main() function into its own file so it can be its
# own executable. Separating out main() means you can add this library to be
# used elsewhere (In our case for testing).

# C++11 compatability
set_target_properties(ElasticMaterials_lib PROPERTIES COMPILE_FLAGS "-std=c++11")
set_target_properties(ElasticMaterials PROPERTIES COMPILE_FLAGS "-std=c++11")

# Link all the libraries (including our own)
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
