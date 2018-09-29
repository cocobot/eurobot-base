extern crate tokio;
extern crate bytes;
extern crate colored;

use std::io;
use futures::sync::mpsc;

use robot::colored::*;
use robot::tokio::prelude::*;
use robot::bytes::Bytes;

use protocol::{Packet, Protocol};

type Rx = mpsc::UnboundedReceiver<Bytes>;

pub struct Robot {
    rx: Rx,
    protocol: Protocol,
}

impl Robot {
     pub fn new(protocol : Protocol) -> Robot {
         let (tx, rx) = mpsc::unbounded();
         Robot {
             rx,
             protocol,
         }
     }
}

impl Future for Robot {
    type Item = ();
    type Error = io::Error;

    fn poll(&mut self) -> Poll<(), io::Error> {
        const PACKET_PER_TICK: usize = 10;

        /*
        // Receive all packets
        for i in 0..PACKET_PER_TICK {
            // Polling an `UnboundedReceiver` cannot fail, so `unwrap` here is
            // safe.
            match self.rx.poll().unwrap() {
                Async::Ready(Some(v)) => {
                    self.protocol.buffer(&v);

                    // If this is the last iteration, the loop will break even
                    // though there could still be packet to read. Because we did
                    // not reach `Async::NotReady`, we have to notify ourselves
                    // in order to tell the executor to schedule the task again.
                    if i+1 == PACKET_PER_TICK {
                        task::current().notify();
                    }
                }
                _ => break,
            }
        }
        */

        /*
        // Flush the write buffer to the socket
        let _ = self.lines.poll_flush()?;
        */

        // Read new packets from the socket
        while let Async::Ready(packet) = self.protocol.poll()? {
            if let Some(packet) = packet {
                match packet {
                    Packet::Unknown{pid} => {
                        eprintln!("{} 0x{:X}", "Unknown PID received:".red(), pid);
                    }
                    _ => {
                        eprintln!("{} {:?}", "Unhandled received packet:".yellow(), packet);
                    }
                }
    
            } else {
                // EOF was reached. The remote client has disconnected. There is
                // nothing more to do.
                return Ok(Async::Ready(()));
            }
        }

        // As always, it is important to not just return `NotReady` without
        // ensuring an inner future also returned `NotReady`.
        //
        // We know we got a `NotReady` from either `self.rx` or `self.lines`, so
        // the contract is respected.
        Ok(Async::NotReady)
    }
}
