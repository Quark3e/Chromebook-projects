#!/bin/env python3


def test(i):
    while i<=100:
        print("test", end=": ")
        yield i**2

for i in range(101):
    print(next(test(i**0.5)))