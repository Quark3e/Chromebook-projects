
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

print ""

d1 = 140 #axial "roll"
d2 = 135 #axial "pitch"
d3 = 70 #axial "pitch"
d4 = 80 #axial "roll"
d5 = 45 #axial "pitch
d6 = 45 #axial "roll" (?)

q1 = 0
q2 = 0
q3 = 0
q4 = 0
q5 = 0
q6 = 0

x = 0
y = 200
z = 200
a = 0 #radians
b = 0
Y = 0

l = (d5+d6)*cos(b)
posX2 = x - l*sin(a)
posY2 = y - l*cos(a)
posZ2 = z - (d5+d6)*sin(b)

print int(posX2), int(posY2), int(posZ2)

if (posY2 < 0) { posY2 = 0 }
if (posY2 == 0) {
    if (posX2 > 0) { q1=90 }
    if (posX2 < 0) { q1=-90 }
    if (posX2 == 0) { q1=0 }
} else { q1 = atan(posX2/posY2) }

q3 = acos(((posX2)**2+(posY2)**2 + (posZ2-d1)**2 - d2**2 - (d3 + d4)**2) / (2*d2*(d3+d4)))

lambdaVar = atan((posZ2 - d1) / sqrt((posX2)**2+(posY2)**2))
muVar = atan( ((d3+d4)*sin(q3)) / (d2+(d3+d4)*cos(q3)) )

q3 = 0-q3

print "lambda", lambdaVar
print "mu", muVar

q2 = lambdaVar+muVar

a1 = a-q1
b1 = b-(q2+q3)

d5x = (d5+d6)*cos(b1)*sin(a1)
d5z = (d5+d6)*sin(b1)

if (b1==0) {
    if (d5x>0) {q4 = 90}
    if (d5x<0) {q4 = -90}
    if (d5x==0) {q4 = 0}
}
if (b1<0 || b1>0) { q4=atan(d5x/d5z) }

test = sqrt(d5x**2+d5z**2)/(d5+d6)
if (test < 1 || test > -1) {
    q5 = asin(sqrt(d5x**2+d5z**2)/(d5+d6))
} else { q5 = 0; print "q5 out of reach"; }
if (d5z<0) { q5=0-q5 }

if (b<=toDegrees(pi)/2 && b>=0-toDegrees(pi)/2) { q6 = Y-q4 }
if (b>=toDegrees(pi)/2 || b<=0-toDegrees(pi)/2) { q6 = toDegrees(pi)-(Y-q6) }

print a1, b1
print int(q1), int(q2), int(q3), int(q4), int(q5), int(q6)

#Note: Indexing in gnuplot starts at 1, not 0
array P1[3] = [0, 0, 0]
array P2[3] = [0, 0, d1]
array P3[3] = [d2*cos(q2)*sin(q1), d2*cos(q2)*cos(q1), d1+d2*sin(q2)]
array P4[3] = [P3[1]+d3*cos(q2+q3)*sin(q1), P3[2]+d3*cos(q2+q3)*cos(q1), P3[3]+d3*sin(q2+q3)]
array P5[3] = [P3[1]+(d3+d4)*cos(q2+q3)*sin(q1), P3[2]+(d3+d4)*cos(q2+q3)*cos(q1), P3[3]+(d3+d4)*sin(q2+q3)]

b = asin(((d5+d6)*sin(q5)*cos(q4))/(d5+d6))+q2+q3
if (toDegrees(q5) == 90 || toDegrees(q5) == -90) {
    if (((d5+d6)*sin(q5)*cos(q4)) == 0) { a=0 }
    if (((d5+d6)*sin(q5)*cos(q4)) > 0) { a = 90 }
    if (((d5+d6)*sin(q5)*cos(q4)) < 0) { a = -90 }
} else {
    a = q1+atan(((d5+d6)*sin(q5)*sin(q4)) / ((d5+d6)*cos(q5)))
}
print "read alpha and beta: ", a, b
array PP[3] = [P5[1]+(d5+d6)*sin(a)*sin(b), P5[2]+(d5+d6)*cos(b)*cos(a), P5[3]+(d5+d6)*sin(b)]
array P6[3] = [P5[1]+(d5)*sin(a)*sin(b), P5[2]+(d5)*cos(b)*cos(a), P5[3]+(d5)*sin(b)]

set title "in-program line connected plotting: x".x." y".y." z".z

set view 90, 90, 1.1, 1

print "\n"
print "P1", P1
print "P2", P2
print "P3", P3
print "P4", P4
print "P5", P5
print "PP", PP


splot P1 u (P1[1]):(P1[2]):(P1[3]):(P2[1]-P1[1]):(P2[2]-P1[2]):(P2[3]-P1[3]) with vectors nohead ls 1 title "d1", \
P2 u (P2[1]):(P2[2]):(P2[3]):(P3[1]-P2[1]):(P3[2]-P2[2]):(P3[3]-P2[3]) with vectors nohead ls 2 title "d2", \
P3 u (P3[1]):(P3[2]):(P3[3]):(P4[1]-P3[1]):(P4[2]-P3[2]):(P4[3]-P3[3]) with vectors nohead ls 3 title "d3", \
P4 u (P4[1]):(P4[2]):(P4[3]):(P5[1]-P4[1]):(P5[2]-P4[2]):(P5[3]-P4[3]) with vectors nohead ls 4 title "d4", \
P5 u (P5[1]):(P5[2]):(P5[3]):(PP[1]-P5[1]):(PP[2]-P5[2]):(PP[3]-P5[3]) with vectors nohead ls 5 title "d5", \
'+' u (P1[1]):(P1[2]):(P1[3]) lt rgb "blue" title "P1", \
'+' u (P2[1]):(P2[2]):(P2[3]) lt rgb "red" title "P2", \
'+' u (P3[1]):(P3[2]):(P3[3]) lt rgb "green" title "P3", \
'+' u (P4[1]):(P4[2]):(P4[3]) lt rgb "orange" title "P4", \
'+' u (P5[1]):(P5[2]):(P5[3]) lt rgb "grey" title "P5", \
'+' u (P6[1]):(P6[2]):(P6[3]) lt rgb "purple" title "P6", \
'+' u (posX2):(posY2):(posZ2) lt rgb "brown" title "pos2", \
'+' u (PP[1]):(PP[2]):(PP[3]) lt rgb "black" title "Given pos"



