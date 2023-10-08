#!/usr/bin/env python3


from sklearn.preprocessing import PolynomialFeatures
from sklearn.linear_model import LinearRegression
from sklearn.metrics import mean_squared_error
# dataframes
import pandas as pd
# computation
import numpy as np
# visualization
import matplotlib.pyplot as plt

# dataset
# https://www.kaggle.com/datasets/ciphernine/brooklyn-real-estate-listings
# place it in the same folder as this workbook
# df = pd.read_csv('archive/brooklyn_listings.csv')
df = pd.read_csv("/home/berkhme/github_repo/Chromebook-projects/teststuff/python/opencv/angleArea/data/raw_csv_dataSet_pf26_fuse-False.csv")

# print(df)
# varNom = {"z":"price","x":"bathrooms","y":"sqft"}
varNom = {"z":"Area","x":"Roll","y":"Pitch"}

# for this example, we're going to estimate the price with sqft, bathroom, and bedrooms
df = df[[varNom["z"],varNom["x"],varNom["y"]]].dropna()

# show some random lines from our data
# print(df.sample(n=500))


x_values = df[[varNom["x"],varNom["y"]]].values
y_values = df[varNom["z"]].values

# print(df[varNom["x"]].values.tolist())

pltVal = [
    df[varNom["x"]].values.tolist(),
    df[varNom["y"]].values.tolist(),
    0,
    df[varNom["z"]].values.tolist()
]

# visual
# print("0 values visual:", x_values[0], y_values[0])
# print(df)

degree=4

# PolynomialFeatures will create a new matrix consisting of all polynomial combinations 
# of the features with a degree less than or equal to the degree we just gave the model (2)
poly_model = PolynomialFeatures(degree=degree)

# transform out polynomial features
poly_x_values = poly_model.fit_transform(x_values)

# should be in the form [1, a, b, a^2, ab, b^2]
# print(f'initial values {x_values[0]}\nMapped to {poly_x_values[0]}')

# [1, a=5, b=2940, a^2=25, 5*2940=14700, b^2=8643600]

poly_model.fit(poly_x_values, y_values)

# we use linear regression as a base!!! ** sometimes misunderstood **
regression_model = LinearRegression()

regression_model.fit(poly_x_values, y_values)
testX_values = [[0], [0]]

for x in range(-90, 91):
    for y in range(-90, 91):
        testX_values[0].append(x)
        testX_values[1].append(y)


# print(poly_x_values)
y_pred = regression_model.predict(poly_x_values)
pltVal[2] = y_pred.tolist()
# print(poly_x_values)

regression_model.coef_

print(y_values.max())

fig = plt.figure(figsize=(12,7))
ax=[0, 0]
ax[0] = fig.add_subplot(1,2,1,projection="3d")
ax[1] = fig.add_subplot(1,2,2,projection="3d")

orient={"azim":0,"elev":0}


for axis in ax:
    axis.set(xlim3d=(-90, 90), xlabel=varNom["x"])
    axis.set(ylim3d=(-90, 90), ylabel=varNom["y"])
    axis.set(zlim3d=(0, int(round(y_values.max()/1000)*1000)), zlabel=varNom["z"])
    axis.view_init(azim=orient["azim"],elev=orient["elev"])
    axis.grid(True)

# for axis in ax:
#     axis.set_xlabel("bathrooms")
#     axis.set_ylabel("sqft")
#     axis.set_xlim([0, 50])
#     axis.set_ylim([0, 30_000])

polyfitGraph0 = ax[0].scatter(pltVal[0], pltVal[1], pltVal[3], c=pltVal[3], cmap="magma", label="correct data")
polyfitGraph1 = ax[1].scatter(pltVal[0], pltVal[1], pltVal[2], c=pltVal[2], cmap="magma", label="predict data")

# fig.colorbar(polyfitGraph0, ax[0], location="bottom")
# fig.colorbar(polyfitGraph1, ax[1], location="bottom")

ax[0].legend()
ax[1].legend()
plt.grid(True)

plt.show()

# mean_squared_error(y_values, y_pred, squared=False)

# # check our accuracy for each degree, the lower the error the better!
# number_degrees = [1,2,3,4,5,6,7]
# plt_mean_squared_error = []
# for degree in number_degrees:

#    poly_model = PolynomialFeatures(degree=degree)
  
#    poly_x_values = poly_model.fit_transform(x_values)
#    poly_model.fit(poly_x_values, y_values)
  
#    regression_model = LinearRegression()
#    regression_model.fit(poly_x_values, y_values)
#    y_pred = regression_model.predict(poly_x_values)
  
#    plt_mean_squared_error.append(mean_squared_error(y_values, y_pred, squared=False))
  
# plt.scatter(number_degrees,plt_mean_squared_error, color="green")
# plt.plot(number_degrees,plt_mean_squared_error, color="red") 

# plt.show()