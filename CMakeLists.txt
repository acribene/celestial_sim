cmake_minimum_required(VERSION 3.20)

# If not provided, auto-wire vcpkg toolchain from VCPKG_ROOT for easier MSVC use.
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE AND DEFINED ENV{VCPKG_ROOT})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")
endif()

project(celestial_sim LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Match warning levels across toolchains.
if(MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Source and include layout mirrors the Makefile.
file(GLOB SRC_FILES CONFIGURE_DEPENDS
    src/source/*.cpp
    src/utils/*.cpp
)

add_executable(celestial_sim ${SRC_FILES})

target_include_directories(celestial_sim PRIVATE
    src/headers
    src/utils
)

# raylib via vcpkg (uses CONFIG package).
find_package(raylib CONFIG REQUIRED)

target_link_libraries(celestial_sim PRIVATE raylib)

if(WIN32)
    target_link_libraries(celestial_sim PRIVATE
        opengl32
        gdi32
        winmm
    )
endif()
