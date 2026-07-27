# Functional Requirements for LFX_MIDI Library

This document outlines the functional requirements for the LFX_MIDI library, a system for managing Launchpad MIDI devices.

## 1. System Lifecycle Management

- **FR-1.1: System Initialization**: The library must provide a function to initialize the global system. This function must accept a callback for handling MIDI input events.
- **FR-1.2: System Shutdown**: The library must provide a function to shut down the system, releasing all allocated resources and destroying all device instances.

## 2. Device Instance Management (Worker)

- **FR-2.1: Device Registration (Handover)**: The library must allow the registration of a new Launchpad device once it has been discovered by a higher layer. The information required for this "handover" includes:
    - Platform-specific handles for the MIDI input and output ports.
    - A unique serial number to identify the instance.
    - The exact Launchpad model.
- **FR-2.2: Device Disconnection**: It must be possible to de-register and destroy a device instance using its serial number. This operation is intended for permanent removal.
- **FR-2.3: List Managed Devices**: The library must provide a function to get the serial numbers of all devices it is currently managing.

## 3. Architecture for Discovery and Hot-Plug

- **FR-3.1: Library's Role (Worker)**: The library acts as a low-level "worker". **It does not implement automatic device discovery mechanisms**. Its sole responsibility is to manage the lifecycle and I/O of the device instances handed over to it.
- **FR-3.2: Application Layer's Responsibility (Manager)**: A higher application layer (the "manager" or "host") is responsible for:
    - Monitoring the operating system to detect the connection (plug-in) and disconnection (plug-out) of MIDI devices.
    - When a compatible device is **connected**, the manager must call `pass_device_info` to hand over its management to the library.
    - When a device is **temporarily disconnected** (hot-plug out), the manager should call `pause_device` to suspend rendering without destroying the instance.
    - When the device is **reconnected** (hot-plug in), the manager should call `resume_device` to resume rendering.
    - When a device is **permanently removed** or on application shutdown, the manager must call `device_disconnected` to allow the library to release the associated resources.
- **FR-3.3: Hot-Plug Support via Lifecycle Hooks**: The `pause_device` and `resume_device` functions are the key mechanisms provided by the library to allow the manager to handle hot-plug events gracefully. This approach avoids the overhead of destroying and recreating an instance for brief disconnections, preserving the device's state.

## 4. Input and Output

- **FR-4.1: MIDI Input Callback**: The system must invoke the callback registered at initialization upon receiving a MIDI input event, forwarding the event type, pad ID, and timestamp.
- **FR-4.2: Direct Write Buffer Access**: To maximize performance, the library must provide a direct pointer to the memory buffer for a device's LED data.
- **FR-4.3: Frame Commit**: After writing to the buffer, a `commit` function must be called to signal that the frame is ready to be sent to the device.

## 5. Non-Functional Requirements

- **NF-5.1: Efficiency**: Direct write buffer access and explicit commit are designed to minimize latency and maximize performance.
- **NF-5.2: Portability**: The library is designed to be used across different platforms (Windows, Android), abstracting platform-specific MIDI hardware details through `void*` handles.
- **NF-5.3: Thread Safety**: The library must ensure that device operations are thread-safe.
