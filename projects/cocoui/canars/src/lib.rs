#[macro_use]
extern crate log;

use std::cell::RefCell;
use std::cell::RefMut;
use std::cmp::Ordering;
use std::collections::VecDeque;
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
pub const TRANSFER_PRIORITY_HIGHEST: u8 = 0;
pub const TRANSFER_PRIORITY_HIGH: u8 = 8;
pub const TRANSFER_PRIORITY_MEDIUM: u8 = 16;
pub const TRANSFER_PRIORITY_LOW: u8 = 24;
pub const TRANSFER_PRIORITY_LOWEST: u8 = 31;

#[derive(PartialEq, Copy, Clone)]
pub enum RequestResponse {
    Response,
    Request,
}

pub struct RxTransfer<'a> {
    payload: &'a Vec<u8>,
    payload_len: u8,
    data_type_id: u16,
    source_node_id: u8,
}

impl<'a> RxTransfer<'a> {
    fn new(
        payload: &'a Vec<u8>,
        payload_len: u8,
        data_type_id: u16,
        source_node_id: u8,
    ) -> RxTransfer {
        RxTransfer {
            payload,
            payload_len,
            data_type_id,
            source_node_id,
        }
    }

    pub fn get_data_type_id(&self) -> u16 {
        self.data_type_id
    }

    pub fn decode_scalar_u8(&self, offset: &mut usize, size: u8) -> Option<u8> {
        //println!("START DECODE -> offset {}/{}", offset, size);
        let mut storage = 0;
        let size = size as usize;
        let mut bit = 0;
        for i in *offset..(*offset + size) {
            storage <<= 1;
            let data = match self.payload.get(i / 8) {
                Some(s) => *s,
                None => return None,
            };
            //println!("   data {}/{}/{}/{}", data, storage, i, bit);
            if data & ((1 as u8) << (7 - (i % 8))) != 0 {
                storage |= 1;
            }
            bit += 1;
            assert!(bit <= 8);
        }
        *offset += size;
        //println!("   final {}/{}/{}", storage, offset, *offset /8);
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

    pub fn decode_scalar_i16(&self, _offset: &mut usize, _size: u8) -> Option<i16> {
        unimplemented!();
    }

    pub fn decode_scalar_i32(&self, _offset: &mut usize, _size: u8) -> Option<i32> {
        unimplemented!();
    }

    pub fn decode_scalar_i64(&self, _offset: &mut usize, _size: u8) -> Option<i64> {
        unimplemented!();
    }

    pub fn decode_scalar_u64(&self, offset: &mut usize, size: u8) -> Option<u64> {
        let mut r = 0;
        let mut i = 0;
        let mut size = size;
        while size > 0 {
            let mut to_read = size;
            if to_read > 8 {
                to_read = 8;
            }
            let p1 = match self.decode_scalar_u8(offset, to_read) {
                Some(s) => s as u64,
                None => return None,
            };
            r |= p1 << i;
            size -= to_read;
            i += 8;
        }
        Some(r)
    }

    pub fn decode_scalar_bool(&self, offset: &mut usize, size: u8) -> Option<bool> {
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
        Some(r > 0)

    }

    pub fn decode_scalar_f32(&self, offset: &mut usize, size: u8) -> Option<f32> {
        if let Some(f_u32) = self.decode_scalar_u32(offset, size) {
            Some(f32::from_bits(f_u32))
        }
        else {
            None
        }
    }

    pub fn get_payload_len(&self) -> u8 {
        self.payload_len
    }

    pub fn get_source_node_id(&self) -> u8 {
        self.source_node_id
    }
}

pub fn encode_scalar_u8(buffer: &mut Vec<u8>, offset: usize, size: usize, value: u8) {
    for i in 0..size {
        let idx = (i + offset) / 8;
        while buffer.len() <= idx {
            buffer.push(0);
        }
        if (value & 1 << i) != 0 {
            buffer[idx] |= 1 << ((i + offset) % 8);
        }
    }
}

pub fn encode_scalar_u16(buffer: &mut Vec<u8>, offset: usize, size: usize, value: u16) {
    let mut size = size;
    let mut value = value;
    let mut offset = offset;
    while size > 0 {
        let mut to_write = size;
        if to_write > 8 {
            to_write = 8;
        }
        encode_scalar_u8(buffer, offset, to_write, (value & 0xFF) as u8);
        size -= to_write;
        value >>= to_write;
        offset += to_write;
    }
}

pub fn encode_scalar_u32(_buffer: &mut Vec<u8>, _offset: usize, _size: usize, _value: u32) {
    unimplemented!();
}

pub fn encode_scalar_f32(_buffer: &mut Vec<u8>, _offset: usize, _size: usize, _value: f32) {
    unimplemented!();
}

pub fn encode_scalar_i16(buffer: &mut Vec<u8>, offset: usize, size: usize, value: i16) {
    let mut uvalue = 0 as u16;
    uvalue |= (value & 0xFF) as u16;
    uvalue |= (((value >> 8) & 0xFF) as u16) << 8;
    encode_scalar_u16(buffer, offset, size, uvalue);
}

pub fn encode_scalar_i32(_buffer: &mut Vec<u8>, _offset: usize, _size: usize, _value: i32) {
    unimplemented!();
}

pub fn encode_scalar_i64(buffer: &mut Vec<u8>, offset: usize, size: usize, value: i64) {
    encode_scalar_u64(buffer, offset, size, value as u64);
}

pub fn encode_scalar_u64(buffer: &mut Vec<u8>, offset: usize, size: usize, value: u64) {
    let mut size = size;
    let mut value = value;
    let mut offset = offset;
    while size > 0 {
        let mut to_write = size;
        if to_write > 8 {
            to_write = 8;
        }
        encode_scalar_u8(buffer, offset, to_write, (value & 0xFF) as u8);
        offset += to_write;
        size -= to_write;
        value >>= to_write;
    }
}

pub fn encode_scalar_bool(buffer: &mut Vec<u8>, offset: usize, size: usize, value: bool) {
    if value {
        encode_scalar_u8(buffer, offset, size, 1);
    }
    else {
        encode_scalar_u8(buffer, offset, size, 0);
    }
}

pub trait Node<U> {
    fn on_transfer_reception(&self, _: &mut U, _: &RxTransfer);
    fn should_accept_transfer(&self, _: &U, _: &mut u64, _: u16, _: TransferType, _: u8) -> bool;
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

#[derive(Clone, Debug, Eq)]
pub struct CANFrame {
    id: u32,
    data: [u8; CANFrame::CAN_FRAME_MAX_DATA_LEN],
    data_len: u8,
}

impl CANFrame {
    pub const CAN_FRAME_MAX_DATA_LEN: usize = 8;

    pub fn new(id: u32, data: [u8; CANFrame::CAN_FRAME_MAX_DATA_LEN], data_len: u8) -> CANFrame {
        CANFrame { id, data, data_len }
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

    pub fn get_id(&self) -> u32 {
        self.id
    }

    pub fn get_data_len(&self) -> u8 {
        self.data_len
    }

    pub fn get_data(&self) -> &[u8; CANFrame::CAN_FRAME_MAX_DATA_LEN] {
        &self.data
    }
}

impl Ord for CANFrame {
    fn cmp(&self, other: &CANFrame) -> Ordering {
        self.id.cmp(&other.get_id())
    }
}

impl PartialOrd for CANFrame {
    fn partial_cmp(&self, other: &CANFrame) -> Option<Ordering> {
        Some(self.cmp(other))
    }
}

impl PartialEq for CANFrame {
    fn eq(&self, other: &CANFrame) -> bool {
        self.id == other.get_id()
    }
}

#[derive(Debug)]
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
        let (tid, _) = self.transfer_id.overflowing_add(1);
        if tid < 32 {
          self.transfer_id = tid;
        }
        else {
          self.transfer_id = 0;
        }
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
}

pub struct Instance<T: Node<U>, U> {
    node_id: u8,
    handler: T,
    user_reference: U,
    rx_states: Vec<RefCell<RxState>>,
    queue: VecDeque<CANFrame>,
}

impl<T: Node<U>, U> Instance<T, U> {
    pub fn init(handler: T, user_reference: U) -> Instance<T, U> {
        Instance {
            node_id: BROADCAST_NODE_ID,
            handler,
            user_reference,
            rx_states: Vec::new(),
            queue: VecDeque::new(),
        }
    }

    pub fn get_handler(&mut self) -> &mut T {
        &mut self.handler
    }

    pub fn pop_tx_queue(&mut self) -> Option<CANFrame> {
        self.queue.pop_front()
    }

    pub fn set_local_node_id(&mut self, node_id: u8) {
        if (self.node_id == BROADCAST_NODE_ID)
            && (node_id >= MIN_NODE_ID)
            && (node_id <= MAX_NODE_ID)
        {
            self.node_id = node_id;
        } else {
            error!("Bad node id: {:?}", self.node_id);
            assert!(false);
        }
    }

    pub fn get_local_node_id(&self) -> u8 {
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
        let state = self
            .rx_states
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

    fn crc_add(crc_val: u16, bytes: &[u8], len: usize) -> u16 {
        let mut crc_val = crc_val;

        for i in 0..len {
            crc_val = Instance::<T, U>::crc_add_byte(crc_val, bytes[i]);
        }
        crc_val
    }

    pub fn cleanup_stale_transfers(&mut self, timestamp_usec: u64) {
        let mut i = 0;
        loop {
            if i >= self.rx_states.len() {
                break;
            }

            let mut state = self.rx_states[i].borrow_mut();
            if timestamp_usec - state.get_timestamp_usec() > TRANSFER_TIMEOUT_USEC {
                state.release_state_payload();
                drop(state);
                self.rx_states.remove(i);
            } else {
                i += 1;
            }
        }
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
            warn!("Unsuported frame");
            return; // Unsupported frame, not UAVCAN - ignore
        }

        if transfer_type != TransferType::Broadcast
            && destination_node_id != self.get_local_node_id()
        {
            return; // Address mismatch
        }

        let source_node_id = CANFrame::source_id_from_id(frame.get_id());
        let data_type_id = frame.extract_data_type();
        let transfer_descriptor = Instance::<T, U>::make_transfer_descriptor(
            data_type_id,
            transfer_type,
            source_node_id,
            destination_node_id,
        );
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
