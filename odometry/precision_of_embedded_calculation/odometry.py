# -*- coding: utf-8 -*-
import sys
import logging
import time
import copy
import math
import csv
import datetime
import os
import traceback

R = 32.5
L = 115.0
SPEED_DT = 60.0
SERVO_DT = 20.0
CPR = 2800
DEG2RAD = (math.pi/180.0)
RAD2DEG = (180.0/math.pi)

def load(file_name) :
    try :
        with open(file_name, 'r') as csvfile:
            rows = csv.reader(csvfile, delimiter=' ', quotechar='|')
            true_rows = []
            for r in rows :
                true_rows.append(r)
            return true_rows
    except Exception as e:
        print("Failed to read : '" + file_name + "'. Error : " + str(e))
        return

def save(rows, file_name) :
    try :
        with open(file_name, 'w') as csvfile:
            data_writer = csv.writer(csvfile, delimiter=' ')
            for r in rows :
                data_writer.writerow(r)
    except Exception as e:
        print("Failed to write : '" + file_name + "'. Error : " + str(e))
        return

def SI2firmware(w) :
    return w*(SPEED_DT/1000.0)*CPR/(2*math.pi)

def firmware2SI(enc_speed) :
  return float(enc_speed) / (SPEED_DT/1000.0) / CPR * (2*math.pi)


def exact_odo(left, right) :
    linearSpeed = firmware2SI((left + right)/2.0)*R
    rotationSpeed = firmware2SI(-(right - left))*(R/L)*RAD2DEG
    dt = SERVO_DT/1000.0
    l = linearSpeed*dt
    if (rotationSpeed == 0) :
        # The robot moves in a straight line
        dy = 0
        dx = l
        dtheta = 0
    else :
        # The robot moves on a portion of a circle whose radius is l/alpha
        alpha = rotationSpeed*dt*DEG2RAD
        radius = (l/alpha)
        dx = radius*math.sin(alpha)
        dy = radius*(math.cos(alpha)-1)
        dtheta = alpha
        
    output = [str(left), str(right), str(linearSpeed), str(rotationSpeed), str(dx), str(dy), str(dtheta)]
    return output

def merge(input, output) :
    merged = [["left", "right", "linearSpeed approx", "linearSpeed exact", "error", "rotationSpeed approx", "rotationSpeed exact", "error", "dx approx", "dx exact", "error", "dy approx", "dy exact", "error", "dtheta approx", "dtheta exact", "error"]]
    index = -1
    for r in input :
        index = index + 1
        line = []
        line.append(r[0]) #left
        line.append(r[1]) #right

        line.append(r[2]) #linearSpeed approx
        line.append(output[index][2]) #linearSpeed exact
        line.append(float(r[2]) - float(output[index][2])) #diff

        line.append(r[3]) #rotationSpeed approx
        line.append(output[index][3]) #rotationSpeed exact
        line.append(float(r[3]) - float(output[index][3])) #diff
        
        line.append(r[4]) #dx approx
        line.append(output[index][4]) #dx exact
        line.append(float(r[4]) - float(output[index][4])) #diff

        line.append(r[5]) #dy approx
        line.append(output[index][5]) #dy exact
        line.append(float(r[5]) - float(output[index][5])) #diff

        line.append(r[6]) #dtheta approx
        line.append(output[index][6]) #dtheta exact
        line.append(float(r[6]) - float(output[index][6])) #diff

        merged.append(line)
    return merged

if __name__ == '__main__':
    if(len(sys.argv) != 2) :
        print("Usage : odometry.py path")
        sys.exit()

    input = load(sys.argv[1])
    print(input)
    output = []
    for r in input :
        left = r[0]
        right = r[1]
        odo = exact_odo(float(left), float(right))
        print(odo)
        output.append(odo)
    merged = merge(input, output)
    save(output, "output.csv")
    save(merged, "merged.csv")


