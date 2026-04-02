#include <RadioLib.h>
#include <hal/RPi/PiHal.h>
#include <fstream>
#include <vector>
#include <iostream>
#include <cstring>
#include <unistd.h>

// HAL
PiHal* hal = new PiHal(0);

// NSS, DIO1, RESET, BUSY
SX1262 lora = new Module(hal, 8, 25, 17, 24);

#define CHUNK_SIZE 180
#define HEADER 0xAA

std::vector<uint8_t> loadImage(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    return std::vector<uint8_t>((std::istreambuf_iterator<char>(file)),
                                std::istreambuf_iterator<char>());
}

int main() {
    std::cout << "Initializing SX1262...\n";

    int state = lora.begin(915.0);  // change to 868.0 if needed
    if (state != RADIOLIB_ERR_NONE) {
        std::cout << "Init failed: " << state << "\n";
        return -1;
    }

    // Optional tuning
    lora.setSpreadingFactor(9);
    lora.setBandwidth(125.0);
    lora.setCodingRate(5);
    lora.setOutputPower(17);

    auto image = loadImage("compressed.jpg");
    size_t totalSize = image.size();

    int totalPackets = (totalSize + CHUNK_SIZE - 1) / CHUNK_SIZE;

    std::cout << "Size: " << totalSize << " bytes\n";
    std::cout << "Packets: " << totalPackets << "\n";

    for (int i = 0; i < totalPackets; i++) {
        uint8_t packet[256];

        int offset = i * CHUNK_SIZE;
        int len = std::min((size_t)CHUNK_SIZE, totalSize - offset);

        packet[0] = HEADER;
        packet[1] = totalPackets;
        packet[2] = i;
        packet[3] = len;  // IMPORTANT: actual length

        memcpy(packet + 4, &image[offset], len);

        int state = lora.transmit(packet, len + 4);

        if (state == RADIOLIB_ERR_NONE) {
            std::cout << "Sent: " << i << "\n";
        } else {
            std::cout << "Error: " << state << "\n";
        }

        usleep(150000);  // SX1262 can go faster than SX127x
    }

    std::cout << "Done.\n";
    return 0;
}
