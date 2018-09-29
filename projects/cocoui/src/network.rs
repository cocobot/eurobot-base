extern crate tokio;
extern crate futures;

use std::sync::{Arc, Mutex};

use network::tokio::prelude::*;
use network::tokio::net::TcpListener;
use network::tokio::net::TcpStream;
use futures::future::{self, Either};

use state::State;
use protocol::Protocol;
use robot::Robot;

fn process(socket: TcpStream, state: Arc<Mutex<State>>) {
    // Define the task that processes the connection.
    let protocol = Protocol::new(socket);
    let task = protocol.into_future()
        .map_err(|(e, _)| e)
        .and_then(|(name, protocol)| {
            println!("Robot ID {:?}", name);
            
            let robot = Robot::new(protocol);

            robot
        })
    // Task futures have an error of type `()`, this ensures we handle
    // the error. We do this by printing the error to STDOUT.
    .map_err(|e| {
        println!("connection error = {:?}", e);
    });

    // Spawn the task. Internally, this submits the task to a thread pool.
    tokio::spawn(task);
}

pub fn init(state: Arc<Mutex<State>>) {
    // Bind the server's socket.
    let addr = "127.0.0.1:10000".parse().unwrap();
    let listener = TcpListener::bind(&addr)
        .expect("unable to bind TCP listener");

    // Pull out a stream of sockets for incoming connections
    let server = listener.incoming()
        .map_err(|e| eprintln!("accept failed = {:?}", e))
        .for_each(move |sock| {
            println!("accepted socket; addr={:?}", sock.peer_addr().unwrap());
            process(sock, state.clone());
            Ok(())
        });

    // Start the Tokio runtime
    tokio::run(server);
}
