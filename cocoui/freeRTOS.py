import sys
if sys.version_info[0] < 3:
    import Tkinter as Tk
    import ttk
else:
    import tkinter as Tk
    from tkinter import ttk as ttk
from .ui import UI


#FreeRTOS debuging information
class FreeRTOS(UI):
    def __init__(self, root, connection):
        UI.__init__(self, root, connection)

        #create UI elements
        group = Tk.LabelFrame(root, text="FreeRTOS")
        self.tk = group

        #create enable button
        self.activation = Tk.IntVar()
        Tk.Checkbutton(group, text="Auto-refresh", variable=self.activation, command=self.autorefresh).pack(anchor=Tk.W)

        #create treeview
        self.tree = ttk.Treeview(group, height=5)
        self.tree['columns'] = ('Task name', 'Current priority', 'Base priority', 'Run time counter', 'Highest stack')
        for i in self.tree['columns']:
            self.tree.heading(i, text=i)
        self.tree.pack(anchor=Tk.S)

        #register command in protocol class
        self.connection.register('freertos', lambda x: self.handle(x))


    def autorefresh(self):
        #if debug is activated, request data every 2s
        if self.activation.get():
            self.connection.send("freertos")
            self.root.after(2000, self.autorefresh)


    def handle(self, event):
        if event[0] == "freertos":
            id = 0
            #clear UI
            for i in self.tree.get_children():
                self.tree.delete(i)
            #fill UI
            for task in event[1]:
                self.tree.insert("" , 'end', text=str(id), values=(task.split(',')))
                id += 1
