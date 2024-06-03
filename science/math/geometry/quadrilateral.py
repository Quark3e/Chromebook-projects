
import math
import numpy as np

toRadians = lambda degrees: (degrees*np.pi)/180
toDegrees = lambda radians: (radians*180)/np.pi



class quadrilateral(object):
    a=1
    b=1     #constant (stays 1)
    c=1
    d=1    

    alpha=1
    beta=1
    gamma=1
    delta=1

    e=1    
    f=1    

    p=1    
    area=1
    gamma1=1
    gamma2=1

    absAng=0 #absolute angle of side c relative to horizontal axis

    isReachable=True
    printErrors=True
    def __init__(self):
        self.solveVars()
    def solve_e(self):
        self.e = math.sqrt(self.a**2 + self.b**2 - 2*self.a*self.b*math.cos(toRadians(self.beta)))
        return self.e
    def solve_f(self):
        self.solve_gamma()
        self.f = math.sqrt(self.b**2 + self.c**2 - 2*self.b*self.c*math.cos(toRadians(self.gamma)))
        return self.f
    def solve_gamma1(self):
        try:
            self.gamma1 = toDegrees(math.acos(round((self.b**2 + self.e**2 - self.a**2) / (2*self.b*self.e), 10)))
            return self.gamma1
        except ValueError:
            self.isReachable = False
            if self.printErrors: print("gamma1 ValueError.")
            return None
        except ZeroDivisionError:
            self.isReachable = False
            if self.printErrors: print("gamma1 ZeroDivisionError")
            return None
    def solve_gamma2(self):
        self.solve_e()
        try:
            if self.e==0: raise ValueError("\"self.e==0\"")
            self.gamma2 = toDegrees(math.acos(round((self.c**2 + self.e**2 - self.d**2) / (2*self.c*self.e), 10)))
            return self.gamma2
        except ValueError:
            self.isReachable = False
            if self.printErrors: print(f"gamma2 ValueError")
            return None
        except ZeroDivisionError:
            self.isReachable = False
            if self.printErrors: print(f"gamma2 ZeroDivisionError")
            return None
    def solve_gamma(self):
        self.solve_gamma2()
        self.solve_gamma1()
        self.gamma = self.gamma1 + self.gamma2
        return self.gamma
    def solve_d(self):
        self.solve_gamma2()
        self.d = math.sqrt(self.c**2 + self.e**2 - 2*self.c*self.e*math.cos(toRadians(self.gamma2)))
        return self.d
    def solve_alpha(self):
        self.solve_f()
        try:
            self.alpha = toDegrees(math.acos((self.a**2 + self.d**2 - self.f**2) / (2*self.a*self.d)))
            return self.alpha
        except ValueError:
            self.isReachable = False
            if self.printErrors: print("alpha ValueError.")
            return None
    def solve_delta(self):
        self.solve_alpha()
        self.solve_gamma()
        self.delta = 360 - self.alpha - self.beta - self.gamma
        return self.delta
    def solve_perimeter(self):
        self.p = self.a + self.b + self.c + self.d
        return self.p
    def solve_area(self):
        try:
            self.area = math.sqrt(4*self.e**2*self.f**2 - (self.b**2 + self.d**2 - self.a**2 - self.c**2)**2)/4
            return self.area
        except ValueError:
            self.isReachable = False
            if self.printErrors: print("area ValueError")
            return None
    def solve_absAng(self):
        self.absAng = -(self.beta + self.gamma - 180)

    def solveVars(self):
        self.isReachable = True
        self.solve_e()
        self.solve_f()
        self.solve_delta()
        self.solve_perimeter()
        self.solve_area()
        self.solve_absAng()

    def set_a(self, a):
        self.a = a
        self.solveVars()
    def set_b(self, b):
        self.b = b
        self.solveVars()
    def set_c(self, c):
        self.c = c
        self.solveVars()
    def set_d(self, d):
        self.d = d
        self.solveVars()
    def set_beta(self, beta):
        self.beta = beta
        self.solveVars()
    def set_gamma(self, gamma):
        self.gamma = gamma
        self.solveVars()

