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
#include <mutex>
#include <iostream>

#ifndef NDEBUG
    inline std::mutex log_mutex;
    #define LOG(stream_args) \
        do { \
            std::lock_guard<std::mutex> lock(log_mutex); \
            std::cout << "[LFX] " << stream_args << std::endl; \
        } while(0)
#else
    #define LOG(stream_args) do {} while (0)
#endif