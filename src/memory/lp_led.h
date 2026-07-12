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
#include <cstdint>

/**
 * @struct lp_led
 * @brief Represents the color of a single LED pad.
 */
struct lp_led {
    /// @brief Red color component (0-255).
    uint8_t r = 0;
    /// @brief Green color component (0-255).
    uint8_t g = 0;
    /// @brief Blue color component (0-255).
    uint8_t b = 0;
};