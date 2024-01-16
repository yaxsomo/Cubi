#include <stdio.h>
#include <terminal.h>
#include <wirish.h>
#include "dc.h"
#include "model.h"
#include "imu.h"
#include "helpers.h"

// Distance between the wheels (mm) // 119.2
// Measure for Cubi
//#define L   120.4 // The true measure is 119.2, unless the center of the wheels is not the correct point of measure
// Measure for pendublum
// #define L   98.0
#define L   80.0

// Radius of the wheels (mm)
// #define R    32.5 // Cubi
#define R    40.0 // Pendublum

// TODO, with calibration
model_struct model;

void model_init() {
    //model.l = DEFAULT_L;
    //model.r = DEFAULT_R;
    model.x = 0;
    model.y = 0;
    model.theta = 0;

    model.x_goal = 0;
    model.y_goal = 0;
    model.theta_goal = 0;

    model.acc = 0;
    model.speed_acc = 0;

    model.mode = MODE_SPEED_CONTROL;

    model.asserv_mode = GOTOXY;
}

model_struct * get_model() {
    return &model;
}

// In mm/s
TERMINAL_PARAMETER_FLOAT(dx, "DX", 0);
TERMINAL_PARAMETER_FLOAT(dy, "DY", 0);
// In deg/s
TERMINAL_PARAMETER_FLOAT(turn, "Turn", 0);

TERMINAL_PARAMETER_FLOAT(turnp, "Turn P", 200.0);
TERMINAL_PARAMETER_FLOAT(turni, "Turn I", 0.01);
TERMINAL_PARAMETER_FLOAT(turnacc, "Turn I acc", 15);

TERMINAL_PARAMETER_FLOAT(speedp, "Speed P", 1);
TERMINAL_PARAMETER_FLOAT(speedi, "Speed I", 0.01);
TERMINAL_PARAMETER_FLOAT(speedacc, "Speed I acc", 15);

TERMINAL_PARAMETER_FLOAT(maxturn, "Maximum rotation speed", 180);
TERMINAL_PARAMETER_FLOAT(maxspeed, "Maximum linear speed", 250);

TERMINAL_PARAMETER_FLOAT(xytol, "Distance tolerance in mm for the gotoxy function", 10);


int dx_old = 0;
int dy_old = 0;
int turn_old = 0;

TERMINAL_COMMAND(rm, "Reset model")
{
    model_reset();
}

void model_reset() {
    model.x = 0;
    model.y = 0;
    model.theta = 0;

    model.x_goal = 0;
    model.y_goal = 0;
    model.theta_goal = 0;

    model.acc = 0;
}

TERMINAL_COMMAND(gotoa, "Go to theta [deg]")
{
    if (argc == 1) {
        float theta = atoi(argv[0])*DEG2RAD;
        go_to_a(theta);
    }
}

TERMINAL_COMMAND(gotoxy, "Go to x y [mm]")
{
    if (argc == 2) {
        int x = atoi(argv[0]);
        int y = atoi(argv[1]);
        
        go_to_xy(x, y);
    }
}


TERMINAL_COMMAND(printodo, "Prints the current x(mm) y(mm) and theta (degs) of the robot since the last reset")
{
    terminal_io()->print(model.x);
    terminal_io()->print(", ");
    terminal_io()->print(model.y);
    terminal_io()->print(", ");
    terminal_io()->print(model.theta*RAD2DEG);
}

TERMINAL_COMMAND(ik, "Inverse Kinematics (linear speed[mm/s] and rotation speed[deg/s])")
{
    if (argc == 2) {
        int speed = atoi(argv[0]);
        int rotation = atoi(argv[1]);
        int left = 0;
        int right = 0;
        ik(speed, rotation, &left, &right);
        dc_motor * motors = get_motors();
        motors[0].speed_target = right;
        motors[1].speed_target = left;
    }
    
}

TERMINAL_COMMAND(mik, "Prints the current the linear speed[mm/s] and rotation speed[deg/s] based on enc measures")
{
    float linearSpeed = 0;
    float rotationSpeed = 0;
    measure_ik(&linearSpeed, &rotationSpeed);

    terminal_io()->print("linear = ");
    terminal_io()->print(linearSpeed);
    terminal_io()->print(", rotation = ");
    terminal_io()->println(rotationSpeed);
}

TERMINAL_COMMAND(deg2firmware, "converts deg/s to imp/SPEED_DT")
{
    if (argc == 1) {
        float input = atoi(argv[0])*DEG2RAD;
        terminal_io()->println(SI2firmware(input));
    }
}

TERMINAL_COMMAND(firmware2deg, "converts imp/SPEED_DT to deg/s")
{
    if (argc == 1) {
        float input = atoi(argv[0]);
        terminal_io()->println(firmware2SI(input*RAD2DEG));
    }
}

TERMINAL_COMMAND(printmodel, "Prints the current model info")
{
    terminal_io()->print("x ");
    terminal_io()->println(model.x);

    terminal_io()->print("y ");
    terminal_io()->println(model.y);

    terminal_io()->print("theta ");
    terminal_io()->println(model.theta);

    terminal_io()->print("x_goal ");
    terminal_io()->println(model.x_goal);

    terminal_io()->print("y_goal ");
    terminal_io()->println(model.y_goal);

    terminal_io()->print("theta_goal ");
    terminal_io()->println(model.theta_goal);

    terminal_io()->print("acc ");
    terminal_io()->println(model.acc);

    terminal_io()->print("mode ");
    terminal_io()->println(model.mode);
    
    terminal_io()->println();
}

TERMINAL_COMMAND(fakeodo, "Prints the odometry steps for a number of inputs")
{
    if (argc == 1) {
        int n = atoi(argv[0]);
        int max_speed = 300; // in imp/SPEED_DT, l=140 r=-140 is 360 deg/s on robots ik rotation
        int increment = max_speed/n;
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                fake_dx_dy(increment*i, increment*j);
                terminal_io()->println();
            }
        }
    } else {
        terminal_io()->println("give a number bro");
    }
}

TERMINAL_COMMAND(printsin, "Prints the sin function. The parameter is the number of values")
{
    if (argc == 1) {
        int n = atoi(argv[0]);
        float max_value = M_PI/2;
        float increment = max_value/n;
        for (int i = 0; i < n; i++) {
            terminal_io()->print(i*increment, 10);
            terminal_io()->print(" ");
            terminal_io()->println(sin(i*increment), 10);
        }
    } else {
        terminal_io()->println("give a number bro");
    }
    
}

void model_tick() {
    if (dx != dx_old || (dy != dy_old) || (turn_old != turn)) {
        // New ik command (y has no meaning)
        dx_old = dx;
        dy_old = dy;
        turn_old = turn;

        int left = 0;
        int right = 0;
        int api_factor = 8;
        ik(dx_old*api_factor, turn_old*api_factor, &left, &right);
        dc_motor * motors = get_motors();
        motors[0].speed_target = right;
        motors[1].speed_target = left;
    }

}

// Inputs in mm/s and deg/s, outputs in imp/SPEED_DT (firmware units)
void ik(float linearSpeed, float rotationSpeed, int * left, int * right) {
    *left = SI2firmware(((float)linearSpeed - (float)rotationSpeed*L*DEG2RAD/2)/R);
    *right = SI2firmware(((float)linearSpeed + (float)rotationSpeed*L*DEG2RAD/2)/R);
}

// Inputs in imp/SPEED_DT (firmware units for the rotation speed of the wheels)
// Outputs in mm/s and deg/s
void dk(int left, int right, float * linearSpeed, float * rotationSpeed) {
    *linearSpeed = firmware2SI((left + right)/2.0)*R; // ((A+B)/2) * R where A and B are in rad/s
    *rotationSpeed = firmware2SI(right - left)*(R/L)*RAD2DEG;
}

// Returns the current linearSpeed (mm/sec) and rotation speed (deg/s) 
void measure_ik(float * linearSpeed, float *rotationSpeed) {
    dc_motor * motors = get_motors();
    int right = motors[0].speed;
    int left = motors[1].speed;
    dk(left, right, linearSpeed, rotationSpeed);
}

/*
* Given the knowledge of linearSpeed (mm/s) and rotationSpeed (deg/s) and assuming they are constant
* during dt (s), estimates dx and dy.
*/
void dx_dy(float linearSpeed, float rotationSpeed, float dt, float * dx, float * dy, float * dtheta) {
    float l = linearSpeed*dt;

    if (rotationSpeed == 0) {
        // The robot moves in a straight line
        *dy = 0;
        *dx = l;
        *dtheta = 0;
    } else {
        // The robot moves on a portion of a circle whose radius is l/alpha
        float alpha = rotationSpeed*dt*DEG2RAD;
        //float radius = (l/alpha);
        //*dx = l*cos(alpha/2); // Approx
        *dx = l*sin(alpha)/alpha; // Exact
        //*dy = l*sin(alpha/2); // Approx
        *dy = l*(cos(alpha)-1)/alpha; // Exact
        *dtheta = alpha;
    }
}
/*
* This function is quite calculation heavy (~340us)
*/
void increment_odometry() {
    float linearSpeed;
    float rotationSpeed;
    measure_ik(&linearSpeed, &rotationSpeed);
    // Constant speed over a duration -> distance
    // Accurate timming
    float dt = (nbDc - prevNbDc)/float(INT_FREQ);
    prevNbDc = nbDc;
    //float dt = SERVO_DT/1000.0;
    float dx = 0;
    float dy = 0;
    float dtheta = 0;

    dx_dy(linearSpeed, rotationSpeed, dt, &dx, &dy, &dtheta);

    // Odometry calculations
    model_struct * model = get_model();
    model->x = model->x + dx*cos(model->theta) - dy*sin(model->theta);
    model->y = model->y + dx*sin(model->theta) + dy*cos(model->theta);
    model->theta = model->theta + dtheta;
}

/*
* Debug function to estimate the precision loss when calculating the entire odometry chain.
* left and right are the number of enc impulses during the usual speed buffer. 
* Output : left right linearSpeed rotationSpeed dx dy dtheta
*/
void fake_dx_dy(int left, int right) {
    terminal_io()->print(left, 10);
    terminal_io()->print(" ");
    terminal_io()->print(right, 10);

    float linearSpeed;
    float rotationSpeed;
    // Returns the current linearSpeed (mm/sec) and rotation speed (deg/s) 
    dk(left, right, &linearSpeed, &rotationSpeed);
    terminal_io()->print(" ");
    terminal_io()->print(linearSpeed, 10);
    terminal_io()->print(" ");
    terminal_io()->print(rotationSpeed, 10);

    // Constant speed over a duration -> distance
    float dt = SERVO_DT/1000.0;
    float dx = 0;
    float dy = 0;
    float dtheta = 0;

    dx_dy(linearSpeed, rotationSpeed, dt, &dx, &dy, &dtheta);
    terminal_io()->print(" ");
    terminal_io()->print(dx, 10);
    terminal_io()->print(" ");
    terminal_io()->print(dy, 10);
    terminal_io()->print(" ");
    terminal_io()->print(dtheta, 10);
}

void go_to_a(float theta) {
    model.theta_goal = theta;
    model.asserv_mode = GOTOA;        
}

void go_to_xy(float x, float y) {
    model.x_goal = x;
    model.y_goal = y;
    model.asserv_mode = GOTOXY;    
}

void ik_asserv_tick() {
    float distance = 0;
    if (model.asserv_mode == GOTOXY) {
        distance = sqrt((model.x_goal - model.x)*(model.x_goal - model.x) + (model.y_goal - model.y)*(model.y_goal - model.y));
        if (distance < xytol) {
            // Close enough
            dc_motor * motors = get_motors();
            motors[0].speed_target = 0;
            motors[1].speed_target = 0;
            return;
        }
        // The angle goal depends on the X Y goals
        float dy = model.y_goal - model.y;
        float dx = model.x_goal - model.x;
        if (~(dx == 0 && dy == 0)) {
            model.theta_goal = atan2(dy, model.x_goal - model.x);
        }
    }
    // TODO implement go backwards when (theta_goal - theta) > pi. And debug this.
    // Turn asserv
    // TODO XXX : this is not tested yet
    float err = angle_diff_deg(model.theta_goal, model.theta);
    model.acc += turni * err;
    _limit(&model.acc, turnacc);
    float p_contribution = turnp * err;
    
    if ((p_contribution < 0 && model.acc >= 0) || (p_contribution > 0 && model.acc <= 0)) {
        // Astuce !
        model.acc = 0;
    }
    float local_turn = p_contribution + model.acc;
    if (local_turn < -maxturn) local_turn = -maxturn;
    if (local_turn > maxturn) local_turn = maxturn;

    float local_speed = 0;
    if (model.asserv_mode != GOTOA) {
        // linear speed asserv
        model.speed_acc += speedi * distance;
        _limit(&model.acc, speedacc);
        p_contribution = speedp * distance;
        
        if ((p_contribution < 0 && model.speed_acc >= 0) || (p_contribution > 0 && model.speed_acc <= 0)) {
            // Astuce !
            model.speed_acc = 0;
        }
        local_speed = p_contribution + model.speed_acc;
        if (local_speed < -maxspeed) local_speed = -maxspeed;
        if (local_speed > maxspeed) local_speed = maxspeed;
    }
    int left = 0;
    int right = 0;
    ik(local_speed, local_turn, &left, &right);
    dc_motor * motors = get_motors();
    motors[0].speed_target = right;
    motors[1].speed_target = left;
}

// TODO is this optimizable with a custom mod ? Has to be like this : mod = (a, n) -> (a % n + n) % n
float angle_diff_deg(float a, float b) {
    float diff = a - b;
    while (diff < -180) {
        diff = diff + 360;
    }
    while (diff > 180) {
        diff = diff - 360;
    }
    return diff;
 }