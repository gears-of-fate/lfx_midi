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

#include "lp_dirty_check.h"
#include "logger.h"
#include <cstring>

lp_dirty_check::lp_dirty_check() : m_is_first_arena(true) {
    memset(&m_last_arena, 0, sizeof(lp_arena));
}

const lp_arena* lp_dirty_check::filter_arena(const lp_arena* current_frame) {
    if (m_is_first_arena) {
        m_is_first_arena = false;
        m_last_arena = *current_frame;
        return current_frame;
    }

    if (memcmp(current_frame, &m_last_arena, sizeof(lp_arena)) == 0) {
        return nullptr;
    }

    m_last_arena = *current_frame;
    return current_frame;
}

void lp_dirty_check::reset() {
    LOG("lp_dirty_check::reset");
    m_is_first_arena = true;
    memset(&m_last_arena, 0, sizeof(lp_arena));
}