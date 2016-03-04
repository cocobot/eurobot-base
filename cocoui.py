#!/usr/bin/env python2

import cocoui.ui
import cocoui.protocol

#################################################################

def cocoui_server():
    #protocol
    protocol = cocoui.protocol.SerialProtocol(port="/dev/ttyUSB0")
    protocol.run()

    # ui thread
    ui_instance = cocoui.ui.UI(protocol=protocol, port=7000)
    ui_instance.run()


if __name__ == '__main__':
    cocoui_server()
