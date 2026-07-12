/*
 * Copyright (C) 2026 Toymacker-System <dev@toymacker-system.dev>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once
#include <thread>
#include <atomic>
#include <memory>
#include "lfx_midi/lp_device_info.h"
#include "memory/lp_triple_buffer.h"
#include "lp_pipeline.h"

/**
 * @class lp_instance
 * @brief Represents a single, active Launchpad device instance.
 *
 * This class encapsulates all resources for one device, including its
 * rendering pipeline, triple buffer, and MIDI transport. It manages a
 * dedicated thread for the rendering loop.
 */
class lp_instance {
public:
    lp_instance(const lp_device_info& info, std::unique_ptr<IMidiTransport> transport);
    ~lp_instance();

    /// @brief Starts the dedicated rendering thread for this device.
    void start();
    /// @brief Stops the rendering thread and joins it.
    void stop();
    /// @brief Pauses the rendering loop.
    void pause();
    /// @brief Resumes the rendering loop.
    void resume();
    
    /**
     * @brief Gets a pointer to the instance's triple buffer.
     * @return A non-owning pointer to the lp_triple_buffer.
     */
    lp_triple_buffer* get_triple_buffer();

private:
    /// @brief The main function for the rendering thread.
    void thread_execution();

    lp_device_info m_instance_info;
    std::unique_ptr<IMidiTransport> m_transport;
    lp_triple_buffer m_buffer;
    lp_pipeline m_pipeline;
    
    std::thread m_gameloop_thread;
    std::atomic<bool> m_is_running;
    std::atomic<bool> m_is_paused;
};