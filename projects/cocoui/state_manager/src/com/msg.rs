use crate::com::dsdl::uavcan::protocol::param::Empty;
use crate::com::dsdl::uavcan::protocol::param::Value;
use crate::com::ComHandler;
use crate::state::StateManagerInstance;
use canars::Instance;
use canars::RequestResponse;

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
            _ => {
                error!("Not implemented: {:?}", self);
                Value::Empty(Empty {})
            }
        }
    }
}

#[derive(Debug, Clone)]
pub enum Msg {
    GetNodeInfo {
        node_id: u8,
    },
    Set {
        node_id: u8,
        name: String,
        value: QValue,
    },
    Restart {
        node_id: u8,
    },
    Program {
        node_id: u8,
    },
    ReadResponse {
        node_id: u8,
        error: bool,
        data: Vec<u8>,
    },
    Servo {
        node_id: u8,
        mode: u8,
        servo_id: u8,
        value: i16,
    },
    Pump {
        node_id: u8,
        pump_id: u8,
        action: u8,
    },
    Meca {
        node_id: u8,
        req: u8,
        arm: u8,
        x: u8,
        y: u8,
        z: u8,
        d: u8,
        a: u8,
    },

}

impl Msg {
    pub fn send(&self, node: &mut Instance<ComHandler, StateManagerInstance>) {
        let mut transfer_id = 0;

        match &self {
            Msg::GetNodeInfo { node_id } => {
                use crate::com::dsdl::uavcan::protocol::GetNodeInfoRequest;
                let (pkt, _) = GetNodeInfoRequest::encode(GetNodeInfoRequest {});
                node.request_or_respond(
                    *node_id,
                    GetNodeInfoRequest::SIGNATURE,
                    GetNodeInfoRequest::ID as u8,
                    &mut transfer_id,
                    canars::TRANSFER_PRIORITY_LOWEST,
                    RequestResponse::Request,
                    &pkt[..],
                );
            }
            Msg::Set {
                node_id,
                name,
                value,
            } => {
                use crate::com::dsdl::uavcan::protocol::param::GetSetRequest;
                let (pkt, _) = GetSetRequest::encode(GetSetRequest {
                    index: 0,
                    name: name.as_bytes().to_vec(),
                    value: value.to_canard_value(),
                });
                node.request_or_respond(
                    *node_id,
                    GetSetRequest::SIGNATURE,
                    GetSetRequest::ID as u8,
                    &mut transfer_id,
                    canars::TRANSFER_PRIORITY_LOWEST,
                    RequestResponse::Request,
                    &pkt[..],
                );
            }
            Msg::Restart { node_id } => {
                use crate::com::dsdl::uavcan::protocol::RestartNodeRequest;
                let (pkt, _) = RestartNodeRequest::encode(RestartNodeRequest {
                    magic_number: RestartNodeRequest::MAGIC_NUMBER,
                });
                node.request_or_respond(
                    *node_id,
                    RestartNodeRequest::SIGNATURE,
                    RestartNodeRequest::ID as u8,
                    &mut transfer_id,
                    canars::TRANSFER_PRIORITY_LOWEST,
                    RequestResponse::Request,
                    &pkt[..],
                );
            }
            Msg::Program { node_id } => {
                use crate::com::dsdl::uavcan::protocol::file::BeginFirmwareUpdateRequest;
                use crate::com::dsdl::uavcan::protocol::file::Path;
                let (pkt, _) = BeginFirmwareUpdateRequest::encode(BeginFirmwareUpdateRequest {
                    source_node_id: node.get_local_node_id(),
                    image_file_remote_path: Path {
                        path: "hex".to_string().into_bytes(),
                    },
                });
                node.request_or_respond(
                    *node_id,
                    BeginFirmwareUpdateRequest::SIGNATURE,
                    BeginFirmwareUpdateRequest::ID as u8,
                    &mut transfer_id,
                    canars::TRANSFER_PRIORITY_LOWEST,
                    RequestResponse::Request,
                    &pkt[..],
                );
            }
            Msg::Servo { node_id, mode, servo_id, value } => {
                use crate::com::dsdl::uavcan::cocobot::ServoCmdRequest;
                let (pkt, _) = ServoCmdRequest::encode(ServoCmdRequest {
                    mode: *mode,
                    servo_id: *servo_id,
                    value: *value,
                });
                debug!("SendCMD {}, {} {} {}", node_id, mode, servo_id, value);
                node.request_or_respond(
                    *node_id,
                    ServoCmdRequest::SIGNATURE,
                    ServoCmdRequest::ID as u8,
                    &mut transfer_id,
                    canars::TRANSFER_PRIORITY_LOWEST,
                    RequestResponse::Request,
                    &pkt[..],
                );
            }
            Msg::Pump { node_id, pump_id, action } => {
                use crate::com::dsdl::uavcan::cocobot::PumpRequest;
                let (pkt, _) = PumpRequest::encode(PumpRequest {
                    pump_id: *pump_id,
                    action: *action,
                });
                node.request_or_respond(
                    *node_id,
                    PumpRequest::SIGNATURE,
                    PumpRequest::ID as u8,
                    &mut transfer_id,
                    canars::TRANSFER_PRIORITY_LOWEST,
                    RequestResponse::Request,
                    &pkt[..],
                );
            }
            Msg::Meca { node_id, req, arm, x, y, z, d, a } => {
                use crate::com::dsdl::uavcan::cocobot::MecaActionRequest;
                let (pkt, _) = MecaActionRequest::encode(MecaActionRequest {
                    req: *req,
                    arm: *arm,
                    x: *x,
                    y: *y,
                    z: *z,
                    d: *d,
                    a: *a,
                });
                node.request_or_respond(
                    *node_id,
                    MecaActionRequest::SIGNATURE,
                    MecaActionRequest::ID as u8,
                    &mut transfer_id,
                    canars::TRANSFER_PRIORITY_LOWEST,
                    RequestResponse::Request,
                    &pkt[..],
                );
            }

            Msg::ReadResponse {
                node_id,
                error,
                data,
            } => {
                use crate::com::dsdl::uavcan::protocol::file::Error;
                use crate::com::dsdl::uavcan::protocol::file::ReadResponse;

                let (pkt, _) = ReadResponse::encode(ReadResponse {
                    error: Error {
                        value: match error {
                            true => Error::IO_ERROR,
                            false => Error::OK,
                        },
                    },
                    data: data.clone(),
                });
                node.request_or_respond(
                    *node_id,
                    ReadResponse::SIGNATURE,
                    ReadResponse::ID as u8,
                    &mut transfer_id,
                    canars::TRANSFER_PRIORITY_LOWEST,
                    RequestResponse::Response,
                    &pkt[..],
                );
            }
        };
    }
}
