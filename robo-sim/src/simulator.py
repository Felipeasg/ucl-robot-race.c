#!/opt/local/bin/python3.2

from __future__ import division
import sys
PYTHON_MAJOR = sys.version_info[0]
if PYTHON_MAJOR == 2:
    from Tkinter import *
else:
    from tkinter import *

#from tkinter import *
import time
import os
from src.rsim_display import Display
#from visualize import *

from src.wallmodel import WallModel
from src.robotmodel import RobotModel, L, R
from src.worldmodel import WorldModel
from src.control import Controller




class Simulator():
    def __init__(self, port):
        self.root = Tk()
        self.root.wm_title("RoboSim")
        self.windowsystem = self.root.call('tk', 'windowingsystem')
        #print(self.windowsystem)
        self.zoom = 10
        self.disp = Display(self.root, self.zoom)

        #robot displays by default at 100 pixels square.
        #read robot is approx 26cm square
        #multiply default scaled distances by scale to get cm

        #internally the model works in default scaled pixels.  But it returns
        #cm on the UI and TCP interfaces.
        self.scale = 26.0/100
        timenow = time.time()
        self.robot = RobotModel(self.disp, 100, self.scale, timenow)
        self.world = WorldModel(self.disp, self.robot, timenow)
        self.world.add_default_walls()
        if (port != 0):
            self.controller = Controller(port,self.robot, 1)
        else:
            self.controller = Controller(55443,self.robot, 0)

        self.robot.set_controller(self.controller)
        self.robot.init_robot_posn(500,350,0)
        self.robot.set_ir_angles(0, 0)
        self.robot.stop()


    def run(self):
        #local variables are fastest and this is the inner loop
        world = self.world
        controller = self.controller
        robot = self.robot
        root = self.root
        realtime = time.time()
        starttime = realtime
        cur_time = 0
        iter_time = 0
        sleep_time = 0.055
        while TRUE:
            realtime = time.time()
            cur_time = cur_time + 1
            controller.poll()
            world.set_time(realtime)
            robot.self_update(cur_time, realtime)
            world.sample_all_sensors()
            world.check_collision()
            root.update()
            #attempt to normalize the speed across different computers
            if cur_time %100 == 0:
                iter_time = (realtime - starttime)/100
                #print(iter_time)
                starttime = realtime
                if iter_time > 0.020:
                    sleep_time = sleep_time - 0.0005
                else:
                    sleep_time = sleep_time + 0.0005
            if sleep_time > 0:
                time.sleep(sleep_time)
                                

#sim = Simulator()
#sim.run()

