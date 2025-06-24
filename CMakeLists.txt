cmake_minimum_required(VERSION 3.19...3.24)

# Generate compile_commands.json
set(CPM_SOURCE_CACHE ~/.cache/cpm)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Not ideal to use this global variable, but necessary to make sure that tooling and projects use the same version
set(CMAKE_CXX_STANDARD 11)
#set(CMAKE_C_STANDARD 99)

# strongly encouraged to enable this globally to avoid conflicts between -Wpedantic being enabled and -std=c++20 and -std=gnu++20 for
# example when compiling with PCH enabled
#set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_C_EXTENSIONS ON)

project(app
        VERSION 0.1.0
        LANGUAGES C CXX)

set(CPM_DOWNLOAD_VERSION 0.40.2)
if(CPM_SOURCE_CACHE)
    # Expand relative path. This is important if the provided path contains a tilde (~)
    get_filename_component(CPM_SOURCE_CACHE ${CPM_SOURCE_CACHE} ABSOLUTE)
    set(CPM_DOWNLOAD_LOCATION "${CPM_SOURCE_CACHE}/cpm/CPM_${CPM_DOWNLOAD_VERSION}.cmake")
elseif(DEFINED ENV{CPM_SOURCE_CACHE})
    set(CPM_DOWNLOAD_LOCATION "$ENV{CPM_SOURCE_CACHE}/cpm/CPM_${CPM_DOWNLOAD_VERSION}.cmake")
else()
    set(CPM_DOWNLOAD_LOCATION "${CMAKE_BINARY_DIR}/cmake/CPM_${CPM_DOWNLOAD_VERSION}.cmake")
endif()
if(NOT (EXISTS ${CPM_DOWNLOAD_LOCATION}))
    message(STATUS "Downloading CPM.cmake to ${CPM_DOWNLOAD_LOCATION}")
    file(DOWNLOAD https://github.com/cpm-cmake/CPM.cmake/releases/download/v${CPM_DOWNLOAD_VERSION}/CPM.cmake ${CPM_DOWNLOAD_LOCATION})
endif()
include(${CPM_DOWNLOAD_LOCATION})

cpmaddpackage(
        NAME
        raylib
        GITHUB_REPOSITORY
        raysan5/raylib
        GIT_TAG
        5.5
)

target_compile_options(raylib PRIVATE $<$<C_COMPILER_ID:GNU,Clang>:-Wno-error=implicit-function-declaration>)
target_compile_options(raylib PRIVATE $<$<C_COMPILER_ID:GNU,Clang>:-Wno-unused-result>)
target_compile_options(raylib PRIVATE $<$<C_COMPILER_ID:GNU>:-Wno-stringop-overflow>)
target_compile_options(raylib PRIVATE $<$<C_COMPILER_ID:Clang>:-Wno-implicit-const-int-float-conversion>)
target_compile_features(raylib PRIVATE c_std_99)
if("${PLATFORM}" STREQUAL "Desktop")
    target_compile_features(glfw PRIVATE c_std_99)
endif()

############################################################################################################################################

if (WIN32)
    add_executable(app)
    file(COPY assets DESTINATION ${CMAKE_BINARY_DIR}/Debug)
endif()

if (NOT WIN32)
    add_executable(app)
    file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})
endif()

target_sources(app PRIVATE src/main.cpp)

target_include_directories(app PRIVATE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/src")
target_link_libraries(app raylib)
if(NOT WIN32)
    target_link_libraries(app m)
endif()

if (${PLATFORM} STREQUAL "Web")
    set_target_properties(app PROPERTIES SUFFIX ".html") # Tell Emscripten to build an example.html file.
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s USE_GLFW=3 -s FORCE_FILESYSTEM=1 -s WASM=1")

    set(web_link_flags)
    if(CMAKE_BUILD_TYPE MATCHES Debug OR CMAKE_BUILD_TYPE MATCHES RelWithDebInfo)
        set(web_link_flags "${web_link_flags} -s ASSERTIONS=1")
    endif()
    set(web_link_flags "${web_link_flags} --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/resources@resources --use-preload-plugins")
    set(web_link_flags "${web_link_flags} --shell-file ${CMAKE_CURRENT_SOURCE_DIR}/configs/minshell.html")

    set_target_properties(app PROPERTIES LINK_FLAGS "${web_link_flags}")
endif()

# Checks if OSX and links appropriate frameworks (only required on MacOS)
if(APPLE)
    target_link_libraries(app "-framework IOKit")
    target_link_libraries(app "-framework Cocoa")
    target_link_libraries(app "-framework OpenGL")
endif()

# misc
set_target_properties(app PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
# set the startup project for the "play" button in MSVC
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT app)
