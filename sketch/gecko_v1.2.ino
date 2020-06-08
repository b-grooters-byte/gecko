/*
 *  Gecko v1.2 Arduino Sketch
 *  Copyright 2020, ByteTrail, LLC 
 *  
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *  
 */
 
//-----------------------------------------------------------------------------
// Pin IO definitions
#define MOTOR_A_PWM  5    // 9110 left motor speed
#define MOTOR_A_DIR  3    // 9110 left motor direction
#define MOTOR_B_PWM  6    // 9110 right motor speed
#define MOTOR_B_DIR  4    // 9110 right motor direction

#define MOTOR_LEFT     0  // pin offsets for left motor
#define MOTOR_RIGHT    1  // pin offsets for right motor
#define PWM_OFFSET     5  // pin offset for motor PWM pin
#define DIR_OFFSET     3  // pin offset for motor direction pin

//-----------------------------------------------------------------------------
// Gecko controller definitions
#define SPEED_MAX           0xFF
#define DIR_FORWARD         HIGH
#define DIR_REVERSE         LOW

//-----------------------------------------------------------------------------
// Command bit masks
#define INPUT_MASK_MOTOR    0x80
#define INPUT_MASK_DIR      0x40
#define INPUT_MASK_SPEED    0x3F

//-----------------------------------------------------------------------------
// function declarations
static void motor_command(uint8_t motor_id, uint8_t speed, uint8_t direction);


uint8_t input = 0;
uint8_t speed = 0;
uint8_t direction = 0;
uint8_t motor_id = 0;

void setup() {
  // left motor
  pinMode( MOTOR_A_DIR, OUTPUT );
  pinMode( MOTOR_A_PWM, OUTPUT );
  digitalWrite( MOTOR_A_DIR, LOW );
  digitalWrite( MOTOR_A_PWM, LOW );
  // right motor
  pinMode( MOTOR_B_DIR, OUTPUT );
  pinMode( MOTOR_B_PWM, OUTPUT );
  digitalWrite( MOTOR_B_DIR, LOW );
  digitalWrite( MOTOR_B_PWM, LOW );
  
  Serial.begin( 9600 );
}

/*!
 *  \brief primary loop
 *  
 *  The Gecko main loop will wait for data to be available on the serial
 *  connection. Each byte read is treated as a motor command. No error 
 *  checking is completed prior to attempting to command the motor.
 */
void loop() {
    while( !Serial.available() ) ; // wait for input
    input = Serial.read();
    motor_id = (input & INPUT_MASK_MOTOR) == 0 ? MOTOR_LEFT : MOTOR_RIGHT;    
    direction = (input & INPUT_MASK_DIR) == 0 ? DIR_FORWARD : DIR_REVERSE;
    speed = (input & INPUT_MASK_SPEED) << 2;
    motor_command(motor_id, speed, direction);
}

/*!
 * \brief Sends a command to a motor
 * 
 *  This function sends a speed and direction command to the motor controller. 
 *  4 motors are connected to the L9110S so either the 2 LEFT or 2 RIGHT motors 
 *  are receiving the command.
 *  
 * \param motor_id The motor identifier: MOTOR_LEFT or MOTOR_RIGHT 
 * \param speed The speed to set. This is a value between 0 and 255 although in practice
 *              a range of ~=50 - ~= 200 is applicable to the N20 motors used
 * \param direction DIR_FORWARD or DIR_REVERSE
 */
static void motor_command(uint8_t motor_id, uint8_t speed, uint8_t direction) {
  // simple error check on motor ID
  if( motor_id > 1 ) {
    return;
  }
  // set the motor speed and direction
  if ( direction == DIR_FORWARD ) {
    speed = SPEED_MAX - speed;
  }
  digitalWrite( motor_id + DIR_OFFSET, direction );       
  analogWrite( motor_id + PWM_OFFSET, speed );   
}


 
