uint8_t com_should_accept_transfer(uint64_t* out_data_type_signature,
		uint16_t data_type_id,
		CanardTransferType transfer_type,
		uint8_t source_node_id)
{
	(void)source_node_id;

  if ((transfer_type == CanardTransferTypeResponse) &&
			(data_type_id == UAVCAN_COCOBOT_MECAACTION_ID)
		 )
	{
		*out_data_type_signature = UAVCAN_COCOBOT_MECAACTION_SIGNATURE;
		return true;
	}

	return false;
}
