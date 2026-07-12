#include <gtest/gtest.h>
#include "../core/lp_discovery.h"
#include "../hardware/IMidiTransport.h"
#include "lfx_midi/lp_device_info.h"

// Un mock transport per evitare di inviare dati reali durante i test
class DiscoveryMockTransport : public IMidiTransport {
public:
    void set_callback(MidiInputCallback cb) override {}
    void send_sysex(const std::vector<uint8_t>& final_sysex) override {}
    void start_listening() override {}
};

TEST(DiscoveryTest, DeviceConnectionLifecycle) {
    lp_discovery discovery;
    
    lp_device_info info{1, e_lp_model::LAUNCHPAD_PRO_MK3, "TEST-SN-001", nullptr, nullptr};

    EXPECT_EQ(discovery.get_fast_buffer(info.serial_number), nullptr);

    // Usiamo il valore di ritorno booleano
    EXPECT_TRUE(discovery.on_device_connected(info, std::make_unique<DiscoveryMockTransport>()));

    EXPECT_NE(discovery.get_fast_buffer(info.serial_number), nullptr);

    EXPECT_TRUE(discovery.on_device_disconnected(info.serial_number));

    EXPECT_EQ(discovery.get_fast_buffer(info.serial_number), nullptr);
}

TEST(DiscoveryTest, MultipleDevicesManagement) {
    lp_discovery discovery;
    
    lp_device_info info1{1, e_lp_model::LAUNCHPAD_PRO_MK3, "SN-1"};
    lp_device_info info2{2, e_lp_model::LAUNCHPAD_MK2, "SN-2"};

    EXPECT_TRUE(discovery.on_device_connected(info1, std::make_unique<DiscoveryMockTransport>()));
    EXPECT_TRUE(discovery.on_device_connected(info2, std::make_unique<DiscoveryMockTransport>()));

    auto* buffer1 = discovery.get_fast_buffer("SN-1");
    auto* buffer2 = discovery.get_fast_buffer("SN-2");
    
    EXPECT_NE(buffer1, nullptr);
    EXPECT_NE(buffer2, nullptr);
    EXPECT_NE(buffer1, buffer2);

    EXPECT_TRUE(discovery.on_device_disconnected("SN-1"));
    
    EXPECT_EQ(discovery.get_fast_buffer("SN-1"), nullptr);
    EXPECT_NE(discovery.get_fast_buffer("SN-2"), nullptr);
}