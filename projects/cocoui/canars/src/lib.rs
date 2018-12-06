#![feature(nll)]

use std::cell::RefCell;
use std::cell::RefMut;
use std::mem::swap;

 const BROADCAST_NODE_ID: u8 = 0;
 const MIN_NODE_ID: u8 = 1;
 const MAX_NODE_ID: u8 = 127;
 const CAN_FRAME_EFF: u32 = (1 << 31);
///< Extended frame format
 const CAN_FRAME_RTR: u32 = (1 << 30);
///< Remote transmission (not used by UAVCAN)
 const CAN_FRAME_ERR: u32 = (1 << 29);
///< Error frame (not used by UAVCAN)
const ANON_MSG_DATA_TYPE_ID_BIT_LEN: u16 = 2;
const TRANSFER_TIMEOUT_USEC: u64 = 2000000;
const TRANSFER_ID_BIT_LEN: usize = 5;

pub struct RxTransfer<'a> {
    timestamp_usec: u64,
    payload: &'a Vec<u8>,
    payload_len: u8,
    data_type_id: u16,
    transfer_type: TransferType,
    transfer_id: u8,
    priority: u8,
    source_node_id: u8,
}

impl<'a> RxTransfer<'a> {
     fn new(
        timestamp_usec: u64,
        payload: &'a Vec<u8>,
        payload_len: u8,
        data_type_id: u16,
        transfer_type: TransferType,
        transfer_id: u8,
        priority: u8,
        source_node_id: u8,
    ) -> RxTransfer {
        RxTransfer {
            timestamp_usec,
            payload,
            payload_len,
            data_type_id,
            transfer_type,
            transfer_id,
            priority,
            source_node_id,
        }
    }

    pub fn get_data_type_id(&self) -> u16 {
         self.data_type_id
    }

    pub fn decode_scalar_u8(&self, offset: &mut usize, size: u8) -> Option<u8> {
        //println!("-> offset {}/{}", offset, size);
        let mut storage = 0;
        let size = size as usize;
        let mut bit = 0;
        for i in *offset..(*offset+size) {
            let data = match self.payload.get(i / 8) {
                Some(s) => *s,
                None => return None,
            };
            //println!("   data {}/{}/{}/{}", data, storage, i, bit);
            if data & (1 << (i % 8)) != 0 {
                storage |= (1 << bit);
            }
            //println!("   step {}/{}/{}/{}", data, storage, i, bit);
            bit += 1;
            assert!(bit <= 8);
        }
        *offset += size;
        //println!("   final {}", storage);
        Some(storage)
    }

    pub fn decode_scalar_u16(&self, offset: &mut usize, size: u8) -> Option<u16> {
        let mut r = 0;
        let mut i = 0;
        let mut size = size;
        while size > 0 {
            let mut to_read = size;
            if to_read > 8 {
                 to_read = 8;
            }
            let p1 = match self.decode_scalar_u8(offset, to_read) {
                Some(s) => s as u16,
                None => return None,
            };
            r |= p1 << i;
            size -= to_read;
            i += 8;
        }
        Some(r)
    }

    pub fn decode_scalar_u32(&self, offset: &mut usize, size: u8) -> Option<u32> {
        let mut r = 0;
        let mut i = 0;
        let mut size = size;
        while size > 0 {
            let mut to_read = size;
            if to_read > 8 {
                 to_read = 8;
            }
            let p1 = match self.decode_scalar_u8(offset, to_read) {
                Some(s) => s as u32,
                None => return None,
            };
            r |= p1 << i;
            size -= to_read;
            i += 8;
        }
        Some(r)
    }

    pub fn decode_scalar_u64(&self, offset: &mut usize, size: u8) -> Option<u64> {
        unimplemented!();
    }

    pub fn decode_scalar_bool(&self, offset: &mut usize, size: u8) -> Option<bool> {
        unimplemented!();
    }

    pub fn get_payload_len(&self) -> u8 {
         self.payload_len
    }

    pub fn get_source_node_id(&self) -> u8 {
         self.source_node_id
    }
}

pub trait Node<U> {
    fn on_transfer_reception(&self, &mut U, &RxTransfer);
    fn should_accept_transfer(&self, &U, &mut u64, u16, TransferType, u8) -> bool;
}

#[derive(PartialEq, Copy, Clone)]
pub enum TransferType {
    Response,
    Request,
    Broadcast,
}

impl TransferType {
    fn value(&self) -> u32 {
        match self {
            TransferType::Response => 0,
            TransferType::Request => 1,
            TransferType::Broadcast => 2,
        }
    }
}

#[derive(Clone, Debug)]
pub struct CANFrame {
    id: u32,
    data: [u8; CANFrame::CAN_FRAME_MAX_DATA_LEN],
    data_len: u8,
}

impl CANFrame {
    pub const CAN_FRAME_MAX_DATA_LEN: usize = 8;

    pub fn new(id: u32, data: [u8; CANFrame::CAN_FRAME_MAX_DATA_LEN], data_len: u8) -> CANFrame {
        CANFrame {
             id,
             data,
             data_len,
        }
    }

    fn service_not_msg_from_id(id: u32) -> bool {
        (((id) >> 7) & 0x1) != 0
    }

    fn request_not_response_from_id(id: u32) -> bool {
        (((id) >> 15) & 0x1) != 0
    }

    fn dest_id_from_id(id: u32) -> u8 {
        (((id) >> 8) & 0x7F) as u8
    }

    fn priority_from_id(id: u32) -> u8 {
        (((id) >> 24) & 0x1F) as u8
    }

    fn source_id_from_id(id: u32) -> u8 {
        (((id) >> 0) & 0x7F) as u8
    }

    fn msg_type_from_id(id: u32) -> u16 {
        (((id) >> 8) & 0xFFFF) as u16
    }

    fn srv_type_from_id(id: u32) -> u8 {
        (((id) >> 16) & 0xFF) as u8
    }

     fn extract_transfer_type(&self) -> TransferType {
        let is_service = CANFrame::service_not_msg_from_id(self.id);
        if !is_service {
            return TransferType::Broadcast;
        } else if CANFrame::request_not_response_from_id(self.id) {
            return TransferType::Request;
        } else {
            return TransferType::Response;
        }
    }

     fn extract_data_type(&self) -> u16 {
        if self.extract_transfer_type() == TransferType::Broadcast {
            let mut dtid = CANFrame::msg_type_from_id(self.id);
            if CANFrame::source_id_from_id(self.id) == BROADCAST_NODE_ID {
                dtid &= (1 << ANON_MSG_DATA_TYPE_ID_BIT_LEN) - 1;
            }
            dtid
        } else {
            CANFrame::srv_type_from_id(self.id) as u16
        }
    }

     fn get_id(&self) -> u32 {
        self.id
    }

     fn get_data_len(&self) -> u8 {
        self.data_len
    }

     fn get_data(&self) -> &[u8; CANFrame::CAN_FRAME_MAX_DATA_LEN] {
        &self.data
    }
}

 struct RxState {
    buffer: Vec<u8>,
    dtid_tt_snid_dnid: u32,
    calculated_crc: u16,
    timestamp_usec: u64,
    transfer_id: u8,
    next_toggle: u8,
    payload_crc: u16,
}

impl RxState {
     fn create(transfer_descriptor: u32) -> RxState {
        RxState {
            buffer: Vec::new(),
            dtid_tt_snid_dnid: transfer_descriptor,
            calculated_crc: 0,
            timestamp_usec: 0,
            transfer_id: 0,
            next_toggle: 0,
            payload_crc: 0,
        }
    }

     fn get_dtid_tt_snid_dnid(&self) -> u32 {
        self.dtid_tt_snid_dnid
    }

     fn set_calculated_crc(&mut self, crc: u16) {
        self.calculated_crc = crc;
    }

     fn get_calculated_crc(&self) -> u16 {
        self.calculated_crc
    }

     fn get_timestamp_usec(&self) -> u64 {
        self.timestamp_usec
    }

     fn set_timestamp_usec(&mut self, value: u64) {
        self.timestamp_usec = value;
    }

     fn get_transfer_id(&self) -> u8 {
        self.transfer_id
    }

     fn set_transfer_id(&mut self, id: u8) {
        self.transfer_id = id;
    }

     fn transfer_id_incr(&mut self) {
        self.transfer_id.overflowing_add(1);
    }

     fn set_next_toggle(&mut self, value: u8) {
        self.next_toggle = value;
    }

     fn get_next_toggle(&mut self) -> u8 {
        self.next_toggle
    }

     fn release_state_payload(&mut self) -> Vec<u8> {
        let mut old_buffer = Vec::new();
        swap(&mut old_buffer, &mut self.buffer);
        old_buffer
    }

     fn prepare_for_next_transfer(&mut self) {
        assert!(self.buffer.len() == 0);
        self.transfer_id_incr();
        self.next_toggle = 0;
    }

     fn buffer_push_bytes(&mut self, data: &[u8]) {
        for i in 0..data.len() {
            self.buffer.push(data[i]);
        }
    }

     fn set_payload_crc(&mut self, crc: u16) {
        self.payload_crc = crc;
    }

     fn get_payload_crc(&mut self) -> u16 {
        self.payload_crc
    }

     fn get_buffer(&self) -> &Vec<u8> {
        &self.buffer
    } 
}

pub struct Instance<T: Node<U>, U> {
    node_id: u8,
    handler: T,
    user_reference: U,
    rx_states: Vec<RefCell<RxState>>,
}

impl<T: Node<U>, U> Instance<T, U> {
     pub fn init(
        handler: T,
        user_reference: U,
    ) -> Instance<T, U> {
        Instance {
            node_id: BROADCAST_NODE_ID,
            handler,
            user_reference,
            rx_states: Vec::new(),
        }
    }

     fn get_user_reference(&self) -> &U {
        &self.user_reference
    }

     fn get_user_reference_as_mut(&mut self) -> &mut U {
        &mut self.user_reference
    }

     fn set_local_node_id(&mut self, self_node_id: u8) {
        if (self.node_id == BROADCAST_NODE_ID)
            && (self.node_id >= MIN_NODE_ID)
            && (self.node_id <= MAX_NODE_ID)
        {
            self.node_id = self_node_id;
        } else {
            assert!(false);
        }
    }

     fn get_local_node_id(&self) -> u8 {
        self.node_id
    }

    fn make_transfer_descriptor(
        data_type_id: u16,
        transfer_type: TransferType,
        source_node_id: u8,
        destination_node_id: u8,
    ) -> u32 {
        ((data_type_id as u32)
            | (transfer_type.value() << 16)
            | ((source_node_id as u32) << 18)
            | ((destination_node_id as u32) << 25))
    }

    fn toggle_bit(byte: u8) -> u8 {
        ((byte) >> 5) & 0x1 as u8
    }

    fn is_start_of_transfer(byte: u8) -> bool {
        (((byte) >> 7) & 0x1) != 0
    }

    fn is_end_of_transfer(byte: u8) -> bool {
        (((byte) >> 6) & 0x1) != 0
    }

    fn transfer_id_from_tail_byte(byte: u8) -> u8 {
        ((byte) & 0x1F) as u8
    }

    fn compute_transfer_id_forward_distance(a: u8, b: u8) -> i16 {
        let mut d = (b as i16) - (a as i16);
        if d < 0 {
            d = d + ((1 << TRANSFER_ID_BIT_LEN) as i16);
        }
        d
    }

     fn prepend_rx_state(&mut self, transfer_descriptor: u32) {
        let new_head = RefCell::new(RxState::create(transfer_descriptor));
        self.rx_states.push(new_head);
    }

    fn find(&self, transfer_descriptor: u32) -> Option<RefMut<RxState>> {
        let state = self.rx_states
            .iter()
            .find(|x| x.borrow().get_dtid_tt_snid_dnid() == transfer_descriptor);
        match state {
             Some(s) => Some(s.borrow_mut()),
             None => None,
        }
    }

    fn has(&self, transfer_descriptor: u32) -> bool {
        self.rx_states
            .iter()
            .find(|x| x.borrow().get_dtid_tt_snid_dnid() == transfer_descriptor)
            .is_some()
    }

    fn crc_add_signature(crc_val: u16, data_type_signature: u64) -> u16 {
        let mut shift_val = 0;
        let mut crc_val = crc_val;
        while shift_val < 64 {
            crc_val =
                Instance::<T, U>::crc_add_byte(crc_val, (data_type_signature >> shift_val) as u8);
            shift_val += 8;
        }
        crc_val
    }

    fn crc_add_byte(crc_val: u16, byte: u8) -> u16 {
        let mut crc_val = crc_val;
        crc_val ^= (byte as u16) << 8;
        for _j in 0..8 {
            if (crc_val & 0x8000) != 0 {
                crc_val = (crc_val << 1) ^ 0x1021;
            } else {
                crc_val = crc_val << 1;
            }
        }
        return crc_val;
    }

     fn crc_add(crc_val: u16, bytes: &[u8], len: u8) -> u16 {
        let mut crc_val = crc_val;

        for i in 0..(len as usize) {
            crc_val = Instance::<T, U>::crc_add_byte(crc_val, bytes[i]);
        }
        crc_val
    }

    pub fn handle_rx_frame(&mut self, frame: CANFrame, timestamp_usec: u64) {
        let transfer_type = frame.extract_transfer_type();

        let destination_node_id = match transfer_type {
            TransferType::Broadcast => BROADCAST_NODE_ID,
            _ => CANFrame::dest_id_from_id(frame.get_id()),
        };

        // TODO: This function should maintain statistics of transfer errors and such.

        if (frame.get_id() & CAN_FRAME_EFF) == 0
            || (frame.get_id() & CAN_FRAME_RTR) != 0
            || (frame.get_id() & CAN_FRAME_ERR) != 0
            || (frame.get_data_len() < 1)
        {
            return; // Unsupported frame, not UAVCAN - ignore
        }

        if transfer_type != TransferType::Broadcast
            && destination_node_id != self.get_local_node_id()
        {
            return; // Address mismatch
        }

        let priority = CANFrame::priority_from_id(frame.get_id());
        let source_node_id = CANFrame::source_id_from_id(frame.get_id());
        let data_type_id = frame.extract_data_type();
        let transfer_descriptor = Instance::<T, U>::make_transfer_descriptor(
            data_type_id,
            transfer_type,
            source_node_id,
            destination_node_id,
        );

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
                }
                else {
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
                timestamp_usec,
                &data,
                frame.get_data_len() - 1,
                data_type_id,
                transfer_type,
                Instance::<T, U>::transfer_id_from_tail_byte(tail_byte),
                priority,
                source_node_id,
            );

            rx_state.prepare_for_next_transfer();
            drop(rx_state);
            self.handler.on_transfer_reception(&mut self.user_reference, &rx_transfer);
            return;
        }

        if Instance::<T, U>::toggle_bit(tail_byte) != rx_state.get_next_toggle() {
            return; // wrong toggle
        }

        if Instance::<T, U>::transfer_id_from_tail_byte(tail_byte) != rx_state.get_transfer_id() {
            return; // unexpected tid
        }

        if Instance::<T, U>::is_start_of_transfer(tail_byte)
            && !Instance::<T, U>::is_end_of_transfer(tail_byte)
        // Beginning of multi frame transfer
        {
            if frame.get_data_len() <= 3 {
                return; // Not enough data
            }

            // take off the crc and store the payload
            rx_state.set_timestamp_usec(timestamp_usec);
            rx_state.buffer_push_bytes(&frame.get_data()[2..]);
            rx_state.set_payload_crc(
                (frame.get_data()[0] as u16) | ((frame.get_data()[1] as u16) << 8),
            );
            let calc_crc = rx_state.get_calculated_crc();
            rx_state.set_calculated_crc(Instance::<T, U>::crc_add(
                calc_crc, 
                &frame.get_data()[2..],
                frame.get_data_len() - 3,
            ));
        } else if !Instance::<T, U>::is_start_of_transfer(tail_byte)
            && !Instance::<T, U>::is_end_of_transfer(tail_byte)
        // Middle of a multi-frame transfer
        {
            rx_state.buffer_push_bytes(&frame.get_data()[..]);
            let calc_crc = rx_state.get_calculated_crc();
            rx_state.set_calculated_crc(Instance::<T, U>::crc_add(
                calc_crc,
                &frame.get_data()[..],
                frame.get_data_len() - 1,
            ));
        } else
        // End of a multi-frame transfer
        {
            let frame_payload_size = (frame.get_data_len() - 1) as usize;

            // Copy the beginning of the frame into the head, point the tail pointer to the remainder
                rx_state
                    .buffer_push_bytes(&frame.get_data()[0..frame_payload_size]);

            // CRC validation
            let calc_crc = rx_state.get_calculated_crc();
            rx_state.set_calculated_crc(Instance::<T, U>::crc_add(
                    calc_crc,
                &frame.get_data()[..],
                frame.get_data_len() - 1,
            ));


            // Making sure the payload is released even if the application didn't bother with it
            let buffer = rx_state.release_state_payload();
            rx_state.prepare_for_next_transfer();

            if rx_state.get_calculated_crc() == rx_state.get_payload_crc() {
                drop(rx_state);
                let rx_transfer = RxTransfer::new(
                    timestamp_usec,
                    &buffer,
                    buffer.len() as u8,
                    data_type_id,
                    transfer_type,
                    Instance::<T, U>::transfer_id_from_tail_byte(tail_byte),
                    priority,
                    source_node_id,
                    );
                self.handler.on_transfer_reception(&mut self.user_reference, &rx_transfer);
            }
            else {
                drop(rx_state);
            }

            return;
        }

        match rx_state.get_next_toggle() {
            0 => rx_state.set_next_toggle(1),
            _ => rx_state.set_next_toggle(0),
        }
    }
}
