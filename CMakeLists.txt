cmake_minimum_required(VERSION 3.24...3.30)
project(pung)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Dependencies
set(RAYLIB_VERSION 5.5)
find_package(raylib ${RAYLIB_VERSION} QUIET) # QUIET or REQUIRED
if (NOT raylib_FOUND) # If there's none, fetch and build raylib
  include(FetchContent)
  FetchContent_Declare(
    raylib
    DOWNLOAD_EXTRACT_TIMESTAMP OFF
    URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
  )
  FetchContent_GetProperties(raylib)
  if (NOT raylib_POPULATED) # Have we downloaded raylib yet?
    set(FETCHCONTENT_QUIET NO)
    FetchContent_MakeAvailable(raylib)
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # Don't build the supplied examples
  endif()
endif()

# Our Project

add_executable(${PROJECT_NAME}
    src/pung.c
    src/screen_ending.c
    src/screen_gameplay.c
    src/screen_instructions.c
    src/screen_start.c
    src/screen_title.c
    src/helpers.c
)

# Output files in a separate folder within the build directory.
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME})

if (${PLATFORM} STREQUAL "Web")
    # Make resources available for web preloading by copying them.
    add_custom_command(
        TARGET ${PROJECT_NAME} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/src/resources $<TARGET_FILE_DIR:${PROJECT_NAME}>/../resources
    )
    # And remove the directory after we're done.
    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E remove_directory $<TARGET_FILE_DIR:${PROJECT_NAME}>/../resources
    )
else()
    # Copy resources so Desktop versions can find them.
    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/src/resources $<TARGET_FILE_DIR:${PROJECT_NAME}>/resources
    )
endif()

#set(raylib_VERBOSE 1)
target_link_libraries(${PROJECT_NAME} raylib)

# Set path for Visual Studio debugger. (I don't use it, so I have not tested it.)
set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:${PROJECT_NAME}>)

# Web Configurations
if (${PLATFORM} STREQUAL "Web")
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".html") # Tell Emscripten to build an example.html file.
    # Configure emscripten flags.
    target_link_options(${PROJECT_NAME}
        PUBLIC -Os # Size optimization.
        PUBLIC -sUSE_GLFW=3
        PUBLIC -sTOTAL_MEMORY=128MB # (default = 16MB)
        PUBLIC -sSTACK_SIZE=1MB
        PUBLIC -sMINIFY_HTML=0
        PUBLIC --preload-file resources
        PUBLIC --shell-file ${CMAKE_SOURCE_DIR}/src/minshell.html
    )
endif()

# Checks if OSX and links appropriate frameworks (Only required on MacOS)
if (APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework IOKit")
    target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
endif()
