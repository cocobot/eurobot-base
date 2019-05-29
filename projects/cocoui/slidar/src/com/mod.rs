extern crate dsdl;

pub mod msg;
pub mod serial;

use crossbeam_channel::unbounded;
use crossbeam_channel::{Receiver, Sender};
use std::sync::Arc;
use std::sync::Mutex;
use std::thread;
use std::time;
use std::time::SystemTime;
use std::time::UNIX_EPOCH;
use std::time::Duration;

use canars::CANFrame;
use canars::Instance;
use canars::Node;
use canars::RxTransfer;
use canars::TransferType;
use crate::ComData;

const MAX_X_ROBOT : f32 = 600.0;
const MAX_Y_ROBOT : f32 = 400.0;

pub struct ComHandler {
    com: Option<Com>
}

impl ComHandler {
    fn new() -> ComHandler {
        ComHandler {
            com: None,
        }
    }
}

impl ComHandler {
    pub fn set_com(&mut self, com: Com) {
        self.com = Some(com);
    }
}

impl Node<ComData> for ComHandler {
    fn should_accept_transfer(
        &self,
        _state: &ComData,
        data_type_signature: &mut u64,
        data_type_id: u16,
        _transfer_type: TransferType,
        _source_node_id: u8,
    ) -> bool {
        //Indicate canars if trame is useful or not
        if dsdl::uavcan::cocobot::GameState::check_id(data_type_id) {
            dsdl::uavcan::cocobot::GameState::set_signature(data_type_signature);
            true
        }
        else if dsdl::uavcan::cocobot::CollisionRequest::check_id(data_type_id) {
            dsdl::uavcan::cocobot::CollisionRequest::set_signature(data_type_signature);
            true
        }
 
        else {
            false
        }
    }

    fn on_transfer_reception(&self, state: &mut ComData, xfer: &RxTransfer) {
        //Route packet depeding on id
        if dsdl::uavcan::cocobot::GameState::check_id(xfer.get_data_type_id()) {
            let gstate = dsdl::uavcan::cocobot::GameState::decode(xfer).unwrap();
            if state.start.elapsed().as_secs() < 15 {
                state.score.set_battery((gstate.battery as f32) / 1000.0);
            }
            else {
                state.score.set_score(gstate.score as usize);
            }
            debug!("{:?}", gstate);
        } 
        else if dsdl::uavcan::cocobot::CollisionRequest::check_id(xfer.get_data_type_id()) {
            let _col = dsdl::uavcan::cocobot::CollisionRequest::decode(xfer).unwrap();
            
            let mut alert_front_left = false;
            let mut alert_front_right = false;
            let mut alert_back_left = false;
            let mut alert_back_right = false;

            let lidar = state.xv11.lock().unwrap();

            for i in 0..360 {
                if let Some(distance) = lidar.get_angle(i) {
                    let distance = distance as f32;
                    let x_robot = distance * f32::cos((i as f32) * std::f32::consts::PI / 180.0);
                    let y_robot = distance * f32::sin((i as f32) * std::f32::consts::PI / 180.0);

                    let mut x_pos_alert = false;
                    let mut y_pos_alert = false;
                    let mut x_neg_alert = false;
                    let mut y_neg_alert = false;

                    if x_robot >= 0.0 && x_robot < MAX_X_ROBOT {
                        x_pos_alert = true;
                    }

                    if x_robot <= 0.0 && x_robot > -MAX_X_ROBOT {
                        x_neg_alert = true;
                    }

                    if y_robot >= 0.0 && y_robot < MAX_Y_ROBOT {
                        y_pos_alert = true;
                    }

                    if y_robot <= 0.0 && y_robot > -MAX_Y_ROBOT {
                        y_neg_alert = true;
                    }

                    if x_pos_alert && y_pos_alert {
                        alert_front_left = true;
                    }

                    if x_pos_alert && y_neg_alert {
                        alert_front_right = true;
                    }

                    if x_neg_alert && y_pos_alert {
                        alert_back_left = true;
                    }

                    if x_neg_alert && y_neg_alert {
                        alert_back_right = true;
                    }
                }
            }

            if let Some(com) = &self.com {
                com.message( msg::Msg::CollisionResponse { 
                    node_id: xfer.get_source_node_id(), 
                    alert_back_left,
                    alert_back_right,
                    alert_front_left,
                    alert_front_right,
                });
            }
        } else {
            error!(
                "Xfer accepted but not implemented: {:?}",
                xfer.get_data_type_id()
            );
        }
    }
}

#[derive(Clone)]
pub struct Com {
    receive_can_frame: Sender<(u64, CANFrame)>,
    send_can_frame: Sender<CANFrame>,
    send_msg: Sender<msg::Msg>,
}

impl Com {
    fn new(
        receive_can_frame: Sender<(u64, CANFrame)>,
        send_can_frame: Sender<CANFrame>,
        send_msg: Sender<msg::Msg>,
    ) -> Com {
        //Allocation for com structure
        Com {
            receive_can_frame,
            send_can_frame,
            send_msg,
        }
    }

    pub fn send_can_frame(&self, frame: CANFrame) {
        match self.send_can_frame.send(frame) {
            Ok(_) => {}
            Err(e) => {
                error!("send_can_frame: {:?}", e);
            }
        }
    }

    pub fn message(&self, msg: msg::Msg) {
        match self.send_msg.send(msg) {
            Ok(_) => {}
            Err(e) => {
                error!("send_msg: {:?}", e);
            }
        }
    }

    pub fn handle_rx_frame(&self, frame: CANFrame) {
        let timestamp = SystemTime::now().duration_since(UNIX_EPOCH).unwrap();
        let timestamp = timestamp.as_secs() * 1_000 + timestamp.subsec_nanos() as u64 / 1_000_000;

        match self.receive_can_frame.send((timestamp, frame)) {
            Ok(_) => {}
            Err(e) => {
                error!("receive_can_frame: {:?}", e);
            }
        }
    }
}

pub fn init(node_id: u8, data: ComData) -> (Com, Receiver<CANFrame>) {
    //Canars node initialisation
    let mut node = Instance::init(ComHandler::new(), data);
    node.set_local_node_id(node_id);

    //protect node with mutex
    let node = Arc::new(Mutex::new(node));

    //create channels for communication
    let (tx_send_can_frame, rx_send_can_frame) = unbounded();
    let (tx_receive_can_frame, rx_receive_can_frame) = unbounded();
    let (tx_send_msg, rx_send_msg) = unbounded();

    //Com initialisation
    let com = Com::new(tx_receive_can_frame, tx_send_can_frame, tx_send_msg);

    //handle cleanup and send messages
    let node_th1 = node.clone();
    thread::spawn(move || {
        //Helper for computing 64bits timestamp
        fn get_timestamp() -> u64 {
            let timestamp = SystemTime::now().duration_since(UNIX_EPOCH).unwrap();
            let timestamp =
                timestamp.as_secs() * 1_000 + timestamp.subsec_nanos() as u64 / 1_000_000;
            timestamp
        }

        let mut next_clean_up = get_timestamp();
        loop {
            let timestamp = get_timestamp();
            
            //compute max waiting time before next cleanup
            let delta = if next_clean_up > timestamp {
                next_clean_up - timestamp
            } else {
                0
            };
            
            //receive new message to send
            if let Ok(msg) = rx_send_msg.recv_timeout(Duration::from_millis(delta)) {
                let mut node = node_th1.lock().unwrap();
                msg.send(&mut node);
            }

            //cleanup transfert
            if timestamp > next_clean_up {
                next_clean_up = get_timestamp() + 1_000;
                let mut node = node_th1.lock().unwrap();
                node.cleanup_stale_transfers(timestamp);
            }
        }
    });

    //handle received messages
    let node_th2 = node.clone();
    thread::spawn(move || {
        loop {
            //receive new frame to parse
            if let Ok((timestamp, frame)) = rx_receive_can_frame.recv() {
                let mut node = node_th2.lock().unwrap();
                node.handle_rx_frame(frame, timestamp)
            }
        }
    });

    //transfert can packet from node to simulator or serial
    let com_th3 = com.clone();
    let node_th3 = node.clone();
    thread::spawn(move || loop {
        let mut node = node_th3.lock().unwrap();
        while let Some(frame) = node.pop_tx_queue() {
            com_th3.send_can_frame(frame);
        }
        drop(node);

        thread::sleep(time::Duration::from_millis(20));
    });

    node.lock().unwrap().get_handler().set_com(com.clone());

    (com, rx_send_can_frame)
}

