#!/usr/bin/env python3

import pandas
from sklearn import linear_model
from time import perf_counter

t1 = perf_counter()
df = pandas.read_csv("data.csv")
print(f"read_csv:\t\t{round(perf_counter()-t1)}")
t1 = perf_counter()

# print(df, end="\n\n")

print(type(df))
X = df[["Weight", "Volume"]]
y = df["CO2"]
print(f"saving dataframe:\t{round(perf_counter()-t1,4)}")
t1 = perf_counter()
print(type(X), type(y))

regr = linear_model.LinearRegression()
print(f"solve LR:\t\t{round(perf_counter()-t1,4)}")
t1 = perf_counter()

regr.fit(X.values, y)
print(f".fit:\t\t\t{round(perf_counter()-t1,4)}")
t1 = perf_counter()

predictedCO2 = regr.predict([[2300, 1300]])
predictedCO2 = int(predictedCO2[0])
print(type(predictedCO2))
print(f"predicting:\t\t{round(perf_counter()-t1,4)}")
t1 = perf_counter()

print(predictedCO2)