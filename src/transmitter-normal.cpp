/*
 * Copyright (c) 2026 Maulana M. Ali
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

extern "C" {
#include "chronos.h"
}

#include "Module.h"
#include "TypeDef.h"
#include "modules/SX126x/SX1262.h"

#include <RadioLib.h>
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <fstream>
#include <hal/RPi/PiHal.h>
#include <iterator>
#include <print>
#include <string>
#include <termios.h>
#include <unistd.h>
#include <vector>

constexpr auto CHUNK_SIZE = 140;
constexpr auto HEADER = 0xAA;

constexpr auto LORA_FREQ = 920.5;
constexpr auto LORA_SF = 8;
constexpr auto LORA_BW = 125.0;
constexpr auto LORA_CR = 5;
constexpr auto LORA_POWER = 19;

constexpr auto SLOT_MS = 500;
constexpr auto GNSS_PORT = "/dev/serial0";
constexpr auto BAUD = B9600;

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

static std::vector<uint8_t>
loadImage(const std::string &path)
{
    std::ifstream file(path, std::ios::binary);
    return std::vector<uint8_t>((std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>());
}

static int
open_gnss(void)
{
    int fd = open(GNSS_PORT, O_RDWR | O_NOCTTY | O_NONBLOCK);

    struct termios tty {};
    tcgetattr(fd, &tty);

    cfsetospeed(&tty, BAUD);
    cfsetispeed(&tty, BAUD);

    tty.c_cflag |= (CLOCAL | CREAD);
    tcsetattr(fd, TCSANOW, &tty);

    return fd;
}

// ===== PARSE GPRMC =====
static bool
parse_gprmc(const std::string &line, int &ms_of_day)
{
    if (line.find("$GPRMC") != 0) return false;

    // Example: $GPRMC,hhmmss.sss,...
    int hh, mm;
    float ss;

    if (sscanf(line.c_str(), "$GPRMC,%2d%2d%f", &hh, &mm, &ss) != 3)
        return false;

    int sec = (int)ss;
    int ms = (int)((ss - sec) * 1000);

    ms_of_day = ((hh * 3600 + mm * 60 + sec) * 1000) + ms;
    return true;
}

// ===== WAIT SLOT =====
static void
wait_slot(int fd)
{
    char buf[256];
    std::string line;

    while (true) {
        int n = read(fd, buf, sizeof(buf));

        for (int i = 0; i < n; i++) {
            if (buf[i] == '\n') {
                int ms;
                if (parse_gprmc(line, ms)) {
                    int slot = ms % SLOT_MS;

                    if (slot < 20) { // 20 ms guard window
                        return;
                    }
                }

                line.clear();
            } else {
                line += buf[i];
            }
        }
    }
}

}

int
main(void)
{
    int gnss = open_gnss();

    // chronos_convert_to_ppm("test.png", "test.jpg");
    std::println("[INFO] Initializing SX1262...");

    /* RadioLib LoRa Initialization */
    auto lora = get_lora();
    auto state = lora.begin(LORA_FREQ);

    if (state != RADIOLIB_ERR_NONE) {
        std::println("[ERROR] LoRa init failed: {}", state);
        return -1;
    }

    /* Optional tuning */
    lora.setSpreadingFactor(LORA_SF);
    lora.setBandwidth(LORA_BW);
    lora.setCodingRate(LORA_CR);
    lora.setOutputPower(LORA_POWER);

    /* Image loading */
    auto image = loadImage("test.ARW");
    auto totalSize = image.size();
    auto totalPackets = (totalSize + CHUNK_SIZE - 1) / CHUNK_SIZE;

    std::println("[INFO] Size: {} bytes", totalSize);
    std::println("[INFO] Packets: {}", totalPackets);

    for (size_t i = 0; i < totalPackets; i++) {
        uint8_t packet[256];

        auto offset = i * CHUNK_SIZE;
        auto len = std::min((size_t)CHUNK_SIZE, totalSize - offset);

        packet[0] = HEADER;
        packet[1] = (uint8_t)totalPackets;
        packet[2] = (uint8_t)i;
        packet[3] = (uint8_t)len; // IMPORTANT: actual length

        memcpy(packet + 4, &image[offset], len);

        // wait_slot(gnss);

        auto state = lora.transmit(packet, len + 4);

        if (state == RADIOLIB_ERR_NONE) {
            std::println("[INFO] Sent: {}", i);
        } else {
            std::println("[ERROR] Error code: {}", state);
        }

        // usleep(150000);
    }

    std::println("[INFO] Transmission done.");
    return 0;
}
