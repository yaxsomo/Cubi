#pragma once

// Motor control modes 
#define MODE_PWM_CONTROL 0
#define MODE_SPEED_CONTROL 1
#define MODE_IK_CONTROL 2

#define GOTOA 0
#define GOTOXY 1

struct model_struct {
    /*
    // Distance between the wheels
    float l;
    //Wheel radius
    float r;*/
    float x;
    float y;
    float theta;

    float x_goal;
    float y_goal;
    float theta_goal;

    float acc;
    float speed_acc;
    int mode;

    int asserv_mode;        

};
void model_reset();

void model_init();

void model_tick();

model_struct * get_model();

// Inputs in mm/s and deg/s, outputs in imp/SPEED_DT (firmware units)
void ik(float linearSpeed, float rotationSpeed, int * left, int * right);

// Inputs in imp/SPEED_DT (firmware units for the rotation speed of the wheels)
// Outputs in mm/s and deg/s
void dk(int left, int right, float * linearSpeed, float * rotationSpeed);

// Returns the current linearSpeed (mm/sec) and rotation speed (deg/s) 
void measure_ik(float * linearSpeed, float *rotationSpeed);

/*
* Given the knowledge of linearSpeed (mm/s) and rotationSpeed (deg/s) and assuming they are constant
* during dt (s), estimates dx and dy.
*
*/
void dx_dy(float linearSpeed, float rotationSpeed, float dt, float * dx, float * dy, float * dtheta);

/*
* Incremets the local dx, dy, dtetha deduced from the encoders into the worl frame
*/ 
void increment_odometry();

/*
* Debug function to estimate the precision loss when calculating the entire odometry chain.
* left and right are the number of enc impulses during the usual speed buffer.
*/
void fake_dx_dy(int left, int right);

void ik_asserv_tick();

void go_to_a(float theta);

void go_to_xy(float x, float y);

float angle_diff_deg(float a, float b);