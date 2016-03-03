#!/usr/bin/env python2

import cocoui.ui

#################################################################

def cocoui_server():
    # ui thread
    ui_instance = cocoui.ui.UI(port=7000)
    ui_instance.run()


if __name__ == '__main__':
    cocoui_server()
