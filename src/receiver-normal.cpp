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

#include "cxxopts.hpp"
#include "fmt/base.h"
 
#include <TypeDef.h>
#include <cstdint>
#include <fmt/core.h>
#include <hal/RPi/PiHal.h>
#include <modules/SX126x/SX1262.h>
#include <modules/SX126x/SX126x_registers.h>
#include <set>
#include <thread>
#include <vector>
#include <fstream>

namespace {

static auto &
get_rpi_hal(void)
{
    static PiHal hal(0);
    return hal;
}

static auto &
get_lora_module(void)
{
    static Module lora(&get_rpi_hal(), 21, 16, 18, 20);
    return lora;
}

static auto &
get_lora(void)
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

// Packet types
const uint8_t PKT_START = 0x01;
const uint8_t PKT_DATA  = 0x02;
const uint8_t PKT_END   = 0x03;
const uint8_t PKT_ACK   = 0x04;

struct ImageSession {
    uint16_t imageId = 0;
    uint16_t totalPackets = 0;
    uint32_t fileSize = 0;
    std::vector<uint8_t> imageBuffer;
    std::set<uint16_t> receivedPackets;
    bool active = false;
};

void sleep_ms(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

bool sendAck(uint16_t packetNum, SX1262& radio) {
    uint8_t ack[3];
    ack[0] = PKT_ACK;
    ack[1] = (packetNum >> 8) & 0xFF;
    ack[2] = packetNum & 0xFF;

    int state = radio.transmit(ack, sizeof(ack));

    if (state == RADIOLIB_ERR_NONE) {
        fmt::println("[INFO] ACK sent for packet {}", packetNum);
        return true;
    }

    fmt::println("[ERR] ACK failed for packet {}", packetNum);
    return false;
}

bool saveImage(const ImageSession& session) {
    std::ofstream outFile("received_image.jpg", std::ios::binary);

    if (!outFile.is_open()) {
        fmt::println("[ERR] Cannot save image.");
        return false;
    }

    outFile.write((char*)session.imageBuffer.data(), session.fileSize);
    outFile.close();

    fmt::println("[INFO] Image saved as received_image.jpg");
    return true;
}

} /* namespace */

int
main(int argc, char **argv)
{
    cxxopts::Options options("receiver-normal", "CHRONOS-LoRa Transmitter");

    options.add_options()("h,help", "Print help information")("freq",
        "Set LoRa frequency (range 920 – 923 MHz for Indonesia)",
        cxxopts::value<float>()->default_value("920.5"))("bw",
        "Set LoRa bandwith [7.8, 10.4, 15.6, 20.8, 31.25, 41.7, 62.5, 125.0 "
        "(max for station), 250.0 (max for end-to-end)]",
        cxxopts::value<float>()->default_value("125.0"))("sf",
        "Set LoRa spreading factor",
        cxxopts::value<uint8_t>()->default_value("7"))("cr",
        "Set LoRa coding rate", cxxopts::value<uint8_t>()->default_value("5"));

    auto opt_result = options.parse(argc, argv);

    if (opt_result.count("help")) {
        fmt::print("{}", options.help());
        exit(0);
    }

    auto radio = get_lora();
    auto state = radio.begin(AS923_2_CHANNEL_FREQUENCY_KHZ, AS923_2_BANDWIDTH,
        AS923_2_SPREADING_FACTOR, AS923_2_CODING_RATE,
        RADIOLIB_SX126X_SYNC_WORD_PUBLIC, AS923_2_EIRP_DBM,
        AS923_2_PREAMBLE_LENGTH, (float)1.6, true);

    if (state != RADIOLIB_ERR_NONE) {
        fmt::println("[ERR] Radio init failed with code {}.", state);
        return (-1);
    }

    fmt::println("[INFO] LoRa receiver initialized.");

    ImageSession session;

    while (true) {
        uint8_t buffer[256];
        size_t len = sizeof buffer;

        int received_state = radio.receive(buffer, len);

        if (received_state != RADIOLIB_ERR_NONE) {
            continue;
        }

        if (len < 9) {
            fmt::println("[ERR] Packet too short.");
            continue;
        }

        uint8_t type = buffer[0];
        uint16_t imageId = (buffer[1] << 8) | buffer[2];
        uint16_t packetNum = (buffer[3] << 8) | buffer[4];
        uint16_t totalPackets = (buffer[5] << 8) | buffer[6];
        uint16_t payloadSize = (buffer[7] << 8) | buffer[8];

        if (payloadSize + 9 > len) {
            fmt::println("[ERR] Invalid payload size.");
            continue;
        }

        switch (type) {
            case PKT_START: {
                if (payloadSize != 4) {
                    fmt::println("[ERR] Invalid START packet");
                    break;
                }

                session = ImageSession();
                session.imageId = imageId;
                session.totalPackets = totalPackets;
                session.fileSize =
                    (buffer[9] << 24) |
                    (buffer[10] << 16) |
                    (buffer[11] << 8) |
                    buffer[12];

                session.imageBuffer.resize(session.fileSize);
                session.active = true;

                fmt::println("[INFO] START received.");
                fmt::println("[INFO] Image ID: {}.", imageId);
                fmt::println("[INFO] File size: {} bytes", session.fileSize);
                fmt::println("[INFO] Total packets: {}", totalPackets);

                sendAck(packetNum, radio);
                break;
            }
            case PKT_DATA: {
                if (!session.active || imageId != session.imageId) {
                    fmt::println("[ERR] No active session.");
                    break;
                }

                if (session.receivedPackets.count(packetNum)) {
                    fmt::println("[WARN] Duplicate packet {}", packetNum);
                    sendAck(packetNum, radio);
                    break;
                }

                size_t offset = packetNum * 200;

                if (offset + payloadSize > session.imageBuffer.size()) {
                    fmt::println("[ERR] PAcket exceeds image size.");
                    break;
                }

                memcpy(
                    &session.imageBuffer[offset],
                    &buffer[9],
                    payloadSize
                );

                session.receivedPackets.insert(packetNum);

                fmt::println("[INFO] Received packet {} / {}", packetNum + 1,
                totalPackets);
                
                sendAck(packetNum, radio);
                break;
            }
            case PKT_END: {
                if (!session.active) {
                    fmt::println("[ERR] END with no active session.");
                    break;
                }

                fmt::println("[INFO] END packet received.");

                sendAck(packetNum, radio);

                if (session.receivedPackets.size() == session.totalPackets) {
                    fmt::println("[INFO] All packets recived successfully.");
                    saveImage(session);
                } else {
                    fmt::println("[ERR] Missing packets detected.");
                    fmt::println("[INFO] Received {} / {}",
                    session.receivedPackets.size(), session.totalPackets);                    
                }

                session.active = false;
                break;
            }

            default:
                fmt::println("[ERR] Unknown packet type.");
                break;
        }

        sleep_ms(20);
    }

    return 0;
}

// int
// main(int argc, char **argv)
// {
//     (void)argc;
//     (void)argv;

//     fmt::print("[RADIOLIB-INFO] SX1262 Initializing...");

//     // auto &hal = get_rpi_hal();
//     auto &radio = get_lora();
//     auto state = radio.begin();

//     if (state != RADIOLIB_ERR_NONE) {
//         fmt::print("[RADIOLIB-ERROR] Failed with code: {}", state);
//         return (1);
//     }

//     radio.setFrequency(920.0);
//     radio.setPacketReceivedAction(set_flag);

//     fmt::print("[SX1262-INFO] Starting to listen...");
//     state = radio.startReceive();

//     if (state != RADIOLIB_ERR_NONE) {
//         fmt::print("[RADIOLIB-ERROR] Failed with code: {}", state);
//         return (1);
//     }

//     for (;;) {
//         if (received_flag) {
//             received_flag = false;

//             std::uint8_t byte_arr[1024] {};
//             // auto len = radio.getPacketLength();
//             // fmt::print("{}", len);
//             // auto bla = radio.getLoRaRxHeaderInfo(, true);
//             auto state_read = radio.readData(byte_arr, sizeof(byte_arr));
//             // auto state_read = radio.receive(byte_arr, sizeof(byte_arr));

//             if (state_read == RADIOLIB_ERR_NONE) {
//                 fmt::print("[SX1262-INFO] Received packet!");

//                 for (size_t i = 0; i < sizeof(byte_arr); ++i) {
//                     fmt::print("[SX1262-INFO] Data: {:c}", byte_arr[i]);
//                 }

//                 fmt::print("[SX1262-INFO] RSSI: {}dBm", radio.getRSSI());
//                 fmt::print("[SX1262-INFO] SNR: {}dB", radio.getSNR());
//                 fmt::print("[SX1262-INFO] Frequency error: {}Hz",
//                     radio.getFrequencyError());
//             } else if (state == RADIOLIB_ERR_CRC_MISMATCH) {
//                 fmt::print("[SX1262-ERROR] CRC error!");
//             } else {
//                 fmt::print("[RADIOLIB-ERROR] Failed with code: {}", state);
//             }
//         }
//     }

//     return 0;
// }
