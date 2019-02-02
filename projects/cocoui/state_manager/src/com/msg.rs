use super::canars::RequestResponse;
use com::dsdl::uavcan::protocol::param::Value;
use com::dsdl::uavcan::protocol::param::Empty;

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
             QValue::I64(i) => {
                 Value::IntegerValue(*i)
             },
             _ => {
                 error!("Not implemented: {:?}", self);
                 Value::Empty(Empty {})
             },
         }
    }
}

#[derive(Debug, Clone)]
pub enum Msg {
    GetNodeInfo {node_id: u8}, 
    Set {node_id: u8, name: String, value: QValue},
    Restart {node_id: u8}, 
}

impl Msg {
    pub fn send(&self, com: &mut super::Com) {
        let node = match com.get_node() {
            Some(n) => n,
            None => { return; },
        };
        let mut node = node.lock().unwrap();

        let mut transfer_id = 0;

        match &self {
            Msg::GetNodeInfo {node_id} => {
                use com::dsdl::uavcan::protocol::GetNodeInfoRequest;
                let (pkt, _) = GetNodeInfoRequest::encode(GetNodeInfoRequest {});
                node.request_or_respond(*node_id, GetNodeInfoRequest::SIGNATURE, GetNodeInfoRequest::ID as u8, &mut transfer_id, canars::TRANSFER_PRIORITY_LOWEST, RequestResponse::Request, &pkt[..]); 
            },
            Msg::Set{node_id, name, value} => {
                use com::dsdl::uavcan::protocol::param::GetSetRequest;
                let (pkt, _) = GetSetRequest::encode(GetSetRequest {
                    index: 0,
                    name: name.as_bytes().to_vec(),
                    value: value.to_canard_value(),
                });
                node.request_or_respond(*node_id, GetSetRequest::SIGNATURE, GetSetRequest::ID as u8, &mut transfer_id, canars::TRANSFER_PRIORITY_LOWEST, RequestResponse::Request, &pkt[..]); 
            },
            Msg::Restart {node_id} => {
                use com::dsdl::uavcan::protocol::RestartNodeRequest;
                let (pkt, _) = RestartNodeRequest::encode(RestartNodeRequest {
                    magic_number: RestartNodeRequest::MAGIC_NUMBER,
                });
                node.request_or_respond(*node_id, RestartNodeRequest::SIGNATURE, RestartNodeRequest::ID as u8, &mut transfer_id, canars::TRANSFER_PRIORITY_LOWEST, RequestResponse::Request, &pkt[..]); 
            },
        };
    }
}
