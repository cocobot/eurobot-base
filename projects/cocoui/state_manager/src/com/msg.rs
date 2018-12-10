use super::canars::RequestResponse;

#[derive(Debug, Copy, Clone)]
pub enum Msg {
    GetNodeInfo {node_id: u8}, 
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
            _ => { 
                println!("UKN pkt: {:?}", &self);
            },
        };
    }
}
