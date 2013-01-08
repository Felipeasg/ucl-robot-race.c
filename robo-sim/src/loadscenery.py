from __future__ import division 
import sys
PYTHON_MAJOR = sys.version_info[0]

if PYTHON_MAJOR == 2:
    import tkMessageBox
else:
    import tkinter.messagebox
import shlex
from src.wallmodel import WallModel
from src.linemodel import LineModel
from src.conemodel import ConeModel

class SceneryParser():
    def __init__(self, display, world, filename):
        self.display = display
        self.world = world
        self.filename = filename
        self.openfile(filename)

    def openfile(self, filename):
        try:
            f = open(filename, "r")
        except:
            print("File open failed")
            if PYTHON_MAJOR == 2:
                tkMessageBox.showerror("File open failed", "File open failed")
            else:
                dialog = tkinter.messagebox.showerror("File open failed", "File open failed")
            return

        self.world.erase_scenery()
        self.world.add_default_walls()        
        for line in f:
#            print("Original:")
#            print(line)
#            print("Shlex:")
            lexer = shlex.split(line)
            if len(lexer) == 0:
                continue
#            for token in lexer:
#                print(repr(token))
#            print("\n")
            if lexer[0] == "wall":
                wall = WallModel(self.display, int(lexer[1]), int(lexer[2]), 
                                 int(lexer[3]), int(lexer[4]))
                self.world.add_object(wall)
            if lexer[0] == "cone":
                cone = ConeModel(self.display, int(lexer[1]), int(lexer[2]), 
                                 int(lexer[3]))
                self.world.add_object(cone)
            if lexer[0] == "line":
                line = LineModel(self.display, int(lexer[1]), int(lexer[2]), 
                                 int(lexer[3]), int(lexer[4]))
                self.world.add_object(line)
            elif lexer[0] == "robot":
                self.world.set_robot_position(int(lexer[1]), int(lexer[2]), 
                                              int(lexer[3]))
                
        
    
