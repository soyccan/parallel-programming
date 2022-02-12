# from 3.5 support compilation database
cmake_minimum_required(VERSION 3.5)

project(vecdot_cl)

find_package(OpenCL REQUIRED)

# to debug cmake: --trace / --trace-expand
set(CMAKE_VERBOSE_MAKEFILE true)

# compliation database
set(CMAKE_EXPORT_COMPILE_COMMANDS true)

# Options: None / Debug / Release / RelWithDebInfo / MinSizeRel
set(CMAKE_BUILD_TYPE Debug)

# Default value can be seen by: cmake --trace
# CMAKE_C_FLAGS_DEBUG = -g
# CMAKE_C_FLAGS_REALEASE = -O3 -DNDEBUG
if(WIN32)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /Wall")
else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wconversion")
endif()
# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED True)

# executables save at project root
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "bin")


#### Options ####
option(input "Input string" "")

#### Constants ####
set(MAIN_EXE vector_cl)

#### Targets ####

# Usage requirements:
#   target_include_directories
#   target_link_libraries
#   target_compile_definitions
#   target_compile_options

# Main program
set(TARGET ${MAIN_EXE})
add_executable(${TARGET} host.c)
target_include_directories(${TARGET} PRIVATE ${OpenCL_INCLUDE_DIR})
target_link_libraries(${TARGET} PRIVATE ${OpenCL_LIBRARY})

# Run main program
# set(TARGET run)
# add_custom_target(${TARGET}
#     COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${MAIN_EXE}
#     DEPENDS ${MAIN_EXE}
#     WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
# )
# message(WARNING ${RUNTIME_OUTPUT_DIRECTORY})