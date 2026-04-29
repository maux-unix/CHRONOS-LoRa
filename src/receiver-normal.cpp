/**
 * @file receiver-normal.cpp
 * @brief CHRONOS-LoRa Receiver program (Normal)
 *
 * Provides a normal Receiver program for Image Transmission via LoRa
 * (without CHRONOS-LoRa)
 *
 * @copyright
 * Copyright (c) 2026 Maulana M. Ali
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <TypeDef.h>
#include <cstddef>
#include <cstdint>
#include <hal/RPi/PiHal.h>
#include <print>

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

volatile bool received_flag = false;

void
set_flag(void)
{
    received_flag = true;
}

} /* namespace */

int
main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    std::println("[RADIOLIB-INFO] SX1262 Initializing...");

    // auto &hal = get_rpi_hal();
    auto &radio = get_lora();
    auto state = radio.begin();

    if (state != RADIOLIB_ERR_NONE) {
        std::println("[RADIOLIB-ERROR] Failed with code: {}", state);
        return (1);
    }

    radio.setFrequency(920.0);
    radio.setPacketReceivedAction(set_flag);

    std::println("[SX1262-INFO] Starting to listen...");
    state = radio.startReceive();

    if (state != RADIOLIB_ERR_NONE) {
        std::println("[RADIOLIB-ERROR] Failed with code: {}", state);
        return (1);
    }

    for (;;) {
        if (received_flag) {
            received_flag = false;

            std::uint8_t byte_arr[1024] {};
            // auto len = radio.getPacketLength();
            // std::println("{}", len);
            // auto bla = radio.getLoRaRxHeaderInfo(, true);
            auto state_read = radio.readData(byte_arr, sizeof(byte_arr));
            // auto state_read = radio.receive(byte_arr, sizeof(byte_arr));
            
            if (state_read == RADIOLIB_ERR_NONE) {
                std::println("[SX1262-INFO] Received packet!");

                for (size_t i = 0; i < sizeof(byte_arr); ++i) {
                    std::println("[SX1262-INFO] Data: {:c}", byte_arr[i]);
                }

                std::println("[SX1262-INFO] RSSI: {}dBm", radio.getRSSI());
                std::println("[SX1262-INFO] SNR: {}dB", radio.getSNR());
                std::println("[SX1262-INFO] Frequency error: {}Hz",
                    radio.getFrequencyError());
            } else if (state == RADIOLIB_ERR_CRC_MISMATCH) {
                std::println("[SX1262-ERROR] CRC error!");
            } else {
                std::println("[RADIOLIB-ERROR] Failed with code: {}", state);
            }
        }
    }

    return 0;
}
