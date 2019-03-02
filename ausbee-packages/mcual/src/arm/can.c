#include <include/generated/autoconf.h>
#ifdef CONFIG_MCUAL_CAN

#include <stdlib.h>
#include <stm32f4xx.h>
#include <mcual.h>

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



void mcual_can_init(mcual_can_timings * const timings, mcual_can_ifaceMode iface)
{
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

    //Todo perform init
    (void)timings;
    (void)iface;
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

    //Todo enable tx isr
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

#endif
