/**
 * @file LS7366R_Single.h
 * @brief LS7366R Quadrature Counter Library for Single Chip
 * 
 * This library provides an interface to the LS7366R 32-bit quadrature counter chip.
 * The LS7366R is designed to decode quadrature encoder signals (A/B/I) and maintain
 * a 32-bit count value.
 * 
 * Features:
 * - 32-bit counter
 * - Quadrature decoding (1x, 2x, 4x modes)
 * - SPI interface
 * - Free-running, single-cycle, range-limit, and modulo-n counting modes
 * - Index pulse handling
 * 
 * @author Based on work by Vladimir Kosmala (2014)
 * @date 2026
 */

#ifndef LS7366R_SINGLE_H
#define LS7366R_SINGLE_H

#include <Arduino.h>

// ============================================================================
// LS7366R Command Definitions
// ============================================================================

/** Command opcodes (bits 7-6) */
#define LS7366R_CMD_CLEAR    0x00  ///< Clear register
#define LS7366R_CMD_READ     0x40  ///< Read register
#define LS7366R_CMD_WRITE    0x80  ///< Write register
#define LS7366R_CMD_LOAD     0xC0  ///< Load register

/** Register addresses (bits 5-3) */
#define LS7366R_REG_MDR0    0x08  ///< Mode Register 0
#define LS7366R_REG_MDR1    0x10  ///< Mode Register 1
#define LS7366R_REG_DTR     0x18  ///< Data Transfer Register
#define LS7366R_REG_CNTR    0x20  ///< Counter Register
#define LS7366R_REG_OTR     0x28  ///< Output Transfer Register
#define LS7366R_REG_STR     0x30  ///< Status Register

// ============================================================================
// MDR0 Configuration Bits
// ============================================================================

/** Quadrature Mode (bits 1-0) */
#define LS7366R_MDR0_QUAD_NON_QUAD   0x00  ///< Non-quadrature (clock/direction)
#define LS7366R_MDR0_QUAD_X1         0x01  ///< 1x quadrature
#define LS7366R_MDR0_QUAD_X2         0x02  ///< 2x quadrature
#define LS7366R_MDR0_QUAD_X4         0x03  ///< 4x quadrature (recommended)

/** Counting Mode (bits 3-2) */
#define LS7366R_MDR0_CNT_FREE_RUN    0x00  ///< Free-running count
#define LS7366R_MDR0_CNT_SINGLE_CYCLE 0x04 ///< Single-cycle count
#define LS7366R_MDR0_CNT_RANGE_LIMIT 0x08  ///< Range-limit count
#define LS7366R_MDR0_CNT_MODULO_N    0x0C  ///< Modulo-n count

/** Index Mode (bits 5-4) */
#define LS7366R_MDR0_IDX_NO_INDEX    0x00  ///< No index
#define LS7366R_MDR0_IDX_LOAD_CNTR   0x10  ///< Load DTR into CNTR on index
#define LS7366R_MDR0_IDX_RESET_CNTR  0x20  ///< Reset CNTR on index
#define LS7366R_MDR0_IDX_LOAD_OTR    0x30  ///< Load CNTR into OTR on index

/** Index Synchronization (bit 6) */
#define LS7366R_MDR0_IDX_ASYNC       0x00  ///< Asynchronous index
#define LS7366R_MDR0_IDX_SYNC        0x40  ///< Synchronous index

/** Clock Filter Division Factor (bit 7) */
#define LS7366R_MDR0_FILTER_DIV1     0x00  ///< Clock division factor = 1
#define LS7366R_MDR0_FILTER_DIV2     0x80  ///< Clock division factor = 2

// ============================================================================
// MDR1 Configuration Bits
// ============================================================================

/** Counter Width (bits 1-0) */
#define LS7366R_MDR1_WIDTH_32BIT     0x00  ///< 32-bit counter
#define LS7366R_MDR1_WIDTH_24BIT     0x01  ///< 24-bit counter
#define LS7366R_MDR1_WIDTH_16BIT     0x02  ///< 16-bit counter
#define LS7366R_MDR1_WIDTH_8BIT      0x03  ///< 8-bit counter

/** Counter Enable (bit 2) */
#define LS7366R_MDR1_COUNT_ENABLE    0x00  ///< Enable counting
#define LS7366R_MDR1_COUNT_DISABLE   0x04  ///< Disable counting

/** Flag Configuration (bits 7-4) */
#define LS7366R_MDR1_FLAG_IDX        0x10  ///< Flag on IDX
#define LS7366R_MDR1_FLAG_CMP        0x20  ///< Flag on CMP (CNTR = DTR)
#define LS7366R_MDR1_FLAG_BW         0x40  ///< Flag on BW (underflow)
#define LS7366R_MDR1_FLAG_CY         0x80  ///< Flag on CY (overflow)

// ============================================================================
// Default Configuration
// ============================================================================

/** Default MDR0: 4x quadrature, free-running, no index, filter div 1 */
#define LS7366R_MDR0_DEFAULT  (LS7366R_MDR0_QUAD_X4 | LS7366R_MDR0_CNT_FREE_RUN | \
                               LS7366R_MDR0_IDX_NO_INDEX | LS7366R_MDR0_IDX_ASYNC | \
                               LS7366R_MDR0_FILTER_DIV1)

/** Default MDR1: 32-bit, enabled, no flags */
#define LS7366R_MDR1_DEFAULT  (LS7366R_MDR1_WIDTH_32BIT | LS7366R_MDR1_COUNT_ENABLE)

// ============================================================================
// Class Definition
// ============================================================================

/**
 * @class LS7366R_Single
 * @brief Interface for single LS7366R quadrature counter chip
 */
class LS7366R_Single {
public:
    /**
     * @brief Constructor
     * @param csPin Chip Select pin (SPI CS)
     * @param mdr0_config MDR0 register configuration (default: 4x quadrature)
     * @param mdr1_config MDR1 register configuration (default: 32-bit, enabled)
     */
    LS7366R_Single(uint8_t csPin, 
                   uint8_t mdr0_config = LS7366R_MDR0_DEFAULT,
                   uint8_t mdr1_config = LS7366R_MDR1_DEFAULT);
    
    /**
     * @brief Initialize the chip
     * @return true if initialization successful
     */
    bool begin();
    
    /**
     * @brief Reset counter to zero
     */
    void reset();
    
    /**
     * @brief Synchronize and read counter value from chip
     * Call this before getCount() to update the cached value
     */
    void sync();
    
    /**
     * @brief Get the current counter value
     * @return 32-bit signed counter value
     * @note Call sync() first to update the value
     */
    int32_t getCount() const { return countValue; }
    
    /**
     * @brief Reconfigure MDR0 and MDR1 registers
     * @param mdr0_config New MDR0 configuration
     * @param mdr1_config New MDR1 configuration
     */
    void reconfigure(uint8_t mdr0_config, uint8_t mdr1_config);
    
    /**
     * @brief Read status register
     * @return Status register value
     */
    uint8_t readStatus();
    
    /**
     * @brief Clear status register (clears phase errors and flags)
     */
    void clearStatus();
    
    /**
     * @brief Enable counting
     */
    void enable();
    
    /**
     * @brief Disable counting
     */
    void disable();
    
    /**
     * @brief Check if counting is enabled
     * @return true if enabled, false if disabled
     */
    bool isEnabled() const;

private:
    uint8_t csPin;           ///< Chip Select pin
    int32_t countValue;      ///< Cached counter value
    uint8_t mdr0Config;      ///< Current MDR0 configuration
    uint8_t mdr1Config;      ///< Current MDR1 configuration
    
    /**
     * @brief Write a register
     * @param reg Register address
     * @param value Value to write
     */
    void writeRegister(uint8_t reg, uint8_t value);
    
    /**
     * @brief Read a register
     * @param reg Register address
     * @return Register value
     */
    uint8_t readRegister(uint8_t reg);
    
    /**
     * @brief SPI transaction helper
     */
    void spiBegin();
    void spiEnd();
};

#endif // LS7366R_SINGLE_H
