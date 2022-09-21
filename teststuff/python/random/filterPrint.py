
#creates a .dat file with the x=iteration, y=value for the iteration filter

f = open("filter_6-4.dat", "w")
f.write("#x y\n")
# open and read the file after the appending:
#f = open("demofile2.txt", "r")
#print(f.read())

x = 0
a, b = 0, 100
y = a
k1, k2 = 0.4, 0.6
n=0
y2 = y - 1
while y<b:
    print("n:", n, " y:", y)
    f.write(str(n) + " " + str(y) + "\n")
    #f.write(str(n) + " " + str(k1*b-k2*y) + "\n")
    n+=1
    y=k1*b+k2*y
    if y == y2:
        break
    y2 = y

f.close()