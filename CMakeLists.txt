cmake_minimum_required(VERSION 3.9)

project(SFML_IMGUI  VERSION 1.0.0 DESCRIPTION "ImGui + SFML")
add_definitions(-std=c++14)

file(GLOB IMGUI_FILES "${CMAKE_CURRENT_SOURCE_DIR}/imgui/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/imgui/*.cpp")

add_executable(SFML_IMGUI
	src/main.cpp 
	src/EdgesVerteces.h
	src/Algorithms.h
	src/Algorithms.cpp
	src/Interface.hpp
	src/EventManager.hpp
	src/LoadSave.hpp
	 ${IMGUI_FILES}
)

source_group("Imgui" FILES ${IMGUI_FILES})
target_include_directories(SFML_IMGUI PRIVATE imgui)

find_package(OpenGL REQUIRED)
include_directories( ${OPENGL_INCLUDE_DIRS} )

target_link_libraries(SFML_IMGUI ${OPENGL_LIBRARIES} )

set(SFML_STATIC_LIBRARIES TRUE)
set(SFML_DIR "C:/repos/SFML-2.5.1/lib/cmake/SFML")

find_package(SFML 2.5 COMPONENTS system window graphics network audio REQUIRED)

target_link_libraries(SFML_IMGUI  sfml-system sfml-window sfml-graphics sfml-network sfml-audio)
