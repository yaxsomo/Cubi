import sys

class Motor(object):
    """
    Represents the state of one motor 
    """
    def __init__(self):
        # For now, the speed control is assumed perfect
        self.speed = 0;


    def __repr__(self):
        s = "speed : {}".format(self.speed)
        return s
    