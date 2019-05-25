use crate::com::dsdl::uavcan::protocol::param::Empty;
use crate::com::dsdl::uavcan::protocol::param::Value;
use crate::com::ComHandler;
use canars::Instance;
use canars::RequestResponse;
use dsdl::uavcan::cocobot::CollisionResponse;
use crate::ComData;

#[derive(Debug, Clone)]
pub enum QValue {
    Empty,
    I64(i64),
    F32(f32),
    Bool(bool),
    String,
}

impl QValue {
    fn to_canard_value(&self) -> Value {
        match self {
            QValue::I64(i) => Value::IntegerValue(*i),
            QValue::Bool(b) => {
                if *b {
                    Value::BooleanValue(1)
                }
                else {
                    Value::BooleanValue(0)
                }
            }
            _ => {
                error!("Not implemented: {:?}", self);
                Value::Empty(Empty {})
            }
        }
    }
}

#[derive(Debug, Clone)]
pub enum Msg {
    CollisionResponse {
        node_id: u8,
        alert_back_left: bool,
        alert_back_right: bool,
        alert_front_left: bool,
        alert_front_right: bool,
    },
}

impl Msg {
    pub fn send(&self, node: &mut Instance<ComHandler, ComData>) {
        let mut transfer_id = 0;

        match &self {
            Msg::CollisionResponse { node_id, alert_back_left, alert_back_right, alert_front_left, alert_front_right } => {
                let alert_front_left = if *alert_front_left {
                    1
                }
                else {
                    0
                };
                let alert_front_right = if *alert_front_right {
                    1
                }
                else {
                    0
                };

                let alert_back_left = if *alert_back_left {
                    1
                }
                else {
                    0
                };
                let alert_back_right = if *alert_back_right {
                    1
                }
                else {
                    0
                };

                let (pkt, _) = CollisionResponse::encode( CollisionResponse {
                    left_back: alert_back_left,
                    left_front: alert_front_left,
                    right_back: alert_back_right,
                    right_front: alert_front_right,
                });
                node.request_or_respond(
                    *node_id,
                    CollisionResponse::SIGNATURE,
                    CollisionResponse::ID as u8,
                    &mut transfer_id,
                    canars::TRANSFER_PRIORITY_LOWEST,
                    RequestResponse::Response,
                    &pkt[..],
                );
            }
        };
    }
}

