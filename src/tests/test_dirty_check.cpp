#include <gtest/gtest.h>
#include "../utils/lp_dirty_check.h"
#include "../memory/lp_arena.h"

TEST(DirtyCheckTest, FirstFrameReturnsFrame) {
    lp_dirty_check checker;
    lp_arena frame1;
    frame1.leds[0].r = 100;

    // La prima volta, deve restituire il puntatore al frame
    const lp_arena* result = checker.filter_arena(&frame1);
    ASSERT_NE(result, nullptr);
    EXPECT_EQ(result, &frame1);
}

TEST(DirtyCheckTest, UnchangedFrameReturnsNull) {
    lp_dirty_check checker;
    lp_arena frame1;
    frame1.leds[0].r = 100;

    // Primo passaggio
    checker.filter_arena(&frame1);

    // Secondo passaggio con lo stesso frame
    const lp_arena* result = checker.filter_arena(&frame1);

    // Deve restituire nullptr perché non ci sono cambiamenti
    EXPECT_EQ(result, nullptr);
}

TEST(DirtyCheckTest, ChangedFrameReturnsFrame) {
    lp_dirty_check checker;
    lp_arena frame1;
    frame1.leds[0].r = 100;

    // Primo passaggio
    checker.filter_arena(&frame1);

    // Secondo frame, modificato
    lp_arena frame2 = frame1;
    frame2.leds[1].g = 50;

    const lp_arena* result = checker.filter_arena(&frame2);

    // Deve restituire il puntatore al nuovo frame perché è cambiato
    ASSERT_NE(result, nullptr);
    EXPECT_EQ(result, &frame2);
}

TEST(DirtyCheckTest, ResetMakesNextFramePass) {
    lp_dirty_check checker;
    lp_arena frame1;
    frame1.leds[0].r = 100;

    // Primo passaggio
    checker.filter_arena(&frame1);

    // Secondo passaggio con lo stesso frame, dovrebbe restituire null
    const lp_arena* result1 = checker.filter_arena(&frame1);
    EXPECT_EQ(result1, nullptr);

    // Resetta lo stato
    checker.reset();

    // Terzo passaggio, di nuovo con lo stesso frame
    const lp_arena* result2 = checker.filter_arena(&frame1);

    // Dopo un reset, il frame deve passare di nuovo, anche se è uguale al precedente
    ASSERT_NE(result2, nullptr);
    EXPECT_EQ(result2, &frame1);
}