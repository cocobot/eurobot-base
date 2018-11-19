extern crate tokio;
extern crate futures;

use std::thread;
use std::sync::mpsc::Sender;

use network::tokio::prelude::*;
use network::tokio::net::TcpListener;
use network::tokio::net::TcpStream;

use protocol::Protocol;
use robot::{Robot, RobotData};

fn process(socket: TcpStream, tx_rdata: Sender<RobotData>) {
    // Define the task that processes the connection.
    let protocol = Protocol::new(socket);
    let task = protocol.into_future()
        .map_err(|(e, _)| e)
        .and_then(|(name, protocol)| {
            println!("Robot ID {:?}", name);
            
            Robot::new(protocol, tx_rdata)
        })
        // Task futures have an error of type `()`, this ensures we handle
        // the error. We do this by printing the error to STDOUT.
        .map_err(|e| {
            println!("connection error = {:?}", e);
        });

    // Spawn the task. Internally, this submits the task to a thread pool.
    tokio::spawn(task);
}

pub fn init(tx_rdata: &Sender<RobotData>) {
    let tx_rdata = tx_rdata.clone();
    thread::spawn(move || {
        // Bind the server's socket.
        let addr = "127.0.0.1:10000".parse().unwrap();
        let listener = TcpListener::bind(&addr)
            .expect("unable to bind TCP listener");

        // Pull out a stream of sockets for incoming connections
        let server = listener.incoming()
            .map_err(|e| eprintln!("accept failed = {:?}", e))
            .for_each(move |sock| {
                println!("accepted socket; addr={:?}", sock.peer_addr().unwrap());
                process(sock, tx_rdata.clone());
                Ok(())
            });

        // Start the Tokio runtime
        tokio::run(server);
    });
}
