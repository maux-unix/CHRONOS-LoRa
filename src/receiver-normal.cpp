/*
 * Copyright (c) 2026 Maulana M. Ali
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "Module.h"
#include "TypeDef.h"
#include "hal/RPi/PiHal.h"
#include "modules/SX126x/SX1262.h"

#include <RadioLib.h>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>
#include <print>

constexpr auto CHUNK_SIZE = 180;
constexpr auto MAX_PACKETS = 300;
constexpr auto HEADER = 0xAA;
constexpr auto LORA_FREQ = 920.5;
constexpr auto LORA_SF = 9;
constexpr auto LORA_BW = 125.0;
constexpr auto LORA_CR = 5;
// constexpr auto LORA_POWER = 19;

namespace {

static PiHal &
get_rpi_hal(void)
{
    static PiHal hal(0);
    return hal;
}

static Module &
get_lora_module(void)
{
    static Module lora(&get_rpi_hal(), 21, 16, 18, 20);
    return lora;
}

static SX1262 &
get_lora(void)
{
    static SX1262 radio(&get_lora_module());
    return radio;
}

static uint8_t imageBuffer[CHUNK_SIZE * MAX_PACKETS];
static bool received[MAX_PACKETS] = { false };
static int packetSizes[MAX_PACKETS] = { 0 };

}

int
main(void)
{
    std::println("[INFO] Receiver starting...");

    auto lora = get_lora();
    int state = lora.begin(LORA_FREQ);

    if (state != RADIOLIB_ERR_NONE) {
        std::println("[ERROR] RadioLib init failed: {}", state);
        return -1;
    }

    lora.setSpreadingFactor(LORA_SF);
    lora.setBandwidth(LORA_BW);
    lora.setCodingRate(LORA_CR);

    int totalPackets = -1;

    while (true) {
        uint8_t buffer[256];
        int state = lora.receive(buffer, sizeof(buffer));

        if (state == RADIOLIB_ERR_NONE) {
            if (buffer[0] != HEADER) continue;

            totalPackets = buffer[1];
            auto index = buffer[2];
            auto len = buffer[3];

            memcpy(&imageBuffer[static_cast<ptrdiff_t>(index * CHUNK_SIZE)],
                buffer + 4, len);

            received[index] = true;
            packetSizes[index] = len;

            std::println("[INFO] Got: {}", index);

            // Check completion
            bool complete = true;
            for (int i = 0; i < totalPackets; i++) {
                if (!received[i]) {
                    complete = false;
                    break;
                }
            }

            if (complete) {
                std::println("[INFO] Reconstructing image...");

                std::ofstream out("output.jpg", std::ios::binary);

                for (int i = 0; i < totalPackets; i++) {
                    out.write((char *)&imageBuffer[static_cast<ptrdiff_t>(
                                  i * CHUNK_SIZE)],
                        packetSizes[i]);
                }

                out.close();
                std::println("[INFO] Saved as output.jpg");
                break;
            }
        }
    }

    return 0;
}
