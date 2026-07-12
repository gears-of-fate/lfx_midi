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

#include "lp_transport_factory.h"
#include "logger.h"
#ifdef __ANDROID__
#include "hardware/android/lp_transport_android.h"
#elif defined(_WIN32)
#include "../hardware/windows/lp_transport_win.h"
#endif

std::unique_ptr<IMidiTransport> lp_transport_factory::create_transport(const lp_device_info& device_info) {
    LOG("lp_transport_factory::create_transport");
    std::string type;
    switch (device_info.model) {
        case e_lp_model::LAUNCHPAD_PRO_MK3:  type = "LAUNCHPAD_PRO_MK3"; break;
        case e_lp_model::LAUNCHPAD_X:        type = "LAUNCHPAD_X"; break;
        case e_lp_model::LAUNCHPAD_MK3_MINI: type = "LAUNCHPAD_MK3_MINI"; break;
        case e_lp_model::LAUNCHPAD_MK2:      type = "LAUNCHPAD_MK2"; break;
        case e_lp_model::LAUNCHPAD_PRO:      type = "LAUNCHPAD_PRO"; break;
    }
#ifdef __ANDROID__
    return std::make_unique<lp_transport_android>(type, device_info.midi_in, device_info.midi_out);
#elif defined(_WIN32)
    return std::make_unique<lp_transport_win>(type, device_info.midi_in, device_info.midi_out);
#endif
}