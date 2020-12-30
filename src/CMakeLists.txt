cmake_minimum_required(VERSION 3.11)

# The name "HTSEngine" is used in the original makefile
# Due to historical reasons, let me use "hts_engine_API" instead first.
# we may want to use "HTSEngine" in the future.
# NOTE: we should use semanitc versioning. 1.09 -> 1.0.9!
project(hts_engine_API VERSION 1.0.9)

# set the default path for built executables to the "bin" directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

# set the default path for built libraries to the "lib" directory
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

configure_file(hts_engine_API.pc.in "${PROJECT_BINARY_DIR}/hts_engine_API.pc" @ONLY)

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/lib/*.c)
file(GLOB_RECURSE HEADER_FILES CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/lib/*.h ${PROJECT_SOURCE_DIR}/include/*.h)

include_directories(${PROJECT_SOURCE_DIR}/include)

# The hts_engine_API library
add_library(${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES
  VERSION ${CMAKE_PROJECT_VERSION}
)

# hts_engine binary
add_executable(hts_engine bin/hts_engine.c)
target_link_libraries(hts_engine hts_engine_API)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    target_link_libraries(${PROJECT_NAME} winmm)
    target_link_libraries(hts_engine winmm)
else()
    target_link_libraries(hts_engine m)
endif()

install(TARGETS ${PROJECT_NAME} hts_engine DESTINATION lib RUNTIME DESTINATION bin)
install(FILES "${PROJECT_BINARY_DIR}/hts_engine_API.pc" DESTINATION lib/pkgconfig/)