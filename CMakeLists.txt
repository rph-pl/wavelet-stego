cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)

project(wavelet_stego)


set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic -Wcast-align -Woverloaded-virtual -ggdb -O0")

find_package(PkgConfig REQUIRED)

pkg_search_module(GSL REQUIRED gsl)
pkg_search_module(GTKMM REQUIRED gtkmm-3.0)

add_executable(wavelet_stego
		main.cpp
		main_window.cpp
		image.cpp
		wavelet.cpp
	)

target_link_libraries(wavelet_stego ${GSL_LIBRARIES} ${GTKMM_LIBRARIES})
target_include_directories(wavelet_stego PUBLIC ${GSL_INCLUDE_DIRS} ${GTKMM_INCLUDE_DIRS})
target_compile_options(wavelet_stego PUBLIC ${GSL_CFLAGS_OTHER} ${GTKMM_CFLAGS_OTHER})

