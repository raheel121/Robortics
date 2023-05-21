#include <stdio.h>
#include <webots/motor.h>
#include <webots/robot.h>
#include <webots/touch_sensor.h>
#include <webots/camera.h>
#include <webots/camera_recognition_object.h>

#define SPEED 4
#define TIME_STEP 64

int main() {
  WbDeviceTag bumper, camera;
  WbDeviceTag left_motor, right_motor;
  int movement_counter = 0;
  int i=0;

  wb_robot_init();

  /* get a handle the the bumper and activate it. */
  bumper = wb_robot_get_device("bumper");
  wb_touch_sensor_enable(bumper, TIME_STEP);
  camera = wb_robot_get_device("camera");
  wb_camera_enable(camera, TIME_STEP);
  wb_camera_recognition_enable(camera, TIME_STEP);

  left_motor = wb_robot_get_device("left wheel motor");
  right_motor = wb_robot_get_device("right wheel motor");
  wb_motor_set_position(left_motor, INFINITY);
  wb_motor_set_position(right_motor, INFINITY);
  wb_motor_set_velocity(left_motor,  SPEED);
  wb_motor_set_velocity(right_motor, SPEED);

  /* control loop */
  while (wb_robot_step(TIME_STEP) != -1) {
    /*
     * When the touch sensor has detected something we begin the avoidance
     * movement.
     */
   int num = wb_camera_recognition_get_number_of_objects(camera);
   const WbCameraRecognitionObject *objects = wb_camera_recognition_get_objects(camera);
   if (num == 0)
   {
      wb_motor_set_velocity(left_motor,  -SPEED/2);
      wb_motor_set_velocity(right_motor, SPEED);
   }
   else if ( objects[i].position_on_image[0] < 125)
   {
      wb_motor_set_velocity(left_motor,  -SPEED/2);
      wb_motor_set_velocity(right_motor, SPEED);
    }
   else if (objects[i].position[0] > 0.25)
   {
      if (wb_touch_sensor_get_value(bumper) > 0)
      movement_counter = 25;
      if (movement_counter == 0) {
      wb_motor_set_velocity(left_motor,  SPEED);
      wb_motor_set_velocity(right_motor, SPEED);
      printf("\n No obstacle \n");
      } 
      else if (movement_counter >= 17) {
      wb_motor_set_velocity(left_motor,  -SPEED);
      wb_motor_set_velocity(right_motor, -SPEED);
      movement_counter--;
      printf("\n Reverse, movement_counter = %d\n", movement_counter);
      } 
      else 
      {
      wb_motor_set_velocity(left_motor,  SPEED);
      wb_motor_set_velocity(right_motor, -SPEED/2);
      movement_counter--;
      printf("\n Turn Left, movement_counter = %d\n", movement_counter);
      }
    }
    else
    {
      wb_motor_set_velocity(left_motor,  0.0);
      wb_motor_set_velocity(right_motor, 0.0);
    }
  }

  wb_robot_cleanup();

  return 0;
}