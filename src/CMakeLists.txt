cmake_minimum_required(VERSION 3.10)

# set(CMAKE_FIND_DEBUG_MODE 1)
project(runes)

set(CMAKE_VERBOSE_MAKEFILE on )
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Display search paths for libraries.

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(SFML COMPONENTS system window graphics CONFIG REQUIRED)

include_directories(${SFML_INCLUDE_DIRS})

add_executable(
    runes
    main.cpp
    Application.cpp
    util/StopCondition.cpp
    states/GameState.cpp
    model/Runes.cpp
    interface/Board.cpp
    interface/Button.cpp
    interface/RuneTile.cpp
    interface/Window.cpp
)

target_include_directories(runes PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(runes PRIVATE sfml-system sfml-network sfml-graphics sfml-window)

if (WIN32)
    target_link_libraries(runes PRIVATE sfml-main Ws2_32)
    target_compile_options(runes PUBLIC /W3 /MT$<$<CONFIG:Debug>:d>)
endif()

install(TARGETS runes DESTINATION bin)
install(FILES $<TARGET_PDB_FILE:${PROJECT_NAME}> DESTINATION bin OPTIONAL)
