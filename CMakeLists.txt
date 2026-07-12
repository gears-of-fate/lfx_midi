#
# Copyright (C) 2026 Toymacker-System <dev@toymacker-system.dev>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.
#

cmake_minimum_required(VERSION 3.24)
project(lfx_midi)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(LFX_MIDI_BUILD_TESTS "Build the unit tests" ON)
option(LFX_MIDI_BUILD_SANDBOX "Build the sandbox example" ON)

add_library(lfx_midi STATIC)

target_sources(lfx_midi PRIVATE
        src/core/lp_discovery.cpp
        src/core/lp_instance.cpp
        src/core/lp_pipeline.cpp
        src/memory/lp_triple_buffer.cpp
        src/utils/lp_dirty_check.cpp
        src/utils/lp_translator.cpp
        src/utils/lp_transport_factory.cpp
        src/ALaunchpad.cpp
)

target_include_directories(lfx_midi
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:include>
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/src
)

if (ANDROID)
    target_sources(lfx_midi PRIVATE src/hardware/android/lp_transport_android.cpp)
    find_library(log-lib log)
    find_library(amidi-lib amidi)
    target_link_libraries(lfx_midi PRIVATE ${log-lib} ${amidi-lib})
elseif (WIN32)
    target_sources(lfx_midi PRIVATE src/hardware/windows/lp_transport_win.cpp)
    target_link_libraries(lfx_midi PRIVATE winmm)
endif()

if(LFX_MIDI_BUILD_TESTS)
    include(FetchContent)
    FetchContent_Declare(
            googletest
            URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    enable_testing()

    add_executable(lfx_midi_tests
            src/tests/test_triple_buffer.cpp
            src/tests/test_translator.cpp
            src/tests/test_dirty_check.cpp
            src/tests/test_pipeline.cpp
            src/tests/test_instance.cpp
            src/tests/test_discovery.cpp
            src/tests/test_launchpad.cpp
    )

    target_include_directories(lfx_midi_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

    target_link_libraries(lfx_midi_tests
            PRIVATE
            lfx_midi
            gtest_main
    )

    include(GoogleTest)
    gtest_discover_tests(lfx_midi_tests)
endif()

if(LFX_MIDI_BUILD_SANDBOX)
    add_executable(lfx_midi_sandbox examples/main.cpp)
    target_link_libraries(lfx_midi_sandbox PRIVATE lfx_midi)

    if (WIN32)
        target_link_libraries(lfx_midi_sandbox PRIVATE winmm)
    endif()
endif()

include(GNUInstallDirs)

install(TARGETS lfx_midi
    EXPORT lfx_midi-targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(DIRECTORY include/lfx_midi
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(EXPORT lfx_midi-targets
    FILE lfx_midi-targets.cmake
    NAMESPACE lfx_midi::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/lfx_midi
)

include(CMakePackageConfigHelpers)
configure_package_config_file(
    "cmake/lfx_midi-config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/lfx_midi-config.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/lfx_midi"
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/lfx_midi-config.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/lfx_midi"
)