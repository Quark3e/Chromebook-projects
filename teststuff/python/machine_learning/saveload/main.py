#!/usr/bin/env python3

from sklearn import svm
from sklearn import datasets

iris = datasets.load_iris()
X, y = iris.data, iris.target

clf = svm.SVC()
clf.fit(X, y)  

##########################
# SAVE-LOAD using pickle #
##########################
import pickle

# save
with open('model.pkl','wb') as f:
    pickle.dump(clf,f)

# load
with open('model.pkl', 'rb') as f:
    clf2 = pickle.load(f)

print(clf2.predict(X[0:1]))
