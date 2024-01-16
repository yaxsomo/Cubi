import sys
import motor
from math import *

# The true measure is 119.2, unless the center of the wheels is not the correct point of measure
L = 120.4 
# Radius of the wheels (mm)
R = 32.5
MODE_PWM_CONTROL = 0
MODE_SPEED_CONTROL = 1
MODE_IK_CONTROL = 2
GOTOA = 0
GOTOXY = 1

class Model(object):
    """
    Represents the robot's state 
    """
    def __init__(self):
        #Distance between the wheels
        self.l = L;
        #Wheel radius
        self.r = R;

        self.x = 0;
        self.y = 0;
        self.theta = 0;

        self.x_goal = 0;
        self.y_goal = 0;
        self.theta_goal = 0;

        self.m1 = motor.Motor()
        self.m2 = motor.Motor()

        self.acc = 0;
        self.speed_acc = 0;
        self.mode = 1;
        self.asserv_mode = GOTOXY;

    def __repr__(self):
        s = "current : {} {} {}".format(self.x, self.y, self.theta)
        s = s + "\ngoal    : {} {} {}".format(self.x_goal, self.y_goal, self.theta_goal)
        s = s + "\nmotors    : {} {}".format(self.m1, self.m2)
        s = s + "acc={}, speed_acc={}, mode={}, asserv_mode){}".format(self.acc, self.speed_acc, self.mode, self.asserv_mode)
        return s
    
    def update(self, dt):
        # Going from wheel speeds to robot speed
        linear_speed = (self.m1.speed + self.m2.speed)/2.0
        rotation_speed = (self.m1.speed - self.m2.speed)/L

        l = dt*linear_speed
        # Updating dx, dy, dtethat
        if (rotation_speed == 0) :
            # The robot moves in a straight line
            dy = 0
            dx = l
            dtheta = 0
        else :
            # The robot moves on a portion of a circle whose radius is l/alpha
            alpha = rotation_speed*dt
            dx = l*sin(alpha)/alpha
            dy = l*(cos(alpha)-1)/alpha
            dtheta = alpha
        #print("dx = {}, dy = {}, dtethat = {}".format(dx, dy, dtheta))
        
        # Updating the robot position
        self.x = self.x + dx*cos(self.theta) - dy*sin(self.theta)
        self.y = self.y + dx*sin(self.theta) + dy*cos(self.theta)
        self.theta = (self.theta + dtheta) # No need to %(2*pi) 
        
   