/**
 * @file chronos_types.hpp
 * @brief Collection of CHRONOS-LoRa Types
 *
 * Provides specific types needed for CHRONOS-LoRa
 *
 * @copyright
 * Copyright (c) 2026 Maulana M. Ali
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#if !defined(CHRONOS_TYPES_HPP_)
#define CHRONOS_TYPES_HPP_

#include <cstdint>

namespace ChronosTypes {
struct CrcFrame {
    const std::uint8_t *data;
    std::uint32_t length;
    std::uint32_t crc;
};
struct ReedSolomonFrame {
    CrcFrame frame;
};
struct LoraPacket { };
}

#endif /* CHRONOS_TYPES_HPP_ */
