#include <gtest/gtest.h>
#include "../utils/lp_translator.h"
#include "../memory/lp_arena.h"

TEST(TranslatorTest, ProgrammerModeSysex) {
    auto sysex_mk3 = lp_translator::get_programmer_mode_sysex(e_lp_model::LAUNCHPAD_PRO_MK3);
    
    // Header MK3: F0 00 20 29 02 0E
    // Comando: 0E 01
    // Fine: F7
    ASSERT_EQ(sysex_mk3.size(), 9);
    EXPECT_EQ(sysex_mk3[0], 0xF0);
    EXPECT_EQ(sysex_mk3[5], 0x0E); // Byte finale dell'header MK3
    EXPECT_EQ(sysex_mk3[6], 0x0E); // Comando Layout
    EXPECT_EQ(sysex_mk3[7], 0x01); // Valore Prog Mode
    EXPECT_EQ(sysex_mk3[8], 0xF7); // EOX

    auto sysex_mk2 = lp_translator::get_programmer_mode_sysex(e_lp_model::LAUNCHPAD_MK2);
    ASSERT_EQ(sysex_mk2.size(), 9);
    EXPECT_EQ(sysex_mk2[5], 0x18); // Byte finale dell'header MK2
    EXPECT_EQ(sysex_mk2[6], 0x22); // Comando Layout
    EXPECT_EQ(sysex_mk2[7], 0x01); // Layout User 1
}

TEST(TranslatorTest, LiveModeSysex) {
    auto sysex_mk3 = lp_translator::get_live_mode_sysex(e_lp_model::LAUNCHPAD_PRO_MK3);
    
    ASSERT_EQ(sysex_mk3.size(), 9);
    EXPECT_EQ(sysex_mk3[0], 0xF0);
    EXPECT_EQ(sysex_mk3[5], 0x0E); // Header MK3
    EXPECT_EQ(sysex_mk3[6], 0x0E); // Mode CMD
    EXPECT_EQ(sysex_mk3[7], 0x00); // Live Mode
    EXPECT_EQ(sysex_mk3[8], 0xF7); // EOX

    auto sysex_mk2 = lp_translator::get_live_mode_sysex(e_lp_model::LAUNCHPAD_MK2);
    ASSERT_EQ(sysex_mk2.size(), 9);
    EXPECT_EQ(sysex_mk2[5], 0x18); // Header MK2
    EXPECT_EQ(sysex_mk2[6], 0x22); // Layout CMD
    EXPECT_EQ(sysex_mk2[7], 0x00); // Layout Session
}

TEST(TranslatorTest, BuildFrameSysexChunkingAndColor) {
    lp_arena frame;
    // Accendiamo tutti i pad validi per forzare il chunking
    for (int i = 0; i < 128; ++i) {
        if (lp_translator::is_valid_pad(e_lp_model::LAUNCHPAD_PRO_MK3, i)) {
            frame.leds[i] = {255, 100, 0};
        }
    }

    auto chunks = lp_translator::build_frame_sysex(e_lp_model::LAUNCHPAD_PRO_MK3, &frame);

    // Con circa 96 pad validi e 5 byte per pad, ci aspettiamo almeno 2 chunk.
    ASSERT_GE(chunks.size(), 2);

    // Verifichiamo il primo chunk
    const auto& first_chunk = chunks[0];
    EXPECT_EQ(first_chunk[0], 0xF0); // Start
    EXPECT_EQ(first_chunk[5], 0x0E); // Header
    EXPECT_EQ(first_chunk[6], 0x03); // Comando globale RGB
    EXPECT_LT(first_chunk.size(), 256); // Dimensione massima
    EXPECT_EQ(first_chunk.back(), 0xF7); // End

    // Verifichiamo il formato del primo pad nel primo chunk (es. pad 1)
    EXPECT_EQ(first_chunk[7], 0x03); // Prefisso RGB
    EXPECT_EQ(first_chunk[8], 1);    // ID Pad
    EXPECT_EQ(first_chunk[9], 255 / 2); // R
    EXPECT_EQ(first_chunk[10], 100 / 2); // G
    EXPECT_EQ(first_chunk[11], 0);      // B
}