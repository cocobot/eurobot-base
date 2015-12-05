#!/usr/bin/env python

import sys
if sys.version_info[0] < 3:
    import Tkinter as Tk
else:
    import tkinter as Tk
import cocoui.protocol
import cocoui.freeRTOS
import cocoui.asserv

#connection variable
pcon = cocoui.protocol.DummyProtocol()

#list of ui elements
uilist = []

#handle connection
def connect():
    global bcon
    global pcon
    #generate new connection and transfert broadcast map for smooth transistion
    old = pcon
    pcon = cocoui.protocol.TCPProtocol()
    pcon.broadcast_map = old.broadcast_map
    #change button to deconnect
    bcon.config(text="Deconnexion", command=deconnect)
    #update ui elements
    for ui in uilist:
        ui.set_connection(pcon)


#handle deconnection
def deconnect():
    global bcon
    global pcon
    #generate new fake connection and transfert broadcast map
    pcon.close()
    old = pcon
    pcon = cocoui.protocol.DummyProtocol()
    pcon.broadcast_map = old.broadcast_map
    #change button to connect
    bcon.config(text="Connexion", command=connect)
    #update ui elements
    for ui in uilist:
        ui.set_connection(pcon)


#handle network in UI thread 
def process_broadcast():
    global pcon
    pcon.process_broadcast()
    root.after(100, process_broadcast)

#generate base UI
root = Tk.Tk()
root.columnconfigure(0, weight=1)
root.columnconfigure(1, weight=1)
root.rowconfigure(0, weight=0, minsize=20)
root.rowconfigure(1, weight=0, minsize=20)
root.rowconfigure(2, weight=1)
root.wm_title("Cocoui")


#generate connection group
group = Tk.LabelFrame(root, text="Connexion")
group.grid(row=0, column=0, columnspan=2)
connection_type = Tk.IntVar()
connection_type.set(1)
Tk.Radiobutton(group, text="Simu", variable=connection_type, value=1).grid(row=0, column=0)
Tk.Radiobutton(group, text="UART", variable=connection_type, value=2).grid(row=0, column=1)
connection_port = Tk.StringVar()
connection_port.set("/dev/ttyUSB0")
Tk.Entry(group, textvariable=connection_port).grid(row=0, column=2)
bcon = Tk.Button(group, text="Connexion", command=connect)
bcon.grid(row=0, column=3)


#add freeRTOS UI element
ui = cocoui.freeRTOS.FreeRTOS(root,pcon)
ui.tk.grid(row=1, column=0, columnspan=2)
uilist.append(ui)


#add asserv UI element
ui = cocoui.asserv.Asserv(root,pcon)
ui.tk.grid(row=2, column=0, columnspan=2, sticky=Tk.W + Tk.E)
uilist.append(ui)

#run
root.after(100, process_broadcast)
Tk.mainloop()
