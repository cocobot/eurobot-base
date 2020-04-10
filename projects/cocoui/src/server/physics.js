const electron = require('electron');
const Matter = require('matter-js');
const BrowserWindow = electron.BrowserWindow;
const Engine = Matter.Engine;
const World = Matter.World;
const Body = Matter.Body;
const Vector = Matter.Vector;

const DEBUG_DO_NOT_SIMULATE_ROBOT_POSITION = false;  ///< Use brain data for robot position

const ENCODER_SPACING_m = 0.230;      ///< distance between the two encoders
const WHEEL_SPACING_m = 0.187;        ///< distance between the two wheels
const WHEELS_RADIUS_m = 0.0603 / 2.0; ///< radius of a wheel
const MAX_RPM = 400;                  ///< Max speed allowed by canon board
const ENGINE_SIMULTION_STEP_ms = 5;   ///< Delta between two simulation step
const UI_REFRESH_RATE_ms = 100;       ///< Delta between two simulation step


class Physics {
  /**
   * @brief Create new Physics engine
   * @param protocol  Instance of the protocol manager
   */
  constructor(protocol) {
    //assign private members
    this._protocol = protocol;
    this._robots = {};
    this._config = {};

    //set physics engine in protocol class
    this._protocol.setPhysics(this);

    //create world
    this._engine = Engine.create();
    this._engine.world.gravity.y = 0;

    //start simulation
    this._simulationTimer = setInterval(() => this._simulationStep(), ENGINE_SIMULTION_STEP_ms);
    this._viewerTimer = setInterval(() => this._sendDataToUI(), UI_REFRESH_RATE_ms);

    this.resetSimulation();
  }

  /**
   * @brief Reset the simulator
   */
  resetSimulation() {
    //generate color configuration
    this._config.color = Math.floor(Math.random() * 2);
  }

  /**
   * @brief Generate robot default value
   * @param name  Robot name (principal or secondaire)
   */
  _initializeRobot(name) {
    //Remove old robot from world if needed
    if(this._robots[name] !== undefined) {
      if(this._robots[name].body !== null) {
        World.remove(this._engine.world, this._robots[name].body);
      }
    }

    this._robots[name] = {};

    //no simulation if not requested
    this._robots[name].simulation = false;

    //set initial position and shape
    let position = null;
    let shape = [];
    let motorscale = {};
    switch(name) {
      case "principal":
        position = {x: 0, y: 0, a: 0};
        shape = [{x:0.090, y:-0.150}, {x:0.090, y:0.150}, {x:-0.090, y:0.150}, {x:-0.090, y:-0.150}];
        motorscale = {left: 1, right: -1};
        break;

      case "secondaire":
        position = {x: 1.190, y: 0.2, a: Math.PI};
        shape = [{x:0.090, y:-0.150}, {x:0.090, y:0.150}, {x:-0.090, y:0.150}, {x:-0.090, y:-0.150}];
        motorscale = {left: 1, right: -1};
        break;
    }
    this._robots[name].position = position;
    this._robots[name].shape = shape;
    this._robots[name].motorscale = motorscale;

    //handle color offset
    if(this._config.color == 0) {
      this._robots[name].position.x = -position.x;
      this._robots[name].position.a = position.a - Math.PI;
    }

    //set simulation data as 0
    this._robots[name].motors = {left: 0, right: 0};
    this._robots[name].encoders = {left: 0, right: 0};

    //set body as not initialized
    this._robots[name].body = null;
  }

  /**
   * @brief Receive new packet relative to the physics engine
   */
  newData(pkt) {
    switch(pkt.decoded._name) {
      case "position":
        //receive new position packet
        if((pkt.ith === undefined) || DEBUG_DO_NOT_SIMULATE_ROBOT_POSITION) {
          //first time ?
          if(this._robots[pkt.decoded._robot] === undefined) {
            this._initializeRobot(pkt.decoded._robot);
          }

          //no simulation
          this._robots[pkt.decoded._robot].position = {
            x: pkt.decoded.x / 1000.0,
            y: pkt.decoded.y / 1000.0,
            a: pkt.decoded.angle * Math.PI / 180.0,
          };

          //disable physics engine for this robot
          this._robots[pkt.decoded._robot].simulation = false;
        }
        break;

      case "set_motor":
        //receive new motor setpoints
        if((pkt.ith !== undefined) && !DEBUG_DO_NOT_SIMULATE_ROBOT_POSITION) {
          //first time ?
          if(this._robots[pkt.decoded._robot] === undefined) {
            this._initializeRobot(pkt.decoded._robot);
          }

          const clamp = (v, max) => {
            if(v >= max) {
              return max;
            }
            if(v <= -max) {
               return -max;
            }
            return v;
          };
          
          //simulation mode
          this._robots[pkt.decoded._robot].motors = {
            left: clamp(pkt.decoded.left, MAX_RPM) * this._robots[pkt.decoded._robot].motorscale.left,
            right: clamp(pkt.decoded.right, MAX_RPM) * this._robots[pkt.decoded._robot].motorscale.right,
          };

          //activate physics engine for this robot
          this._robots[pkt.decoded._robot].simulation = true;
        }
        break;

      case "gs_req_cfg":
        {
          if(this._robots[pkt.decoded._robot] === undefined) {
            //not ready. Do nothing
            break;
          }

          if(this._robots[pkt.decoded._robot].simulation === true) {
            this._initializeRobot(pkt.decoded._robot);

            //in simulation mode, we need to emulate the configuration feedback from com
            this._protocol.formatAndSendtoAll({
              pid: 0x3001,
              fmt: "B",
              args: [this._config.color],
            });
          }
        }
    }
  }

  /**
   * @brief Perform a new step for the game simulation
   */
  _simulationStep() {
    //set velocities
    for(let name in  this._robots) {
      const robot = this._robots[name];
      if(robot.simulation) {
        //this robot has the simulator activated. First check if body is valid
        if(robot.body === null) {
          //body never created. Let's do it
          robot.body = Matter.Bodies.fromVertices(robot.position.x, robot.position.y, robot.shape);
          Body.setMass(robot.body, Infinity);
          Body.rotate(robot.body, robot.position.a - Math.PI / 2);
          World.add(this._engine.world, robot.body);
        }
        const speed_left = (robot.motors.left / 60.0 *  2.0 * Math.PI * WHEELS_RADIUS_m);
        const speed_right = (robot.motors.right / 60.0 *  2.0 * Math.PI * WHEELS_RADIUS_m);
        const speed_dist = (speed_left + speed_right) / 2 * ENGINE_SIMULTION_STEP_ms / 1000.0;
        const speed_angu = ((speed_right - speed_left) / WHEEL_SPACING_m) * (ENGINE_SIMULTION_STEP_ms / 1000.0);

        //add velocities
        Body.setAngularVelocity(robot.body, speed_angu);
        const vect_speed_dist = Vector.mult(robot.body.axes[0], speed_dist);
        Body.setVelocity(robot.body, vect_speed_dist);
      }
    }

    //run the simulation
    Engine.update(this._engine, ENGINE_SIMULTION_STEP_ms);

    //update positions
    for(let name in  this._robots) {
      const robot = this._robots[name];
      if(robot.simulation) {
        //compute distance modification
        const vector_delta_d = Vector.sub(robot.body.position, robot.position);
        const dep =  Math.sign(Vector.dot(vector_delta_d, robot.body.axes[0]));
        const delta_d = Vector.magnitude(vector_delta_d) * dep / 2;

        //compute angle modification
        let delta_a = robot.body.angle - (robot.position.a - Math.PI / 2);
        while(delta_a < 0) {
          delta_a += 2 * Math.PI;
        }
        while(delta_a > Math.PI) {
          delta_a -= 2 * Math.PI;
        }

        delta_a *= ENCODER_SPACING_m / 4;

        robot.encoders.left += delta_d - delta_a;
        robot.encoders.right += delta_d + delta_a;
        robot.position.x = robot.body.position.x;
        robot.position.y = robot.body.position.y;
        robot.position.a = robot.body.angle + Math.PI / 2;

        //send position to brain
        this._protocol.formatAndSendtoAll({
          pid: 0x9000,
          fmt: "BFF",
          args: [
            name == "principal" ? 0x0A : 0x02, 
            robot.encoders.left,
            robot.encoders.right,
          ],
        });
      }
    }
  }

  /**
   * @brief Send simulation results to the electron user interface
   */
  _sendDataToUI() {
    //send result to all windosw
    BrowserWindow.getAllWindows().forEach((x) => {
      x.webContents.send('physics', {
        robots: this._robots,
      });
    });
  }
}

//export class
module.exports = Physics;
