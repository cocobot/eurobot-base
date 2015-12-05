import sys
if sys.version_info[0] < 3:
    import Tkinter as Tk
    from tkinter import ttk as ttk
else:
    import tkinter as Tk
    from tkinter import ttk as ttk
from .ui import UI
from .ui import CEntry
from matplotlib.figure import Figure
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import matplotlib.animation as animation
import time

#Asserv charts and configuration
class Asserv(UI):
    def __init__(self, root, connection):
        UI.__init__(self, root, connection)
        self.start = time.time() - 10

        #craete UI elements
        group = Tk.LabelFrame(root, text="Asserv")
        self.tk = group
        group.columnconfigure(0, weight=1)
        group.columnconfigure(1, weight=1)
        group.columnconfigure(2, weight=1)
        group.columnconfigure(3, weight=1)
        group.rowconfigure(0, weight=0)
        group.rowconfigure(1, weight=0)
        group.rowconfigure(2, weight=1)

        #create enable buttons
        self.debug_distance_ramp = Tk.IntVar()
        self.debug_angular_ramp = Tk.IntVar()
        self.debug_distance_pid = Tk.IntVar()
        self.debug_angular_pid = Tk.IntVar()
        Tk.Checkbutton(group, text="Debug distance ramp", variable=self.debug_distance_ramp, command=self.update_enable).grid(row=0, column=0)
        Tk.Checkbutton(group, text="Debug angular ramp", variable=self.debug_angular_ramp, command=self.update_enable).grid(row=0, column=1)
        Tk.Checkbutton(group, text="Debug distance PID", variable=self.debug_distance_pid, command=self.update_enable).grid(row=1, column=0)
        Tk.Checkbutton(group, text="Debug angular PID", variable=self.debug_angular_pid, command=self.update_enable).grid(row=1, column=1)


        #create charts
        self.figure = Figure()
        self.plot_dr = self.generate_ramp_graph(221, 'distance')
        self.plot_ar = self.generate_ramp_graph(222, 'angular')

        #add charts to UI
        self.canvas = FigureCanvasTkAgg(self.figure, master=group)
        self.figure.tight_layout()
        self.ani = animation.FuncAnimation(self.figure, self.animate, interval=250)
        self.canvas.get_tk_widget().grid(row=2, column=0, columnspan=4, sticky=Tk.W + Tk.E)
        self.canvas.show()

        #register commands in protocol class
        self.connection.register('ramp_distance', lambda x: self.handle_ramp_data(x, self.plot_dr))
        self.connection.register('ramp_angular', lambda x: self.handle_ramp_data(x, self.plot_ar))


    def generate_ramp_graph(self, subplot, name):
        plot = dict()
        
        #create plots
        plot['plot'] = [None] * 2
        plot['plot'][0] = self.figure.add_subplot(subplot)
        plot['plot'][1] = plot['plot'][0].twinx()

        #set margin
        plot['plot'][0].set_ymargin(0.1)
        plot['plot'][1].set_ymargin(0.1)

        #hack color cycle for secondary axe
        next(plot['plot'][1]._get_lines.prop_cycler)
        next(plot['plot'][1]._get_lines.prop_cycler)
        next(plot['plot'][1]._get_lines.prop_cycler)

        #create charts
        plot['data'] = [[float('nan')] * 200]
        plot['data'] += [[float('nan')] * 200]
        plot['chart'] = plot['plot'][0].plot(plot['data'][0], plot['data'][1], label='target')
        plot['data'] += [[float('nan')] * 200]
        plot['chart'] += plot['plot'][0].plot(plot['data'][0], plot['data'][2], label='position')
        plot['data'] += [[float('nan')] * 200]
        plot['chart'] += plot['plot'][0].plot(plot['data'][0], plot['data'][3], label='output')
        plot['data'] += [[float('nan')] * 200]
        plot['chart'] += plot['plot'][1].plot(plot['data'][0], plot['data'][4], label='speed target')
        plot['data'] += [[float('nan')] * 200]
        plot['chart'] += plot['plot'][1].plot(plot['data'][0], plot['data'][5], label='speed')

        #generate legends
        h1, l1 = plot['plot'][0].get_legend_handles_labels()
        h2, l2 = plot['plot'][1].get_legend_handles_labels()
        plot['plot'][0].legend(h1 + h2, l1 + l2, prop={'size':10},bbox_to_anchor=(0., 1.02, 1., .102), loc=3, ncol=5, mode="expand", borderaxespad=0.)

        return plot


    def handle_ramp_data(self, event, storage):
        now = time.time() - self.start
        #parse incomming data and add them to the data list
        data = list(map(lambda x: float(x), event[1].split(',')))
        storage['data'][0] = storage['data'][0][1:] + [now]
        for i in range(1, len(storage['data'])):
            storage['data'][i] = storage['data'][i][1:] + [data[i - 1]]


    def animate(self, i):
        now = time.time() - self.start
        self.animate_ramp(now, self.plot_dr)
        self.animate_ramp(now, self.plot_ar)

    
    def animate_ramp(self, now, storage):
        #update all plots
        for i in range(0, len(storage['chart'])):
            storage['chart'][i].set_data(storage['data'][0], storage['data'][i + 1])
        #reset axis ranges
        storage['plot'][0].relim()
        storage['plot'][0].set_xlim([now - 10, now])
        storage['plot'][0].autoscale_view(False,False,True)
        storage['plot'][1].relim()
        storage['plot'][1].set_xlim([now - 10, now])
        storage['plot'][1].autoscale_view(False,False,True)


    def update_enable(self):
        #update requested debug
        if self.debug_distance_ramp.get():
            self.connection.send("ramp_distance_debug", 1)
        else:
            self.connection.send("ramp_distance_debug", 0)
        if self.debug_angular_ramp.get():
            self.connection.send("ramp_angular_debug", 1)
        else:
            self.connection.send("ramp_angular_debug", 0)

    def set_connection(self, connection):
        #connection has changed, resend data
        UI.set_connection(self, connection)
        self.update_enable()
        #self.speed.set_connection(connection)
        #self.accel.set_connection(connection)
