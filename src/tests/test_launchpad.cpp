#include <gtest/gtest.h>
#include "lfx_midi/ALaunchpad.h"
#include <vector>
#include <string>

// Usiamo una Test Fixture per gestire correttamente lo stato globale della Facade
class LaunchpadFacadeTest : public ::testing::Test {
protected:
    void TearDown() override {
        ALaunchpad_h::shutdown_system();
    }
};

TEST_F(LaunchpadFacadeTest, InitAndShutdown) {
    EXPECT_TRUE(ALaunchpad_h::init_system(nullptr));
    EXPECT_FALSE(ALaunchpad_h::init_system(nullptr));
    ALaunchpad_h::shutdown_system();
    EXPECT_TRUE(ALaunchpad_h::init_system(nullptr));
}

TEST_F(LaunchpadFacadeTest, DeviceConnectionAndJniPointers) {
    ALaunchpad_h::init_system(nullptr);

    std::string test_serial = "FACADE-001";

    EXPECT_EQ(ALaunchpad_h::get_jni_write_pointer(test_serial), nullptr);

    lp_device_info info;
    info.device_id = 1;
    info.model = e_lp_model::LAUNCHPAD_MK2; // Usiamo MK2 che non ha logica speciale di porte
    info.serial_number = test_serial;
    info.midi_in = reinterpret_cast<void*>(0x01);
    info.midi_out = reinterpret_cast<void*>(0x02);

    EXPECT_TRUE(ALaunchpad_h::pass_device_info(info));

    uint8_t* jni_ptr = ALaunchpad_h::get_jni_write_pointer(test_serial);
    EXPECT_NE(jni_ptr, nullptr);

    ALaunchpad_h::commit_frame(test_serial);

    EXPECT_TRUE(ALaunchpad_h::device_disconnected(test_serial));
    EXPECT_EQ(ALaunchpad_h::get_jni_write_pointer(test_serial), nullptr);
}