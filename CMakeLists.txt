cmake_minimum_required(VERSION 3.10)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(Falling-Sand-Engine)

include(FetchContent)

### 3rd party dependencies

# toml dependency
FetchContent_Declare(
	tomlplusplus
	GIT_REPOSITORY https://github.com/marzer/tomlplusplus
	GIT_TAG master
)

FetchContent_MakeAvailable(tomlplusplus)

# I do not need this for compilation, however I do it so that I get it shown in VS Community File tab
file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp)
file(GLOB_RECURSE SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

file(GLOB_RECURSE CHILI_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/chili/*)
file(GLOB_RECURSE ENGINE_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/engine2d/*)
file(GLOB_RECURSE FALLING_SAND_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/falling-sand/*)

# To satissfy some ChilliFramework stuff
add_definitions(-DUNICODE -D_UNICODE)

add_executable(Falling-Sand-Engine WIN32 ${SRC_FILES} ${HEADERS})
target_include_directories(Falling-Sand-Engine PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/)
target_include_directories(Falling-Sand-Engine PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/shaders/)
# 3rd party
target_include_directories(Falling-Sand-Engine PUBLIC ${tomlplusplus_SOURCE_DIR}/include/)

message(${CMAKE_BINARY_DIR})
add_custom_command(TARGET Falling-Sand-Engine POST_BUILD 
		COMMAND ${CMAKE_COMMAND} -E copy_directory
		${CMAKE_SOURCE_DIR}/assets
		${CMAKE_BINARY_DIR}/assets)

# for visual studio
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src/chili PREFIX "chili" FILES ${CHILI_SRC})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src/engine2d PREFIX "engine2d" FILES ${ENGINE_SRC})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src/falling-sand PREFIX "falling-sand" FILES ${FALLING_SAND_SRC})

target_compile_definitions(Falling-Sand-Engine PRIVATE TOML_ENABLE_WINDOWS_COMPAT=0)
