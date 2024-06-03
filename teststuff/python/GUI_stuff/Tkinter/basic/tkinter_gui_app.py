#!/usr/bin/env python3


import tkinter as tk

def main():
    # Create a Tkinter window
    window = tk.Tk()

    # Set the window title
    window.title('My GUI App')

    # Set the window size
    window.geometry('300x200')

    # Create a label
    label = tk.Label(window, text='Hello, Tkinter!')
    label.pack()

    # Create a button
    button = tk.Button(window, text='Click me!', command=on_button_click)
    button.pack()

    # Start the main event loop
    window.mainloop()

def on_button_click():
    print('Button clicked!')

if __name__=="__main__":
    main()
