import sys
if sys.version_info[0] < 3:
    import Tkinter as Tk
    import ttk
else:
    import tkinter as Tk
    from tkinter import ttk as ttk


#Base class for all UI element
class UI:
    def __init__(self, root, connection):
        self.root = root
        self.connection = connection


    def set_connection(self, connection):
        self.connection = connection



#Special input with colored state
class CEntry:
    def __init__(self, root, connection, name, cmd):
        #create UI elements
        self.tk = Tk.LabelFrame(root, text=name)
        self.var = Tk.StringVar()
        self.entry = Tk.Entry(self.tk,width=10,textvariable=self.var)
        self.entry.bind("<KeyPress>", lambda x: self.keydown(x))
        self.entry.pack()
        self.cmd = cmd

        #register command in protocol class
        connection.register(self.cmd, lambda x: self.handle(x))
        self.set_connection(connection)


    def set_connection(self, connection):
        #if connection has changed 
        self.connection = connection
        self.connection.send(self.cmd)


    def handle(self, event):
        #new value received. Put entry in green
        self.entry.config(bg="lightgreen")
        self.var.set(str(event[1][0])) 


    def keydown(self, e):
        #entry value has changed. Put entry in yellow
        self.entry.config(bg="yellow")
        if e.char == '\r':
            #enter has been pressed. Send UI
            self.connection.send(self.cmd, self.var.get())

