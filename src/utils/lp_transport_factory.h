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
#include <memory>
#include "../hardware/IMidiTransport.h"
#include "lfx_midi/lp_device_info.h"

/**
 * @class lp_transport_factory
 * @brief A static factory for creating platform-specific MIDI transports.
 */
class lp_transport_factory {
public:
    /**
     * @brief Creates a MIDI transport instance based on the current platform.
     * @param device_info The information of the device for which to create the transport.
     * @return A unique_ptr to an IMidiTransport implementation.
     */
    static std::unique_ptr<IMidiTransport> create_transport(const lp_device_info& device_info);
};