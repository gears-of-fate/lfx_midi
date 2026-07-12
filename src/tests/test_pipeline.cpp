#include <gtest/gtest.h>
#include "../core/lp_pipeline.h"
#include "../utils/lp_translator.h"

// 1. Creiamo un Transport fittizio per intercettare i dati
class MockMidiTransport : public IMidiTransport {
public:
    std::vector<std::vector<uint8_t>> sent_messages;
    
    void set_callback(MidiInputCallback cb) override {}
    
    void send_sysex(const std::vector<uint8_t>& final_sysex) override {
        sent_messages.push_back(final_sysex);
    }

    void start_listening() override {}
};

// 2. Testiamo la pipeline
TEST(PipelineTest, ProcessFrameSendsCorrectSysex) {
    lp_triple_buffer buffer;
    MockMidiTransport mock_transport;
    
    lp_pipeline pipeline(&buffer, &mock_transport, e_lp_model::LAUNCHPAD_PRO_MK3);

    uint8_t* write_ptr = buffer.get_jni_write_pointer();
    lp_arena* arena_write = reinterpret_cast<lp_arena*>(write_ptr);
    arena_write->leds[1].r = 255;
    arena_write->leds[1].g = 0;
    arena_write->leds[1].b = 0;
    buffer.commit_from_jni();

    pipeline.process_frame();

    ASSERT_FALSE(mock_transport.sent_messages.empty());
    
    const auto& first_chunk = mock_transport.sent_messages[0];
    
    EXPECT_EQ(first_chunk[0], 0xF0);
    EXPECT_EQ(first_chunk[5], 0x0E); // Header MK3
    EXPECT_EQ(first_chunk[6], 0x03); // Comando globale RGB

    bool found = false;
    for (size_t i = 7; i < first_chunk.size() - 4; i += 5) {
        if (first_chunk[i] == 0x03 && first_chunk[i+1] == 1) {
            found = true;
            EXPECT_EQ(first_chunk[i+2], 255 / 2); // R
            EXPECT_EQ(first_chunk[i+3], 0);       // G
            EXPECT_EQ(first_chunk[i+4], 0);       // B
            break;
        }
    }
    ASSERT_TRUE(found);
}

TEST(PipelineTest, ProcessFrameWithNullTransportDoesNotCrash) {
    lp_triple_buffer buffer;
    lp_pipeline pipeline(&buffer, nullptr, e_lp_model::LAUNCHPAD_PRO_MK3);

    uint8_t* write_ptr = buffer.get_jni_write_pointer();
    lp_arena* arena_write = reinterpret_cast<lp_arena*>(write_ptr);
    arena_write->leds[1].r = 255;
    buffer.commit_from_jni();

    EXPECT_NO_FATAL_FAILURE({ pipeline.process_frame(); });
}