#!/usr/bin/env python3

import numpy as np
import pandas as pd
from sklearn.linear_model import LinearRegression
from sklearn.preprocessing import PolynomialFeatures

from sklearn.datasets import fetch_california_housing

import time
import requests
import sys

# print(fetch_california_housing().feature_names)

data_url = "http://lib.stat.cmu.edu/datasets/boston"

# print(pd.read_csv(data_url, sep="\s+", skiprows=22, header=None))
# raw_df = pd.read_csv(data_url, sep="\s+", skiprows=22, header=None, names=["CRIM","ZN","INDUS","CHAS","NOX","RM","AGE","DIS","RAD","TAX","PTRATIO","B","LSTAT","MEDV"])
raw_df = pd.read_csv("testFile.csv", sep="\t", skiprows=0, header=0)

# print(raw_df[:, ::-1] )
data = np.hstack([raw_df.values[::2, :], raw_df.values[1::2, :2]])
print(raw_df.values[::2, :])
target = raw_df.values[1::2, 2]
# print(raw_df.values)


# print(raw_df["ZN"].iloc[1::2].values)
# new_df = pd.DataFrame(
#     {'CRIM':raw_df['CRIM'].iloc[::2].values,
#      'ZN':raw_df['ZN'].iloc[::2].values,
#      'INDUS':raw_df['INDUS'].iloc[::2].values,
#      'CHAS':raw_df['CHAS'].iloc[::2].values,
#      'NOX':raw_df['NOX'].iloc[::2].values,
#      'RM':raw_df['RM'].iloc[::2].values,
#      'AGE':raw_df['AGE'].iloc[::2].values,
#      'DIS':raw_df['DIS'].iloc[::2].values,
#      'RAD':raw_df['RAD'].iloc[::2].values,
#      'TAX':raw_df['TAX'].iloc[::2].values,
#      'PTRATIO':raw_df['PTRATIO'].iloc[::2].values,
#      'B':raw_df['CRIM'].iloc[1::2].values,
#      'LSTAT':raw_df['ZN'].iloc[1::2].values,
#      'MEDV':raw_df['INDUS'].iloc[1::2].values
#      }
#     )
# print(new_df)
# new_df.groupby('one')['value'].apply(','.join).reset_index()
# print(new_df)
# new_df.to_csv("testFile.csv", sep="\t", index=False)
# sys.exit()

df = pd.DataFrame(target.data, columns=["CRIM","ZN","INDUS","CHAS","NOX","RM","AGE","DIS","RAD","TAX","PTRATIO","B","LSTAT","MEDV"])
df['PRICE'] = target.target

X = df.iloc[:,:-1].values
y = df.iloc[:, -1].values

poly = PolynomialFeatures(degree=2)
X_poly = poly.fit_transform(X)

model = LinearRegression()
model.fit(X_poly, y)

new_data = np.array([[0.03, 18, 2.5]]) # new data with crime rate, number of rooms, and distance from city center
new_data_poly = poly.transform(new_data)

predicted_price = model.predict(new_data_poly)
print(predicted_price)