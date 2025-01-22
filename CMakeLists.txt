cmake_minimum_required(VERSION 3.18)
set(PROJECT_NAME "SkiaInput")
project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")

link_directories(
    "D:\\sdk\\skia\\out"
)
include_directories(
    "D:\\sdk\\skia"
)
add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE 
	"main.cpp" 
	"WindowInput.h" 
	"WindowInput.cpp" "WindowInputKey.cpp" "WindowInputMouse.cpp" "WindowInputNativeMsg.cpp" "WindowInputPaint.cpp")

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")
set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DPI_AWARE "PerMonitor")
target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<CONFIG:Debug>:UNICODE DEBUG NOMINMAX _SILENCE_ALL_CXX20_DEPRECATION_WARNINGS WIN32 _WINDOWS>)
target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<CONFIG:Release>:UNICODE NDEBUG NOMINMAX _SILENCE_ALL_CXX20_DEPRECATION_WARNINGS WIN32 _WINDOWS>)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} 
	skia 
	dwmapi
	kernel32 
	user32 
	shell32
	usp10
	imm32
)