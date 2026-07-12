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
#include "lp_led.h"
#include <array>

/**
 * @struct lp_arena
 * @brief Represents the entire grid of LEDs for a Launchpad.
 *
 * This struct acts as a frame buffer, holding the color state for each
 * individual LED pad on the device.
 */
struct lp_arena {
    /// @brief An array holding the state of all LEDs. The size should be sufficient for the largest supported device.
    std::array<lp_led, 128> leds;
};