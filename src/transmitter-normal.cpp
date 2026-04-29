/**
 * @file transmitter-normal.cpp
 * @brief CHRONOS-LoRa Transmitter program (Normal)
 *
 * Provides a normal Transmitter program for Image Transmission via LoRa
 * (without CHRONOS-LoRa)
 *
 * @copyright
 * Copyright (c) 2026 Maulana M. Ali
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "cxxopts.hpp"
#include "hal/RPi/PiHal.h"

#include <RadioLib.h>
#include <cstdio>
#include <filesystem>
#include <modules/SX126x/SX1262.h>
#include <print>
#include <protocols/PhysicalLayer/PhysicalLayer.h>

namespace {

static PiHal &
get_rpi_hal(void)
{
    static PiHal hal(1);
    return hal;
}

static Module &
get_lora_module(void)
{
    static Module lora(&get_rpi_hal(), 21, 16, 18, 20);
    return lora;
}

static SX1262 &
get_lora()
{
    static SX1262 radio(&get_lora_module());
    return radio;
}

} /* namespace */

namespace fs = std::filesystem;

int
main(int argc, char **argv)
{
    cxxopts::Options options("transmitter-normal", "CHRONOS-LoRa Transmitter");

    options.add_options()("h,help", "Print help information")("image",
        "Input image file", cxxopts::value<fs::path>())("freq",
        "Set LoRa frequency (range 920 – 923 MHz for Indonesia)",
        cxxopts::value<float>()->default_value("920.5"))("bw",
        "Set LoRa bandwith [7.8, 10.4, 15.6, 20.8, 31.25, 41.7, 62.5, 125.0 "
        "(max for station), 250.0 (max for end-to-end)]",
        cxxopts::value<float>()->default_value("125.0"));

    auto opt_result = options.parse(argc, argv);

    if (opt_result.count("help")) {
        std::println("{}", options.help());
        exit(0);
    }

    std::println("[RADIOLIB-INFO] SX1262 Initializing...");

    static auto &hal = get_rpi_hal();
    static auto &radio = get_lora();
    static auto state = radio.begin();

    radio.setFrequency(920.0);

    if (state != RADIOLIB_ERR_NONE) {
        std::println("[RADIOLIB-ERROR] Failed with code {}.", state);
        return (1);
    }

    std::println("[RADIOLIB-INFO] Success!");

    int count = 0;
    for (;;) {
        std::println("[SX1262-INFO] Transmitting packet ... ");

        char str[64];
        (void)sprintf(str, "Hello World! #%d", count++);

        state = radio.transmit(str);
        if (state == RADIOLIB_ERR_NONE) {
            std::println("[SX1262-INFO] Packet successfully transmitted!");
            hal.delay(1000);
        } else {
            std::println("[SX1262-ERROR] Failed with code {}", state);
        }
    }

    return (0);
}
