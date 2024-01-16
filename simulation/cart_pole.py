#!/usr/bin/env python
# -*- coding: utf-8 -*-

import random, math, pygame
import pygame.draw
from pygame.locals import *
import sys
import os
import copy
import motor
import model
from math import *


#constants (we should put these in another file)
INVISIBLE = (111,111,0)
WHITE = 255, 240, 200
BLACK = 20, 20, 40
gold = [212, 175, 55]

# Used
WINSIZE = [1500, 840]
WINCENTER = [WINSIZE[0]/2, WINSIZE[1]/2]
cart_color = [200, 0, 0]
cart_width = 0.2
cart_height = 0.1
mass_size = 0.05
mass_color = [0, 120, 0]
pole_color = [0, 0, 120]
u_color = [150, 0, 0]

LQR_ON = False
# mass at the end of the pole (kg)
mp = 0.23
# cart mass (kg)
mc = 2.4
# average g on earth
g = 9.81
# length of the pole (m)
lp = 0.5
# In the paper, they find theta_p=40 theta_d=8, x_p=1, x_d=3  
theta_p = 400
theta_d = -1
x_p = -10
x_d = 0
theta_0 = pi - pi/4
x_0 = 0.1

# u = -Kx where x = [theta, dtheta/dt, x, dx/dt]
K = [-137.7896, -25.9783, -22.3607, -27.5768]

''' Works
# mass at the end of the pole (kg)
mp = 0.23
# cart mass (kg)
mc = 2.4
# average g on earth
g = 9.81
# length of the pole (m)
lp = 1

# In the paper, they find theta_p=40 theta_d=8, x_p=1, x_d=3  
theta_p = 400
theta_d = -1
x_p = -10
x_d = 0
theta_0 = pi - pi/4
'''


''' Gota turn fast
theta_p = 40
theta_d = -1
x_p = 10
x_d = 0
'''
PERTURBATION = 50
PERTUBATION_DURATION = 0.2
nb_calculations_per_rotation = 100
freq = 60
dt = 1/float(freq*nb_calculations_per_rotation)

print("Calculation dt precision : {}".format(dt))
 
def draw_state(screen, theta, x, u):
    # Purely graphic
    meter2pixel = 150
    # Drawing the cart
    center_pos = [int(meter2pixel*x + WINCENTER[0]), int(WINCENTER[1])]
    pygame.draw.rect(screen, cart_color, (center_pos[0] - meter2pixel*cart_width/2, center_pos[1] - meter2pixel*cart_height/2, meter2pixel*cart_width, meter2pixel*cart_height), 5)

    # Drawing the mass and a line between them
    mass_pos = [int(center_pos[0] + meter2pixel*lp*sin(theta)), int(center_pos[1] + meter2pixel*lp*cos(theta))]
    pygame.draw.line(screen, pole_color, center_pos, mass_pos, 5)
    pygame.draw.circle(screen, mass_color, mass_pos, int(meter2pixel*mass_size), 0)

    # Drawing the current force applied to the cart
    width = u
    height = 30
    pygame.draw.rect(screen, u_color, (WINCENTER[0] - width, 1.8*WINCENTER[1] - height/2, width, height), 0)

    pygame.draw.line(screen, [150, 150, 0], (WINCENTER[0], 0), (WINCENTER[0], WINSIZE[1]), 1)
    pygame.draw.line(screen, [150, 150, 0], (0, WINCENTER[1]), (WINSIZE[0], WINCENTER[1]), 1)
    


def calculate_x_accel(u, theta, theta_speed) :
    result = u + mp*lp*sin(theta)*pow(theta_speed, 2) - mp*g*cos(theta)*sin(theta)
    result = result / (mc+mp-mp*pow(cos(theta),2))
    return result

def calculate_theta_accel(u, theta, theta_speed) :
    result = u*cos(theta) + (mc+mp)*g*sin(theta) + mp*lp*cos(theta)*sin(theta)*pow(theta_speed, 2)
    result = result / (mp*lp*pow(cos(theta), 2) - (mc + mp)*lp)
    return result

def update_state(u, theta, theta_speed, theta_accel, x, x_speed, x_accel, dt) :
    theta_accel = calculate_theta_accel(u, theta, theta_speed)
    x_accel = calculate_x_accel(u, theta, theta_speed)

    theta_speed_old = theta_speed
    theta_speed = theta_speed + theta_accel*dt
    x_speed_old = x_speed
    x_speed = x_speed + x_accel*dt

    # the '(theta_speed + theta_speed_old)/2' instead of just 'theta_speed' helps a lot
    theta = (theta + (theta_speed + theta_speed_old)/2*dt)%(2*pi)
    x = x + (x_speed + x_speed_old)/2*dt

    return theta, theta_speed, theta_accel, x, x_speed, x_accel

def play(screen) :
    clock = pygame.time.Clock()
    t0 = pygame.time.get_ticks()/1000.0
    t1 = 0
    screen.fill(BLACK)
    theta = theta_0
    theta_speed = 0
    theta_accel = 0
    x = x_0
    x_speed = 0
    x_accel = 0
    u = 0
    perturbation = 0

    while True :
        t = pygame.time.get_ticks()/1000.0 - t0        
        # 'e' for 'event'
        for e in pygame.event.get():
            if e.type == QUIT :
                sys.exit()
            elif e.type == KEYDOWN:
                key = e.dict['unicode']
                if (key == "q") :
                    print("Rage quited !")
                    return                   
                if (e.key == pygame.K_RIGHT) :
                    t1 = pygame.time.get_ticks()/1000.0 - t0
                    perturbation = -PERTURBATION                    
                if (e.key == pygame.K_LEFT) :
                    t1 = pygame.time.get_ticks()/1000.0 - t0
                    perturbation = PERTURBATION
        if (abs(t - t1) > PERTUBATION_DURATION) :
            perturbation = 0
        for i in range(nb_calculations_per_rotation) :
            # Calculating at a higher frequency than the screen, otherwise the errors are too big
            u = asserv(theta, theta_speed, pi, x, x_speed, 0.0) + perturbation
            theta, theta_speed, theta_accel, x, x_speed, x_accel = update_state(u, theta, theta_speed, theta_accel, x, x_speed, x_accel, dt)
            #print(u)
        screen.fill(BLACK)

        draw_state(screen, theta, x, u)
        text = font.render("Cart pole simulation !", 1, gold)
        screen.blit(text, [0, 0])

        time_pos = [20, 30] 
        
        try :
            # Erasing the previous text
            time_text.fill(BLACK)
            screen.blit(time_text, time_pos)
        except Exception :
            None
        # Writing new text
        str_time = "{0:.2f}".format(t)
        time_text = font.render("Time : " + str_time, 1, (0,150,0))
        screen.blit(time_text, time_pos)

        pygame.display.update()
        # That juicy 60 Hz :D
        clock.tick(freq)

def asserv(theta, theta_speed, theta_goal, x, x_speed, x_goal) :
    #theta = theta - pi/2
    #theta_goal = theta_goal - pi/2
    diff = angle_diff(theta_goal, theta)
    #print("theta = {}, theta_goal = {}, diff = {}".format(theta, theta_goal, diff))
    u = theta_p*diff + theta_d*theta_speed
    u = u + x_p*(x_goal - x) + x_d*x_speed
    # u = -Kx where x = [theta, dtheta/dt, x, dx/dt]
    if (LQR_ON) :
        u = u -(K[0]*theta + K[1]*theta_speed + K[2]*x + K[3]*x_speed) 
    return u

# Returns the smallest distance between 2 angles
def angle_diff(a, b) :
    d = a - b
    d = ((d + math.pi) % (2*math.pi)) - math.pi
    return d

def main():
    #initialize and prepare screen
    pygame.init()
    screen = pygame.display.set_mode(WINSIZE)
    pygame.display.set_caption('Cart pole simulation')
    # Font init
    global font, score_font, small_font
    font = pygame.font.SysFont("monospace", 30)
    
    screen.fill(BLACK)

    result = play(screen)

    sys.exit()

# if python says run, then we should run
if __name__ == '__main__':
    #if(len(sys.argv) > 1 and sys.argv[1] == "-s") :
    #graph_test()
    #sys.exit()
    main()
