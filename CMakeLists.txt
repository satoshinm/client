cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(konstructs)

#-----------------------------------------------------------------------
# Check for C++11 enabled compiler
#-----------------------------------------------------------------------

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

if(MINGW)
  add_definitions(-DWIN32)
  add_definitions(-DEIGEN_DONT_ALIGN)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unknown-pragmas")
endif()

if(MSVC)
	add_definitions(-DNOMINMAX)
endif()

#-----------------------------------------------------------------------
# Nano GUI
#-----------------------------------------------------------------------
SET(GLFW_USE_OPTIMUS_HPG ON CACHE BOOL "Build glfw with HPG (High Performance Graphics) on windows")
option(NANOGUI_BUILD_PYTHON OFF)
option(NANOGUI_BUILD_EXAMPLE OFF)
option(NANOGUI_BUILD_SHARED OFF)
add_subdirectory(dependencies/nanogui)

#-----------------------------------------------------------------------
# Konstructs lib
#-----------------------------------------------------------------------
add_subdirectory(lib)

#-----------------------------------------------------------------------
# Misc
#-----------------------------------------------------------------------

set(CMAKE_BUILD_TYPE Debug)

if (NOT EMSCRIPTEN)
    find_package(ZLIB REQUIRED)

    include_directories(
      dependencies/nanogui/include
      dependencies/nanogui/ext/glfw/include
      dependencies/nanogui/ext/glew/include
      dependencies/nanogui/ext/eigen
      dependencies/nanogui/ext/nanovg/src
      dependencies/optional-lite
      dependencies/tinyobjloader
      lib/include)
else ()
    include_directories(
      dependencies/nanogui/include
      #dependencies/nanogui/ext/glfw/include
      #dependencies/nanogui/ext/glew/include
      dependencies/nanogui/ext/eigen
      dependencies/nanogui/ext/nanovg/src
      dependencies/optional-lite
      dependencies/tinyobjloader
      lib/include)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s USE_GLFW=3 --embed-file ../shaders/")
  set(CMAKE_EXECUTABLE_SUFFIX ".html")
endif ()

FILE(
  GLOB SOURCE_FILES
  src/*.cpp)

add_executable(konstructs ${SOURCE_FILES})

if (NOT EMSCRIPTEN)
    set(konstructs_LIBS
        konstructs-lib
        ${ZLIB_LIBRARIES}
        nanogui ${NANOGUI_EXTRA_LIBS}
        glfw
    )
else ()
    set(konstructs_LIBS
        konstructs-lib
        ${ZLIB_LIBRARIES}
        nanogui ${NANOGUI_EXTRA_LIBS}
        #glfw
    )

endif ()

if(WIN32 OR MINGW)
    set(konstructs_LIBS ${konstructs_LIBS} ws2_32.lib)
endif()

target_link_libraries(konstructs ${konstructs_LIBS})

install(TARGETS konstructs DESTINATION .)
install(DIRECTORY textures/ DESTINATION textures)
install(DIRECTORY models/ DESTINATION models)
install(DIRECTORY shaders/ DESTINATION shaders)
