#include <mcual.h>
#include <platform.h>
#include <FreeRTOS.h>
#include <task.h>
#include <cocobot.h>
#include "SPIRIT_Radio.h"
#include "SPIRIT_Management.h"
#include <semphr.h>

#define BASE_FREQUENCY              868.0e6
#define CHANNEL_SPACE               20e3
#define CHANNEL_NUMBER              0
#define MODULATION_SELECT           FSK
#define DATARATE                    38400
#define FREQ_DEVIATION              20e3
#define BANDWIDTH                   100E3
#define POWER_DBM                   11.6

#define RSSI_THRESHOLD              0x20

/*  Packet configuration parameters  */
#define PREAMBLE_LENGTH             PKT_PREAMBLE_LENGTH_04BYTES
#define SYNC_LENGTH                 PKT_SYNC_LENGTH_4BYTES
#define SYNC_WORD                   0x88888888
#define LENGTH_TYPE                 PKT_LENGTH_VAR
#define LENGTH_WIDTH                8
#define CRC_MODE                    PKT_CRC_MODE_8BITS
#define CONTROL_LENGTH              PKT_CONTROL_LENGTH_0BYTES
#define EN_ADDRESS                  S_ENABLE
#define EN_FEC                      S_DISABLE
#define EN_WHITENING                S_ENABLE

/*  Addresses configuration parameters  */
#define EN_FILT_MY_ADDRESS          S_DISABLE
#define MY_ADDRESS                  0x34
#define EN_FILT_MULTICAST_ADDRESS   S_DISABLE
#define MULTICAST_ADDRESS           0xEE
#define EN_FILT_BROADCAST_ADDRESS   S_DISABLE
#define BROADCAST_ADDRESS           0xFF
#define DESTINATION_ADDRESS         0x44

/* Wake Up timer in ms for LDC mode */
#define WAKEUP_TIMER                100.0
#define XTAL_OFFSET_PPM             0


SRadioInit xRadioInit = {
  XTAL_OFFSET_PPM,
  BASE_FREQUENCY,
  CHANNEL_SPACE,
  CHANNEL_NUMBER,
  MODULATION_SELECT,
  DATARATE,
  FREQ_DEVIATION,
  BANDWIDTH
};
PktBasicInit xBasicInit={
  PREAMBLE_LENGTH,
  SYNC_LENGTH,
  SYNC_WORD,
  LENGTH_TYPE,
  LENGTH_WIDTH,
  CRC_MODE,
  CONTROL_LENGTH,
  EN_ADDRESS,
  EN_FEC,
  EN_WHITENING
};
PktBasicAddressesInit xAddressInit={
  EN_FILT_MY_ADDRESS,
  MY_ADDRESS,
  EN_FILT_MULTICAST_ADDRESS,
  MULTICAST_ADDRESS,
  EN_FILT_BROADCAST_ADDRESS,
  BROADCAST_ADDRESS
};
SGpioInit xGpioIRQ={
  SPIRIT_GPIO_3,
  SPIRIT_GPIO_MODE_DIGITAL_OUTPUT_LP,
  SPIRIT_GPIO_DIG_OUT_IRQ
};
SGpioInit xGpioIRQ2={
  SPIRIT_GPIO_2,
  SPIRIT_GPIO_MODE_DIGITAL_OUTPUT_LP,
  SPIRIT_GPIO_DIG_OUT_IRQ
};

uint8_t xTxDoneFlag = 0;
SpiritIrqs xIrqStatus;
uint8_t vectcTxBuff[20]={4,5,4,4,5,6,7,8,9,10,11,4,13,14,4,4,17,4,19,4};
static SemaphoreHandle_t _irq_sem;

void rf_irq(void)
{
  platform_led_set(PLATFORM_LED_GREEN_3);
  BaseType_t xHigherPriorityTaskWoken;
  xHigherPriorityTaskWoken = pdFALSE;
  xSemaphoreGiveFromISR( _irq_sem, &xHigherPriorityTaskWoken );
  portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

static void rf_irq_task(void * unused)
{
  (void)unused;

  for( ;; )
  {
    xSemaphoreTake( _irq_sem, portMAX_DELAY );

    platform_led_set(PLATFORM_LED_GREEN_4);

    /* Get the IRQ status */
    SpiritIrqGetStatus(&xIrqStatus);

    /* Check the SPIRIT TX_DATA_SENT IRQ flag */
    if(xIrqStatus.IRQ_TX_DATA_SENT)
    {
      /* set the tx_done_flag to manage the event in the main() */
      xTxDoneFlag = 1;
    }
  }
}


void test_rf(void * unused)
{
  (void)unused;

  platform_led_set(PLATFORM_LED_RED_0);

  platform_gpio_set(PLATFORM_GPIO_SDN);
  vTaskDelay(100);

  platform_spi_select(PLATFORM_SPI_RF_SELECT);
  platform_spi_select(PLATFORM_SPI_CS_UNSELECT);
  platform_gpio_clear(PLATFORM_GPIO_SDN);
  vTaskDelay(100);

  do{
    /* Delay for state transition */
    for(volatile uint8_t i=0; i!=0xFF; i++);

    /* Reads the MC_STATUS register */
    SpiritRefreshStatus();
  }while(g_xStatus.MC_STATE!=MC_STATE_READY);


  SpiritManagementWaExtraCurrent();
  SpiritRadioSetXtalFrequency(50000000);

  /* Spirit IRQ config */
  SpiritGpioInit(&xGpioIRQ);
  SpiritGpioInit(&xGpioIRQ2);

  /* uC IRQ enable */
  _irq_sem = xSemaphoreCreateBinary();
  mcual_gpio_set_interrupt(MCUAL_GPIOD, MCUAL_GPIO_PIN14, MCUAL_GPIO_FALLING_EDGE, rf_irq);
 // mcual_gpio_set_interrupt(MCUAL_GPIOD, MCUAL_GPIO_PIN15, MCUAL_GPIO_RISING_EDGE, rf_irq);
  xTaskCreate(rf_irq_task, "rf irq", 512, NULL, 9, NULL );

  /* Spirit Radio config */
  SpiritRadioInit(&xRadioInit);

  /* Spirit Radio set power */
  SpiritRadioSetPALeveldBm(7,POWER_DBM);
  SpiritRadioSetPALevelMaxIndex(7);

  /* Spirit Packet config */
  SpiritPktBasicInit(&xBasicInit);
  SpiritPktBasicAddressesInit(&xAddressInit);

  /* Spirit IRQs enable */
  SpiritIrqDeInit(NULL);
  SpiritIrq(TX_DATA_SENT , S_ENABLE);

  /* payload length config */
  SpiritPktBasicSetPayloadLength(20);

  /* destination address.
  By default it is not used because address is disabled, see struct xAddressInit*/
  SpiritPktBasicSetDestinationAddress(0x35);

  /* IRQ registers blanking */
  SpiritCmdStrobeSabort();
  SpiritIrqClearStatus();
 


  platform_led_clear(PLATFORM_LED_RED_0);
  platform_led_set(PLATFORM_LED_GREEN_0);

  platform_led_set(PLATFORM_LED_GREEN_1);

  /* infinite loop */
  while (1){
    platform_led_toggle(PLATFORM_LED_RED_1);
    /* fit the TX FIFO */
    SpiritCmdStrobeFlushTxFifo();
    SpiritSpiWriteLinearFifo(20, vectcTxBuff);

    /* send the TX command */
    SpiritCmdStrobeTx();

    /* wait for TX done */
    platform_led_set(PLATFORM_LED_GREEN_2);
    while(!xTxDoneFlag)
    {
      if(mcual_gpio_get(MCUAL_GPIOD, MCUAL_GPIO_PIN14))
      {
        platform_led_clear(PLATFORM_LED_RED_9);
        platform_led_set(PLATFORM_LED_GREEN_9);
      }
      else
      {
        platform_led_set(PLATFORM_LED_RED_9);
        platform_led_clear(PLATFORM_LED_GREEN_9);
      }
    }
    xTxDoneFlag = 0;
    platform_led_clear(PLATFORM_LED_GREEN_2);

    /* pause between two transmissions */
    vTaskDelay(500);

    //clear all
    platform_led_clear(PLATFORM_LED_RED_8);
    platform_led_clear(PLATFORM_LED_GREEN_3);
    platform_led_clear(PLATFORM_LED_GREEN_4);
  }
}
 
int main(void) 
{
  platform_init();

  xTaskCreate(test_rf, "RF", 512, NULL, 5, NULL );

  vTaskStartScheduler();

  return 0;
}
