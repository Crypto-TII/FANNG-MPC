# Copyright (c) 2024, Technology Innovation Institute, Yas Island, Abu Dhabi, United Arab Emirates.

from mpc_math import pow_fx, sqrt_simplified_fx
from Compiler.types import sfix


# private sigmoid
def sigmoid(x):
    from types import sint, cint, Array, Matrix, sfix

    mx = -1 * x
    e_x = pow_fx(2.71828, mx)
    divisor = sfix(1) + e_x
    dividend = sfix(1)
    sig = dividend / divisor

    return sig


# X is a 2D matrix
def sigmoid_2d(X):
    rows_X = len(X)
    cols_X = len(X[0])

    sigmoid_X = sfix.Matrix(rows_X, cols_X)
    for i in range(rows_X):
        for j in range(cols_X):
            sigmoid_X[i][j] = sigmoid(X[i][j])

    return sigmoid_X


# X is a 3D matrix
def sigmoid_3d(X):
    dimensions = len(X)
    rows_X = len(X[0])
    cols_X = len(X[0][0])

    sigmoid_X = []

    for k in range(dimensions):
        Y = sfix.Matrix(rows_X, cols_X)
        for i in range(rows_X):
            for j in range(cols_X):
                Y[i][j] = sigmoid(X[k][i][j])
        sigmoid_X.append(Y)

    return sigmoid_X
