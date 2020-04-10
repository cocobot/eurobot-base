#include <include/generated/autoconf.h>
#ifdef CONFIG_MCUAL_CAN

#include <stdlib.h>
#include <stdbool.h>
#ifdef CONFIG_DEVICE_STM32L496xx
# include <stm32l4xx.h>
#else
# include <stm32f4xx.h>
#endif
#include <mcual.h>
#include <unistd.h>
#include <platform.h>

#ifdef CONFIG_MCUAL_CAN_USE_FREERTOS_QUEUES
#include <FreeRTOS.h>
#include <queue.h>
#endif

#ifdef CONFIG_MCUAL_USE_CAN1
#define BXCAN CAN1
#else
#define BXCAN CAN2
#endif

#ifdef CONFIG_MCUAL_CAN_USE_FREERTOS_QUEUES
QueueHandle_t can_tx_queue;
QueueHandle_t can_rx_queue;
#else
volatile mcual_can_frame_t can_tx_buffer[CONFIG_MCUAL_CAN_TX_SIZE];
volatile int32_t tx_index_read;
volatile int32_t tx_index_write;
volatile mcual_can_frame_t can_rx_buffer[CONFIG_MCUAL_CAN_RX_SIZE];
volatile int32_t rx_index_read;
volatile int32_t rx_index_write;
#endif

static bool isFramePriorityHigher(uint32_t a, uint32_t b)
{
    /*
     * Plain ID arbitration - greater value loses.
     */
    return (a & 0xFFF) < (b & 0xFFF);
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
#ifdef CONFIG_MCUAL_CAN_USE_FREERTOS_QUEUES
        vTaskDelay(1/portTICK_PERIOD_MS);
#else
        usleep(1000);           // TODO: This function may be missing on some platforms
#endif
    }
    return false;
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
        return -1;
    }

    if ((timings == NULL) ||
        (timings->bit_rate_prescaler < 1) || (timings->bit_rate_prescaler > 1024) ||
        (timings->max_resynchronization_jump_width < 1) || (timings->max_resynchronization_jump_width > 4) ||
        (timings->bit_segment_1 < 1) || (timings->bit_segment_1 > 16) ||
        (timings->bit_segment_2 < 1) || (timings->bit_segment_2 > 8))
    {
        return -2;
    }

#ifdef CONFIG_MCUAL_CAN_USE_FREERTOS_QUEUES
    can_tx_queue = xQueueCreate(CONFIG_MCUAL_CAN_TX_SIZE, sizeof(mcual_can_frame_t));
    can_rx_queue = xQueueCreate(CONFIG_MCUAL_CAN_RX_SIZE, sizeof(mcual_can_frame_t));
#else
    tx_index_read = 0;
    tx_index_write = 0;
    rx_index_read = 0;
    rx_index_write = 0;
#endif

    //clock
#ifdef RCC_APB1ENR_CAN1EN
    RCC->APB1ENR |= RCC_APB1ENR_CAN1EN;
    RCC->APB1ENR |= RCC_APB1ENR_CAN2EN;
#endif
#ifdef RCC_APB1ENR1_CAN1EN
    RCC->APB1ENR1 |= RCC_APB1ENR1_CAN1EN;
    RCC->APB1ENR1 |= RCC_APB1ENR1_CAN2EN;
#endif

    /*
     * Initial setup
     */
    //memset(&g_stats, 0, sizeof(g_stats));

    //g_abort_tx_on_error = (iface_mode == CanardSTM32IfaceModeAutomaticTxAbortOnError);


    //CAN1 first
#ifdef CONFIG_MCUAL_USE_CAN2
    CAN1->IER = 0;                   // We need no interrupts
    CAN1->MCR &= ~CAN_MCR_SLEEP;     // Exit sleep mode
    CAN1->MCR |= CAN_MCR_INRQ;       // Request init

    if (!waitMSRINAKBitStateChange(CAN1, true))                // Wait for synchronization
    {
        CAN1->MCR = CAN_MCR_RESET;
        return -3;
    }

#endif
    BXCAN->IER = 0;                   // We need no interrupts
    BXCAN->MCR &= ~CAN_MCR_SLEEP;     // Exit sleep mode
    BXCAN->MCR |= CAN_MCR_INRQ;       // Request init

    if (!waitMSRINAKBitStateChange(BXCAN, true))                // Wait for synchronization
    {
        BXCAN->MCR = CAN_MCR_RESET;
        return -4;
    }

    /*
     * Hardware initialization (the hardware has already confirmed initialization mode, see above)
     */
    BXCAN->MCR = CAN_MCR_ABOM | CAN_MCR_AWUM | CAN_MCR_INRQ; 

    BXCAN->BTR = (((timings->max_resynchronization_jump_width - 1U) &    3U) << 24U) |
                 (((timings->bit_segment_1 - 1U)                    &   15U) << 16U) |
                 (((timings->bit_segment_2 - 1U)                    &    7U) << 20U) |
                 ((timings->bit_rate_prescaler - 1U)                & 1023U) |
                 ((iface == mcualCanIfaceModeSilent) ? CAN_BTR_SILM : 0);

    BXCAN->MCR &= ~CAN_MCR_INRQ;   // Leave init mode

    if (!waitMSRINAKBitStateChange(BXCAN, false))
    {
        BXCAN->MCR = CAN_MCR_RESET;
        return -5;
    }

    uint32_t fmr = CAN1->FMR & 0xFFFFC0F1U;
    fmr |= 14 << 8U;                // CAN2 start bank = 14 (if CAN2 is present)
    CAN1->FMR = fmr | CAN_FMR_FINIT;

    CAN1->FM1R = 0;                                        // Indentifier Mask mode
    CAN1->FS1R = 0x0FFFFFFF;                               // All 32-bit

    // Filters are alternating between FIFO0 and FIFO1 in order to equalize the load.
    /*
     * Default filter configuration. Note that ALL filters are available ONLY via CAN1!
     * CAN2 filters are offset by 14.
     * We use 14 filters at most always which simplifies the code and ensures compatibility with all
     * MCU within the STM32 family.
     */
    CAN1->FFA1R = 0x0AAAAAAA;

#ifdef CONFIG_MCUAL_USE_CAN2
    CAN1->sFilterRegister[14].FR1 = 0;
    CAN1->sFilterRegister[14].FR2 = 0;
    CAN1->FA1R = 1 << 14;                                        // One filter enabled
#else
    CAN1->sFilterRegister[0].FR1 = 0;
    CAN1->sFilterRegister[0].FR2 = 0;
    CAN1->FA1R = 1;                                        // One filter enabled
#endif

    CAN1->FMR &= ~CAN_FMR_FINIT;              // Leave initialization mode

    //Enable Rx Fifo pending message isr
    BXCAN->IER |= CAN_IER_FMPIE1 | CAN_IER_FMPIE0;

    //Enable isr here
#ifdef CONFIG_MCUAL_USE_CAN2
    NVIC_SetPriority(CAN2_TX_IRQn, 7);
    NVIC_SetPriority(CAN2_RX0_IRQn, 8);
    NVIC_SetPriority(CAN2_RX1_IRQn, 9);
    NVIC_EnableIRQ(CAN2_TX_IRQn);
    NVIC_EnableIRQ(CAN2_RX0_IRQn);
    NVIC_EnableIRQ(CAN2_RX1_IRQn);
#else
    NVIC_SetPriority(CAN1_TX_IRQn, 7);
    NVIC_SetPriority(CAN1_RX0_IRQn, 8);
    NVIC_SetPriority(CAN1_RX1_IRQn, 9);
    NVIC_EnableIRQ(CAN1_TX_IRQn);
    NVIC_EnableIRQ(CAN1_RX0_IRQn);
    NVIC_EnableIRQ(CAN1_RX1_IRQn);
#endif

    //Todo : Missing: stats, mode automaticTxAbortOnError

    //Need to prime the pump for TX isr
    //Bad hack?
    BXCAN->sTxMailBox[0].TIR = CAN_TI0R_TXRQ;
    return 0;
}


int16_t mcual_can_transmit(const mcual_can_frame_t * const frame)
{
#ifdef CONFIG_MCUAL_CAN_USE_FREERTOS_QUEUES
    xQueueSend(can_tx_queue, frame, portMAX_DELAY);
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
    BXCAN->IER |= CAN_IER_TMEIE;
    return 1;
}

void mcual_can_wait_tx_ended()
{
#ifdef CONFIG_MCUAL_CAN_USE_FREERTOS_QUEUES
    //need to do better than that
    while(BXCAN->IER & CAN_IER_TMEIE)
        vTaskDelay(1 / portTICK_PERIOD_MS);
#else
    //while the interrupt is enable, the buffer is not empty
    while(BXCAN->IER & CAN_IER_TMEIE)
        ;
#endif
}

int16_t mcual_can_recv(mcual_can_frame_t * const out_frame)
{
#ifdef CONFIG_MCUAL_CAN_USE_FREERTOS_QUEUES
    xQueueReceive(can_rx_queue, out_frame, portMAX_DELAY);
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

int16_t mcual_can_recv_no_wait(mcual_can_frame_t * const out_frame)
{
#ifdef CONFIG_MCUAL_CAN_USE_FREERTOS_QUEUES
    if(xQueueReceive(can_rx_queue, out_frame, 0) == pdFALSE)
    {
        return 0;
    }
    return 1;
#else
    int16_t return_value = 0;
    if(rx_index_read != rx_index_write)
    {
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
            __enable_irq();
        }
    }
    return return_value;
#endif
}

static void mcual_can_add_in_mailbox(uint8_t tx_mailbox, volatile mcual_can_frame_t * frame)
{
    volatile CAN_TxMailBox_TypeDef* const mb = &BXCAN->sTxMailBox[tx_mailbox];

    mb->TDTR = frame->data_len;                         // DLC equals data length except in CAN FD

    mb->TDHR = (((uint32_t)frame->data[7]) << 24U) |
               (((uint32_t)frame->data[6]) << 16U) |
               (((uint32_t)frame->data[5]) <<  8U) |
               (((uint32_t)frame->data[4]) <<  0U);
    mb->TDLR = (((uint32_t)frame->data[3]) << 24U) |
               (((uint32_t)frame->data[2]) << 16U) |
               (((uint32_t)frame->data[1]) <<  8U) |
               (((uint32_t)frame->data[0]) <<  0U);

    mb->TIR = ((frame->id << 3) | CAN_TI0R_IDE | CAN_TI0R_TXRQ;    // Go.

#if !defined(CONFIG_MCUAL_CAN_USE_FREERTOS_QUEUES)
    tx_index_read += 1;
    if(tx_index_read >= CONFIG_MCUAL_CAN_TX_SIZE)
        tx_index_read = 0;

    //if there is nothing left to send
    if(tx_index_read == tx_index_write)
        BXCAN->IER &= ~CAN_IER_TMEIE;
#endif
}

#ifdef CONFIG_MCUAL_USE_CAN2
void CAN2_TX_IRQHandler(void)
#else
void CAN1_TX_IRQHandler(void)
#endif
{
    const uint32_t AllTME = CAN_TSR_TME0 | CAN_TSR_TME1 | CAN_TSR_TME2;
    bool is_empty = false;
    mcual_can_frame_t frame;  
#if CONFIG_MCUAL_CAN_USE_FREERTOS_QUEUES
    BaseType_t xTaskWokenByReceive = pdFALSE;
    if(!xQueueReceiveFromISR(can_tx_queue, &frame, &xTaskWokenByReceive))
    {
        BXCAN->IER &= ~CAN_IER_TMEIE;
        return;
    }
#else
    frame = can_tx_buffer[tx_index_read];
#endif
    bool tme[3];

    if ((BXCAN->TSR & AllTME) != AllTME) // At least one TX mailbox is used, detailed check is needed
    {
        tme[0] = ((BXCAN->TSR & CAN_TSR_TME0) != 0);
        tme[1] = ((BXCAN->TSR & CAN_TSR_TME1) != 0);
        tme[2] = ((BXCAN->TSR & CAN_TSR_TME2) != 0);
    }
    else
        is_empty = true;

    if(BXCAN->TSR & CAN_TSR_TME0)
    {
        //All mailboxes are empty, add directly in mailbox 0
        if(is_empty)
            mcual_can_add_in_mailbox(0, &frame);
        else
        {
            for(int i = 1; i < 3; i++)
            {
                //if empty
                if(!tme[i])
                {
                    if (!isFramePriorityHigher(frame.id, BXCAN->sTxMailBox[i].TIR))
                    {
                        // There's a mailbox whose priority is higher or equal the priority of the new frame.
                        // Priority inversion would occur! Reject transmission.
#if CONFIG_MCUAL_CAN_USE_FREERTOS_QUEUES
                        //The message is not send, it needs to be moved back into the queue
                        xTaskWokenByReceive = false;
                        xQueueSendToFrontFromISR(can_tx_queue, &frame, &xTaskWokenByReceive);
#endif
                        return; 
                    }
                }
            } 
            mcual_can_add_in_mailbox(0, &frame);
        }
    }
    else if(BXCAN->TSR & CAN_TSR_TME1)
    {
        //All mailboxes are empty, add directly in mailbox 1
        if(is_empty)
            mcual_can_add_in_mailbox(1, &frame);
        else
        {
            for(int i = 0; i < 3; i++)
            {
                if(i != 1)
                {
                    //if empty
                    if(!tme[i])
                    {
                        if (!isFramePriorityHigher(frame.id, BXCAN->sTxMailBox[i].TIR))
                        {
                            // There's a mailbox whose priority is higher or equal the priority of the new frame.
#if CONFIG_MCUAL_CAN_USE_FREERTOS_QUEUES
                        //The message is not send, it needs to be moved back into the queue
                        xTaskWokenByReceive = false;
                        xQueueSendToFrontFromISR(can_tx_queue, &frame, &xTaskWokenByReceive);
#endif
                            return;                            // Priority inversion would occur! Reject transmission.
                        }
                    }
                }
            } 
            mcual_can_add_in_mailbox(1, &frame);
        }
    }
    else if(BXCAN->TSR & CAN_TSR_TME2)
    {
        //All mailboxes are empty, add directly in mailbox 2
        if(is_empty)
            mcual_can_add_in_mailbox(2, &frame);
        else
        {
            for(int i = 0; i < 2; i++)
            {
                //if empty
                if(!tme[i])
                {
                    if (!isFramePriorityHigher(frame.id, BXCAN->sTxMailBox[i].TIR))
                    {
                        // There's a mailbox whose priority is higher or equal the priority of the new frame.
#if CONFIG_MCUAL_CAN_USE_FREERTOS_QUEUES
                        //The message is not send, it needs to be moved back into the queue
                        xTaskWokenByReceive = false;
                        xQueueSendToFrontFromISR(can_tx_queue, &frame, &xTaskWokenByReceive);
#endif
                        return;                            // Priority inversion would occur! Reject transmission.
                    }
                }
            } 
            mcual_can_add_in_mailbox(2, &frame);
        }
    }
#if CONFIG_MCUAL_CAN_USE_FREERTOS_QUEUES
    if( xTaskWokenByReceive != pdFALSE )
        taskYIELD ();
#endif
}

static void mcual_can_rcev_frame(volatile CAN_FIFOMailBox_TypeDef* const mb)
{
    mcual_can_frame_t frame;

    frame.id = mb->RIR >> 3;
    frame.data_len = (uint8_t)(mb->RDTR & CAN_RDT0R_DLC);

    // Catching to regular (non volatile) memory for faster reads
    const uint32_t rdlr = mb->RDLR;
    const uint32_t rdhr = mb->RDHR;

    frame.data[0] = (uint8_t)(0xFFU & (rdlr >>  0U));
    frame.data[1] = (uint8_t)(0xFFU & (rdlr >>  8U));
    frame.data[2] = (uint8_t)(0xFFU & (rdlr >> 16U));
    frame.data[3] = (uint8_t)(0xFFU & (rdlr >> 24U));
    frame.data[4] = (uint8_t)(0xFFU & (rdhr >>  0U));
    frame.data[5] = (uint8_t)(0xFFU & (rdhr >>  8U));
    frame.data[6] = (uint8_t)(0xFFU & (rdhr >> 16U));
    frame.data[7] = (uint8_t)(0xFFU & (rdhr >> 24U));

#if CONFIG_MCUAL_CAN_USE_FREERTOS_QUEUES
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xQueueSendToBackFromISR(can_rx_queue, &frame, &xHigherPriorityTaskWoken);
    if( xHigherPriorityTaskWoken != pdFALSE )
        taskYIELD ();
#else
    can_rx_buffer[rx_index_write] = frame;
    //Manage index
    rx_index_write += 1;
    if(rx_index_write >= CONFIG_MCUAL_CAN_RX_SIZE)
        rx_index_write = 0;
#endif
}

#ifdef CONFIG_MCUAL_USE_CAN2
void CAN2_RX0_IRQHandler(void)
#else
void CAN1_RX0_IRQHandler(void)
#endif
{
    // Release FIFO entry we just read
    BXCAN->RF0R =  CAN_RF0R_FOVR0 | CAN_RF0R_FULL0;
    
    if(!(BXCAN->RF0R & CAN_RF0R_FMP0))
    {
        //spurious ?
        return;
    }
    //Manage error?
    
    volatile CAN_FIFOMailBox_TypeDef* const mb = &BXCAN->sFIFOMailBox[0];

    mcual_can_rcev_frame(mb);
    BXCAN->RF0R = CAN_RF0R_RFOM0;
}

#ifdef CONFIG_MCUAL_USE_CAN2
void CAN2_RX1_IRQHandler(void)
#else
void CAN1_RX1_IRQHandler(void)
#endif
{
    // Release FIFO entry we just read
    BXCAN->RF1R = CAN_RF1R_FOVR1 | CAN_RF1R_FULL1;
    if(!(BXCAN->RF1R & CAN_RF1R_FMP1))
    {
        //spurious ?
        return;
    }
    volatile CAN_FIFOMailBox_TypeDef* const mb = &BXCAN->sFIFOMailBox[1];

    mcual_can_rcev_frame(mb);
    BXCAN->RF1R = CAN_RF1R_RFOM1;
}

#ifdef CONFIG_MCUAL_USE_CAN2
void CAN2_SCE_IRQHandler(void)
#else
void CAN1_SCE_IRQHandler(void)
#endif
{
    ;
}
#endif
