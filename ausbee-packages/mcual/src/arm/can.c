#include <include/generated/autoconf.h>
#ifdef CONFIG_MCUAL_CAN

#include <stdlib.h>
#include <stm32f4xx.h>
#include <mcual.h>
#include <unistd.h>

#ifdef CONFIG_MCUAL_CAN_USE_FREERTOS_QUEUES
#include <FreeRTOS.h>
#include <queue.h>
#endif


#ifdef CONFIG_MCUAL_CAN_USE_FREERTOS_QUEUES
QueueHandle_t tx_queue;
QueueHandle_t rx_queue;
#else
volatile CanardCANFrame can_tx_buffer[CONFIG_MCUAL_CAN_TX_SIZE];
volatile int32_t tx_index_read;
volatile int32_t tx_index_write;
volatile CanardCANFrame can_rx_buffer[CONFIG_MCUAL_CAN_RX_SIZE];
volatile int32_t rx_index_read;
volatile int32_t rx_index_write;
#endif

static bool isFramePriorityHigher(uint32_t a, uint32_t b)
{
    const uint32_t clean_a = a & CANARD_CAN_EXT_ID_MASK;
    const uint32_t clean_b = b & CANARD_CAN_EXT_ID_MASK;

    /*
     * STD vs EXT - if 11 most significant bits are the same, EXT loses.
     */
    const bool ext_a = (a & CANARD_CAN_FRAME_EFF) != 0;
    const bool ext_b = (b & CANARD_CAN_FRAME_EFF) != 0;
    if (ext_a != ext_b)
    {
        const uint32_t arb11_a = ext_a ? (clean_a >> 18U) : clean_a;
        const uint32_t arb11_b = ext_b ? (clean_b >> 18U) : clean_b;
        if (arb11_a != arb11_b)
        {
            return arb11_a < arb11_b;
        }
        else
        {
            return ext_b;
        }
    }

    /*
     * RTR vs Data frame - if frame identifiers and frame types are the same, RTR loses.
     */
    const bool rtr_a = (a & CANARD_CAN_FRAME_RTR) != 0;
    const bool rtr_b = (b & CANARD_CAN_FRAME_RTR) != 0;
    if ((clean_a == clean_b) && (rtr_a != rtr_b))
    {
        return rtr_b;
    }

    /*
     * Plain ID arbitration - greater value loses.
     */
    return clean_a < clean_b;
}

static bool waitMSRINAKBitStateChange(volatile const CAN_TypeDef * const bxcan, const bool target_state)
{
    /**
     * A properly functioning bus will exhibit 11 consecutive recessive bits at the end of every correct transmission,
     * or while the bus is idle. The 11 consecutive recessive bits are made up of:
     *  1 bit - acknowledgement delimiter
     *  7 bit - end of frame bits
     *  3 bit - inter frame space
     * This adds up to 11; therefore, it is not really necessary to wait longer than a few frame TX intervals.
     */
    static const uint16_t TimeoutMilliseconds = 1000;

    for (uint16_t wait_ack = 0; wait_ack < TimeoutMilliseconds; wait_ack++)
    {
        const bool state = (bxcan->MSR & CAN_MSR_INAK) != 0;
        if (state == target_state)
        {
            return true;
        }

        // Sleep 1 millisecond
        usleep(1000);           // TODO: This function may be missing on some platforms
    }

    return false;
}

/// Converts libcanard ID value into the bxCAN TX ID register format.
static uint32_t convertFrameIDCanardToRegister(const uint32_t id)
{
    uint32_t out = 0;

    if (id & CANARD_CAN_FRAME_EFF)
    {
        out = ((id & CANARD_CAN_EXT_ID_MASK) << 3U) | CAN_TI0R_IDE;
    }
    else
    {
        out = ((id & CANARD_CAN_STD_ID_MASK) << 21U);
    }

    if (id & CANARD_CAN_FRAME_RTR)
    {
        out |= CAN_TI0R_RTR;
    }

    return out;
}

/// Converts bxCAN TX/RX (sic! both RX/TX are supported) ID register value into the libcanard ID format.
static uint32_t convertFrameIDRegisterToCanard(const uint32_t id)
{
//Error with the macro
/*
#if (CAN_TI0R_RTR != CAN_RI0R_RTR) ||\
    (CAN_TI0R_IDE != CAN_RI0R_IDE)
# error "RIR bits do not match TIR bits, TIR --> libcanard conversion is not possible"
#endif
*/
    uint32_t out = 0;

    if ((id & CAN_RI0R_IDE) == 0)
    {
        out = (CANARD_CAN_STD_ID_MASK & (id >> 21U));
    }
    else
    {
        out = (CANARD_CAN_EXT_ID_MASK & (id >> 3U)) | CANARD_CAN_FRAME_EFF;
    }

    if ((id & CAN_RI0R_RTR) != 0)
    {
        out |= CANARD_CAN_FRAME_RTR;
    }

    return out;
}

int16_t mcual_can_init(mcual_can_timings * const timings, mcual_can_ifaceMode iface)
{
    /*
     * Paranoia time.
     */
    if ((iface != mcualCanIfaceModeNormal) &&
        (iface != mcualCanIfaceModeSilent) &&
        (iface != mcualCanIfaceModeAutomaticTxAbortOnError))
    {
        return -CANARD_ERROR_INVALID_ARGUMENT;
    }

    if ((timings == NULL) ||
        (timings->bit_rate_prescaler < 1) || (timings->bit_rate_prescaler > 1024) ||
        (timings->max_resynchronization_jump_width < 1) || (timings->max_resynchronization_jump_width > 4) ||
        (timings->bit_segment_1 < 1) || (timings->bit_segment_1 > 16) ||
        (timings->bit_segment_2 < 1) || (timings->bit_segment_2 > 8))
    {
        return -CANARD_ERROR_INVALID_ARGUMENT;
    }

#ifdef CONFIG_MCUAL_CAN_USE_FREERTOS_QUEUES
    tx_queues = xQueueCreate(CONFIG_MCUAL_CAN_TX_SIZE, sizeof(CanardCANFrame));
    rx_queues = xQueueCreate(CONFIG_MCUAL_CAN_RX_SIZE, sizeof(CanardCANFrame));
#else
    tx_index_read = 0;
    tx_index_write = 0;
    rx_index_read = 0;
    rx_index_write = 0;
#endif

    //clock
    RCC->APB1ENR |= RCC_APB1ENR_CAN1EN;

    /*
     * Initial setup
     */
    //memset(&g_stats, 0, sizeof(g_stats));

    //g_abort_tx_on_error = (iface_mode == CanardSTM32IfaceModeAutomaticTxAbortOnError);


    CAN1->IER = 0;                   // We need no interrupts
    CAN1->MCR &= ~CAN_MCR_SLEEP;     // Exit sleep mode
    CAN1->MCR |= CAN_MCR_INRQ;       // Request init

    if (!waitMSRINAKBitStateChange(CAN1, true))                // Wait for synchronization
    {
        CAN1->MCR = CAN_MCR_RESET;
        return -MCUAL_CAN_ERROR_MSR_INAK_NOT_SET;
    }

    /*
     * Hardware initialization (the hardware has already confirmed initialization mode, see above)
     */
    CAN1->MCR = CAN_MCR_ABOM | CAN_MCR_AWUM | CAN_MCR_INRQ; 

    CAN1->BTR = (((timings->max_resynchronization_jump_width - 1U) &    3U) << 24U) |
                 (((timings->bit_segment_1 - 1U)                    &   15U) << 16U) |
                 (((timings->bit_segment_2 - 1U)                    &    7U) << 20U) |
                 ((timings->bit_rate_prescaler - 1U)                & 1023U) |
                 ((iface == mcualCanIfaceModeSilent) ? CAN_BTR_SILM : 0);

    //Todo, to be removed
    CANARD_ASSERT(0 == CAN1->IER);             // Making sure the iterrupts are indeed disabled

    CAN1->MCR &= ~CAN_MCR_INRQ;   // Leave init mode

    if (!waitMSRINAKBitStateChange(CAN1, false))
    {
        CAN1->MCR = CAN_MCR_RESET;
        return -MCUAL_CAN_ERROR_MSR_INAK_NOT_CLEARED;
    }

    /*
     * Default filter configuration. Note that ALL filters are available ONLY via CAN1!
     * CAN2 filters are offset by 14.
     * We use 14 filters at most always which simplifies the code and ensures compatibility with all
     * MCU within the STM32 family.
     */
    {
        uint32_t fmr = CAN1->FMR & 0xFFFFC0F1U;
        fmr |= MCUAL_CAN_NUM_ACCEPTANCE_FILTERS << 8U;                // CAN2 start bank = 14 (if CAN2 is present)
        CAN1->FMR = fmr | CAN_FMR_FINIT;
    }

    CANARD_ASSERT(((CAN1->FMR >> 8U) & 0x3FU) == MCUAL_CAN_NUM_ACCEPTANCE_FILTERS);

    CAN1->FM1R = 0;                                        // Indentifier Mask mode
    CAN1->FS1R = 0x0FFFFFFF;                               // All 32-bit

    // Filters are alternating between FIFO0 and FIFO1 in order to equalize the load.
    // This will cause occasional priority inversion and frame reordering on reception,
    // but that is acceptable for UAVCAN, and a majority of other protocols will tolerate
    // this too, since there will be no reordering within the same CAN ID.
    CAN1->FFA1R = 0x0AAAAAAA;

    CAN1->sFilterRegister[0].FR1 = 0;
    CAN1->sFilterRegister[0].FR2 = 0;
    CAN1->FA1R = 1;                                        // One filter enabled

    CAN1->FMR &= ~CAN_FMR_FINIT;              // Leave initialization mode

    //Todo : Missing: isr handling, stats, mode automaticTxAbortOnError

    return 0;
}



int16_t mcual_can_transmit(const CanardCANFrame* const frame)
{
#ifdef CONFIG_MCUAL_CAN_USE_FREERTOS_QUEUES
    xQueueSend(tx_queues, frame, portMAX_DELAY);
#else
    uint8_t done = 0;
    while(!done)
    {
        done = 0;
        int32_t next_write = tx_index_write + 1;

        if(next_write >= CONFIG_MCUAL_CAN_TX_SIZE)
        {
            next_write = 0;
        }

        //if(next_write != tx_index_read)
        //{
            __disable_irq();
            can_tx_buffer[tx_index_write] = *frame;
            tx_index_write = next_write;
            done = 1;
            __enable_irq();
        //}
    }
#endif
    
    //Enable TX interrupt
    CAN1->IER |= CAN_IER_TMEIE;
    return 1;
}

void mcual_can_wait_tx_ended()
{
    ;
}

int16_t mcual_can_recv(CanardCANFrame* const out_frame)
{
#ifdef CONFIG_MCUAL_CAN_USE_FREERTOS_QUEUES
    xQueueReceive(rx_queues, out_frame, portMAX_DELAY);
#else
    int16_t r = 0;
    while(!r)
    {
        r = mcual_can_recv_no_wait(out_frame);
    }
#endif
    //todo
    return 1;
}

int16_t mcual_can_recv_no_wait(CanardCANFrame* const out_frame)
{
#ifdef CONFIG_MCUAL_CAN_USE_FREERTOS_QUEUES
    uint8_t byte;
    if(xQueueReceive(rx_queues, out_frame, 0) == pdFALSE)
    {
        return 0;
    }
    return 1;
#else
    int16_t return_value = 0;
    __disable_irq();
    if(rx_index_read != rx_index_write)
    {
        *out_frame = can_rx_buffer[rx_index_read];
        rx_index_read += 1;
        if(rx_index_read >= CONFIG_MCUAL_CAN_RX_SIZE)
        {
            rx_index_read = 0;
        }
        return_value = 1;
    }
    __enable_irq();
    return return_value;
#endif
}

static void mcual_can_add_in_mailbox(uint8_t tx_mailbox, volatile CanardCANFrame * frame)
{
    volatile CAN_TxMailBox_TypeDef* const mb = &CAN1->sTxMailBox[tx_mailbox];

    mb->TDTR = frame->data_len;                         // DLC equals data length except in CAN FD

    mb->TDHR = (((uint32_t)frame->data[7]) << 24U) |
               (((uint32_t)frame->data[6]) << 16U) |
               (((uint32_t)frame->data[5]) <<  8U) |
               (((uint32_t)frame->data[4]) <<  0U);
    mb->TDLR = (((uint32_t)frame->data[3]) << 24U) |
               (((uint32_t)frame->data[2]) << 16U) |
               (((uint32_t)frame->data[1]) <<  8U) |
               (((uint32_t)frame->data[0]) <<  0U);

    mb->TIR = convertFrameIDCanardToRegister(frame->id) | CAN_TI0R_TXRQ;    // Go.

    tx_index_read += 1;
    if(tx_index_read >= CONFIG_MCUAL_CAN_TX_SIZE)
        tx_index_read = 0;
}

void CAN1_TX_IRQHandler(void)
{
    static const uint32_t AllTME = CAN_TSR_TME0 | CAN_TSR_TME1 | CAN_TSR_TME2;
    static bool is_empty = false;
    volatile CanardCANFrame * frame = &can_tx_buffer[tx_index_read];
    bool tme[3];

    if ((CAN1->TSR & AllTME) != AllTME) // At least one TX mailbox is used, detailed check is needed
    {
        tme[0] = ((CAN1->TSR & CAN_TSR_TME0) != 0);
        tme[1] = ((CAN1->TSR & CAN_TSR_TME1) != 0);
        tme[2] = ((CAN1->TSR & CAN_TSR_TME2) != 0);
    }
    else
        is_empty = true;

    if(CAN1->TSR & CAN_TSR_TME0)
    {
        //All mailboxes are empty, add directly in mailbox 0
        if(is_empty)
            mcual_can_add_in_mailbox(0, frame);
        else
        {
            for(int i = 1; i < 3; i++)
            {
                //if empty
                if(!tme[i])
                {
                    if (!isFramePriorityHigher(frame->id, convertFrameIDRegisterToCanard(CAN1->sTxMailBox[i].TIR)))
                    {
                        // There's a mailbox whose priority is higher or equal the priority of the new frame.
                        return;                            // Priority inversion would occur! Reject transmission.
                    }
                }
            } 
            mcual_can_add_in_mailbox(0, frame);
        }
    }
    else if(CAN1->TSR & CAN_TSR_TME1)
    {
        //All mailboxes are empty, add directly in mailbox 0
        if(is_empty)
            mcual_can_add_in_mailbox(1, frame);
        else
        {
            for(int i = 0; i < 3; i++)
            {
                if(i != 1)
                {
                    //if empty
                    if(!tme[i])
                    {
                        if (!isFramePriorityHigher(frame->id, convertFrameIDRegisterToCanard(CAN1->sTxMailBox[i].TIR)))
                        {
                            // There's a mailbox whose priority is higher or equal the priority of the new frame.
                            return;                            // Priority inversion would occur! Reject transmission.
                        }
                    }
                }
            } 
            mcual_can_add_in_mailbox(1, frame);
        }
    }
    else if(CAN1->TSR & CAN_TSR_TME2)
    {
        //All mailboxes are empty, add directly in mailbox 0
        if(is_empty)
            mcual_can_add_in_mailbox(2, frame);
        else
        {
            for(int i = 0; i < 2; i++)
            {
                //if empty
                if(!tme[i])
                {
                    if (!isFramePriorityHigher(frame->id, convertFrameIDRegisterToCanard(CAN1->sTxMailBox[i].TIR)))
                    {
                        // There's a mailbox whose priority is higher or equal the priority of the new frame.
                        return;                            // Priority inversion would occur! Reject transmission.
                    }
                }
            } 
            mcual_can_add_in_mailbox(1, frame);
        }
    }

    if(tx_index_read == tx_index_write)
        CAN1->IER &= ~CAN_IER_TMEIE;
}

void CAN1_RX0_IRQHandler(void)
{
    ;
}

void CAN1_RX1_IRQHandler(void)
{
    ;
}

void CAN1_SCE_IRQHandler(void)
{
    ;
}
#endif
