/*
 * Copyright (c) 2026 Maulana M. Ali
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "Module.h"
#include "TypeDef.h"
#include "modules/SX126x/SX1262.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <iterator>
#include <print>
#include <string>
#include <unistd.h>
#include <vector>

#include <RadioLib.h>
#include <hal/RPi/PiHal.h>

constexpr auto CHUNK_SIZE = 180;
constexpr auto HEADER = 0xAA;

namespace {

static PiHal &
get_rpi_hal()
{
    static PiHal hal(0);
    return hal;
}

static Module &
get_lora_module()
{
    static Module lora(&get_rpi_hal(), 8, 25, 17, 24);
    return lora;
}

static SX1262 &
get_lora()
{
    static SX1262 radio(&get_lora_module());
    return radio;
}

static std::vector<uint8_t>
loadImage(const std::string &path)
{
    std::ifstream file(path, std::ios::binary);
    return std::vector<uint8_t>((std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>());
}

}

int
main(void)
{
    std::println("[INFO] Initializing SX1262...");

    /* RadioLib LoRa Initialization */
    auto lora = get_lora();
    auto state = lora.begin(915.0); // change to 868.0 if needed

    if (state != RADIOLIB_ERR_NONE) {
        std::println("[ERROR] RadioLib init failed: {}", state);
        return -1;
    }

    /* Optional tuning */
    lora.setSpreadingFactor(9);
    lora.setBandwidth(125.0);
    lora.setCodingRate(5);
    lora.setOutputPower(17);

    /* Image loading */
    auto image = loadImage("compressed.jpg");
    auto totalSize = image.size();
    auto totalPackets = (totalSize + CHUNK_SIZE - 1) / CHUNK_SIZE;

    std::println("[INFO] Size: {} bytes", totalSize);
    std::println("[INFO] Packets: {}", totalPackets);

    for (size_t i = 0; i < totalPackets; i++) {
        uint8_t packet[256];

        auto offset = i * CHUNK_SIZE;
        auto len = std::min((size_t)CHUNK_SIZE, totalSize - offset);

        packet[0] = HEADER;
        packet[1] = (uint8_t) totalPackets;
        packet[2] = (uint8_t) i;
        packet[3] = (uint8_t) len; // IMPORTANT: actual length

        memcpy(packet + 4, &image[offset], len);

        auto state = lora.transmit(packet, len + 4);

        if (state == RADIOLIB_ERR_NONE) {
            std::println("[INFO] Sent: {}", i);
        } else {
            std::println("[ERROR] Error code: {}", state);
        }

        usleep(150000); // SX1262 can go faster than SX127x
    }

    std::println("[INFO] Transmission done.");
    return 0;
}
