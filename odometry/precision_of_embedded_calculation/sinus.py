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


if __name__ == '__main__':
    if(len(sys.argv) != 2) :
        print("Usage : sinus.py nbSamples")
        sys.exit()

    nb_samples = sys.argv[1]
    max = math.pi/2
    increment = max/float(nb_samples)
    i = -increment
    while i < max :
        i = i + increment
        print(str(i) + " " + str(math.sin(i)))
