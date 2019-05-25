        //debug!(" -> SOURCE {:?}", source_node_id);
        //debug!(" -> FRAMe {:?}", frame);

        if (frame.get_data_len() - 1) as usize > frame.get_data().len() {
            error!("Frame data len = {}", frame.get_data_len());
            return; //Very bad packet !
        }
        let tail_byte = frame.get_data()[(frame.get_data_len() - 1) as usize];

        let mut rx_state;

        if Instance::<T, U>::is_start_of_transfer(tail_byte) {
            let mut data_type_signature = 0;

            if self.handler.should_accept_transfer(
                &self.user_reference,
                &mut data_type_signature,
                data_type_id,
                transfer_type,
                source_node_id,
            ) {
                if self.has(transfer_descriptor) {
                    rx_state = self.find(transfer_descriptor);
                } else {
                    self.prepend_rx_state(transfer_descriptor);
                    rx_state = self.find(transfer_descriptor);
                }

                match rx_state.as_mut() {
                    Some(state) => {
                        state.set_calculated_crc(Instance::<T, U>::crc_add_signature(
                            0xFFFF,
                            data_type_signature,
                        ));
                    }
                    _ => {
                        warn!("NO state");
                        return;
                    }
                }
            } else {
                return; // The application doesn't want this transfer
            }
        } else {
            rx_state = self.find(transfer_descriptor);

            if rx_state.is_none() {
                return;
            }
        }

        assert!(rx_state.is_some()); // All paths that lead to NULL should be terminated with return above
        let mut rx_state = rx_state.unwrap();

        // Resolving the state flags:
        let not_initialized = rx_state.get_timestamp_usec() == 0;
        let tid_timed_out =
            (timestamp_usec - rx_state.get_timestamp_usec()) > TRANSFER_TIMEOUT_USEC;
        let first_frame = Instance::<T, U>::is_start_of_transfer(tail_byte);
        let not_previous_tid = Instance::<T, U>::compute_transfer_id_forward_distance(
            rx_state.get_transfer_id(),
            Instance::<T, U>::transfer_id_from_tail_byte(tail_byte),
        ) > 1;

        let need_restart =
            (not_initialized) || (tid_timed_out) || (first_frame && not_previous_tid);

        if need_restart {
            rx_state.set_transfer_id(Instance::<T, U>::transfer_id_from_tail_byte(tail_byte));
            rx_state.set_next_toggle(0);
            rx_state.release_state_payload();
            if !Instance::<T, U>::is_start_of_transfer(tail_byte) {
                // missed the first frame
                rx_state.transfer_id_incr();
                warn!("Missed the first frame");
                return;
            }
        }

        if Instance::<T, U>::is_start_of_transfer(tail_byte)
            && Instance::<T, U>::is_end_of_transfer(tail_byte)
        // single frame transfer
        {
            let data = frame.get_data().to_vec();
            rx_state.set_timestamp_usec(timestamp_usec);
            let rx_transfer = RxTransfer::new(
                &data,
                frame.get_data_len() - 1,
                data_type_id,
                source_node_id,
            );

            rx_state.prepare_for_next_transfer();
            drop(rx_state);
            self.handler
                .on_transfer_reception(&mut self.user_reference, &rx_transfer);
            return;
        }

        if Instance::<T, U>::toggle_bit(tail_byte) != rx_state.get_next_toggle() {
            error!("Wrong toggle");
            return; // wrong toggle
        }

        if Instance::<T, U>::transfer_id_from_tail_byte(tail_byte) != rx_state.get_transfer_id() {
            return; // unexpected tid
        }
        else {
        }

        if Instance::<T, U>::is_start_of_transfer(tail_byte)
            && !Instance::<T, U>::is_end_of_transfer(tail_byte)
        // Beginning of multi frame transfer
        {
            if frame.get_data_len() <= 3 {
                warn!("Not enough data");
                return; // Not enough data
            }

            // take off the crc and store the payload
            rx_state.set_timestamp_usec(timestamp_usec);
            rx_state.buffer_push_bytes(&frame.get_data()[2..(frame.get_data().len() - 1)]);
            rx_state.set_payload_crc(
                (frame.get_data()[0] as u16) | ((frame.get_data()[1] as u16) << 8),
            );
            let calc_crc = rx_state.get_calculated_crc();
            rx_state.set_calculated_crc(Instance::<T, U>::crc_add(
                calc_crc,
                &frame.get_data()[2..(frame.get_data().len() - 1)],
                (frame.get_data_len() - 3) as usize,
            ));
        } else if !Instance::<T, U>::is_start_of_transfer(tail_byte)
            && !Instance::<T, U>::is_end_of_transfer(tail_byte)
        // Middle of a multi-frame transfer
        {
            rx_state.buffer_push_bytes(&frame.get_data()[..(frame.get_data().len() - 1)]);
            let calc_crc = rx_state.get_calculated_crc();
            rx_state.set_calculated_crc(Instance::<T, U>::crc_add(
                calc_crc,
                &frame.get_data()[..(frame.get_data().len() - 1)],
                (frame.get_data_len() - 1) as usize,
            ));
        } else
        // End of a multi-frame transfer
        {
            let frame_payload_size = (frame.get_data_len() - 1) as usize;

            // Copy the beginning of the frame into the head, point the tail pointer to the remainder
            rx_state.buffer_push_bytes(&frame.get_data()[0..frame_payload_size]);

            // CRC validation
            let calc_crc = rx_state.get_calculated_crc();
            rx_state.set_calculated_crc(Instance::<T, U>::crc_add(
                calc_crc,
                &frame.get_data()[..(frame.get_data().len() - 1)],
                (frame.get_data_len() - 1) as usize,
            ));

            // Making sure the payload is released even if the application didn't bother with it
            let buffer = rx_state.release_state_payload();

            rx_state.prepare_for_next_transfer();

            if rx_state.get_calculated_crc() == rx_state.get_payload_crc() {
                drop(rx_state);
                let rx_transfer =
                    RxTransfer::new(&buffer, buffer.len() as u8, data_type_id, source_node_id);
                self.handler
                    .on_transfer_reception(&mut self.user_reference, &rx_transfer);
            } else {
                drop(rx_state);
            }

            return;
        }

        match rx_state.get_next_toggle() {
            0 => rx_state.set_next_toggle(1),
            _ => rx_state.set_next_toggle(0),
        }
    }

    pub fn request_or_respond(
        &mut self,
        destination_node_id: u8,
        data_type_signature: u64,
        data_type_id: u8,
        inout_transfer_id: &mut u8,
        priority: u8,
        kind: RequestResponse,
        payload: &[u8],
    ) {
        if self.get_local_node_id() == 0 {
            panic!("Node ID is not set");
        }

        let can_id = ((priority as u32) << 24)
            | ((data_type_id as u32) << 16)
            | ((kind as u32) << 15)
            | ((destination_node_id as u32) << 8)
            | ((1 as u32) << 7)
            | (self.get_local_node_id() as u32);
        let mut crc = 0xFFFF;

        if payload.len() > 7 {
            crc = Instance::<T, U>::crc_add_signature(crc, data_type_signature);
            crc = Instance::<T, U>::crc_add(crc, payload, payload.len() as usize);
        }

        self.enqueue_tx_frames(can_id, inout_transfer_id, crc, payload);

        if kind == RequestResponse::Request {
            // Response Transfer ID must not be altered
            Instance::<T, U>::increment_transfer_id(inout_transfer_id);
        }
    }

    fn increment_transfer_id(transfer_id: &mut u8) {
        *transfer_id += 1;
        if *transfer_id >= 32 {
            *transfer_id = 0;
        }
    }

    fn enqueue_tx_frames(&mut self, can_id: u32, transfer_id: &mut u8, crc: u16, payload: &[u8]) {
        if payload.len() < CANFrame::CAN_FRAME_MAX_DATA_LEN {
            // Single frame transfer
            let id = can_id | CAN_FRAME_EFF;
            let data_len = (payload.len() + 1) as u8;
            let mut data = [0; CANFrame::CAN_FRAME_MAX_DATA_LEN];
            for i in 0..payload.len() {
                data[i] = payload[i];
            }
            data[payload.len()] = 0xC0 | (*transfer_id & 31);

            let frame = CANFrame::new(id, data, data_len);
            self.queue.push_back(frame);
        } else {
            // Multi frame transfer
            let mut data_index = 0;
            let mut toggle = 0;
            let mut sot_eot = 0x80;

            while payload.len() - data_index != 0 {
                let mut data = [0; CANFrame::CAN_FRAME_MAX_DATA_LEN];

                let mut i;
                if data_index == 0 {
                    // add crc
                    data[0] = (crc) as u8;
                    data[1] = (crc >> 8) as u8;
                    i = 2;
                } else {
                    i = 0;
                }

                while i < (CANFrame::CAN_FRAME_MAX_DATA_LEN - 1) && data_index < payload.len() {
                    data[i] = payload[data_index];
                    i += 1;
                    data_index += 1;
                }
                // tail byte
                if data_index == payload.len() {
                    sot_eot = 0x40;
                }

                data[i] = sot_eot | (toggle << 5) | (*transfer_id & 31);
                let frame = CANFrame::new(can_id | CAN_FRAME_EFF, data, (i + 1) as u8);
                self.queue.push_back(frame);

                toggle ^= 1;
                sot_eot = 0;
            }
        }
    }
}
