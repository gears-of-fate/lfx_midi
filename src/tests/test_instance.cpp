#include <gtest/gtest.h>
#include "../core/lp_instance.h"
#include "../hardware/IMidiTransport.h"
#include "lfx_midi/lp_device_info.h"
#include <thread>
#include <chrono>
#include <atomic>
#include "../utils/logger.h"

// Usiamo un mock transport per non far esplodere nulla
class DummyTransport : public IMidiTransport {
public:
    std::atomic<int> frames_sent{0};
    void set_callback(MidiInputCallback cb) override {}
    void send_sysex(const std::vector<uint8_t>&) override {
        frames_sent++;
    }
    void start_listening() override {}
};

TEST(InstanceTest, LifecycleStartStop) {
    lp_device_info dummy_info;
    dummy_info.model = e_lp_model::LAUNCHPAD_PRO_MK3;
    
    auto dummy_transport = std::make_unique<DummyTransport>();
    auto* transport_ptr = dummy_transport.get();
    
    lp_instance instance(dummy_info, std::move(dummy_transport));
    
    instance.start();
    
    transport_ptr->frames_sent.store(0);

    lp_triple_buffer* buffer = instance.get_triple_buffer();
    for (int i = 0; i < 5; i++) {
        uint8_t* ptr = buffer->get_jni_write_pointer();
        ptr[0] = static_cast<uint8_t>(i + 1);
        buffer->commit_from_jni();
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }

    instance.stop();
    EXPECT_GT(transport_ptr->frames_sent, 0);
}

TEST(InstanceTest, PauseResumesCorrectly) {
    lp_device_info dummy_info;
    dummy_info.model = e_lp_model::LAUNCHPAD_PRO_MK3;
    auto dummy_transport = std::make_unique<DummyTransport>();
    auto* transport_ptr = dummy_transport.get();
    
    lp_instance instance(dummy_info, std::move(dummy_transport));

    instance.start();
    
    instance.pause();
    
    int sent_before = transport_ptr->frames_sent;
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    int sent_after = transport_ptr->frames_sent;
    
    EXPECT_EQ(sent_before, sent_after);

    instance.resume();
    instance.stop();
}

TEST(InstanceTest, NullTransportTolerance) {
    lp_device_info dummy_info;
    dummy_info.model = e_lp_model::LAUNCHPAD_PRO_MK3;
    
    lp_instance instance(dummy_info, nullptr);
    
    EXPECT_NO_FATAL_FAILURE({
        instance.start();
        
        lp_triple_buffer* buffer = instance.get_triple_buffer();
        buffer->get_jni_write_pointer()[0] = 255;
        buffer->commit_from_jni();
        
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
        instance.stop();
    });
}