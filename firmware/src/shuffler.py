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
                    words.append(w)

            return words
    except Exception as e:
        logger.error("Failed to read : '" + file_name + "'. Error : ", e)
        return

def mix(notes, tempo) :
    if (len(notes) != len(tempo)) :
        print ("Not the same number of elements !")
        print(str(len(notes)) + " vs " + str(len(tempo)))
        sys.exit()
    output = "static struct buzzer_note melody[] = {\n"
    for i in range(0, len(notes)) :
        delay = int(1.3*1000/float(tempo[i]))
        output = output + "{" + notes[i] + ", " + str(1000/int(tempo[i])) + "}, "
        output = output + "{0, " + str(delay) + "} "
        if (i < (len(notes)-1)) :
            output = output + ",\n"
    output = output + "\n};"
    return output

if __name__ == '__main__':
    if(len(sys.argv) != 3) :
        print("Usage : shuffler notes tempo")
        sys.exit()

    notes = load(sys.argv[1])
    tempo = load(sys.argv[2])
    print(notes)
    print(tempo)
    output = mix(notes, tempo)
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