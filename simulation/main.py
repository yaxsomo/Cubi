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


# constants (we should put these in another file)
INVISIBLE = (111, 111, 0)
WHITE = 255, 240, 200
BLACK = 20, 20, 40
gold = [212, 175, 55]

# Used
WINSIZE = [1500, 840]
WINCENTER = [WINSIZE[0] / 2, WINSIZE[1] / 2]
OFFSET = [300, 100]
center_color = [200, 0, 0]
center_size = 10
wheel_color = [0, 0, 150]
wheel_size = 10
goal_color = [0, 150, 0]
goal_size = 5

fake_color = [100, 100, 100]
fake_size = center_size / 4

wheel_speed_inc = 1
xytol = 5
turnp = 0.03
speedp = 0.01


def draw_robot(screen, m, fake=False):
    # Usually, we have X in front of us and Y to the left. Let's keep that : y = -y, theta = theta-pi/2
    center_pos = [int(m.x + WINCENTER[0]), int(-m.y + WINCENTER[1])]
    color = center_color
    size = center_size
    if fake:
        color = fake_color
        size = int(fake_size)
    pygame.draw.circle(screen, color, center_pos, int(size), 0)

    r = int(model.L / 2)
    theta = m.theta - math.pi / 2
    wheel_pos = [
        int(center_pos[0] + r * math.cos(theta)),
        int(center_pos[1] - r * math.sin(theta)),
    ]
    color = wheel_color
    size = wheel_size
    if fake:
        color = fake_color
        size = fake_size
    pygame.draw.circle(screen, color, wheel_pos, int(size), 0)

    wheel_pos = [
        int(center_pos[0] - r * math.cos(theta)),
        int(center_pos[1] + r * math.sin(theta)),
    ]
    pygame.draw.circle(screen, color, wheel_pos, int(size), 0)


def draw_goal(screen, m):
    # Usually, we have X in front of us and Y to the left. Let's keep that : y = -y, theta = theta-pi/2
    center_pos = [int(m.x_goal + WINCENTER[0]), int(-m.y_goal + WINCENTER[1])]
    pygame.draw.circle(screen, goal_color, center_pos, int(goal_size), 0)


def draw_state(screen, m):
    draw_robot(screen, m)
    draw_goal(screen, m)


def play(screen):
    m = model.Model()
    m.x_goal = 50
    m.y_goal = 50
    clock = pygame.time.Clock()
    t0 = pygame.time.get_ticks() / 1000.0
    screen.fill(BLACK)

    while True:
        # 'e' for 'event'
        for e in pygame.event.get():
            if e.type == QUIT:
                sys.exit()
            elif e.type == KEYDOWN:
                key = e.dict["unicode"]
                if key == "q":
                    print("Rage quited !")
                    return
                if key == "r":
                    None
                if e.key == pygame.K_SPACE:
                    None
                if e.key == pygame.K_RETURN:
                    m.theta = float(raw_input("theta = "))
                if e.key == pygame.K_UP:
                    m.m1.speed = m.m1.speed + wheel_speed_inc
                if e.key == pygame.K_DOWN:
                    m.m1.speed = m.m1.speed - wheel_speed_inc
                if e.key == pygame.K_RIGHT:
                    m.m2.speed = m.m2.speed + wheel_speed_inc
                if e.key == pygame.K_LEFT:
                    m.m2.speed = m.m2.speed - wheel_speed_inc
            elif e.type == pygame.MOUSEMOTION:
                mx, my = e.pos
            elif e.type == MOUSEBUTTONDOWN and e.button == 1:
                m.x_goal = mx - WINCENTER[0]
                m.y_goal = -(my - WINCENTER[1])
        asserv(m)
        m.update(1)
        screen.fill(BLACK)

        # Creating a fake robot to trace the futur :)
        fake_m = copy.deepcopy(m)
        for i in range(20):
            asserv(fake_m)
            fake_m.update(20)
            draw_robot(screen, fake_m, fake=True)

        draw_state(screen, m)
        print(m)
        text = font.render("Cubi !", 1, gold)
        screen.blit(text, [0, 0])

        t = pygame.time.get_ticks() / 1000.0 - t0
        time_pos = [20, 30]

        try:
            # Erasing the previous text
            time_text.fill(BLACK)
            screen.blit(time_text, time_pos)
        except Exception:
            None
        # Writing new text
        str_time = "{0:.2f}".format(t)
        time_text = font.render("Time : " + str_time, 1, (0, 150, 0))
        screen.blit(time_text, time_pos)

        pygame.display.update()
        # That juicy 60 Hz :D
        clock.tick(60)


def ik(local_speed, local_turn):
    m1_speed = local_speed - local_turn * model.L / 2
    m2_speed = local_speed + local_turn * model.L / 2
    return m1_speed, m2_speed


def asserv(m):
    distance = math.sqrt(
        (m.x_goal - m.x) * (m.x_goal - m.x) + (m.y_goal - m.y) * (m.y_goal - m.y)
    )
    if distance < xytol:
        # Close enough
        m.m1.speed = 0
        m.m2.speed = 0
        return

    # The angle goal depends on the X Y goals
    dy = m.y_goal - m.y
    dx = m.x_goal - m.x
    if not (dx == 0 and dy == 0):
        m.theta_goal = math.atan2(dy, m.x_goal - m.x)

    # TODO implement go backwards when (theta_goal - theta) > pi. And debug this.
    # Turn asserv
    err = angle_diff(m.theta, m.theta_goal)
    # print("***err = {}, goal = {}, theta = {}".format(err*180/math.pi, m.theta_goal*180/math.pi, m.theta*180/math.pi))
    # model.acc += turni * err;
    # _limit(&model.acc, turnacc);
    p_contribution = turnp * err

    """
    if ((p_contribution < 0 && model.acc >= 0) || (p_contribution > 0 && model.acc <= 0)) {
        // Astuce !
        model.acc = 0;
    }
    """
    local_turn = p_contribution + m.acc

    local_speed = 0
    # linear speed asserv
    # model.speed_acc += speedi * distance;
    # _limit(&model.acc, speedacc);
    p_contribution = speedp * distance

    """
    if ((p_contribution < 0 && model.speed_acc >= 0) || (p_contribution > 0 && model.speed_acc <= 0)) {
        // Astuce !
        model.speed_acc = 0;
    }
    """

    local_speed = p_contribution + m.speed_acc

    m1_speed, m2_speed = ik(local_speed, local_turn)
    m.m1.speed = m1_speed
    m.m2.speed = m2_speed


# Returns the smallest distance between 2 angles
def angle_diff(a, b):
    d = a - b
    d = ((d + math.pi) % (2 * math.pi)) - math.pi
    return d


def main():
    # initialize and prepare screen
    pygame.init()
    screen = pygame.display.set_mode(WINSIZE)
    pygame.display.set_caption("Cubi simulation")
    # Font init
    global font, score_font, small_font
    font = pygame.font.SysFont("monospace", 30)

    screen.fill(BLACK)

    result = play(screen)

    sys.exit()


# if python says run, then we should run
if __name__ == "__main__":
    # if(len(sys.argv) > 1 and sys.argv[1] == "-s") :
    # graph_test()
    # sys.exit()
    main()
