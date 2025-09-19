cmake_minimum_required(VERSION 3.10)

# set(CMAKE_FIND_DEBUG_MODE 1)
project(runes)

set(CMAKE_VERBOSE_MAKEFILE on)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Display search paths for libraries.

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(SFML_STATIC_LIBRARIES TRUE)
find_package(SFML 3 COMPONENTS System Window Network Graphics CONFIG REQUIRED)

include_directories(${SFML_INCLUDE_DIRS})

add_executable(
    runes
    main.cpp
    system/logic/Runes.cpp
    system/logic/RuneSystem.cpp
    system/interface/view/Game.cpp
    system/interface/widget/Board.cpp
    system/render/RenderSystem.cpp
    utility/StopCondition.cpp
)

target_precompile_headers(
    runes
    PUBLIC
    "ECS.h"
    "Messaging.h"
)

target_include_directories(runes PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(runes PRIVATE SFML::System SFML::Network SFML::Graphics SFML::Window)

if (UNIX)
    target_compile_options(runes PRIVATE $<$<CONFIG:Debug>:-g> $<$<CONFIG:Release>:-O3> -Wall -Werror -Wpedantic)
elseif (WIN32)
    target_compile_options(runes PUBLIC /W3 /MT$<$<CONFIG:Debug>:d>)
    target_link_libraries(runes PRIVATE Ws2_32)
endif()

install(TARGETS runes DESTINATION bin)

if (WIN32)
    install(FILES $<TARGET_PDB_FILE:${PROJECT_NAME}> DESTINATION bin OPTIONAL)
endif()
