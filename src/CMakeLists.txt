cmake_minimum_required(VERSION 3.10)

# set(CMAKE_FIND_DEBUG_MODE 1)
project(runes)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

# Display search paths for libraries.

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# vcpkg\installed\x64-windows-static\share\sfml\SFMLConfig.cmake
find_package(SFML COMPONENTS system window graphics CONFIG REQUIRED)
find_package(Eigen3 REQUIRED)

include_directories(${EIGEN3_INCLUDE_DIRS} ${SFML_INCLUDE_DIRS})

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

# Additional logic depending on the operating system.
target_compile_options(
    runes
    PUBLIC
    /W3
)
target_link_libraries(runes PUBLIC Ws2_32)

install(TARGETS runes DESTINATION bin)
