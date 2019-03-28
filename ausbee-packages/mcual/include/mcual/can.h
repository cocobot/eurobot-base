#ifndef MCUAL_CAN_H
#define MCUAL_CAN_H

#include <canard.h>
#include <string.h>

#define MCUAL_CAN_NUM_ACCEPTANCE_FILTERS                            14U
#define MCUAL_CAN_ERROR_UNSUPPORTED_BIT_RATE                        1000
#define MCUAL_CAN_ERROR_MSR_INAK_NOT_SET                            1001
#define MCUAL_CAN_ERROR_MSR_INAK_NOT_CLEARED                        1002
#define MCUAL_CAN_ERROR_UNSUPPORTED_FRAME_FORMAT                    1003


/**
 * These parameters define the timings of the CAN controller.
 * Please refer to the documentation of the bxCAN macrocell for explanation.
 * These values can be computed by the developed beforehand if ROM size is of a concern,
 * or they can be computed at run time using the function defined below.
 */
typedef struct
{
    uint16_t bit_rate_prescaler;                        /// [1, 1024]
    uint8_t bit_segment_1;                              /// [1, 16]
    uint8_t bit_segment_2;                              /// [1, 8]
    uint8_t max_resynchronization_jump_width;           /// [1, 4] (recommended value is 1)
} mcual_can_timings;

/**
 * The interface can be initialized in either of these modes.
 *
 * The Silent mode is useful for automatic CAN bit rate detection, where the interface is initialized at an
 * arbitrarily guessed CAN bit rate (typically either 1 Mbps, 500 Kbps, 250 Kbps, or 125 Kbps, these are the
 * standard values defined by the UAVCAN specification), and the bus is then listened for 1 second in order to
 * determine whether the bit rate was guessed correctly. It is paramount to use the silent mode in this case so
 * as to not interfere with ongoing communications on the bus if the guess was incorrect.
 *
 * The automatic TX abort on error mode should be used during dynamic node ID allocation. The reason for that
 * is well explained in the UAVCAN specification, please read it.
 *
 * The normal mode should be used for all other use cases, particularly for the normal operation of the node,
 * hence the name.
 */
typedef enum
{
    mcualCanIfaceModeNormal,                         //!< Normal mode
    mcualCanIfaceModeSilent,                         //!< Do not affect the bus, only listen
    mcualCanIfaceModeAutomaticTxAbortOnError         //!< Abort pending TX if a bus error has occurred
} mcual_can_ifaceMode;


/**
 * @brief Initialize can1 (including clock and interrupt handling)
 *
 */
int16_t mcual_can_init(mcual_can_timings * const timings, mcual_can_ifaceMode iface);

/**
 * @brief Set a new frame to be send in the tx buffer
 *
 * @param[in] frame The frame to be sent
 *
 * @retval      1               Transmitted successfully
 * @retval      0               No space in the buffer
 * @retval      negative        Error
 */
int16_t mcual_can_transmit(const CanardCANFrame* const frame);

/**
 * @brief Blocking function that returns when the tx queue is empty
 */
void mcual_can_wait_tx_ended();

/**
 * @brief Receive a new frame from rx queue.
 *
 * @details This a blocking function. 
 * If no frame is available, it blocks until a frame arrives.
 *
 * @param[out] out_frame Pointer on the received frame
 *
 * @retval      1               Transmitted successfully
 */
int16_t mcual_can_recv(CanardCANFrame* const out_frame);

/**
 * @brief Receive a new frame from rx queue.
 *
 * @details This is the non-blocking function to receive a frame
 *
 * @param[out] out_frame Pointer on the received frame
 *
 * @retval      1               A new frame is recieved, pointed by out_frame
 * @retval      0               No frame is received
 * @retval      negative        Error
 */
int16_t mcual_can_recv_no_wait(CanardCANFrame* const out_frame);

static inline int16_t mcual_can_compute_timings(const uint32_t peripheral_clock_rate,
                                     const uint32_t target_bitrate,
                                     mcual_can_timings * const out_timings)
{
    if (target_bitrate < 1000)
    {
        return -MCUAL_CAN_ERROR_UNSUPPORTED_BIT_RATE;
    }

    CANARD_ASSERT(out_timings != NULL);  // NOLINT
    memset(out_timings, 0, sizeof(*out_timings));

    /*
     * Hardware configuration
     */
    static const uint8_t MaxBS1 = 16;
    static const uint8_t MaxBS2 = 8;

    /*
     * Ref. "Automatic Baudrate Detection in CANopen Networks", U. Koppe, MicroControl GmbH & Co. KG
     *      CAN in Automation, 2003
     *
     * According to the source, optimal quanta per bit are:
     *   Bitrate        Optimal Maximum
     *   1000 kbps      8       10
     *   500  kbps      16      17
     *   250  kbps      16      17
     *   125  kbps      16      17
     */
    const uint8_t max_quanta_per_bit = (uint8_t)((target_bitrate >= 1000000) ? 10 : 17);    // NOLINT
    CANARD_ASSERT(max_quanta_per_bit <= (MaxBS1 + MaxBS2));

    static const uint16_t MaxSamplePointLocationPermill = 900;

    /*
     * Computing (prescaler * BS):
     *   BITRATE = 1 / (PRESCALER * (1 / PCLK) * (1 + BS1 + BS2))       -- See the Reference Manual
     *   BITRATE = PCLK / (PRESCALER * (1 + BS1 + BS2))                 -- Simplified
     * let:
     *   BS = 1 + BS1 + BS2                                             -- Number of time quanta per bit
     *   PRESCALER_BS = PRESCALER * BS
     * ==>
     *   PRESCALER_BS = PCLK / BITRATE
     */
    const uint32_t prescaler_bs = peripheral_clock_rate / target_bitrate;

    /*
     * Searching for such prescaler value so that the number of quanta per bit is highest.
     */
    uint8_t bs1_bs2_sum = (uint8_t)(max_quanta_per_bit - 1);    // NOLINT

    while ((prescaler_bs % (1U + bs1_bs2_sum)) != 0)
    {
        if (bs1_bs2_sum <= 2)
        {
            return -MCUAL_CAN_ERROR_UNSUPPORTED_BIT_RATE ;          // No solution
        }
        bs1_bs2_sum--;
    }

    const uint32_t prescaler = prescaler_bs / (1U + bs1_bs2_sum);
    if ((prescaler < 1U) || (prescaler > 1024U))
    {
        return -MCUAL_CAN_ERROR_UNSUPPORTED_BIT_RATE;              // No solution
    }

    /*
     * Now we have a constraint: (BS1 + BS2) == bs1_bs2_sum.
     * We need to find such values so that the sample point is as close as possible to the optimal value,
     * which is 87.5%, which is 7/8.
     *
     *   Solve[(1 + bs1)/(1 + bs1 + bs2) == 7/8, bs2]  (* Where 7/8 is 0.875, the recommended sample point location *)
     *   {{bs2 -> (1 + bs1)/7}}
     *
     * Hence:
     *   bs2 = (1 + bs1) / 7
     *   bs1 = (7 * bs1_bs2_sum - 1) / 8
     *
     * Sample point location can be computed as follows:
     *   Sample point location = (1 + bs1) / (1 + bs1 + bs2)
     *
     * Since the optimal solution is so close to the maximum, we prepare two solutions, and then pick the best one:
     *   - With rounding to nearest
     *   - With rounding to zero
     */
    uint8_t bs1 = (uint8_t)(((7 * bs1_bs2_sum - 1) + 4) / 8);       // Trying rounding to nearest first  // NOLINT
    uint8_t bs2 = (uint8_t)(bs1_bs2_sum - bs1);  // NOLINT
    CANARD_ASSERT(bs1_bs2_sum > bs1);

    {
        const uint16_t sample_point_permill = (uint16_t)(1000U * (1U + bs1) / (1U + bs1 + bs2));  // NOLINT

        if (sample_point_permill > MaxSamplePointLocationPermill)   // Strictly more!
        {
            bs1 = (uint8_t)((7 * bs1_bs2_sum - 1) / 8);             // Nope, too far; now rounding to zero
            bs2 = (uint8_t)(bs1_bs2_sum - bs1);
        }
    }

    const bool valid = (bs1 >= 1) && (bs1 <= MaxBS1) && (bs2 >= 1) && (bs2 <= MaxBS2);

    /*
     * Final validation
     * Helpful Python:
     * def sample_point_from_btr(x):
     *     assert 0b0011110010000000111111000000000 & x == 0
     *     ts2,ts1,brp = (x>>20)&7, (x>>16)&15, x&511
     *     return (1+ts1+1)/(1+ts1+1+ts2+1)
     */
    if ((target_bitrate != (peripheral_clock_rate / (prescaler * (1U + bs1 + bs2)))) ||
        !valid)
    {
        // This actually means that the algorithm has a logic error, hence assert(0).
        CANARD_ASSERT(0);  // NOLINT
        return -MCUAL_CAN_ERROR_UNSUPPORTED_BIT_RATE;
    }

    out_timings->bit_rate_prescaler = (uint16_t) prescaler;
    out_timings->max_resynchronization_jump_width = 1;      // One is recommended by UAVCAN, CANOpen, and DeviceNet
    out_timings->bit_segment_1 = bs1;
    out_timings->bit_segment_2 = bs2;

    return 0;
}

#ifdef AUSBEE_SIM
void mcual_can_recv_new_frame(CanardCANFrame * frame);
#endif //AUSBEE_SIM


#endif
