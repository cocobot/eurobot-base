#include <include/generated/autoconf.h>
#ifdef CONFIG_LIBCOCOBOT_CAN

#include <canard.h>

static CanardInstance _canard;
static uint8_t _canard_memory_pool[1024];


static void onTransferReceived(CanardInstance* ins,
                               CanardRxTransfer* transfer)
{
  (void)ins;
  (void)transfer;
}

static bool shouldAcceptTransfer(const CanardInstance* ins,
                                 uint64_t* out_data_type_signature,
                                 uint16_t data_type_id,
                                 CanardTransferType transfer_type,
                                 uint8_t source_node_id)
{
  (void)ins;
  (void)out_data_type_signature;
  (void)data_type_id;
  (void)transfer_type;
  (void)source_node_id;
  return false;
}

void cocobot_can_init(void)
{
  canardInit(&_canard, _canard_memory_pool, sizeof(_canard_memory_pool), onTransferReceived, shouldAcceptTransfer, NULL);
}

#endif
