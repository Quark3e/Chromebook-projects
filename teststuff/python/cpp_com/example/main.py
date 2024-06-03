#!/usr/bin/env python3

# ctypes_test.py
import ctypes
import pathlib

if __name__ == "__main__":
    # Load the shared library into ctypes
    libname = pathlib.Path().absolute() / "cmult.c"
    c_lib = ctypes.CDLL(libname)