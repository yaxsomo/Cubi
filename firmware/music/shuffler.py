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

def load(file_name) :
    try :
        with open(file_name, 'r') as csvfile:
            rows = csv.reader(csvfile, delimiter=' ', quotechar='|')
            words = []
            for r in rows :
                for w in r :
                    if (len(w.strip()) < 1) :
                        continue
                    words.append(w.strip())

            return words
    except Exception as e:
        logger.error("Failed to read : '" + file_name + "'. Error : ", e)
        return

def mix(notes, tempo, mode=0) :
    if (len(notes) != len(tempo)) :
        print ("Not the same number of elements !")
        print(str(len(notes)) + " vs " + str(len(tempo)))
        sys.exit()
    output = "static struct buzzer_note melody[] = {\n"
    for i in range(0, len(notes)) :
        print("note = " + notes[i])
        print("tempo = " + tempo[i])
        delayFactor = 1.0
        if int(mode) == 0 :
            duration = int(1000/float(tempo[i]))
            delay = delayFactor*duration
        else :
            delay = int(delayFactor*float(tempo[i]))
            duration = int(tempo[i])
            delay = delayFactor*duration

        output = output + "{" + notes[i] + ", " + str(duration) + "}, "
        output = output + "{0, " + str(delay) + "},\n"

    #Last note is actually an end code
    output = output + "{0, 0}\n};"
    return output

if __name__ == '__main__':
    if(len(sys.argv) != 4) :
        # Mode 1 is when the timming is already in ms
        print("Usage : shuffler notes tempo mode(0 or 1)")
        sys.exit()

    notes = load(sys.argv[1])
    tempo = load(sys.argv[2])
    mode = sys.argv[3]
    print(notes)
    print(tempo)
    output = mix(notes, tempo, mode)
    print("Result = ")
    print(output)


'''
static struct buzzer_note melody_alert_fast[] = {
    {2000, 100},
    {200, 100},
    {2000, 100},
    {200, 100},
    {0, 0}
};
'''