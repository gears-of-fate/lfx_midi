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
#include "hardware/IMidiTransport.h"
#include "memory/lp_triple_buffer.h"
#include "lfx_midi/lp_device_info.h"
#include "utils/lp_dirty_check.h"
#include <atomic>

/**
 * @class lp_pipeline
 * @brief The rendering pipeline for a single Launchpad instance.
 *
 * This class is responsible for taking a frame from the triple buffer,
 * optimizing it, translating it into SysEx messages, and sending it to the
 * MIDI transport.
 */
class lp_pipeline {
public:
    lp_pipeline(lp_triple_buffer* buffer_sys, IMidiTransport* transport, e_lp_model model);

    /// @brief Processes a single frame from the read buffer.
    void process_frame();

    /// @brief Requests a state reset for the next frame (e.g., for the dirty checker).
    void request_reset();

private:
    lp_triple_buffer* m_buffer_sys;
    IMidiTransport* m_transport;
    e_lp_model m_model;
    std::atomic<bool> m_needs_reset{false};

    lp_dirty_check m_dirty_checker;
};