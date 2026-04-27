/**
 * @file crc32c.hpp
 * @brief CRC32-Castagnoli header using Hardware-accelerated &
 *        Parallel Computation CRC32C intrinsics from ARMv8 and ARMv9
 *
 * Provides fast CRC32C checksum computation through ARM ACLE intrinsics,
 * specifically optimized for processors supporting CRC32 instructions.
 *
 * @copyright
 * Copyright (c) 2026 Maulana M. Ali
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef CRC32C_HPP_
#define CRC32C_HPP_

#include <arm_acle.h>
#include <cstddef>
#include <cstdint>
#include <print>

namespace crc32c {

/**
 * @struct Frame
 * @brief Represents a dynamic array for CRC32 processing.
 */
struct Frame {
    const std::uint8_t* data;
    std::uint32_t length;
    std::uint32_t crc;
};

/**
 * @brief Compute the CRC32C checksum of a memory span.
 *
 * Uses ARM hardware-accelerated CRC32C byte intrinsics for efficient
 * byte-wise checksum computation.
 *
 * Initial CRC seed is `0xFFFFFFFF`, and the final result is XOR'd with
 * `0xFFFFFFFF` per CRC32C standard conventions.
 *
 * Debug information is printed before and after processing.
 *
 * @param frame The memory frame to process.
 *
 * @return The finalized CRC32C checksum.
 *
 * @note Requires ARMv8 or newer architecture with CRC32 instruction support.
 */
[[nodiscard]] inline std::uint32_t
encode(const Frame& frame, std::uint32_t previous_crc = 0)
{
    std::uint32_t crc = ~previous_crc;
    const auto *bytes = frame.data;

    for (std::size_t i = 0; i < frame.length; ++i) {
        crc = __crc32cb(crc, bytes[i]);
    }

    return ~crc;
}

}

#endif /* CRC32C_HPP_ */
