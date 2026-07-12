# LFX_MIDI: A Modern C++ Library for Novation Launchpad

LFX_MIDI is a high-performance, cross-platform C++20 library designed for direct and efficient control of Novation Launchpad devices. It provides a clean, modern C++ interface to manage device communication, state, and high-framerate LED rendering, making it ideal for interactive light shows, custom controllers, and complex musical applications.

## Features

-   **High Performance:** Utilizes a lock-free triple buffer for state updates, enabling tear-free LED rendering at 60+ FPS on modern devices.
-   **Cross-Platform:** Supports both Windows (via WinMM) and Android (via AMidi NDK) with a unified interface.
-   **Modern C++:** Written in C++20, leveraging modern features for clean, safe, and efficient code.
-   **Thread-Safe:** Designed for use in multi-threaded applications, with clear separation of concerns between the UI/logic thread and the rendering thread.
-   **Multi-Device Management:** Robustly manages multiple connected Launchpad devices simultaneously.
-   **Extensive Device Support:**
    -   Launchpad Pro MK3
    -   Launchpad X
    -   Launchpad Mini MK3
    -   Launchpad MK2
    -   Launchpad Pro (Original)

## Getting Started

### Prerequisites

-   A C++20 compatible compiler (e.g., GCC, Clang, MSVC)
-   CMake (version 3.24 or higher)

### Building the Library and Examples

1.  **Clone the repository:**
    ```sh
    git clone https://github.com/gears-of-fate/lfx_midi.git
    cd lfx_midi
    ```

2.  **Configure CMake:**
    -   For a Debug build (includes tests and sandbox example):
        ```sh
        cmake -S . -B cmake-build-debug -DCMAKE_BUILD_TYPE=Debug
        ```
    -   For a Release build (optimized library only):
        ```sh
        cmake -S . -B cmake-build-release -DCMAKE_BUILD_TYPE=Release -DLFX_MIDI_BUILD_TESTS=OFF -DLFX_MIDI_BUILD_SANDBOX=OFF
        ```

3.  **Build the project:**
    ```sh
    cmake --build cmake-build-debug
    ```
    (or `cmake --build cmake-build-release` for the Release version)

### Quick Start: Example Usage

The following example demonstrates how to initialize the system, register a device, and run a simple color animation. The platform-specific device discovery part is omitted for brevity (see `examples/main.cpp` for a full implementation).

```cpp
#include "lfx_midi/ALaunchpad.h"
#include <iostream>
#include <thread>
#include <chrono>

// 1. Define a callback for MIDI input
void my_midi_callback(const std::string& type, int pad_id, uint64_t timestamp) {
    std::cout << "Input from " << type << ": Pad " << pad_id << std::endl;
}

int main() {
    // 2. Initialize the LFX_MIDI system
    if (!ALaunchpad_h::init_system(my_midi_callback)) {
        std::cerr << "Failed to initialize system." << std::endl;
        return 1;
    }

    // 3. Discover a device and register it with the library
    // (This part requires platform-specific code to find MIDI ports)
    lp_device_info my_device;
    // ... populate my_device with the correct port handles, model, and a unique serial ...
    
    if (!ALaunchpad_h::pass_device_info(my_device)) {
        std::cerr << "Failed to register device." << std::endl;
    }

    // 4. Get the write buffer and update the LEDs in a loop
    auto start_time = std::chrono::steady_clock::now();
    while (std::chrono::steady_clock::now() - start_time < std::chrono::seconds(10)) {
        
        double time = std::chrono::duration<double>(std::chrono::steady_clock::now() - start_time).count();
        uint8_t r = (sin(time * 2.0) + 1.0) / 2.0 * 127;
        uint8_t g = (sin(time * 2.0 + 2.0) + 1.0) / 2.0 * 127;
        
        uint8_t* write_ptr = ALaunchpad_h::get_jni_write_pointer(my_device.serial_number);
        if (write_ptr) {
            auto* arena = reinterpret_cast<lp_arena*>(write_ptr);
            for (int i = 0; i < 128; ++i) {
                arena->leds[i] = {r, g, 0};
            }
            // 5. Commit the frame to make it visible
            ALaunchpad_h::commit_frame(my_device.serial_number);
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    // 6. Shut down the system
    ALaunchpad_h::shutdown_system();

    return 0;
}
```

## Using LFX_MIDI in Your Project (CMake)

This library is designed to be easily integrated using CMake's `find_package` command.

1.  **Build and Install LFX_MIDI:**
    Follow the steps above to create a release build, then run the install command:
    ```sh
    # This will create a clean installation in the 'lfx_midi-install' directory
    cmake --install cmake-build-release --prefix lfx_midi-install
    ```

2.  **Integrate into Your Project:**
    In your `CMakeLists.txt`, add the following:
    ```cmake
    # Tell CMake where to find the installed lfx_midi package
    set(CMAKE_PREFIX_PATH ${CMAKE_CURRENT_SOURCE_DIR}/vendor/lfx_midi-install) # Adjust path as needed

    find_package(lfx_midi REQUIRED)

    # Link the library to your executable
    target_link_libraries(YourProjectName PRIVATE lfx_midi::lfx_midi)
    ```

## Reference Documentation

This library was developed with the help of Novation's official Programmer's Reference Guides. For detailed information on SysEx commands and device-specific features, please consult the following manuals:

-   [Launchpad Pro MK3 Programmer's Reference Guide](https://fael-downloads-prod.focusrite.com/customer/prod/s3fs-public/downloads/LPP3_prog_ref_guide_200415.pdf)
-   [Launchpad X Programmer's Reference Guide](https://fael-downloads-prod.focusrite.com/customer/prod/s3fs-public/downloads/Launchpad%20X%20-%20Programmers%20Reference%20Manual.pdf)
-   [Launchpad MK2 Technical Reference Manual](https://fael-downloads-prod.focusrite.com/customer/prod/s3fs-public/downloads/Launchpad%20MK2%20Programmers%20Reference%20Manual%20v1.03.pdf)
-   [Launchpad Pro (Original) Programmer's Reference Guide](https://fael-downloads-prod.focusrite.com/customer/prod/s3fs-public/downloads/Launchpad%20Pro%20Programmers%20Reference%20Guide%201.01.pdf)
-   [Launchpad mini MK3 Programmer's Reference Guide](https://www.djshop.gr/Attachment/DownloadFile?downloadId=10737)

## License

This project is licensed under the GNU General Public License v3.0. See the [LICENSE](LICENSE) file for details.
```