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
#include "fmt/base.h"

#include <cstdint>
#include <cstdlib>
#include <fmt/core.h>
#include <fstream>
#include <hal/RPi/PiHal.h>
#include <ios>
#include <string>
#include <thread>
#include <vector>

namespace {

static auto &
get_rpi_hal(void)
{
    static PiHal hal(1);
    return hal;
}

static auto &
get_lora_module(void)
{
    static Module lora(&get_rpi_hal(), 21, 16, 18, 20);
    return lora;
}

static auto &
get_lora()
{
    static SX1262 radio(&get_lora_module());
    return radio;
}

/*
 * Default settings for Indonesia
 *
 * References:
 * (1) LoRa Alliance (2022) RP002-1.0.4 Regional Parameters
 * (2) Peraturan Menteri Komunikasi dan Informatika Nomor 2 Tahun 2023 tentang
 *     Penggunaan Spektrum Frekuensi Radio Berdasarkan Izin Kelas.
 */
constexpr int32_t AS923_FREQ_HZ = 923200000;
constexpr int32_t AS923_2_FREQ_OFFSET = static_cast<int32_t>(0xFFFFB9B0);
constexpr int32_t AS923_2_FREQ_OFFSET_HZ = 100 * AS923_2_FREQ_OFFSET;
constexpr float AS923_2_CHANNEL_FREQUENCY_KHZ = static_cast<float>(
    (static_cast<float>(AS923_FREQ_HZ) +
        static_cast<float>(AS923_2_FREQ_OFFSET_HZ)) /
    1000000.0);
constexpr uint8_t AS923_2_EIRP_DBM = 16;
constexpr float AS923_2_BANDWIDTH = 125.0;
constexpr uint8_t AS923_2_CODING_RATE = 5;
constexpr uint8_t AS923_2_SPREADING_FACTOR = 7;
constexpr uint16_t AS923_2_PREAMBLE_LENGTH = 8;

/* Packet Settings */
constexpr size_t CHUNK_SIZE = 200;
constexpr int MAX_RETRIES = 5;
constexpr int ACK_TIMEOUT_MS = 3000;

// Packet types
constexpr uint8_t PKT_START = 0x01;
constexpr uint8_t PKT_DATA = 0x02;
constexpr uint8_t PKT_END = 0x03;
constexpr uint8_t PKT_ACK = 0x04;

template <typename T, typename Options, typename Setter>
void
apply_radio_option(const Options &options, const char *key, const char *label,
    Setter setter)
{
    if (options.count(key)) {
        T value = options[key].template as<T>();
        int16_t state = setter(value);

        if (state != RADIOLIB_ERR_NONE) {
            fmt::print("[RADIOLIB-ERR] Failed with code: {}", state);
            exit(1);
        }

        fmt::print("[LORA-SET] {}: {}", label, value);
    }
}

static void
sleep_ms(int ms)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

static bool
waitForAck(uint16_t expectedPacket, SX1262 &radio)
{
    uint8_t buffer[32];
    size_t len = sizeof(buffer);

    auto start = std::chrono::steady_clock::now();

    while (true) {
        int state = radio.receive(buffer, len);

        if (state == RADIOLIB_ERR_NONE) {
            if (buffer[0] == PKT_ACK) {
                uint16_t ackNum = (buffer[1] << 8) | buffer[2];
                if (ackNum == expectedPacket) {
                    fmt::println("[INFO] ACK received for packet {}",
                        expectedPacket);
                    return true;
                }
            }
        }

        auto now = std::chrono::steady_clock::now();
        int elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            now - start)
                          .count();

        if (elapsed > ACK_TIMEOUT_MS) {
            return false;
        }
    }
}

static std::vector<uint8_t>
build_packet(uint8_t type, uint16_t image_id, uint16_t packet_num,
    uint16_t total_packets, const uint8_t *payload, uint16_t payload_size)
{
    std::vector<uint8_t> packet;

    packet.push_back(type);
    packet.push_back(image_id >> 8);
    packet.push_back(image_id & 0xFF);
    packet.push_back(packet_num >> 8);
    packet.push_back(packet_num & 0xFF);
    packet.push_back(total_packets >> 8);
    packet.push_back(total_packets & 0xFF);
    packet.push_back(payload_size >> 8);
    packet.push_back(payload_size & 0xFF);

    for (int i = 0; i < payload_size; i++) {
        packet.push_back(payload[i]);
    }

    return packet;
}

static bool
sendPacket(const std::vector<uint8_t> &packet, uint16_t packet_num,
    SX1262 &radio)
{
    for (int attempt = 1; attempt <= MAX_RETRIES; attempt++) {
        int state = radio.transmit(packet.data(), packet.size());

        if (state == RADIOLIB_ERR_NONE) {
            fmt::println("Sent packet {} (attempt {})", packet_num, attempt);

            if (waitForAck(packet_num, radio)) {
                return true;
            }
        }

        fmt::println("[INFO] Retry packet {}", packet_num);
        sleep_ms(500);
    }

    fmt::println("[ERR] Failed packet {}", packet_num);
    return false;
}

} /* namespace */

int
main(int argc, char **argv)
{
    /* Making command-line options */
    cxxopts::Options options("transmitter-normal", "CHRONOS-LoRa Transmitter");

    options.add_options()("h,help", "Print help information")("image",
        "Input image file", cxxopts::value<std::string>())("freq",
        "Set LoRa frequency (range 920 – 923 MHz for Indonesia)",
        cxxopts::value<float>()->default_value("920.5"))("bw",
        "Set LoRa bandwith [7.8, 10.4, 15.6, 20.8, 31.25, 41.7, 62.5, 125.0 "
        "(max for station), 250.0 (max for end-to-end)]",
        cxxopts::value<float>()->default_value("125.0"))("sf",
        "Set LoRa spreading factor",
        cxxopts::value<uint8_t>()->default_value("7"))("cr",
        "Set LoRa coding rate", cxxopts::value<uint8_t>()->default_value("5"));

    /* Parse the options */
    auto opt_result = options.parse(argc, argv);

    if (opt_result.count("help")) {
        fmt::print("{}", options.help());
        exit(0);
    }

    auto value = opt_result["image"].as<std::string>();
    uint16_t image_id = 1;
    std::ifstream file(value, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        fmt::print("[IMAGE] Cannot open image file.");
        return (-1);
    }

    std::streamsize file_size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<uint8_t> image_data((size_t)file_size);
    if (!file.read((char *)image_data.data(), file_size)) {
        fmt::println("[ERR] Failed reading image.");
        return (-1);
    }

    file.close();

    uint16_t total_packets = (uint16_t)((file_size + CHUNK_SIZE - 1) /
        CHUNK_SIZE);

    fmt::println("[INFO] Image size: {} bytes", file_size);
    fmt::println("[INFO] Total packets: {}", total_packets);

    /* Initializing LoRa */
    fmt::println("[RADIOLIB-INFO] SX1262 Initializing...");
    auto &hal = get_rpi_hal();
    auto &radio = get_lora();
    radio.setModem(RADIOLIB_MODEM_LORA);

    /* Configure LoRa (default for Indonesia) */
    auto state = radio.begin(AS923_2_CHANNEL_FREQUENCY_KHZ, AS923_2_BANDWIDTH,
        AS923_2_SPREADING_FACTOR, AS923_2_CODING_RATE,
        RADIOLIB_SX126X_SYNC_WORD_PUBLIC, AS923_2_EIRP_DBM,
        AS923_2_PREAMBLE_LENGTH, (float)1.6, true);

    if (state != RADIOLIB_ERR_NONE) {
        fmt::println("[RADIOLIB-ERROR] Failed with code {}.", state);
        exit(1);
    } else {
        fmt::println("[DEFAULT] Frequency: {}", AS923_2_CHANNEL_FREQUENCY_KHZ);
        fmt::println("[DEFAULT] Bandwidth: {}", AS923_2_BANDWIDTH);
        fmt::println("[DEFAULT] Spreading Factor: {}",
            AS923_2_SPREADING_FACTOR);
        fmt::println("[DEFAULT] Coding Rate: {}", AS923_2_CODING_RATE);
    }

    /* Configure LoRa from CLI options */
    apply_radio_option<float>(opt_result, "freq", "Frequency",
        [&](float v) -> int16_t { return radio.setFrequency(v); });
    apply_radio_option<float>(opt_result, "bw", "Bandwidth",
        [&](float v) -> int16_t { return radio.setBandwidth(v); });
    apply_radio_option<uint8_t>(opt_result, "sf", "Spreading Factor",
        [&](uint8_t v) -> int16_t { return radio.setSpreadingFactor(v); });
    apply_radio_option<uint8_t>(opt_result, "cr", "Coding Rate",
        [&](uint8_t v) -> int16_t { return radio.setCodingRate(v); });

    fmt::println("[RADIOLIB-INFO] Success!");

    uint8_t start_payload[4];
    start_payload[0] = (file_size >> 24) & 0xFF;
    start_payload[1] = (file_size >> 16) & 0xFF;
    start_payload[2] = (file_size >> 8) & 0xFF;
    start_payload[3] = file_size & 0xFF;

    auto start_packet = build_packet(PKT_START, image_id, 0, total_packets,
        start_payload, 4);

    if (!sendPacket(start_packet, 0, radio)) {
        fmt::println("[ERR] Failed to send START packet");
        return (-1);
    }

    // DATA packets
    for (uint16_t pkt = 0; pkt < total_packets; pkt++) {
        size_t offset = pkt * CHUNK_SIZE;
        size_t remaining = file_size - offset;
        uint16_t payloadSize = std::min((size_t)CHUNK_SIZE, remaining);

        auto packet = build_packet(PKT_DATA, image_id, pkt, total_packets,
            &image_data[offset], payloadSize);

        if (!sendPacket(packet, pkt, radio)) {
            fmt::println("[ERR] Transmission aborted");
            return (-1);
        }

        sleep_ms(100);
    }

    // END packet
    auto endPacket = build_packet(PKT_END, image_id, total_packets,
        total_packets, nullptr, 0);

    if (!sendPacket(endPacket, total_packets, radio)) {
        fmt::println("[ERR] Failed END packet.");
        return (-1);
    }

    fmt::println("[INFO] Image transmission complete.");

    return 0;
}
