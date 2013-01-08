#!/opt/local/bin/python3.2
from __future__ import division 
import sys
PYTHON_MAJOR = sys.version_info[0]

if PYTHON_MAJOR == 2:
    from Tkinter import *
    import tkFileDialog
else:
    from tkinter import *
    import tkinter.filedialog

import os
from src.loadscenery import SceneryParser
from src.constants import L, R, NO_NOISE, GAUSSIAN_NOISE, ANGULAR_NOISE
from math import sqrt

class Display(Frame):
    def __init__(self, root, zoom):
        __version__ = "124"
        version = ''.join(c for c in __version__ if c.isdigit())
        root.wm_title("RoboSim v" + __version__)
        self.windowsystem = root.call('tk', 'windowingsystem')
        #print(self.windowsystem)
        root.option_add('*tearOff', FALSE)

        #bind TCL's exit command to our exit command, so the quit
        #button works on MacOS under aqua
        root.createcommand('exit', self.exit_sim)

        menubar = Menu(root)
        root['menu'] = menubar
        self.count = 0
        self.zoom=zoom
        self.ir_front_enable = IntVar()
        self.ir_front_enable.set(0)
        self.ir_side_enable = IntVar()
        self.ir_side_enable.set(0)
        self.us_enable = IntVar()
        self.us_enable.set(0)
        self.ir_front_used = FALSE
        self.ir_side_used = FALSE
        self.us_used = FALSE
        self.centred = BooleanVar()
        if (self.windowsystem == 'aqua'): 
            #scrolling is too slow using aqua to centre by default
            self.centred.set(False)
        else:
            self.centred.set(True)
        self.joystick_shown = BooleanVar()
        self.joystick_shown.set(False)
        self.noise_model = IntVar()
        self.noise_model.set(0)

        #self.frame = Frame(root)
        #self.frame.pack()
        self.frame = root
        self.tb = Frame(self.frame, relief=GROOVE, bd=2)
        self.tb.pack(side = TOP, fill=X)
        self.leftf = Frame(self.tb)
        self.leftf.pack(side = LEFT)

        self.filemenu = Menu(menubar)
        menubar.add_cascade(menu=self.filemenu, label='File')
        self.filemenu.add_command(label="Load Scenery", command=self.load_pressed);

        #On aqua, we don't need a Quit entry because it's already on
        #the Python menu, but on everything else we do
        if self.windowsystem != 'aqua':
            self.filemenu.add_command(label="Quit", command=self.exit_sim);

        self.ctrlmenu = Menu(menubar)
        menubar.add_cascade(menu=self.ctrlmenu, label='Controls')
        self.ctrlmenu.add_checkbutton(label="Show Joystick", 
                                      variable=self.joystick_shown, 
                                      command=self.show_joystick,
                                      onvalue = TRUE, offvalue = FALSE)
        self.ctrlmenu.add_command(label="Reset Robot Position", command=self.reset_posn)
        self.ctrlmenu.add_checkbutton(label="Keep Robot Centred", 
                                       variable=self.centred, command=self.set_centre,
                                       onvalue = TRUE, offvalue = FALSE)

        self.modelmenu = Menu(menubar)
        menubar.add_cascade(menu=self.modelmenu, label='Model')
        self.modelmenu.add_radiobutton(label="No sensor noise",
                                       variable=self.noise_model, 
                                       value=NO_NOISE,
                                       command=self.set_noise_model)
        self.modelmenu.add_radiobutton(label="Gaussian noise model",
                                       variable=self.noise_model, 
                                       value=GAUSSIAN_NOISE,
                                       command=self.set_noise_model)
        self.modelmenu.add_radiobutton(label="Angular distance noise model",
                                       variable=self.noise_model, 
                                       value=ANGULAR_NOISE,
                                       command=self.set_noise_model)
        

        self.dash = Frame(self.tb, relief=GROOVE, bd=2)
        #self.dash.pack(side = LEFT)
        self.left_l = Label(self.dash, text="L")
        self.left_l.pack(side = LEFT)
        self.mf = Frame(self.dash);
        self.mf.pack(side = LEFT)
        self.fwd_l = Label(self.mf, text="Fwd")
        self.fwd_l.pack(side = TOP)
        self.joystick = Canvas(self.mf, relief=SUNKEN, bd=2, width=80,height=80)
        self.joystick.pack(side = TOP)
        self.jb = Frame(self.joystick, width=20, height=20, relief=RAISED, bd=3, bg="grey")
        self.jb.bind("<Button-1>", self.jb_press)
        self.jb.bind("<B1-Motion>", self.jb_move)
        self.jb.bind("<ButtonRelease-1>", self.jb_release)
        self.jb.bind("<Enter>", self.jb_enter)
        self.jb.bind("<Leave>", self.jb_leave)
        self.joystick.create_window(43, 43, window=self.jb, tags="stick")
        x,y = self.joystick.coords("stick")
        self.jb_pos = [x,y]
        self.back_l = Label(self.mf, text="Back")
        self.back_l.pack(side = TOP)
        self.right_l = Label(self.dash, text="R")
        self.right_l.pack(side = LEFT)

        
        sensor_frame = Frame(self.tb, relief=GROOVE, bd=2)
        sensor_frame.pack(side = LEFT, fill=Y)
        self.sensor_label = Label(sensor_frame, text="Sensors")
        self.sensor_label.pack(side=TOP)
                             
        ir_front_frame = Frame(sensor_frame, relief=GROOVE, bd=2)
        ir_front_frame.pack(side = LEFT, fill=Y)

        ir_front_on = Checkbutton(ir_front_frame, text="Front IR", variable=self.ir_front_enable,
                                  command=self.set_ir_front)
        ir_front_on.pack(side = TOP, fill=Y)
        range_frame = Frame(ir_front_frame)
        range_frame.pack(side=TOP)
        left_ir_frame = Frame(range_frame, relief=SOLID, bd=1, pady=3)
        left_ir_frame.pack(side = LEFT, padx=2)
        self.lservo = Scale(left_ir_frame, orient="horizontal", from_=-90, to=90, 
                            command=self.lservo_set, label="Left IR Angle")
        self.lservo.pack(side = LEFT, padx=2)
        ldist_f = Frame(left_ir_frame, bd=0)
        ldist_f.pack(side = LEFT)
        ldist_l = Label(ldist_f, text="Range")
        ldist_l.pack(side = TOP)
        self.left_dist = Label(ldist_f, text="0", width=4, relief=SUNKEN, bd=1)
        self.left_dist.pack(side = TOP)

        right_ir_frame = Frame(range_frame, relief=SOLID, bd=1, pady=3)
        right_ir_frame.pack(side = LEFT, padx=2)
        self.rservo = Scale(right_ir_frame, orient="horizontal", from_=-90, to=90,
                            command=self.rservo_set, label="Right IR Angle")
        self.rservo.pack(side = LEFT, padx=2)
        rdist_f = Frame(right_ir_frame, bd=0)
        rdist_f.pack(side = LEFT)
        rdist_l = Label(rdist_f, text="Range")
        rdist_l.pack(side = TOP)
        self.right_dist = Label(rdist_f, text="0", width=4, relief=SUNKEN, bd=1)
        self.right_dist.pack(side = TOP)

        ir_side_frame = Frame(sensor_frame, relief=GROOVE, bd=2)
        ir_side_frame.pack(side = LEFT, fill=Y)

        ir_side_on = Checkbutton(ir_side_frame, text="Side IR", variable=self.ir_side_enable,
                                  command=self.set_ir_side)
        ir_side_on.pack(side = TOP, fill=Y)
        range_frame = Frame(ir_side_frame)
        range_frame.pack(side=TOP)
        left_ir_frame = Frame(range_frame, relief=SOLID, bd=1, pady=3)
        left_ir_frame.pack(side = LEFT, padx=2)
        ldist_f = Frame(left_ir_frame, bd=0)
        ldist_f.pack(side = LEFT)
        ldist_l = Label(ldist_f, text="Left")
        ldist_l.pack(side = TOP)
        ldist_l = Label(ldist_f, text="Range")
        ldist_l.pack(side = TOP)
        self.side_left_dist = Label(ldist_f, text="0", width=4, relief=SUNKEN, bd=1)
        self.side_left_dist.pack(side = TOP)

        right_ir_frame = Frame(range_frame, relief=SOLID, bd=1, pady=3)
        right_ir_frame.pack(side = LEFT, padx=2)
        rdist_f = Frame(right_ir_frame, bd=0)
        rdist_f.pack(side = LEFT)
        rdist_l = Label(rdist_f, text="Right")
        rdist_l.pack(side = TOP)
        rdist_l = Label(rdist_f, text="Range")
        rdist_l.pack(side = TOP)
        self.side_right_dist = Label(rdist_f, text="0", width=4,  bd=1, relief=SUNKEN)
        self.side_right_dist.pack(side = TOP)

        us_frame = Frame(sensor_frame, relief=GROOVE, bd=2)
        us_frame.pack(side = LEFT, fill=Y)

        us_on = Checkbutton(us_frame, text="Ultrasound", variable=self.us_enable,
                                  command=self.set_us)
        us_on.pack(side = TOP, fill=Y)
#        us_range_frame = Frame(us_frame)
#        us_range_frame_frame.pack(side = LEFT, padx=2, fill=X)
        dist_f = Frame(us_frame, relief=SOLID, bd=1, pady=3)
        dist_f.pack(side = TOP, fill=X, padx=2)
        dist_l = Label(dist_f, text="U/S")
        dist_l.pack(side = TOP)
        dist_l = Label(dist_f, text="Range")
        dist_l.pack(side = TOP)
        self.us_dist = Label(dist_f, text="0", width=4, relief=SUNKEN, bd=1)
        self.us_dist.pack(side = TOP)


        zoom_frame = Frame(self.tb, relief=GROOVE, bd=2)
        zoom_frame.pack(side = LEFT, padx=2, fill=Y)
        view_label = Label(zoom_frame, text="View")
        view_label.pack(side = TOP)
#        zoom_centre = Checkbutton(zoom_frame, text="Centre Robot", variable=self.centre, 
#                                  command=self.set_centre)
#        zoom_centre.pack(side = TOP)
        zoom_scale = Scale(zoom_frame, orient="horizontal", from_=1, to=20, 
                           command=self.set_zoom, label="Zoom")
        zoom_scale.set(10)
        zoom_scale.pack(side = TOP, padx=2)
#        reset_posn_b = Button(zoom_frame, text="Reset Position", command=self.reset_posn)
#        reset_posn_b.pack(side = TOP)
#        zoom_f = Frame(zoom_frame, bd=0)
#        zoom_f.pack(side = LEFT)


        lcd_frame = Frame(self.tb, relief=GROOVE, bd=2)
        lcd_frame.pack(side = LEFT, padx=2, fill=Y)
        lcd_label = Label(lcd_frame, text="LCD")
        lcd_label.pack(side = TOP)
        lcd_disp_frame = Frame(lcd_frame, relief=SUNKEN, bd=2)
        lcd_disp_frame.pack(side = TOP)
        lcd_disp_line1 = Label(lcd_disp_frame, width = 16, text="", justify=LEFT, anchor="w",
                               bg="blue", fg="white", font="courier")
        lcd_disp_line1.pack(side = TOP)
        lcd_disp_line2 = Label(lcd_disp_frame, width = 16, text="", justify=LEFT, anchor="w",
                               bg="blue", fg="white", font="courier")
        lcd_disp_line2.pack(side = TOP)
        self.lcd_display = [lcd_disp_line1, lcd_disp_line2]

        self.mb = Frame(self.frame, relief=GROOVE, bd=2)
        self.mb.pack(side = TOP)
        self.hscroll = Scrollbar(self.mb, orient="horizontal")
        self.vscroll = Scrollbar(self.mb, orient="vertical")
        self.canvas = Canvas(self.mb, width=1000,height=700, 
                             xscrollcommand=self.hscroll.set,
                             yscrollcommand=self.vscroll.set,
                             scrollregion=(0,0,2000,2000))
        self.hscroll.config(command=self.canvas.xview)
        self.vscroll.config(command=self.canvas.yview)
        self.hscroll.pack(side = BOTTOM, fill=X, expand=TRUE)
        self.vscroll.pack(side = RIGHT, fill=Y, expand=TRUE)
        self.canvas.pack(side = LEFT, fill=BOTH, expand=TRUE)

        # preserve the default config colours - we'll need them later
        self.active_bg = self.vscroll.cget("activebackground");
        self.bg = self.vscroll.cget("bg");

        self.draw_grid()
        self.set_centre()

    def register_world(self, world):
        self.world = world

    def jb_press(self, event):
        self.offset = [event.x, event.y]

    def jb_release(self, event):
        x,y = self.joystick.coords("stick")
        xdiff = self.jb_pos[0] - x
        ydiff = self.jb_pos[1] - y
        self.joystick.move("stick", xdiff, ydiff)
        newx = x + xdiff
        newy = y + ydiff
        self.xval = (43 - newx) * 100.0 / 30.0
        self.yval = (43 - newy) * 100.0 / 30.0
        self.world.robot.set_speed(L, 0)
        self.world.robot.set_speed(R, 0)
        
    def foo(self):
        print("foo\n")

    def jb_move(self, event):
        x,y = self.joystick.coords("stick")
        xdiff = event.x - self.offset[0]
        ydiff = event.y - self.offset[1]
        newx = x + xdiff
        newy = y + ydiff
        if newx < 13:
            xdiff = 13 - x
            newx = 13
        if newx > 73:
            xdiff = 73 - x
            newx = 73
        if newy < 13:
            ydiff = 13 - y
            newy = 13
        if newy > 73:
            ydiff = 73 - y
            newy = 73
        self.joystick.move("stick", xdiff, ydiff)
        self.xval = (43 - newx) * 100.0 / 30.0
        self.yval = (43 - newy) * 100.0 / 30.0
        norm = sqrt(self.xval * self.xval + self.yval * self.yval)/100.0
        if (norm != 0):
            lspeed = (40 * self.yval/100.0 - 10 * self.xval / 100.0)/norm
            rspeed = (40 * self.yval/100.0 + 10 * self.xval / 100.0)/norm
        else:
            lspeed = 0
            rspeed = 0
        self.world.robot.set_speed(L, lspeed)
        self.world.robot.set_speed(R, rspeed)

    def jb_enter(self, event):
        self.jb.configure(bg="white")

    def jb_leave(self, event):
        self.jb.configure(bg="grey")

    def exit_sim(self):
        os._exit(0)

    def load_pressed(self):
        if PYTHON_MAJOR == 2:
            filename = tkFileDialog.askopenfilename()
        else:
            filename = tkinter.filedialog.askopenfilename()
        parser = SceneryParser(self, self.world, filename)

    def stop_robot(self):
        self.world.robot.stop()

    def fwd_robot(self):
        self.world.robot.fwd(20)

    def back_robot(self):
        self.world.robot.fwd(-20)

    def left_robot(self):
        self.world.robot.turn(-7)

    def right_robot(self):
        self.world.robot.turn(7)

    def set_display_text(self, line, disptext):
        self.lcd_display[line-1].configure(text=disptext[:16])

    def set_ir_front(self):
        if self.ir_front_enable.get() == 1:
            self.world.robot.show_front_ir(TRUE)
        else:
            self.world.robot.show_front_ir(FALSE)

    def set_ir_side(self):
        if self.ir_side_enable.get() == 1:
            self.world.robot.show_side_ir(TRUE)
        else:
            self.world.robot.show_side_ir(FALSE)

    def set_us(self):
        if self.us_enable.get() == 1:
            self.world.robot.show_us(TRUE)
        else:
            self.world.robot.show_us(FALSE)

    def set_noise_model(self):
        noise_model = self.noise_model.get()
        #print(noise_model)
        if noise_model == NO_NOISE:
            self.sensor_label.configure(text="Sensors (no noise)")
        elif noise_model == GAUSSIAN_NOISE:
            self.sensor_label.configure(text="Sensors (gaussian noise)")
        elif noise_model == ANGULAR_NOISE:
            self.sensor_label.configure(text="Sensors (angular noise)")
        self.world.robot.set_noise_model(noise_model)

    def set_zoom(self,value):
        self.zoom=int(value)/10.0
        self.redraw_grid()
        self.world.redraw()
        self.canvas.config(scrollregion=(0,0,2000*self.zoom,2000*self.zoom))

    def set_centre(self):
        if self.centred.get() == FALSE:
            self.hscroll.config(command=self.canvas.xview, activebackground=self.active_bg)
            self.vscroll.config(command=self.canvas.yview, activebackground=self.active_bg)
        else:
            self.hscroll.config(command=self.no_op, activebackground=self.bg)
            self.vscroll.config(command=self.no_op, activebackground=self.bg)

    def show_joystick(self):
        if self.joystick_shown.get() == FALSE:
            self.dash.pack_forget()
        else:
            self.dash.pack(side = LEFT)


    def reset_posn(self):
        self.world.robot.reset_posn()
        
    def no_op(self, *args):
        #do nothing
        x=0

    def lservo_set(self, value):
        self.world.robot.set_ir_angle(L, int(value))

    def rservo_set(self, value):
        self.world.robot.set_ir_angle(R, int(value))
        
    def set_sensor_visibility(self, ir_front, ir_side, us, line):
        self.ir_front_used = ir_front
        self.ir_side_used = ir_side
        self.us_used = us

    def set_ir_front_dists(self, left_dist, right_dist):
        if self.ir_front_used or self.ir_front_enable.get() == 1:
            self.left_dist.configure(text=int(left_dist))
            self.right_dist.configure(text=int(right_dist))
        else:
            self.left_dist.configure(text="")
            self.right_dist.configure(text="")

    def set_ir_side_dists(self, left_dist, right_dist):
        if self.ir_side_used or self.ir_side_enable.get() == 1:
            self.side_left_dist.configure(text=int(left_dist))
            self.side_right_dist.configure(text=int(right_dist))
        else:
            self.side_left_dist.configure(text="")
            self.side_right_dist.configure(text="")

    def set_us_dist(self, dist):
        if self.us_used or self.us_enable.get() == 1:
            self.us_dist.configure(text=int(dist))
        else:
            self.us_dist.configure(text="")

    def erase_grid(self):
        for line in self.lines:
            self.canvas.delete(line)

    def robot_position(self, x, y):
        self.count = self.count + 1
        if self.centred.get() == TRUE:
            w = int(self.canvas.cget("width"))
            h = int(self.canvas.cget("height"))
            self.canvas.xview_moveto((x - w/(2*self.zoom))/2000.0)
            self.canvas.yview_moveto((y - h/(2*self.zoom))/2000.0)

    def draw_grid(self):
        self.lines=[]
        for i in range(int(100)):
            self.lines.append(
                self.canvas.create_line(i*20*self.zoom,0,i*20*self.zoom,2000*self.zoom, fill="grey"))
        for i in range(int(100)):
            self.lines.append(self.canvas.create_line(0,i*20*self.zoom,2000*self.zoom,i*20*self.zoom, fill="grey"))

    def redraw_grid(self):
        self.erase_grid()
        self.draw_grid()


