#!/opt/local/bin/python3.2
from __future__ import division 
import sys
from src.constants import L,M,R,FRONT,SIDE
PYTHON_MAJOR = sys.version_info[0]

if PYTHON_MAJOR == 2:
    from Tkinter import *
else:
    from tkinter import *
from math import *

class BaseObjVis():
    def __init__(self, display):
        self.canvas = display.canvas
        self.display = display

    def draw_self(self):
        print("base draw self");

class WallVis(BaseObjVis):
    def __init__(self, display, x1, y1, x2, y2):
        BaseObjVis.__init__(self, display)
        self.x1 = x1
        self.y1 = y1
        self.x2 = x2
        self.y2 = y2

    def draw(self):
        zoom = self.display.zoom
        self.wall = self.canvas.create_line(self.x1*zoom,self.y1*zoom,
                                            self.x2*zoom,self.y2*zoom, 
                                            width = 3, fill="black")

    def redraw(self):
        self.erase()
        self.draw()

    def erase(self):
        self.canvas.delete(self.wall)

class ConeVis(BaseObjVis):
    def __init__(self, display, x, y, r):
        BaseObjVis.__init__(self, display)
        self.x1 = x-r
        self.y1 = y-r
        self.x2 = x+r
        self.y2 = y+r

    def draw(self):
        zoom = self.display.zoom
        self.cone = self.canvas.create_oval(self.x1*zoom,self.y1*zoom,
                                            self.x2*zoom,self.y2*zoom, 
                                            width = 3, fill="black")

    def redraw(self):
        self.erase()
        self.draw()

    def erase(self):
        self.canvas.delete(self.cone)

class LineVis(BaseObjVis):
    def __init__(self, display, x1, y1, x2, y2):
        BaseObjVis.__init__(self, display)
        self.x1 = x1
        self.y1 = y1
        self.x2 = x2
        self.y2 = y2

    def draw(self):
        zoom = self.display.zoom
        self.line1 = self.canvas.create_line(self.x1*zoom,self.y1*zoom,
                                            self.x2*zoom,self.y2*zoom, 
                                            width = 9, fill="darkgrey")
        self.line2 = self.canvas.create_line(self.x1*zoom,self.y1*zoom,
                                            self.x2*zoom,self.y2*zoom, 
                                            width = 3, fill="white")

    def redraw(self):
        self.erase()
        self.draw()

    def erase(self):
        self.canvas.delete(self.line1)
        self.canvas.delete(self.line2)

class Blob():
    def __init__(self, x, y, color):
        self.x = x
        self.y = y
        self.blob = 0
        self.color = color

    def redraw(self, canvas, zoom):
        if self.blob != 0:
            canvas.delete(self.blob)
        x1 = (self.x-2)*zoom
        y1 = (self.y-2)*zoom
        x2 = (self.x+2)*zoom
        y2 = (self.y+2)*zoom
        self.blob = canvas.create_polygon(x1,y1,x2,y1,x2,y2,x1,y2, fill=self.color)
    def erase(self, canvas):
        if self.blob != 0:
            canvas.delete(self.blob)

class RobotVis(BaseObjVis):
    def __init__(self, display, x, y, angle, size, \
                     max_front_ir_range, max_side_ir_range):
        BaseObjVis.__init__(self, display)
        self.display = display
        self.count = 0
        self.x = x
        self.y = y
        self.size = size
        self.max_front_ir_range = max_front_ir_range
        self.max_side_ir_range = max_side_ir_range
        self.angle = angle * (pi / 180)
        self.ir_angle_left = 0
        self.ir_angle_right = 0
        self.ir_beam_front_on = TRUE
        self.ir_beam_front_left = 0
        self.ir_beam_front_right = 0
        self.ir_beam_side_on = TRUE
        self.ir_beam_side_left = 0
        self.ir_beam_side_right = 0
        self.us_beam_on = TRUE
        self.us_beam = []
        self.blob = []
        self.trail = []
        self.points = []
        zoom = self.display.zoom
        self.prevzoom = zoom
        self.draw()
        x1 = (x-5)*zoom
        y1 = (y-5)*zoom
        x2 = (x+5)*zoom
        y2 = (y+5)*zoom
        self.blob.append(self.canvas.create_polygon(x1,y1,x2,y1,x2,y2,x1,y2, fill="red"))
        self.blob.append(self.canvas.create_polygon(x1,y1,x2,y1,x2,y2,x1,y2, fill="red"))
        self.blob.append(self.canvas.create_polygon(x1,y1,x2,y1,x2,y2,x1,y2, fill="red"))
        self.blob.append(self.canvas.create_polygon(x1,y1,x2,y1,x2,y2,x1,y2, fill="red"))
        self.blob.append(self.canvas.create_polygon(x1,y1,x2,y1,x2,y2,x1,y2, fill="red"))
        self.blob.append(self.canvas.create_polygon(x1,y1,x2,y1,x2,y2,x1,y2, fill="red"))
        self.blob.append(self.canvas.create_polygon(x1,y1,x2,y1,x2,y2,x1,y2, fill="pink"))
        self.blob.append(self.canvas.create_polygon(x1,y1,x2,y1,x2,y2,x1,y2, fill="pink"))
        self.blob.append(self.canvas.create_polygon(x1,y1,x2,y1,x2,y2,x1,y2, fill="pink"))

    def set_display_text(self, line, text):
        self.display.set_display_text(line, text)

    def set_sensor_visibility(self, ir_front, ir_side, us, line):
        self.ir_beam_front_on = ir_front
        self.ir_beam_side_on = ir_side
        self.us_beam_on = us
        self.display.set_sensor_visibility(ir_front, ir_side, us, line)
        
    def rotate(self, x, y, ox, oy, angle):
        nx = ox - ((x-ox) * cos(angle) + (y-oy) * sin(angle))
        ny = oy + (y-oy) * cos(angle) - (x-ox) * sin(angle) 
        return tuple([nx, ny])

    def draw(self):
        zoom = self.display.zoom
        prevzoom = self.prevzoom
        if (zoom != prevzoom):
            self.prevzoom = zoom
            self.redraw_trail()
            self.redraw_points()
        
        x = self.x * zoom
        y = self.y * zoom
        size = self.size * zoom
        
        #df is half the width of the front straight side
        df = 0.3*size
        #dr is half the width of the robot
        dr = 0.5*size

        x1 = x - dr
        y1 = y - df
        x2 = x - df
        y2 = y - dr
        
        x3 = x + df
        y3 = y - dr
        x4 = x + dr
        y4 = y - df

        x5 = x + dr
        y5 = y + df
        x6 = x + df
        y6 = y + dr

        x7 = x - df
        y7 = y + dr
        x8 = x - dr
        y8 = y + df
        x1,y1 = self.rotate(x1,y1,x, y, self.angle)
        x2,y2 = self.rotate(x2,y2,x, y, self.angle)
        x3,y3 = self.rotate(x3,y3,x, y, self.angle)
        x4,y4 = self.rotate(x4,y4,x, y, self.angle)
        x5,y5 = self.rotate(x5,y5,x, y, self.angle)
        x6,y6 = self.rotate(x6,y6,x, y, self.angle)
        x7,y7 = self.rotate(x7,y7,x, y, self.angle)
        x8,y8 = self.rotate(x8,y8,x, y, self.angle)
        self.base = self.canvas.create_polygon(x1,y1,x2,y2,x3,y3,x4,y4,
                                               x5,y5,x6,y6,x7,y7,x8,y8, fill="darkgrey")

        x1 = x - size/2
        y1 = y - size/5
        x2 = x1 + size/10
        y2 = y1
        x3 = x2
        y3 = y + size/5
        x4 = x1
        y4 = y3
        x1,y1 = self.rotate(x1,y1,x, y, self.angle)
        x2,y2 = self.rotate(x2,y2,x, y, self.angle)
        x3,y3 = self.rotate(x3,y3,x, y, self.angle)
        x4,y4 = self.rotate(x4,y4,x, y, self.angle)
        self.left_wheel = self.canvas.create_polygon(x1,y1,x2,y2,x3,y3,x4,y4, fill="red")

        x1 = x + size/2
        y1 = y - size/5
        x2 = x1 - size/10
        y2 = y1
        x3 = x2
        y3 = y + size/5
        x4 = x1
        y4 = y3
        x1,y1 = self.rotate(x1,y1,x, y, self.angle)
        x2,y2 = self.rotate(x2,y2,x, y, self.angle)
        x3,y3 = self.rotate(x3,y3,x, y, self.angle)
        x4,y4 = self.rotate(x4,y4,x, y, self.angle)
        self.right_wheel = self.canvas.create_polygon(x1,y1,x2,y2,x3,y3,x4,y4, fill="red")

        x1 = x - (2.5*size)/40
        y1 = y - (12*size)/40
        x2 = x1 + size/20
        y2 = y1
        x3 = x2
        y3 = y - (10*size)/40
        x4 = x1
        y4 = y3
        x1,y1 = self.rotate(x1,y1,x, y, self.angle)
        x2,y2 = self.rotate(x2,y2,x, y, self.angle)
        x3,y3 = self.rotate(x3,y3,x, y, self.angle)
        x4,y4 = self.rotate(x4,y4,x, y, self.angle)
        self.us_sensor1 = self.canvas.create_polygon(x1,y1,x2,y2,x3,y3,x4,y4, fill="blue")

        x1 = x + (0.5*size)/40
        y1 = y - (12*size)/40
        x2 = x1 + size/20
        y2 = y1
        x3 = x2
        y3 = y - (10*size)/40
        x4 = x1
        y4 = y3
        x1,y1 = self.rotate(x1,y1,x, y, self.angle)
        x2,y2 = self.rotate(x2,y2,x, y, self.angle)
        x3,y3 = self.rotate(x3,y3,x, y, self.angle)
        x4,y4 = self.rotate(x4,y4,x, y, self.angle)
        self.us_sensor2 = self.canvas.create_polygon(x1,y1,x2,y2,x3,y3,x4,y4, fill="blue")
        self.do_front_ir_display()
        self.do_side_ir_display()
        self.do_us_display()


    def do_front_ir_display(self):
        x = self.x * self.display.zoom
        y = self.y * self.display.zoom
        size = self.size * self.display.zoom
        max_ir_range = self.max_front_ir_range * self.display.zoom
        x1 = x - (15*size)/40
        y1 = y - (16*size)/40
        x2 = x1 + (5*size)/40
        y2 = y1
        x3 = x2
        y3 = y - (14*size)/40
        x4 = x1
        y4 = y3
        cx = x - (12.5*size)/40
        cy = y - (15*size)/40
        x1,y1 = self.rotate(x1,y1,cx, cy, self.ir_angle_right)
        x1,y1 = self.rotate(x1,y1,x, y, self.angle)
        x2,y2 = self.rotate(x2,y2,cx, cy, self.ir_angle_right)
        x2,y2 = self.rotate(x2,y2,x, y, self.angle)
        x3,y3 = self.rotate(x3,y3,cx, cy, self.ir_angle_right)
        x3,y3 = self.rotate(x3,y3,x, y, self.angle)
        x4,y4 = self.rotate(x4,y4,cx, cy, self.ir_angle_right)
        x4,y4 = self.rotate(x4,y4,x, y, self.angle)
        self.ir_front_right = self.canvas.create_polygon(x1,y1,x2,y2,x3,y3,x4,y4, fill="blue")

        x1 = x + (15*size)/40
        y1 = y - (16*size)/40
        x2 = x1 - (5*size)/40
        y2 = y1
        x3 = x2
        y3 = y - (14*size)/40
        x4 = x1
        y4 = y3
        cx = x + (12.5*size)/40
        cy = y - (15*size)/40
        x1,y1 = self.rotate(x1,y1,cx, cy, self.ir_angle_left)
        x1,y1 = self.rotate(x1,y1,x, y, self.angle)
        x2,y2 = self.rotate(x2,y2,cx, cy, self.ir_angle_left)
        x2,y2 = self.rotate(x2,y2,x, y, self.angle)
        x3,y3 = self.rotate(x3,y3,cx, cy, self.ir_angle_left)
        x3,y3 = self.rotate(x3,y3,x, y, self.angle)
        x4,y4 = self.rotate(x4,y4,cx, cy, self.ir_angle_left)
        x4,y4 = self.rotate(x4,y4,x, y, self.angle)
        self.ir_front_left = self.canvas.create_polygon(x1,y1,x2,y2,x3,y3,x4,y4, fill="green")

        if self.ir_beam_front_on:
            x1 = x - (12.5*size)/40
            y1 = y - (16*size)/40
            x2 = x1
            y2 = y1 - max_ir_range
            cx = x - (12.5*size)/40
            cy = y - (15*size)/40
            x1,y1 = self.rotate(x1,y1,cx, cy, self.ir_angle_right)
            x1,y1 = self.rotate(x1,y1,x, y, self.angle)
            x2,y2 = self.rotate(x2,y2,cx, cy, self.ir_angle_right)
            x2,y2 = self.rotate(x2,y2,x, y, self.angle)
            self.ir_beam_front_right = self.canvas.create_line(x1,y1,x2,y2, fill="blue")

            x1 = x + (12.5*size)/40
            y1 = y - (16*size)/40
            x2 = x1
            y2 = y1 - max_ir_range
            cx = x + (12.5*size)/40
            cy = y - (15*size)/40
            x1,y1 = self.rotate(x1,y1,cx, cy, self.ir_angle_left)
            x1,y1 = self.rotate(x1,y1,x, y, self.angle)
            x2,y2 = self.rotate(x2,y2,cx, cy, self.ir_angle_left)
            x2,y2 = self.rotate(x2,y2,x, y, self.angle)
            self.ir_beam_front_left = self.canvas.create_line(x1,y1,x2,y2, fill="darkgreen")

    def do_side_ir_display(self):
        x = self.x * self.display.zoom
        y = self.y * self.display.zoom
        size = self.size * self.display.zoom
        side_angle = 3.14*90.0/180.0
        max_ir_range = self.max_side_ir_range * self.display.zoom
        x1 = x - (16*size)/40
        y1 = y + (14*size)/40
        x2 = x1 + (2*size)/40
        y2 = y1
        x3 = x2
        y3 = y + (12*size)/40
        x4 = x1
        y4 = y3
        cx = x - (12.5*size)/40
        cy = y + (15*size)/40
        #x1,y1 = self.rotate(x1,y1,cx, cy, -side_angle)
        x1,y1 = self.rotate(x1,y1,x, y, self.angle)
        #x2,y2 = self.rotate(x2,y2,cx, cy, -side_angle)
        x2,y2 = self.rotate(x2,y2,x, y, self.angle)
        #x3,y3 = self.rotate(x3,y3,cx, cy, -side_angle)
        x3,y3 = self.rotate(x3,y3,x, y, self.angle)
        #x4,y4 = self.rotate(x4,y4,cx, cy, -side_angle)
        x4,y4 = self.rotate(x4,y4,x, y, self.angle)
        self.ir_side_right = self.canvas.create_polygon(x1,y1,x2,y2,x3,y3,x4,y4, fill="blue")

        x1 = x + (16*size)/40
        y1 = y + (14*size)/40
        x2 = x1 - (2*size)/40
        y2 = y1
        x3 = x2
        y3 = y + (12*size)/40
        x4 = x1
        y4 = y3
        cx = x + (12.5*size)/40
        cy = y + (15*size)/40
        #x1,y1 = self.rotate(x1,y1,cx, cy, side_angle)
        x1,y1 = self.rotate(x1,y1,x, y, self.angle)
        #x2,y2 = self.rotate(x2,y2,cx, cy, side_angle)
        x2,y2 = self.rotate(x2,y2,x, y, self.angle)
        #x3,y3 = self.rotate(x3,y3,cx, cy, side_angle)
        x3,y3 = self.rotate(x3,y3,x, y, self.angle)
        #x4,y4 = self.rotate(x4,y4,cx, cy, side_angle)
        x4,y4 = self.rotate(x4,y4,x, y, self.angle)
        self.ir_side_left = self.canvas.create_polygon(x1,y1,x2,y2,x3,y3,x4,y4, fill="green")

        if self.ir_beam_side_on:
            x1 = x - (16*size)/40
            y1 = y + (13*size)/40
            x2 = x1 - max_ir_range
            y2 = y1 
            #cx = x - (16*size)/40
            #cy = y + (13*size)/40
            #x1,y1 = self.rotate(x1,y1,cx, cy, -side_angle)
            x1,y1 = self.rotate(x1,y1,x, y, self.angle)
            #x2,y2 = self.rotate(x2,y2,cx, cy, -side_angle)
            x2,y2 = self.rotate(x2,y2,x, y, self.angle)
            self.ir_beam_side_right = self.canvas.create_line(x1,y1,x2,y2, fill="navyblue")

            x1 = x + (16*size)/40
            y1 = y + (13*size)/40
            x2 = x1 + max_ir_range
            y2 = y1
            #cx = x + (16*size)/40
            #cy = y + (13*size)/40
            #x1,y1 = self.rotate(x1,y1,cx, cy, side_angle)
            x1,y1 = self.rotate(x1,y1,x, y, self.angle)
            #x2,y2 = self.rotate(x2,y2,cx, cy, side_angle)
            x2,y2 = self.rotate(x2,y2,x, y, self.angle)
            self.ir_beam_side_left = self.canvas.create_line(x1,y1,x2,y2, fill="darkgreen")

    def do_us_display(self):
        x = self.x * self.display.zoom
        y = self.y * self.display.zoom
        size = self.size * self.display.zoom
        for beam in self.us_beam:
            self.canvas.delete(beam)
        while self.us_beam != []:
            self.us_beam.pop()
        if self.us_beam_on == FALSE:
            return
        o_x1 = x 
        o_y1 = y - (12*size)/40 
        o_x1,o_y1 = self.rotate(o_x1,o_y1,x, y, self.angle)
        for i in [0.5,1,1.5,2,2.5,3,3.5,4]:
            x2 = o_x1 + size*i
            y2 = o_y1 + size*i
            x1 = o_x1 - size*i
            y1 = o_y1 - size*i
            arc = self.canvas.create_arc(x1,y1,x2,y2, extent=46, start= 67 - (180 * self.angle / pi), style="arc", outline="grey", width = 3)
            self.us_beam.append(arc)

    def front_ir_erase(self):
        self.canvas.delete(self.ir_front_left)
        self.canvas.delete(self.ir_front_right)
        if self.ir_beam_front_left != 0:
            self.canvas.delete(self.ir_beam_front_left)
            self.canvas.delete(self.ir_beam_front_right)

    def side_ir_erase(self):
        self.canvas.delete(self.ir_side_left)
        self.canvas.delete(self.ir_side_right)
        if self.ir_beam_side_left != 0:
            self.canvas.delete(self.ir_beam_side_left)
            self.canvas.delete(self.ir_beam_side_right)

    def erase(self):
        self.canvas.delete(self.base)
        self.canvas.delete(self.left_wheel)
        self.canvas.delete(self.right_wheel)
        self.canvas.delete(self.us_sensor1)
        self.canvas.delete(self.us_sensor2)
        self.front_ir_erase()
        self.side_ir_erase()

    def set_posn(self, x, y, angle):
        self.x = x
        self.y = y
        self.angle = angle
        self.erase()
        self.draw()

    def set_ir_angle(self, ir_angle_left, ir_angle_right):
        #zero angles are at 45 degrees.
        #invert sign so positive angles are clockwize
        self.ir_angle_left = ((pi/4) - ir_angle_left)
        self.ir_angle_right = (-(pi/4) - ir_angle_right)
        self.front_ir_erase()
        self.do_front_ir_display()
        self.do_us_display()

    def set_ir_dists(self, ir_dists):
        self.display.set_ir_front_dists(ir_dists[L]*26.0/100, ir_dists[R]*26/100)
        self.display.set_ir_side_dists(ir_dists[L+SIDE*2]*26/100, ir_dists[R+SIDE*2]*26/100)
        
    def set_us_dist(self, dist):
        self.display.set_us_dist(dist)
        
    def set_blob(self, num, x,y):
        zoom = self.display.zoom
        if (num <= 5):
            x1 = (x-5)*zoom
            y1 = (y-5)*zoom
            x2 = (x+5)*zoom
            y2 = (y+5)*zoom
            if self.blob[num] != 0:
                self.canvas.delete(self.blob[num])
            if ((num == 0 or num == 1) and self.ir_beam_front_on == FALSE):
                self.blob[num] == 0 
                return
            if ((num == 4) and self.us_beam_on == FALSE):
                self.blob[num] == 0 
                return
            if ((num == 2 or num == 3) and self.ir_beam_side_on == FALSE):
                self.blob[num] == 0 
                return

            self.blob[num] = self.canvas.create_polygon(x1,y1,x2,y1,x2,y2,x1,y2, fill="red")
        else:
            x1 = (x-2)*zoom
            y1 = (y-2)*zoom
            x2 = (x+2)*zoom
            y2 = (y+2)*zoom
            self.canvas.delete(self.blob[num])
            self.blob[num] = self.canvas.create_polygon(x1,y1,x2,y1,x2,y2,x1,y2, fill="pink")

    def clear_trail(self):
        while len(self.trail) > 0:
            self.trail[0].erase(self.canvas)
            self.trail.pop(0)
            
    def redraw_trail(self):
        for blob in self.trail:
            blob.redraw(self.canvas, self.display.zoom)

    def set_trail(self, x, y):
        blob = Blob(x, y, "brown")
        self.trail.append(blob);
        blob.redraw(self.canvas, self.display.zoom)

    def clear_points(self):
        while len(self.points) > 0:
            self.points[0].erase(self.canvas)
            self.points.pop(0)

    def redraw_points(self):
        for blob in self.points:
            blob.redraw(self.canvas, self.display.zoom)

    def set_point(self, x, y):
        blob = Blob(x, y, "red")
        self.points.append(blob);
        blob.redraw(self.canvas, self.display.zoom)

