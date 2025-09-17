cmake_minimum_required(VERSION 3.10)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(Falling-Sand-Engine)

# I do not need this for compilation, however I do it so that I get it shown in VS Community File tab
file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp)
file(GLOB_RECURSE SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

# To satissfy some ChilliFramework stuff
add_definitions(-DUNICODE -D_UNICODE)

add_executable(Falling-Sand-Engine ${SRC_FILES} ${HEADERS})
target_include_directories(Falling-Sand-Engine PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/)
target_include_directories(Falling-Sand-Engine PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/shaders/)

message(${CMAKE_BINARY_DIR})
add_custom_command(TARGET Falling-Sand-Engine POST_BUILD 
		COMMAND ${CMAKE_COMMAND} -E copy_directory
		${CMAKE_SOURCE_DIR}/assets
		${CMAKE_BINARY_DIR}/assets)

