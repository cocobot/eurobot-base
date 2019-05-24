#include <include/generated/autoconf.h>
#ifdef CONFIG_LIBCOCOBOT_COM_RF
#include <stdint.h>
#include <mcual.h>
#include <canard.h>
#include <platform.h>
#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>
#include <queue.h>
#include "SPIRIT_Radio.h"
#include "SPIRIT_Management.h"

#define POWER_DBM                   11.6
#define CHANNEL_SPACE               100e3
#define FREQ_DEVIATION              127e3
#define BANDWIDTH                   540.0e3
#define MODULATION_SELECT           GFSK_BT1
#define DATARATE                    250000

#define XTAL_OFFSET_PPM                 0
#define BASE_FREQUENCY                  868.0e6
#define CHANNEL_NUMBER                  0
#define PREAMBLE_LENGTH                 PKT_PREAMBLE_LENGTH_04BYTES
#define SYNC_LENGTH                     PKT_SYNC_LENGTH_4BYTES
#define SYNC_WORD                       0x88888888
#define LENGTH_TYPE                     PKT_LENGTH_VAR
#define LENGTH_WIDTH                    8
#define CRC_MODE                        PKT_CRC_MODE_8BITS
#define CONTROL_LENGTH                  PKT_CONTROL_LENGTH_0BYTES
#define EN_ADDRESS                      S_DISABLE
#define EN_FEC                          S_DISABLE
#define EN_WHITENING                    S_ENABLE
#define EN_FILT_MY_ADDRESS              S_DISABLE
#define MY_ADDRESS                      0xC2
#define EN_FILT_MULTICAST_ADDRESS       S_DISABLE
#define MULTICAST_ADDRESS               0xC2
#define EN_FILT_BROADCAST_ADDRESS       S_DISABLE
#define BROADCAST_ADDRESS               0xC2
#define PERSISTENT_MODE_EN              S_DISABLE
#define CS_PERIOD                       TBIT_TIME_64
#define CS_TIMEOUT                      TCCA_TIME_3
#define MAX_NB                          5
#define BU_COUNTER_SEED                 0xFA21
#define CU_PRESCALER                    32
#define RSSI_THR 0x20

typedef enum {
  LED_INIT_SUCCESS,
  LED_INIT_FAILURE,
  LED_INIT_DONE,
  LED_TX_ACTIVITY,
  LED_RX_ACTIVITY,
  LED_TX_BUSY,
} rf_led_state;

static uint8_t _init;
SRadioInit _radio_init = {
  XTAL_OFFSET_PPM,
  BASE_FREQUENCY,
  CHANNEL_SPACE,
  CHANNEL_NUMBER,
  MODULATION_SELECT,
  DATARATE,
  FREQ_DEVIATION,
  BANDWIDTH
};

PktBasicInit _basic_init = {
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

PktBasicAddressesInit _address_init = {
  EN_FILT_MY_ADDRESS,
  MY_ADDRESS,
  EN_FILT_MULTICAST_ADDRESS,
  MULTICAST_ADDRESS,
  EN_FILT_BROADCAST_ADDRESS,
  BROADCAST_ADDRESS
};

CsmaInit _csma_init={
  PERSISTENT_MODE_EN,
  CS_PERIOD,
  CS_TIMEOUT,
  MAX_NB,
  BU_COUNTER_SEED,
  CU_PRESCALER
};

static uint64_t _next_tx_activity_clear_us;
static uint64_t _next_rx_activity_clear_us;
static volatile uint32_t _tx_in_progress;
static SemaphoreHandle_t _irq_sem;
static QueueHandle_t _tx_queue;
static QueueHandle_t _rx_queue;


static void cocobot_com_rf_set_led_state(rf_led_state state, int arg)
{
  uint16_t set_red = 0;
  uint16_t set_green = 0;
  uint16_t clear_red = 0;
  uint16_t clear_green = 0;

  switch(state)
  {
    case LED_INIT_SUCCESS:
      clear_red = 0x3FF;
      clear_green = 0x3FF;
      set_red = 1 << 9;
      set_green = 1 << 9 | arg;
      break;

    case LED_INIT_FAILURE:
      clear_red = 0x3FF;
      clear_green = 0x3FF;
      set_red = 1 << 9 | arg;
      break;

    case LED_INIT_DONE:
      clear_red = 0x3FF;
      clear_green = 0x3FF;
      set_green = 1 << 9;
      break;

    case LED_TX_ACTIVITY:
      switch(arg)
      {
        case 0:
          clear_green = 1 << 8;
          clear_red = 1 << 8;
          break;

        case 1:
          set_green = 1 << 8;
          clear_red = 1 << 8;
          break;

        case 2:
          set_green = 1 << 8;
          set_red = 1 << 8;
          break;

        case 3:
          clear_green = 1 << 8;
          set_red = 1 << 8;
          break;

      }
      break;

    case LED_RX_ACTIVITY:
      if(arg)
      {
         set_green = 1 << 7;
      }
      else
      {
         clear_green = 1 << 7;
      }
      break;

    case LED_TX_BUSY:
      if(arg)
      {
         set_red = 1 << 9;
         set_green = 1 << 9;
      }
      else
      {
        clear_red = 1 << 9;
        set_green = 1 << 9;
      }
      break;
  }

  platform_led_clear(clear_red << 0);
  platform_led_clear(clear_green << 10);
  platform_led_set(set_red << 0);
  platform_led_set(set_green << 10);
}

void cocobot_com_rf_init(void)
{
  _init = 0;
  _next_tx_activity_clear_us = 0;
  _next_rx_activity_clear_us = 0;
  _tx_in_progress = 0;
}

void cocobot_com_rf_irq(void)
{
  BaseType_t xHigherPriorityTaskWoken;
  xHigherPriorityTaskWoken = pdFALSE;
  xSemaphoreGiveFromISR( _irq_sem, &xHigherPriorityTaskWoken );
  portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

static void cocobot_com_rf_irq_task(void * unused)
{
  (void)unused;

  for( ;; )
  {
    xSemaphoreTake( _irq_sem, portMAX_DELAY );
    SpiritIrqs xIrqStatus;
    SpiritIrqGetStatus(&xIrqStatus);

    if(xIrqStatus.IRQ_TX_DATA_SENT)
    {
      uint8_t buf[sizeof(CanardCANFrame)];
      if(xQueueReceive(_tx_queue, buf, 0) == pdFALSE)
      {
        SpiritCmdStrobeSabort();
        SpiritCsma(S_DISABLE);
        _tx_in_progress = 0;
        cocobot_com_rf_set_led_state(LED_TX_BUSY, 0);
      }
      else
      {
        SpiritSpiWriteLinearFifo(sizeof(CanardCANFrame), (uint8_t *)buf);
        SpiritCmdStrobeTx();
        continue;
      }
    }

    if(xIrqStatus.IRQ_RX_DATA_READY)
    {
      uint8_t buf[96];

      int cRxData = SpiritLinearFifoReadNumElementsRxFifo();
      SpiritSpiReadLinearFifo(cRxData, buf);

      for(int i = 0; i < cRxData; i += sizeof(CanardCANFrame))
      {
        xQueueSend(_rx_queue, buf + i, portMAX_DELAY);
      }
    }

    if(!_tx_in_progress)
    {
      SpiritCmdStrobeRx();
    }
    else
    {
      if(g_xStatus.MC_STATE != MC_STATE_TX)
      {
        SpiritCmdStrobeTx();
      }
    }
  }
}

void cocobot_com_rf_run_init(void)
{
  _init = 1;
  _tx_queue = xQueueCreate(32, sizeof(CanardCANFrame));
  _rx_queue = xQueueCreate(32, sizeof(CanardCANFrame));

  //reset SPI
  cocobot_com_rf_set_led_state(LED_INIT_SUCCESS, 0);
  platform_gpio_set(PLATFORM_GPIO_SDN);
  vTaskDelay(100);
  platform_spi_select(PLATFORM_SPI_RF_SELECT);
  platform_spi_select(PLATFORM_SPI_CS_UNSELECT);
  platform_gpio_clear(PLATFORM_GPIO_SDN);
  vTaskDelay(100);
  cocobot_com_rf_set_led_state(LED_INIT_SUCCESS, 1);

  do{
    /* Delay for state transition */
    for(volatile uint8_t i=0; i!=0xFF; i++);

    /* Reads the MC_STATUS register */
    SpiritRefreshStatus();
  }while(g_xStatus.MC_STATE!=MC_STATE_READY);
  cocobot_com_rf_set_led_state(LED_INIT_SUCCESS, 2);

  SpiritManagementWaExtraCurrent();
  SpiritRadioSetXtalFrequency(50000000);
  cocobot_com_rf_set_led_state(LED_INIT_SUCCESS, 3);
  cocobot_com_rf_set_led_state(LED_INIT_SUCCESS, 4);

  SGpioInit xGpioIRQ={
    SPIRIT_GPIO_3,
    SPIRIT_GPIO_MODE_DIGITAL_OUTPUT_LP,
    SPIRIT_GPIO_DIG_OUT_IRQ
  };
  SpiritGpioInit(&xGpioIRQ);
  _irq_sem = xSemaphoreCreateBinary();
  mcual_gpio_set_interrupt(MCUAL_GPIOD, MCUAL_GPIO_PIN14, MCUAL_GPIO_FALLING_EDGE, cocobot_com_rf_irq);
  xTaskCreate( cocobot_com_rf_irq_task, "rf irq", 512, NULL, 9, NULL );

  if(SpiritRadioInit(&_radio_init) != 0)
  {
    cocobot_com_rf_set_led_state(LED_INIT_FAILURE, 5);
    while(1); //Unrecoverable issue
  }
  cocobot_com_rf_set_led_state(LED_INIT_SUCCESS, 5);

  SpiritIrqs xIrqStatus;
  SpiritIrqDeInit(&xIrqStatus);
  SpiritIrq(RX_DATA_DISC,S_ENABLE);
  SpiritIrq(RX_DATA_READY,S_ENABLE);
  SpiritIrq(TX_DATA_SENT, S_ENABLE);

  /* Spirit Radio set power */
  SpiritRadioSetPALeveldBm(7,POWER_DBM);
  SpiritRadioSetPALevelMaxIndex(7);
  cocobot_com_rf_set_led_state(LED_INIT_SUCCESS, 6);

  /* Spirit Packet config */
  SpiritPktBasicInit(&_basic_init);
  SpiritPktBasicAddressesInit(&_address_init);
  cocobot_com_rf_set_led_state(LED_INIT_SUCCESS, 7);

  SpiritPktBasicSetPayloadLength(sizeof(CanardCANFrame));
  SpiritPktBasicSetDestinationAddress(BROADCAST_ADDRESS);
  cocobot_com_rf_set_led_state(LED_INIT_SUCCESS, 8);

  ///* Spirit CSMA config */
  SpiritCsmaInit(&_csma_init);
  SpiritCsma(S_ENABLE);
  SpiritQiSetRssiThresholddBm(RSSI_THR);
  cocobot_com_rf_set_led_state(LED_INIT_SUCCESS, 9);

  SpiritQiSetSqiThreshold(SQI_TH_0);
  SpiritQiSqiCheck(S_ENABLE);
  cocobot_com_rf_set_led_state(LED_INIT_SUCCESS, 10);

  SpiritTimerSetRxTimeoutMs(2500.0);
  SpiritTimerSetRxTimeoutStopCondition(SQI_ABOVE_THRESHOLD);
  cocobot_com_rf_set_led_state(LED_INIT_SUCCESS, 11);


  SpiritCalibrationVco(S_ENABLE);
  SpiritCmdStrobeLockTx();
  cocobot_com_rf_set_led_state(LED_INIT_SUCCESS, 12);
  
  while(g_xStatus.MC_STATE != MC_STATE_LOCK){
      SpiritRefreshStatus();
  }
  cocobot_com_rf_set_led_state(LED_INIT_SUCCESS, 13);
  

  uint8_t calData = SpiritCalibrationGetVcoCalDataTx();
  SpiritCalibrationSetVcoCalDataTx(calData);
  cocobot_com_rf_set_led_state(LED_INIT_SUCCESS, 14);
  
  SpiritCmdStrobeReady();
  SpiritCalibrationVco(S_DISABLE);
  cocobot_com_rf_set_led_state(LED_INIT_SUCCESS, 15);


  SpiritIrqClearStatus();
  SpiritCmdStrobeRx();

  cocobot_com_rf_set_led_state(LED_INIT_DONE, 0);
}

int16_t cocobot_com_rf_receive(CanardCANFrame* const frame, uint64_t timestamp_us)
{
  if(!_init)
  {
    cocobot_com_rf_run_init();
  }

  if(timestamp_us > _next_tx_activity_clear_us)
  {
    _next_tx_activity_clear_us = 0;
    cocobot_com_rf_set_led_state(LED_TX_ACTIVITY, 0);
  }
  if(timestamp_us > _next_rx_activity_clear_us)
  {
    _next_rx_activity_clear_us = 0;
    cocobot_com_rf_set_led_state(LED_RX_ACTIVITY, 0);
  }

  if(xQueueReceive(_rx_queue, frame, 0) == pdFALSE)
  {
    return 0;
  }
  else
  {
    if(_next_rx_activity_clear_us == 0)
    {
      cocobot_com_rf_set_led_state(LED_RX_ACTIVITY, 1);
      _next_rx_activity_clear_us = timestamp_us + 5000;
    }
    return sizeof(CanardCANFrame);
  }
}

int16_t cocobot_com_rf_transmit(const CanardCANFrame* const frame, uint64_t timestamp_us)
{
  (void)frame;
  if(!_init)
  {
    cocobot_com_rf_run_init();
  }

  if(!_tx_in_progress)
  {
    if(_next_tx_activity_clear_us == 0)
    {
      cocobot_com_rf_set_led_state(LED_TX_ACTIVITY, 1);
      _next_tx_activity_clear_us = timestamp_us + 5000;
    }

    _tx_in_progress = 1;
    cocobot_com_rf_set_led_state(LED_TX_BUSY, 1);
    SpiritCsma(S_ENABLE);
    SpiritCmdStrobeSabort();
    SpiritCmdStrobeFlushTxFifo();
    SpiritSpiWriteLinearFifo(sizeof(CanardCANFrame), (uint8_t *)frame);
    SpiritCmdStrobeTx();
    return sizeof(CanardCANFrame);
  }
  else
  {
    if(xQueueSend(_tx_queue, frame, 0) == pdTRUE)
    {
      if(_next_tx_activity_clear_us == 0)
      {
        cocobot_com_rf_set_led_state(LED_TX_ACTIVITY, 2);
        _next_tx_activity_clear_us = timestamp_us + 5000;
      }
      return sizeof(CanardCANFrame);
    }
    else
    {
      cocobot_com_rf_set_led_state(LED_TX_ACTIVITY, 3);
      _next_tx_activity_clear_us = timestamp_us + 1000000;
      return sizeof(CanardCANFrame);
    }
  }
}


/* ** Glue for SPIRIT library ** */

#define HEADER_WRITE_MASK     0x00                                /*!< Write mask for header byte*/
#define HEADER_READ_MASK      0x01                                /*!< Read mask for header byte*/
#define HEADER_ADDRESS_MASK   0x00                                /*!< Address mask for header byte*/
#define HEADER_COMMAND_MASK   0x80                                /*!< Command mask for header byte*/
#define LINEAR_FIFO_ADDRESS 0xFF
/* SPIRIT1_Spi_config_Private_Macros */
#define BUILT_HEADER(add_comm, w_r) (add_comm | w_r)                             /*!< macro to build the header byte*/
#define WRITE_HEADER        BUILT_HEADER(HEADER_ADDRESS_MASK, HEADER_WRITE_MASK) /*!< macro to build the write
                                                                                                         header byte*/
#define READ_HEADER         BUILT_HEADER(HEADER_ADDRESS_MASK, HEADER_READ_MASK)  /*!< macro to build the read
                                                                                                         header byte*/
#define COMMAND_HEADER      BUILT_HEADER(HEADER_COMMAND_MASK, HEADER_WRITE_MASK) /*!< macro to build the command
                                                                                                         header byte*/
#define CS_TO_SCLK_DELAY     0x0100


/**
* @brief  Read single or multiple SPIRIT1 register
* @param  cRegAddress: base register's address to be read
* @param  cNbBytes: number of registers and bytes to be read
* @param  pcBuffer: pointer to the buffer of registers' values read
* @retval StatusBytes
*/
StatusBytes RadioSpiReadRegisters(uint8_t cRegAddress, uint8_t cNbBytes, uint8_t* pcBuffer)
{
  uint16_t tmpstatus = 0x00;
  StatusBytes *pStatus = (StatusBytes *)&tmpstatus;

  uint8_t aHeader[2] = {0};
  uint8_t dummy = 0xFF;

  /* Built the aHeader bytes */
  aHeader[0] = READ_HEADER;
  aHeader[1] = cRegAddress;


  /* Put the SPI chip select low to start the transaction */
  platform_spi_select(PLATFORM_SPI_RF_SELECT);

  for (volatile uint16_t Index = 0; Index < CS_TO_SCLK_DELAY; Index++);

  /* Write the aHeader bytes and read the SPIRIT1 status bytes */
  uint8_t r = platform_spi_transfert(aHeader[0]);
  tmpstatus = (r << 8) & 0xFF00;

  /* Write the aHeader bytes and read the SPIRIT1 status bytes */
  r = platform_spi_transfert(aHeader[1]);
  tmpstatus |= r;

  for (int index = 0; index < cNbBytes; index++)
  {
    pcBuffer[index] = platform_spi_transfert(dummy);
  }

  /* Put the SPI chip select high to end the transaction */
  platform_spi_select(PLATFORM_SPI_CS_UNSELECT);

  return *pStatus;

}

/**
* @brief  Send a command
* @param  cCommandCode: command code to be sent
* @retval StatusBytes
*/
StatusBytes RadioSpiCommandStrobes(uint8_t cCommandCode)
{
  uint8_t aHeader[2] = {0};
  uint16_t tmpstatus = 0x0000;

  StatusBytes *pStatus = (StatusBytes *)&tmpstatus;

  /* Built the aHeader bytes */
  aHeader[0] = COMMAND_HEADER;
  aHeader[1] = cCommandCode;

  /* Puts the SPI chip select low to start the transaction */
  platform_spi_select(PLATFORM_SPI_RF_SELECT);

  for (volatile uint16_t Index = 0; Index < CS_TO_SCLK_DELAY; Index++);

  /* Write the aHeader bytes and read the SPIRIT1 status bytes */
  tmpstatus = platform_spi_transfert(aHeader[0]);
  tmpstatus = (tmpstatus << 8) & 0xFF00;

  /* Write the aHeader bytes and read the SPIRIT1 status bytes */
  tmpstatus |= platform_spi_transfert(aHeader[1]);

  /* Puts the SPI chip select high to end the transaction */
  platform_spi_select(PLATFORM_SPI_CS_UNSELECT);

  return *pStatus;
}

/**
* @brief  Write single or multiple RF Transceivers register
* @param  cRegAddress: base register's address to be write
* @param  cNbBytes: number of registers and bytes to be write
* @param  pcBuffer: pointer to the buffer of values have to be written into registers
* @retval StatusBytes
*/
StatusBytes RadioSpiWriteRegisters(uint8_t cRegAddress, uint8_t cNbBytes, uint8_t* pcBuffer)
{
  uint8_t aHeader[2] = {0};
  uint16_t tmpstatus = 0x0000;
  StatusBytes *pStatus=(StatusBytes *)&tmpstatus;

  /* Built the aHeader bytes */
  aHeader[0] = WRITE_HEADER;
  aHeader[1] = cRegAddress;


  /* Puts the SPI chip select low to start the transaction */
  platform_spi_select(PLATFORM_SPI_RF_SELECT);

  for (volatile uint16_t Index = 0; Index < CS_TO_SCLK_DELAY; Index++);

  /* Write the aHeader bytes and read the SPIRIT1 status bytes */
  tmpstatus = platform_spi_transfert(aHeader[0]);
  tmpstatus = tmpstatus << 8;

  /* Write the aHeader bytes and read the SPIRIT1 status bytes */
  tmpstatus |= platform_spi_transfert(aHeader[1]);

  /* Writes the registers according to the number of bytes */
  for (int index = 0; index < cNbBytes; index++)
  {
    platform_spi_transfert(pcBuffer[index]);
  }

  /* Puts the SPI chip select high to end the transaction */
  platform_spi_select(PLATFORM_SPI_CS_UNSELECT);

  return *pStatus;
}

StatusBytes RadioSpiWriteFifo(uint8_t cNbBytes, uint8_t* pcBuffer)
{
  uint16_t tmpstatus = 0x0000;
  StatusBytes *pStatus = (StatusBytes *)&tmpstatus;

  uint8_t aHeader[2] = {0};

  /* Built the aHeader bytes */
  aHeader[0] = WRITE_HEADER;
  aHeader[1] = LINEAR_FIFO_ADDRESS;

  /* Put the SPI chip select low to start the transaction */
  platform_spi_select(PLATFORM_SPI_RF_SELECT);

  for (volatile uint16_t Index = 0; Index < CS_TO_SCLK_DELAY; Index++);

  /* Write the aHeader bytes and read the SPIRIT1 status bytes */
  tmpstatus = platform_spi_transfert(aHeader[0]);
  tmpstatus = tmpstatus << 8;

  /* Write the aHeader bytes and read the SPIRIT1 status bytes */
  tmpstatus |= platform_spi_transfert(aHeader[1]);

  /* Writes the registers according to the number of bytes */
  for (int index = 0; index < cNbBytes; index++)
  {
    platform_spi_transfert(pcBuffer[index]);
  }

  /* Put the SPI chip select high to end the transaction */
  platform_spi_select(PLATFORM_SPI_CS_UNSELECT);

  return *pStatus;
}

StatusBytes RadioSpiReadFifo(uint8_t cNbBytes, uint8_t* pcBuffer)
{
  uint16_t tmpstatus = 0x0000;
  StatusBytes *pStatus = (StatusBytes *)&tmpstatus;
  
  uint8_t aHeader[2];
  uint8_t dummy=0xFF;
  
  /* Built the aHeader bytes */
  aHeader[0]=READ_HEADER;
  aHeader[1]=LINEAR_FIFO_ADDRESS;
  
  /* Put the SPI chip select low to start the transaction */
  platform_spi_select(PLATFORM_SPI_RF_SELECT);
  
  for (volatile uint16_t Index = 0; Index < CS_TO_SCLK_DELAY; Index++);

  /* Write the aHeader bytes and read the SPIRIT1 status bytes */
  tmpstatus = platform_spi_transfert(aHeader[0]);
  tmpstatus = tmpstatus<<8;  
  
  /* Write the aHeader bytes and read the SPIRIT1 status bytes */
  tmpstatus |= platform_spi_transfert(aHeader[1]);

  for (int index = 0; index < cNbBytes; index++)
  { 
    pcBuffer[index] = platform_spi_transfert(dummy);
  } 
  
  /* Put the SPI chip select high to end the transaction */
  platform_spi_select(PLATFORM_SPI_CS_UNSELECT);
  
  return *pStatus;  
}

#endif
