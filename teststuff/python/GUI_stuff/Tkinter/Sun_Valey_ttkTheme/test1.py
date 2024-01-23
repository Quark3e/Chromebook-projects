#!/usr/bin/env python3


import tkinter
from tkinter import ttk

import sv_ttk

root = tkinter.Tk()

button = ttk.Button(root, text="Button")
button.pack()

sv_ttk.use_dark_theme()
# sv_ttk.use_light_theme()

root.mainloop()