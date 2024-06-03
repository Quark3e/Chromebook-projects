#!/usr/bin/env python3

import os.path

filePath = str(os.path.realpath(__file__))[:len(str(os.path.realpath(__file__)))-len("absPath.py")]
#filePath = filePath

print(filePath)
