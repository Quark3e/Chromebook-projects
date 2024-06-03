
set xlabel "x -axis"
set ylabel "y -axis"
set zlabel "z -axis"

set xrange[-300:300]
set yrange[-25:250]
set zrange[0:300]

set grid
set cntrparam

set ticslevel 0

set angles degrees

set style fill transparent

toRadians(deg) = (deg*180)/pi
toDegrees(rad) = (rad*pi)/180

getDistance(x1, y1, z1, x2, y2, z2) = sqrt((x2-x1)**2+(y2-y1)**2+(z2-z1)**2)

print ""

d1 = 140 #axial "roll"
d2 = 135 #axial "pitch"
d3 = 70 #axial "pitch"
d4 = 80 #axial "roll"
d5 = 45 #axial "pitch
d6 = 30 #axial "roll" (?)

q1 = 0
q2 = 0
q3 = 0
q4 = 0
q5 = 0
q6 = 0


posX = -150
posY = 150
posZ = 50
a = 0
b = -90 #29 and 30 is the error...
Y = 0

label_ab = " a:".int(a)." b:".int(b)

posX = 0-posX
# set view 99, 295, 1.1, 1 #for -150, 150, 150, 0:-90:0
set view 45, 205, 1.5, 1


l = (d5+d6)*cos(b)
posX2 = posX - l*sin(a)
posY2 = posY - l*cos(a)
posZ2 = posZ - (d5+d6)*sin(b)

print int(posX2), int(posY2), int(posZ2)

if (posY2 < 0) { posY2 = 0 }
if (posY2 == 0) {
    if (posX2 > 0) { q1=90 }
    if (posX2 < 0) { q1=-90 }
    if (posX2 == 0) { q1=0 }
} else { q1 = atan(-posX2/posY2) }

q3 = acos(((posX2)**2+(posY2)**2 + (posZ2-d1)**2 - d2**2 - (d3 + d4)**2) / (2*d2*(d3+d4)))

lambdaVar = atan((posZ2 - d1) / sqrt((posX2)**2+(posY2)**2))
muVar = atan( ((d3+d4)*sin(q3)) / (d2+(d3+d4)*cos(q3)) )


print "lambda", lambdaVar
print "mu", muVar

q2 = lambdaVar+muVar

q3 = 0-q3

a1 = a-q1
a1 = 0 
#NOTE: check with prototype on whether the orientation AND position checks out 
#      (and if the simulation is wrong)
b1 = b-(q2+q3)

if (b1>90 || b1<-90) {
    set label "b1 exceeded 90/-90 limit" at 0,200,0
}

# q3 = 0-q3

d5x = (d5+d6)*cos(b1)*sin(a1)
# d5x = 0
d5z = (d5+d6)*sin(b1)

print " d5x:", d5x, \
"\n d5z:", d5z

if (b1==0) {
    if (d5x>0) {q4 = 90}
    if (d5x<0) {q4 = -90}
    if (d5x==0) {q4 = 0}
}
if (b1<0 || b1>0) {
    q4=atan(d5x/d5z)
    # q4=atan()
}

test = sqrt(d5x**2+d5z**2)/(d5+d6)
if (test < 1 || test > -1) {
    q5 = asin(sqrt(d5x**2+d5z**2)/(d5+d6))
} else { q5 = 0; print "q5 out of reach"; }
if (d5z<0) { q5=-q5 } #doesnt matter if b>0

if (b<=toDegrees(pi)/2 && b>=0-toDegrees(pi)/2) { q6 = Y-q4 }
if (b>=toDegrees(pi)/2 || b<=0-toDegrees(pi)/2) { q6 = toDegrees(pi)-(Y-q6) }



print " a1:", a1, \
"\n b1:", b1
print ""
print int(q1), int(q2), int(q3), int(q4), int(q5), int(q6)

# Forward kinematics start here

#Note: Indexing in gnuplot starts at 1, not 0
array P1[3] = [0, 0, 0]
array P2[3] = [0, 0, d1]
array P3[3] = [d2*cos(q2)*sin(q1), d2*cos(q2)*cos(q1), d1+d2*sin(q2)]
array P4[3] = [P3[1]+d3*cos(q2+q3)*sin(q1), P3[2]+d3*cos(q2+q3)*cos(q1), P3[3]+d3*sin(q2+q3)]
array P5[3] = [P3[1]+(d3+d4)*cos(q2+q3)*sin(q1), P3[2]+(d3+d4)*cos(q2+q3)*cos(q1), P3[3]+(d3+d4)*sin(q2+q3)]

b = asin(((d5+d6)*sin(q5)*cos(q4))/(d5+d6))+q2+q3
if q5 == 90 || q5 == -90) {
    if (((d5+d6)*sin(q5)*cos(q4)) == 0) { a=0 }
    if (((d5+d6)*sin(q5)*cos(q4)) > 0) { a = 90 }
    if (((d5+d6)*sin(q5)*cos(q4)) < 0) { a = -90 }
} else {
    a = q1+atan(((d5+d6)*sin(q5)*sin(q4)) / ((d5+d6)*cos(q5)))
}

print "read alpha and beta: ", a, b
array P6[3] = [P5[1]+(d5)*sin(a)*sin(b), P5[2]+(d5)*cos(b)*cos(a), P5[3]+(d5)*sin(b)]
array PP[3] = [P5[1]+(d5+d6)*sin(a)*sin(b), P5[2]+(d5+d6)*cos(b)*cos(a), P5[3]+(d5+d6)*sin(b)]

r_d1 = getDistance(P1[1], P1[2], P1[3], P2[1], P2[2], P2[3])
r_d2 = getDistance(P2[1], P2[2], P2[3], P3[1], P3[2], P3[3])
r_d3 = getDistance(P3[1], P3[2], P3[3], P4[1], P4[2], P4[3])
r_d4 = getDistance(P4[1], P4[2], P4[3], P5[1], P5[2], P5[3])
r_d5 = getDistance(P5[1], P5[2], P5[3], P6[1], P6[2], P6[3])
r_d6 = getDistance(P6[1], P6[2], P6[3], PP[1], PP[2], PP[3])

set title "in-program line connected plotting: x".posX." y".posY." z".posZ

label_q1 = " q1:".int(q1)
label_q2 = " q2:".int(q2)
label_q3 = " q3:".int(q3)
label_q4 = " q4:".int(q4*10**0)
label_q5 = " q5:".int(q5)
label_q6 = " q6:".int(q6)
label_a1b1 = " a1:".int(a1)." b1:".int(b1)
label_readOrie = " read a:".int(a)." b:".int(b)
label_frame2 = " d5x:".int(d5x)." d5z:".int(d5z)

label_rd1 = " rd_1:".int(ceil(r_d1))
label_rd2 = " rd_2:".int(ceil(r_d2))
label_rd3 = " rd_3:".int(ceil(r_d3))
label_rd4 = " rd_4:".int(ceil(r_d4))
label_rd5 = " rd_5:".int(ceil(r_d5))
label_rd6 = " rd_6:".int(ceil(r_d6))
set label label_rd1 at P1[1]-75,33,0
set label label_rd2 at P1[1]-75,66,0
set label label_rd3 at P1[1]-75,99,0
set label label_rd4 at P1[1]-75,133,0
set label label_rd5 at P1[1]-75,166,0
set label label_rd6 at P1[1]-75,200,0

set label label_q1 at P1[1],P1[2],P1[3]
set label label_q2 at P2[1],P2[2],P2[3]
set label label_q3 at P3[1],P3[2],P3[3]
set label label_q4 at P4[1],P4[2],P4[3]
set label label_q5 at P5[1],P5[2],P5[3]
set label label_q6 at P6[1],P6[2],P6[3]
set label label_ab at 250,150,220
set label label_a1b1 at 250,150,200
set label label_readOrie at 250,150,100
set label label_frame2 at 250,150,180


print "\n P1", P1, \
"P2", P2, \
"P3", P3, \
"P4", P4, \
"P5", P5, \
"PP", PP


splot P1 u (P1[1]):(P1[2]):(P1[3]):(P2[1]-P1[1]):(P2[2]-P1[2]):(P2[3]-P1[3]) with vectors nohead ls 1 title "d1", \
P2 u (P2[1]):(P2[2]):(P2[3]):(P3[1]-P2[1]):(P3[2]-P2[2]):(P3[3]-P2[3]) with vectors nohead ls 2 title "d2", \
P3 u (P3[1]):(P3[2]):(P3[3]):(P4[1]-P3[1]):(P4[2]-P3[2]):(P4[3]-P3[3]) with vectors nohead ls 3 title "d3", \
P4 u (P4[1]):(P4[2]):(P4[3]):(P5[1]-P4[1]):(P5[2]-P4[2]):(P5[3]-P4[3]) with vectors nohead ls 4 title "d4", \
P5 u (P5[1]):(P5[2]):(P5[3]):(PP[1]-P5[1]):(PP[2]-P5[2]):(PP[3]-P5[3]) with vectors nohead ls 5 title "d5", \
'+' u (P1[1]):(P1[2]):(P1[3]) lt rgb "blue" title "P1: ".int(q1), \
'+' u (P2[1]):(P2[2]):(P2[3]) lt rgb "red" title "P2: ".int(q2), \
'+' u (P3[1]):(P3[2]):(P3[3]) lt rgb "green" title "P3: ".int(q3), \
'+' u (P4[1]):(P4[2]):(P4[3]) lt rgb "orange" title "P4: ".int(q4), \
'+' u (P5[1]):(P5[2]):(P5[3]) lt rgb "grey" title "P5: ".int(q5), \
'+' u (P6[1]):(P6[2]):(P6[3]) lt rgb "purple" title "P6: ".int(q6), \
'+' u (-posX2):(posY2):(posZ2) lt rgb "brown" title "pos2", \
'+' u (PP[1]):(PP[2]):(PP[3]) lt rgb "black" title "PP", \
'+' u (-posX):(posY):(posZ) lt rgb "red" title "given pos"