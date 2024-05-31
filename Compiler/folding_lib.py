# Copyright (c) 2024, Technology Innovation Institute, Yas Island, Abu Dhabi, United Arab Emirates.

from mpc_math import pow_fx, sqrt_simplified_fx
from Compiler import matrix_lib
from AdvInteger import TruncPr, TruncPr_parallel
from instructions import vldms, vstms, vldmc, vstmc, vsubs, vadds, vaddm, vaddc, vmulm, vmulc, vstartopen, vstopopen, \
    addsi, addci, sums, sumc, vtriple


#### NAMING CONVENTIONS FOR THIS LIBRARY ####
# A Matrix in memory is denoted with capital letter: A
# A Matrix in a registry vector is denoted with low case and suffix "_f" (flattened): a_f
# A transformed matrix uses the suffix "tr": Atr or atr_f
# A multiplication of 2 matrixes is dentoted with "_" between them: A_B or a_b_f
# A matrix in memory uses the type as suffix when is used with different types: A_sint / A_sfix
# A transposed matrix uses the suffix "tp": Atp
# A pruned matrix (some rows removed by the stride) attaches suffix pr: Apr (normally applied to Atr, so Atrpr)
# The number of rows/columns of a matrix is denoted with prefix "rows_ / cols_": cols_A
# The array of rows of columns of a matrix is denoted with suffix "_rows / _cols": A_cols
# Single-use matrixes (without algorithmic meaning) are denoted with prefix "_" or "__" (Erlang-like): _A or _a_f
# Note that, the suffix "_" does not have any special meaning and can be used arbitrarily
# sometimes a single row/column is allocated in registry memory, this is denoted as: a_row_f / a_col_f


##############################################  FOLDING LAYERS  ##############################################
##############################################  FOLDING LAYERS  ##############################################
##############################################  FOLDING LAYERS  ##############################################

class Trunc_Mode:
    OFF, ON = range(2)

# FOLDING (TYPE:: sfix)
# OPTION 1: X can be a 2D matrix representing a 3D feature [height * width][input_channels]
# OPTION 2: X can be a 3D feature [input_channels][height][width]
# h=-1 activates OPTION 2, for OPTION 1 h = feature_height and w = feature_weight
# type= "avg_pool" or "max_pool"
# padding and stride accept positive values, but only stride 1 and 2 tested
def folding(X, kh, kw, stride, type, h=-1, w=-1, padding=0, mode=Trunc_Mode.ON):
    from types import Matrix

    if h == -1:
        if padding > 0:
            X = matrix_lib.pad_feature_with_zeros_3d(X, padding)
        dimensions = len(X)
        output = []

    else:
        if padding > 0:
            X = matrix_lib.pad_feature_with_zeros_2d(X, h, w, padding)
            h = h + (2 * padding)
            w = w + (2 * padding)
        dimensions = len(X[0])
        rows_Y = ((h - kh) // stride) + 1
        cols_Y = ((w - kw) // stride) + 1
        output = Matrix(rows_Y * cols_Y, dimensions, X.value_type)
        X = matrix_lib.traspose(X)

    for k in range(dimensions):
        if type == "avg_pool":
            Y = avg_pool(X[k], kh, kw, stride, h, w, mode)
        if type == "max_pool":
            Y = max_pool(X[k], kh, kw, stride, h, w, mode)

        if h == -1:
            output.append(Y)
        else:
            for i in range(rows_Y * cols_Y):
                output[i][k] = Y[i]

    return output


# AVG POOLING (TYPE:: sfix)
# this function is called by "FOLDING()"
def avg_pool(X, kh, kw, stride, h=-1, w=-1, mode=Trunc_Mode.ON):
    from types import sint, cint, Matrix, sfix

    if h == -1:
        rows_X = len(X)
        cols_X = len(X[0])
    else:
        rows_X = h
        cols_X = w

    rows_Y = ((rows_X - kh) // stride) + 1
    cols_Y = ((cols_X - kw) // stride) + 1

    if h == -1:
        Y = sfix.Matrix(rows_Y, cols_Y)
    else:
        Y = sfix.Array(rows_Y * cols_Y)

    for i in range(rows_Y):
        for j in range(cols_Y):
            if h == -1:
                Y[i][j] = 0
            else:
                Y[i * cols_Y + j] = 0

            initial_i = i * stride
            initial_j = j * stride

            for ii in range(kh):
                for jj in range(kw):
                    if h == -1:
                        Y[i][j] += X[initial_i + ii][initial_j + jj]
                    else:
                        Y[i * cols_Y + j] += X[(initial_i + ii) * w + (initial_j + jj)]

    scale = 1.0 / (kh * kw)
    Z = scale_all_values(Y, scale, h, w, mode)

    return Z


# SCALE (TYPE:: sfix)
# this function is called by "AVG_POOL()"
# it multiplies a matrix of sfix values by the same cfix value
def scale_all_values(X, scale, h, w, mode):
    from types import sint, sfix, cint, cfix, Array

    if h == -1:
        rows_X = len(X)
        cols_X = len(X[0])
        k = X[0][0].k * 2
        m = X[0][0].f
        kappa = X[0][0].kappa
    else:
        rows_X = h
        cols_X = w
        k = X[0].k * 2
        m = X[0].f
        kappa = X[0].kappa

    scale_cfix = cfix.Array(rows_X * cols_X)
    for i in range(rows_X * cols_X):
        scale_cfix[i] = scale
    scale_cint = cint(size=rows_X * cols_X)

    X_sint = sint(size=rows_X * cols_X)

    scaled_X_sint = sint(size=rows_X * cols_X)
    vldms(rows_X * cols_X, X_sint, X.address)
    vldmc(rows_X * cols_X, scale_cint, scale_cfix.address)
    vmulm(rows_X * cols_X, scaled_X_sint, X_sint, scale_cint)

    if mode == Trunc_Mode.ON:
        scaled_X_sint = TruncPr_parallel(scaled_X_sint, k, m, kappa)

    if h == -1:
        scaled_X_sfix = sfix.Matrix(rows_X, cols_X)
    else:
        scaled_X_sfix = sfix.Array(rows_X * cols_X)

    vstms(rows_X * cols_X, scaled_X_sint, scaled_X_sfix.address)

    return scaled_X_sfix


# MAX POOLING (TYPE:: sfix)
# this function is called by "FOLDING()"
def max_pool(X, kh, kw, stride, h=-1, w=-1):
    from types import sint, cint, Matrix, sfix

    if h == -1:
        rows_X = len(X)
        cols_X = len(X[0])
    else:
        rows_X = h
        cols_X = w

    rows_Y = ((rows_X - kh) // stride) + 1
    cols_Y = ((cols_X - kw) // stride) + 1

    if h == -1:
        Y = sfix.Matrix(rows_Y, cols_Y)
    else:
        Y = sfix.Array(rows_Y * cols_Y)

    for i in range(rows_Y):
        for j in range(cols_Y):

            initial_i = i * stride
            initial_j = j * stride
            if h == -1:
                max_value = X[initial_i][initial_j]
            else:
                max_value = X[initial_i * w + initial_j]

            for ii in range(kh):
                for jj in range(kw):
                    if (ii == 0) and (jj == 0):
                        continue
                    if h == -1:
                        bool_value = max_value < X[initial_i + ii][initial_j + jj]
                        max_value = max_value + bool_value * (X[initial_i + ii][initial_j + jj] - max_value)
                        Y[i][j] = max_value
                    else:
                        bool_value = max_value < X[(initial_i + ii) * w + (initial_j + jj)]
                        max_value = max_value + bool_value * (X[(initial_i + ii) * w + (initial_j + jj)] - max_value)
                        Y[i * cols_Y + j] = max_value

    return Y


# SOFTMAX (TYPE:: sfix) - (Taken from Marcel Keller's paper)
# it gets an array and returns a single opened value
# if X is a matrix, the sie must be (N x 1) and not (1 x N)
# for values higher than K this will not work
# VERSION: - private  - it only outputs the maximum value [index & probability] {{secret shared}}
# VERSION: - complete - outputs Y with all probabilities {{secret shared}}
# scale - "applies a right shift to reduce the fractional part in the division.
# The neg_bound is the minimum negative exponent allowed, lower than that the value is nullified (this avoids overflow)
def softmax_scaled(X, neg_bound, complete=False):
    from types import sint, cint, Array, Matrix, sfix
    from Compiler.library import print_str

    elements_X = len(X)

    max_value = X[0]
    max_index = sint(0)

    for i in range(elements_X):
        bool_value = max_value < X[i]
        max_value = max_value + bool_value * (X[i] - max_value)
        max_index = max_index + (bool_value * (i - max_index))

    divisor = sfix(0)
    for i in range(elements_X):
        scaled_value = X[i] - max_value
        bool_value = scaled_value > neg_bound
        exp = pow_fx(2.71828, scaled_value)
        exp = exp * bool_value
        divisor += exp

    if (complete == False):

        dividend = sfix(1)
        probability_value = dividend / divisor
        output = [max_index, probability_value]

        return output

    elif (complete == True):

        Y = sfix.Array(elements_X)

        for i in range(elements_X):
            scaled_value = X[i] - max_value
            bool_value = scaled_value > neg_bound
            exp = pow_fx(2.71828, scaled_value)
            dividend = exp * bool_value
            Y[i] = dividend / divisor

        return Y


# GET MAX (TYPE:: sfix)
# gets max value in an array
# binary search should be done in parallel, but there is no way to do that in SM.
def get_max_with_binary_search(X):
    length = len(X)

    while (length > 1):
        X = binary_search_1_round(X)
        length = len(X)

    return X


# called by the function above
# gets an array and compares elements in pairs, returns max values
def binary_search_1_round(X):
    from types import sint, cint, Array, Matrix, sfix

    elements_X = len(X)

    if ((elements_X % 2) == 1):
        out_length = ((elements_X - 1) / 2) + 1
        Y = sfix.Array(out_length)
        Y[out_length - 1] = X[elements_X - 1]
        elements_to_fill = out_length - 1

    else:
        out_length = elements_X / 2
        Y = sfix.Array(out_length)
        elements_to_fill = out_length

    # this are sequential binary decisions, not parallel
    for i in range(elements_to_fill):
        Y[i] = X[2 * i] + (X[2 * i] < X[2 * i + 1]) * (X[2 * i + 1] - X[2 * i])

    return Y


##############################################  BATCH PROCESSING  ##############################################
##############################################  BATCH PROCESSING  ##############################################
##############################################  BATCH PROCESSING  ##############################################


# BATCH NORMALIZATION - VECTORIZED VERSION (TYPE:: sfix)
# version for inference
# gets input X and applies elements-wise factors Beta' and Gamma'
def batch_normalization(X, Gp, Bp, mode=Trunc_Mode.ON):
    from types import Matrix, sint, cint, sint, sfix

    rows_X = len(X)
    cols_X = len(X[0])
    n = rows_X * cols_X

    Y = sfix.Matrix(rows_X, cols_X)

    x_f = sint(size=n)  #
    gp_f = sint(size=n)  #
    bp_f = sint(size=n)  #

    vldms(n, x_f, X.address)
    vldms(n, gp_f, Gp.address)
    vldms(n, bp_f, Bp.address)

    a_f = sint(size=n)  # triple A
    b_f = sint(size=n)  # triple B
    c_f = sint(size=n)  # triple C
    epsilon = sint(size=n)  #
    delta = sint(size=n)  #
    epsilon_clear = cint(size=n)  #
    delta_clear = cint(size=n)  #

    vtriple(n, a_f, b_f, c_f)

    vsubs(n, epsilon, x_f, a_f)
    vsubs(n, delta, gp_f, b_f)

    vstartopen(n, epsilon)
    vstopopen(n, epsilon_clear)
    vstartopen(n, delta)
    vstopopen(n, delta_clear)

    z1 = sint(size=n)
    z2 = sint(size=n)
    z3 = cint(size=n)

    vmulm(n, z1, b_f, epsilon_clear)
    vmulm(n, z2, a_f, delta_clear)
    vmulc(n, z3, epsilon_clear, delta_clear)

    y = sint(size=n)
    _y = sint(size=n)
    __y = sint(size=n)
    ___y = sint(size=n)
    #____y = sint(size=n)
    #_____y = sint(size=n)

    vadds(n, _y, z1, z2)
    vaddm(n, __y, _y, z3)
    vadds(n, ___y, __y, c_f)

    k = X[0][0].k * 2
    m = X[0][0].f
    kappa = X[0][0].kappa

    if mode == Trunc_Mode.ON:
        ___y = TruncPr_parallel(___y, k, m, kappa)

    vadds(n, y, ___y, bp_f)
    vstms(n, y, Y.address)

    return Y


# BATCH NORMALIZATION - TESTING ONLY - NOT VECTORIZED (TYPE:: sfix)
# version for inference
# gets input X and applies elements-wise factors Beta' and Gamma'
def batch_normalization_testing(X, Gp, Bp):
    from types import Matrix, sfix

    rows_X = len(X)
    cols_X = len(X[0])
    Y = sfix.Matrix(rows_X, cols_X)

    for i in range(rows_X):
        for j in range(cols_X):
            Y[i][j] = (X[i][j] * Gp[i][j]) + Bp[i][j]

    return Y


# BATCH NORMALIZATION (TYPE:: sfix)
# version for training
# gets input X and applies elements-wise factors Beta' and Gamma'
# gets input feature X as 2D matrix [height * width][channel]
# gets elements-wise factors Means (M) and variance (V) of mini-batch
# gets constant epsilon ep (numerical stability)
# TODO: not sufficiently tested - under development
def batch_normalization_trn(X, G, B, M, V, ep):
    from types import Matrix, sfix

    rows_X = len(X)
    cols_X = len(X[0])
    Y = sfix.Matrix(rows_X, cols_X)

    for i in range(rows_X):
        for j in range(cols_X):
            dividend = (X[i][j] - M[i][j])
            divisor = sqrt_simplified_fx(V[i][j] + ep)
            Y[i][j] = dividend / divisor
            Y[i][j] = (Y[i][j] * G[i][j]) + B[i][j]

    return Y


# PREPROCESSING for BATCH NORMALIZATION (TYPE:: sfix)
# gives MEAN and VARIANCE
# version for training
# gets input X as 3D matrix [batch][height * width][channel]
# gives mean and variance
# TODO: not sufficiently tested - under development
def batch_normalization_process_minibatch(X):
    from types import Matrix, sfix

    batch_size = len(X)
    inverse_batch_size = 1.0 / batch_size
    rows_X = len(X[0])
    cols_X = len(X[0][0])

    Mean = sfix.Matrix(rows_X, cols_X)
    Variance = sfix.Matrix(rows_X, cols_X)

    for i in range(rows_X):
        for j in range(cols_X):
            for k in range(batch_size):
                Mean[i][j] += X[k][i][j]

            Mean[i][j] = Mean[i][j] * inverse_batch_size

    for i in range(rows_X):
        for j in range(cols_X):
            for k in range(batch_size):
                Temp = X[k][i][j] - Mean[i][j]
                Variance[i][j] += (Temp * Temp)

            Variance[i][j] = Variance[i][j] * inverse_batch_size

    output = [Mean, Variance]

    return output


# PREPROCESSING for BATCH NORMALIZATION (TYPE:: sfix)
# gives Gamma and Beta
# version for training
# gets input M,V,G,B as 3D matrixes [batch][height * width][channel]
# TODO: not sufficiently tested - under development
def batch_normalization_get_parameters_Bp_and_Gp(M, V, G, B, ep, m):
    from types import Matrix, sfix

    batch_size = len(M)
    inverse_batch_size = 1.0 / batch_size
    rows_M = len(M[0])
    cols_M = len(M[0][0])

    E_Means = sfix.Matrix(rows_M, cols_M)
    E_Var = sfix.Matrix(rows_M, cols_M)
    Gp = sfix.Matrix(rows_M, cols_M)
    Bp = sfix.Matrix(rows_M, cols_M)

    for i in range(rows_M):
        for j in range(cols_M):
            E_Means[i][j] = 0
            E_Var[i][j] = 0
            for k in range(batch_size):
                E_Var[i][j] += V[k][i][j]
                E_Means[i][j] += M[k][i][j]

            E_Means[i][j] = E_Means[i][j] * inverse_batch_size
            E_Var[i][j] = E_Var[i][j] * (m / ((m - 1) * batch_size))
            divisor = sqrt_simplified_fx(E_Var[i][j] + ep)
            Gp[i][j] = G[i][j] / divisor
            Bp[i][j] = B[i][j] - (Gp[i][j] * E_Means[i][j])

    output = [Gp, Bp]

    return output


##############################################  INPUT PROCESSING  ##############################################
##############################################  INPUT PROCESSING  ##############################################
##############################################  INPUT PROCESSING  ##############################################

# MEAN VALUE (TYPE::sfix)
## gets a 3D Tensor and returns the mean value
def get_mean(X):
    from types import sfix

    dimensions = len(X)
    rows_X = len(X[0])
    cols_X = len(X[0][0])

    scale = 1.0 / (dimensions * rows_X * cols_X)

    mean = sfix(0)

    for k in range(dimensions):
        for i in range(rows_X):
            for j in range(cols_X):
                mean += X[k][i][j]

    mean = mean * scale

    return mean


# VARIANCE VALUE (TYPE::sfix)
## gets a 3D Tensor and returns the variance
def get_variance(X):
    from types import sfix

    mean = get_mean(X)

    dimensions = len(X)
    rows_X = len(X[0])
    cols_X = len(X[0][0])

    variance = sfix(0)

    scale = 1.0 / (dimensions * rows_X * cols_X)

    for k in range(dimensions):
        for i in range(rows_X):
            for j in range(cols_X):
                value = X[k][i][j] - mean
                value = value * value
                variance += value

    variance = variance * scale

    return variance


# MIN & MAX VALUE (TYPE::sfix)
## gets a 3D Tensor and returns the min and max values
def get_min_and_max(X):
    dimensions = len(X)
    rows_X = len(X[0])
    cols_X = len(X[0][0])

    max_value = X[0][0][0]
    min_value = X[0][0][0]

    for k in range(dimensions):
        for i in range(rows_X):
            for j in range(cols_X):
                if (k == 0) and (i == 0) and (j == 0):
                    continue
                bool_value_max = max_value < X[k][i][j]
                bool_value_min = min_value > X[k][i][j]
                max_value = max_value + bool_value_max * (X[k][i][j] - max_value)
                min_value = min_value + bool_value_min * (X[k][i][j] - min_value)

    output = [max_value, min_value]

    return output


# NORMALIZATION (TYPE::sfix)
# gets a 3D Tensor and returns a normalized 3D tensor
def input_normalization(X, max, min):
    from types import sfix, Matrix

    dimensions = len(X)
    rows_X = len(X[0])
    cols_X = len(X[0][0])

    dif = max - min

    Normalized_X = []

    for k in range(dimensions):
        Y = sfix.Matrix(rows_X, cols_X)
        for i in range(rows_X):
            for j in range(cols_X):
                Y[i][j] = (X[k][i][j] - min) / dif

        Normalized_X.append(Y)

    return Normalized_X


# STANDARDIZATION VALUE (TYPE::sfix)
# gets a 3D Tensor and returns a standardized 3D tensor
def input_standardization(X, mean, var):
    from types import sfix, Matrix

    dimensions = len(X)
    rows_X = len(X[0])
    cols_X = len(X[0][0])

    Standardized_X = []

    for k in range(dimensions):
        Y = sfix.Matrix(rows_X, cols_X)
        for i in range(rows_X):
            for j in range(cols_X):
                Y[i][j] = (X[k][i][j] - mean) / var

        Standardized_X.append(Y)

    return Standardized_X
