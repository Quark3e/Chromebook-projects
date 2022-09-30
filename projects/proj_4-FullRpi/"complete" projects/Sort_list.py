
import time
import math

print("\tThis sorter will sort the values from the mean value of x and y inaccuracies")
fileExtension = ".dat"
fileName = input("Enter filename (without .dat): ")
rowLength = int(input("\nEnter number of rows in the file (note: The file can't have #xyz comments): "))

f = open(fileName + fileExtension, "r")

allRows = ["test"] * rowLength
sortedRows = ["test"] * rowLength
for i in range(0, rowLength):
    allRows[i] = f.readline()



f.close()