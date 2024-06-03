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
from matplotlib import cm
import sys
import os

absPath = os.path.realpath(__file__)[:-7]

# dataset
# https://www.kaggle.com/datasets/ciphernine/brooklyn-real-estate-listings
# place it in the same folder as this workbook
# df = pd.read_csv('archive/brooklyn_listings.csv')

orient = {"azim":40, "elev":30}

dataSetName = "raw_csv_dataSet_pf27_fuse-True"

df = pd.read_csv("/home/berkhme/github_repo/Chromebook-projects/teststuff/python/opencv/angleArea/data/"+dataSetName+".csv")

varNom = {"z":"Area","x":"Roll","y":"Pitch"}

df = df[[varNom["z"],varNom["x"],varNom["y"]]].dropna()


x_values = df[[varNom["x"],varNom["y"]]].values
y_values = df[varNom["z"]].values

pltVal = [
    df[varNom["x"]].values.tolist(),
    df[varNom["y"]].values.tolist(),
    0,
    df[varNom["z"]].values.tolist()
]
pltRes = [
    df[varNom["x"]].values.tolist(),
    df[varNom["y"]].values.tolist(),
    0,
]


degree=2
poly_model = PolynomialFeatures(degree=degree)
poly_x_values = poly_model.fit_transform(x_values)
poly_model.fit(poly_x_values, y_values)

regression_model = LinearRegression()

regression_model.fit(poly_x_values, y_values)

print(regression_model)
print(regression_model.intercept_)
print(regression_model.coef_)
print(regression_model.coef_[2])

useNew = True #variable to switch between predicting existing xy values or to use new values
testNew = False

testX_values = [[], []]

for x in range(-90, 91):
    for y in range(-90, 91):
        testX_values[0].append(x)
        testX_values[1].append(y)


if not useNew: y_pred = regression_model.predict(poly_x_values)
elif useNew and not testNew:
    # test_df = pd.DataFrame({"Roll":testX_values[0],"Pitch":testX_values[1]})
    # xTest_values = test_df[["Roll","Pitch"]].values
    # tempTest = np.array([[testX_values[0][i],testX_values[1][i]] for i in range(len(testX_values[0]))])
    # print("original:\n",xTest_values)
    # print("test:\n", tempTest)
    # xTest = poly_model.fit_transform(tempTest)
    y_pred = regression_model.predict( \
        poly_model.fit_transform( \
            np.array( \
                [[testX_values[0][i],testX_values[1][i]] for i in range(len(testX_values[0]))] \
            ) \
        ) \
    )
    # pltRes[0] = test_df["Roll"].values.tolist()
    # pltRes[1] = test_df["Pitch"].values.tolist()
    pltRes[0] = testX_values[0]
    pltRes[1] = testX_values[1]
elif useNew and testNew:
    xArr = np.array([[testX_values[0][i],testX_values[1][i]] for i in range(len(testX_values[0]))])
    print(xArr)
    sys.exit()

if not testNew: pltRes[2] = y_pred.tolist()

# regression_model.coef_


fig = plt.figure(figsize=(12,7))
ax=[0, 0]
ax[0] = fig.add_subplot(1,2,1,projection="3d")
ax[1] = fig.add_subplot(1,2,2,projection="3d")



for axis in ax:
    axis.set(xlim3d=(-90, 90), xlabel=varNom["x"])
    axis.set(ylim3d=(-90, 90), ylabel=varNom["y"])
    axis.set(zlim3d=(0, int(round(y_values.max()/1000)*1000)), zlabel=varNom["z"])
    axis.view_init(azim=orient["azim"],elev=orient["elev"])
    axis.grid(True)


ax[0].scatter(pltVal[0], pltVal[1], pltVal[3], c=pltVal[3], cmap="magma", label="correct data")

# ax[1].plot_trisurf(pltRes[0], pltRes[1], pltRes[2], cmap="magma")
ax[1].scatter(pltRes[0], pltRes[1], pltRes[2], c=pltRes[2], cmap="magma", label="predict data")

ax[0].legend()
ax[1].legend()
plt.grid(True)

plt.savefig(absPath+"media/"+dataSetName+"_"+f"n{degree}_"+f"[{orient['azim']}:{orient['elev']}]"+".png")

plt.show()


# mean_squared_error(y_values, y_pred, squared=False)

# # check our accuracy for each degree, the lower the error the better!
# number_degrees = [1,2,3,4,5,6,7,8,9,10,11,12]
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
