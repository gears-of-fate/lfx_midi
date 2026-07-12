#include <gtest/gtest.h>
#include <thread>
#include <atomic>
#include <chrono>
#include "../memory/lp_triple_buffer.h"
#include "../memory/lp_arena.h" // Assumendo che contenga la definizione di lp_arena

TEST(TripleBufferTest, WriteAndReadSingleFrame) {
    lp_triple_buffer buffer;

    // 1. Simula JNI che scrive un dato nel buffer
    lp_arena* write_arena = reinterpret_cast<lp_arena*>(buffer.get_jni_write_pointer());
    write_arena->leds[0].r = 42; // Scriviamo un valore fittizio
    
    // Il buffer in lettura dovrebbe ancora essere intatto (inizializzato a zero)
    const lp_arena* read_arena = buffer.get_read_buffer();
    EXPECT_EQ(read_arena->leds[0].r, 0);

    // 2. Commit del frame (JNI ha finito)
    buffer.commit_from_jni();

    // 3. Il consumatore legge il nuovo frame
    read_arena = buffer.get_read_buffer();
    
    // Ora dovremmo vedere il valore aggiornato
    EXPECT_EQ(read_arena->leds[0].r, 42);
}

TEST(TripleBufferTest, ConcurrentReadWriteSim) {
    lp_triple_buffer buffer;
    std::atomic<bool> keep_running{true};
    std::atomic<int> frames_read{0};

    // Thread Consumatore (Es. Motore Grafico/Logico)
    std::thread reader([&]() {
        int expected_value = 1;
        while (keep_running) {
            const lp_arena* read_arena = buffer.get_read_buffer();
            
            // Se c'è un dato nuovo, verifichiamo che sia sensato
            if (read_arena->leds[0].r != 0) {
                // Siccome il lettore potrebbe perdersi dei frame a causa del triple buffer (che è il comportamento corretto e voluto in un triple buffer per evitare tearing), 
                // controlliamo solo che il valore cresca o sia uguale
                EXPECT_GE(read_arena->leds[0].r, expected_value); 
                expected_value = read_arena->leds[0].r;
                frames_read++;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(2));
        }
    });

    // Thread Produttore (Simula JNI che sputa eventi MIDI ad alta frequenza)
    for (int i = 1; i <= 50; ++i) {
        lp_arena* write_arena = reinterpret_cast<lp_arena*>(buffer.get_jni_write_pointer());
        write_arena->leds[0].r = static_cast<uint8_t>(i); // Mettiamo un contatore
        buffer.commit_from_jni();
        std::this_thread::sleep_for(std::chrono::milliseconds(1)); // Più veloce del lettore
    }

    // Ferma tutto
    keep_running = false;
    reader.join();

    // Verifichiamo che il lettore abbia effettivamente letto dei frame
    EXPECT_GT(frames_read, 0);
}
