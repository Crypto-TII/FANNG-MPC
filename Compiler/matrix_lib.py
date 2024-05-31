# Copyright (c) 2024, Technology Innovation Institute, Yas Island, Abu Dhabi, United Arab Emirates.

# from Compiler import AdvInteger
from AdvInteger import TruncPr, TruncPr_parallel
from instructions import vldms, vstms, vldmc, vstmc, vsubs, vadds, vaddm, vaddc, vmulm, vmulc, vstartopen, vstopopen, \
    addsi, addci, sums, sumc
from matrix_triple_lib import get_specific_matrix_triple_as_matrix


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


#Specifies if linear transformations return values truncated (ON) or not (OFF)
class Trunc_Mode:
    OFF, ON = range(2)


# CONVOLUTION (TYPE:: sint)
# X is a 2D matrix representing a 3D feature [height * width][input_channels]
# Y is a 2D matrix representing the 4D kernels [kernel_height * kernel_width * input_channels][output_channels]
# A,B,C is a convolutional triple
# h = feature_height; w = feature_width
# kh = kernel_height; kw = kernel_width;
# s = input_channels; s_ = output_channels
# TODO: stride bigger than 2
def conv3d_sint2sint(X, Y, A, B, C, kh, kw, s, s_, h, w, stride=1, padding=0):
    from types import sint, cint, Matrix

    if padding > 0:
        X = pad_feature_with_zeros_2d(X, h, w, padding)
        h = (h + (2 * padding))
        w = (w + (2 * padding))

    rows_A = len(X)
    cols_A = len(X[0])
    rows_B = len(Y)
    cols_B = len(Y[0])
    rows_C = (w - kw + 1) * (h - kh + 1)
    cols_C = s_

    if stride == 2:
        wp = (w - kw + 1)
        hp = (h - kh + 1)
        reduced_h = (hp // stride)
        reduced_w = (wp // stride)

        if (h % stride) > 0:
            reduced_h += 1

        if (w % stride) > 0:
            reduced_w += 1

        rows_C = reduced_h * reduced_w

    if stride > 2:  ## not implemented for this case
        raise CompilerError('stride > 2 not expected')

    n1 = rows_A * cols_A
    n2 = rows_B * cols_B
    n3 = rows_C * cols_C

    x_f = sint(size=n1)  # features
    y_f = sint(size=n2)  # kernels

    epsilon = sint(size=n1)
    delta = sint(size=n2)
    epsilon_clear = cint(size=n1)
    delta_clear = cint(size=n2)

    vldms(n1, x_f, X.address)
    vldms(n2, y_f, Y.address)

    ############ - CONVOLUTIONAL TRIPLE - #############
    ###################################################
    a_f = sint(size=n1)  # triple A
    b_f = sint(size=n2)  # triple B
    c_f = sint(size=n3)  # triple C

    vldms(n1, a_f, A.address)
    vldms(n2, b_f, B.address)
    vldms(n3, c_f, C.address)

    # a_f, b_f, c_f = matrix_triple_lib.get_specific_matrix_triple_as_vector(triple_type)

    ###################################################

    vsubs(n1, epsilon, x_f, a_f)
    vsubs(n2, delta, y_f, b_f)

    vstartopen(n1, epsilon)
    vstopopen(n1, epsilon_clear)
    vstartopen(n2, delta)
    vstopopen(n2, delta_clear)

    E = cint.Matrix(rows_A, cols_A)
    D = cint.Matrix(rows_B, cols_B)

    vstmc(n1, epsilon_clear, E.address)
    vstmc(n2, delta_clear, D.address)

    ############# - - TRANSFORMATIONS - - #############
    ###################################################
    Etr = transform_input_features(E, h, w, s, kh, kw, stride)
    Dtr = transform_kernels(D, kh, kw, s, s_)
    Atr = transform_input_features(A, h, w, s, kh, kw, stride)
    Btr = transform_kernels(B, kh, kw, s, s_)
    ###################################################

    Etr_Btr = multmat_cint2sint(Etr, Btr)  # sint class
    Atr_Dtr = multmat_sint2cint(Atr, Dtr)  # sint class
    Etr_Dtr = multmat_cint2cint(Etr, Dtr)  # cint class

    etr_btr_f = sint(size=n3)
    atr_dtr_f = sint(size=n3)
    etr_dtr_f = cint(size=n3)

    __z_f = sint(size=n3)  # intermediate value only
    _z_f = sint(size=n3)  # intermediate value only
    z_f = sint(size=n3)

    vldms(n3, etr_btr_f, Etr_Btr.address)
    vldms(n3, atr_dtr_f, Atr_Dtr.address)
    vldmc(n3, etr_dtr_f, Etr_Dtr.address)

    vadds(n3, __z_f, etr_btr_f, atr_dtr_f)
    vadds(n3, _z_f, __z_f, c_f)
    vaddm(n3, z_f, _z_f, etr_dtr_f)

    Z = sint.Matrix(rows_C, cols_C)
    vstms(n3, z_f, Z.address)

    return Z


# CONVOLUTION (TYPE:: sfix)
# X_sfix input features
# Y_sfix input kernels
# it calls the function above conv3D_sint2sint
def conv3d_sfix2sfix(X_sfix, Y_sfix, triple_type, kh, kw, s, s_, h, w, stride=1, padding=0, mode=Trunc_Mode.ON):
    from types import sfix, sint, Matrix

    rows_X = len(X_sfix)
    cols_X = len(X_sfix[0])
    rows_Y = len(Y_sfix)
    cols_Y = len(Y_sfix[0])

    x_f = sint(size=rows_X * cols_X)
    y_f = sint(size=rows_Y * cols_Y)

    vldms(rows_X * cols_X, x_f, X_sfix.address)
    vldms(rows_Y * cols_Y, y_f, Y_sfix.address)

    X_sint = Matrix(rows_X, cols_X, sint)
    Y_sint = Matrix(rows_Y, cols_Y, sint)

    vstms(rows_X * cols_X, x_f, X_sint.address)
    vstms(rows_Y * cols_Y, y_f, Y_sint.address)

    A, B, C = get_specific_matrix_triple_as_matrix(triple_type)

    Z_sint = conv3d_sint2sint(X_sint, Y_sint, A, B, C, kh, kw, s, s_, h, w, stride, padding)

    if mode == Trunc_Mode.ON:
        Z_sfix = truncate_sfix_matrix(Z_sint, X_sfix[0][0])
    else:
        Z_sfix = sint_to_sfix_matrix(Z_sint)

    return Z_sfix


# CONVOLUTION WITH ADJUSTED PADDING (TYPE:: sint)
# X is a 2D matrix representing a 3D feature [height * width][input_channels]
# Y is a 2D matrix representing the 4D kernels [kernel_height * kernel_width * input_channels][output_channels]
# A,B,C is a convolutional triple
# h = feature_height; w = feature_width
# l = padding -> the kernel size is [2l+1][2l+1]
# s = input_channels; s_ = output_channels
# TODO: stride bigger than 2
def conv3d_sint2sint_adjusted_padding(X, Y, A, B, C, l, s, s_, h, w, stride=1):
    from types import sint, cint, Matrix
    from Compiler.exceptions import CompilerError

    rows_A = len(X)
    cols_A = len(X[0])
    rows_B = len(Y)
    cols_B = len(Y[0])
    rows_C = h * w
    cols_C = s_

    if stride == 2:

        reduced_h = (h // stride)
        reduced_w = (w // stride)

        if (h % stride) > 0:
            reduced_h += 1

        if (w % stride) > 0:
            reduced_w += 1

        rows_C = reduced_h * reduced_w

    if stride > 2:  ## not implemented for this case
        raise CompilerError('stride > 2 not expected')

    n1 = rows_A * cols_A
    n2 = rows_B * cols_B
    n3 = rows_C * cols_C

    x_f = sint(size=n1)  # features
    y_f = sint(size=n2)  # kernels

    a_f = sint(size=n1)  # triple A
    b_f = sint(size=n2)  # triple B
    c_f = sint(size=n3)  # triple C

    epsilon = sint(size=n1)
    delta = sint(size=n2)
    epsilon_clear = cint(size=n1)
    delta_clear = cint(size=n2)

    vldms(n1, x_f, X.address)
    vldms(n2, y_f, Y.address)

    ############ - - CONVOLUTIONAL TRIPLE - - #########
    ###################################################
    vldms(n1, a_f, A.address)
    vldms(n2, b_f, B.address)
    vldms(n3, c_f, C.address)
    ###################################################

    vsubs(n1, epsilon, x_f, a_f)
    vsubs(n2, delta, y_f, b_f)

    vstartopen(n1, epsilon)
    vstopopen(n1, epsilon_clear)
    vstartopen(n2, delta)
    vstopopen(n2, delta_clear)

    E = cint.Matrix(rows_A, cols_A)
    D = cint.Matrix(rows_B, cols_B)

    vstmc(n1, epsilon_clear, E.address)
    vstmc(n2, delta_clear, D.address)

    ############# - - TRANSFORMATIONS - - #############
    ###################################################
    Etr = transform_input_features_adjusted_padding(E, l, h, w, s, stride)
    Dtr = transform_kernels_adjusted_padding(D, l, s, s_)
    Atr = transform_input_features_adjusted_padding(A, l, h, w, s, stride)
    Btr = transform_kernels_adjusted_padding(B, l, s, s_)
    ###################################################

    Etr_Btr = multmat_cint2sint(Etr, Btr)  # sint class
    Atr_Dtr = multmat_sint2cint(Atr, Dtr)  # sint class
    Etr_Dtr = multmat_cint2cint(Etr, Dtr)  # cint class

    etr_btr_f = sint(size=n3)
    atr_dtr_f = sint(size=n3)
    etr_dtr_f = cint(size=n3)

    __z_f = sint(size=n3)  # intermediate value only
    _z_f = sint(size=n3)  # intermediate value only
    z_f = sint(size=n3)

    vldms(n3, etr_btr_f, Etr_Btr.address)
    vldms(n3, atr_dtr_f, Atr_Dtr.address)
    vldmc(n3, etr_dtr_f, Etr_Dtr.address)

    vadds(n3, __z_f, etr_btr_f, atr_dtr_f)
    vadds(n3, _z_f, __z_f, c_f)
    vaddm(n3, z_f, _z_f, etr_dtr_f)

    Z = sint.Matrix(rows_C, cols_C)
    vstms(n3, z_f, Z.address)

    return Z


# CONVOLUTION WITH ADJUSTED PADDING (TYPE:: sfix)
# X_sfix input features
# Y_sfix input kernels
# it calls the function above conv3D_sint2sint_adjusted_padding
def conv3d_sfix2sfix_adjusted_padding(X_sfix, Y_sfix, triple_type, l, s, s_, h, w, stride=1, mode=Trunc_Mode.ON):
    from types import sfix, sint, Matrix

    rows_X = len(X_sfix)
    cols_X = len(X_sfix[0])
    rows_Y = len(Y_sfix)
    cols_Y = len(Y_sfix[0])

    x_f = sint(size=rows_X * cols_X)
    y_f = sint(size=rows_Y * cols_Y)

    vldms(rows_X * cols_X, x_f, X_sfix.address)
    vldms(rows_Y * cols_Y, y_f, Y_sfix.address)

    X_sint = Matrix(rows_X, cols_X, sint)
    Y_sint = Matrix(rows_Y, cols_Y, sint)

    vstms(rows_X * cols_X, x_f, X_sint.address)
    vstms(rows_Y * cols_Y, y_f, Y_sint.address)

    A, B, C = get_specific_matrix_triple_as_matrix(triple_type)

    Z_sint = conv3d_sint2sint_adjusted_padding(X_sint, Y_sint, A, B, C, l, s, s_, h, w, stride)

    if mode == Trunc_Mode.ON:
        Z_sfix = truncate_sfix_matrix(Z_sint, X_sfix[0][0])
    else:
        Z_sfix = sint_to_sfix_matrix(Z_sint)

    return Z_sfix


# MATRIX-MATRIX MULTIPLICATION (TYPE:: sint)
# X,Y input matrixes
# A,B,C matrix triple
def multmat_sint2sint(X, Y, A, B, C):
    from types import sint, cint, Matrix

    rows_A = len(X)
    cols_A = len(X[0])
    rows_B = len(Y)  # =colsA
    cols_B = len(Y[0])
    rows_C = rows_A
    cols_C = cols_B

    n1 = rows_A * cols_A
    n2 = rows_B * cols_B
    n3 = rows_C * cols_C

    x_f = sint(size=n1)  # features
    a_f = sint(size=n1)  # triple A
    y_f = sint(size=n2)  # kernels
    b_f = sint(size=n2)  # triple B
    c_f = sint(size=n3)  # triple C

    epsilon = sint(size=n1)
    delta = sint(size=n2)
    epsilon_clear = cint(size=n1)
    delta_clear = cint(size=n2)

    vldms(n1, x_f, X.address)
    vldms(n2, y_f, Y.address)

    ############ - - MATRIX TRIPLE - - ############
    ###############################################
    vldms(n1, a_f, A.address)
    vldms(n2, b_f, B.address)
    vldms(n3, c_f, C.address)
    ####################################

    vsubs(n1, epsilon, x_f, a_f)
    vsubs(n2, delta, y_f, b_f)

    vstartopen(n1, epsilon)
    vstopopen(n1, epsilon_clear)
    vstartopen(n2, delta)
    vstopopen(n2, delta_clear)

    E = cint.Matrix(rows_A, cols_A)
    D = cint.Matrix(rows_B, cols_B)

    vstmc(n1, epsilon_clear, E.address)
    vstmc(n2, delta_clear, D.address)

    E_B = multmat_cint2sint(E, B)  # sint matrix
    A_D = multmat_sint2cint(A, D)  # sint matrix
    E_D = multmat_cint2cint(E, D)  # cint matrix

    e_b_f = sint(size=n3)
    a_d_f = sint(size=n3)
    e_d_f = cint(size=n3)

    __z_f = sint(size=n3)  # intermediate value
    _z_f = sint(size=n3)  # intermediate value
    z_f = sint(size=n3)

    vldms(n3, e_b_f, E_B.address)
    vldms(n3, a_d_f, A_D.address)
    vldmc(n3, e_d_f, E_D.address)

    vadds(n3, __z_f, e_b_f, a_d_f)
    vadds(n3, _z_f, __z_f, c_f)
    vaddm(n3, z_f, _z_f, e_d_f)

    Z = sint.Matrix(rows_C, cols_C)
    vstms(n3, z_f, Z.address)

    return Z


# MATRIX-MATRIX MULTIPLICATION (TYPE:: sfix)
# X_sfix, input matrix
# Y_sfix, input matrix
# A,B,C matrix triple (type sint)
# it calls the function above multmat_sint2sint
def multmat_sfix2sfix(X_sfix, Y_sfix, triple_type, mode=Trunc_Mode.ON):
    from types import sfix, sint, Matrix

    rows_X = len(X_sfix)
    cols_X = len(X_sfix[0])
    rows_Y = len(Y_sfix)
    cols_Y = len(Y_sfix[0])

    x_f = sint(size=rows_X * cols_X)
    y_f = sint(size=rows_Y * cols_Y)

    vldms(rows_X * cols_X, x_f, X_sfix.address)
    vldms(rows_Y * cols_Y, y_f, Y_sfix.address)

    X_sint = Matrix(rows_X, cols_X, sint)  # sint version of the input matrix
    Y_sint = Matrix(rows_Y, cols_Y, sint)  # sint version of the input matrix

    vstms(rows_X * cols_X, x_f, X_sint.address)
    vstms(rows_Y * cols_Y, y_f, Y_sint.address)

    A, B, C = get_specific_matrix_triple_as_matrix(triple_type)

    Z_sint = multmat_sint2sint(X_sint, Y_sint, A, B, C)

    if mode == Trunc_Mode.ON:
        Z_sfix = truncate_sfix_matrix(Z_sint, X_sfix[0][0])
    else:
        Z_sfix = sint_to_sfix_matrix(Z_sint)

    return Z_sfix


# PROBABILISTIC TRUNCATION OF A MATRIX
# X, imput sfix matrix to be truncated
# value, sfix with precission config
def truncate_sfix_matrix(X, value, number_of_truncs = 1):
    from types import sfix, sint, cint, Matrix, Array

    rows_X = len(X)
    cols_X = len(X[0])

    length = rows_X * cols_X

    x = sint(size=length)
    vldms(length, x, X.address)
    Z = Matrix(rows_X, cols_X, sfix)

    k = value.k * number_of_truncs
    m = value.f
    kappa = value.kappa

    c_p = TruncPr_parallel(x, k, m, kappa)

    vstms(length, c_p, Z.address)

    return Z


def truncate_sfix_matrix_plus_ReLU(X, number_of_truncs = 1):
    from types import sfix, sint, Matrix
    import relu_lib

    rows_X = len(X)
    cols_X = len(X[0])

    length = rows_X * cols_X

    value = sfix(0)
    truncates_bits = value.f * number_of_truncs

    x = sint(size=length)
    vldms(length, x, X.address)

    z = relu_lib.relu_trunc(x, truncates_bits)

    Z = Matrix(rows_X, cols_X, sfix)
    vstms(length, z, Z.address)

    return Z


def scale_matrix(X, number_of_truncs=1):
    from types import sfix, cint, sint, Matrix

    rows_X = len(X)
    cols_X = len(X[0])

    length = rows_X * cols_X

    value = sfix()
    shift_bits = value.f * number_of_truncs

    shift = cint((2**shift_bits), size=length)
    x = sint(size=length)
    z = sint(size=length)
    vldms(length, x, X.address)
    vmulm(length, z, x, shift)

    Z = Matrix(rows_X, cols_X, sfix)
    vstms(length, z, Z.address)

    return Z

# TRANSFORM SINT MATRIX TO SFIX
def sint_to_sfix_matrix(X):
    from types import sfix, sint, Matrix

    rows_X = len(X)
    cols_X = len(X[0])

    length = rows_X * cols_X

    x = sint(size=length)
    vldms(length, x, X.address)
    Z = Matrix(rows_X, cols_X, sfix)
    vstms(length, x, Z.address)

    return Z

# MATRIX-MATRIX MULTIPLICATION (TYPE:: sint & cint)
def multmat_sint2cint(A, B):
    from types import sint, cint, Matrix, Array

    Btp = traspose(B)

    rows_A = len(A)
    cols_A = len(A[0])
    cols_B = len(B[0])

    A_rows = []
    Btp_rows = []

    A_B = sint.Matrix(rows_A, cols_B)

    for i in range(rows_A):
        a_row_f = sint(size=cols_A)
        vldms(cols_A, a_row_f, A[i].address)
        A_rows.append(a_row_f)

    for i in range(cols_B):
        b_col_f = cint(size=cols_A)  # cols_A == cols_Btp
        vldmc(cols_A, b_col_f, Btp[i].address)
        Btp_rows.append(b_col_f)

    for i in range(rows_A):
        for j in range(cols_B):
            a_b_rowcol_f = sint(size=cols_A)
            vmulm(cols_A, a_b_rowcol_f, A_rows[i], Btp_rows[j])
            element = sint()
            sums(element, a_b_rowcol_f, cols_A)
            A_B[i][j] = element

    return A_B


# MATRIX-MATRIX MULTIPLICATION (TYPE:: cint & sint)
def multmat_cint2sint(A, B):
    from types import sint, cint, Matrix, Array

    Btp = traspose(B)

    rows_A = len(A)
    cols_A = len(A[0])
    cols_B = len(B[0])

    A_rows = []
    Btp_rows = []

    A_B = sint.Matrix(rows_A, cols_B)

    for i in range(rows_A):
        a_row_f = cint(size=cols_A)
        vldmc(cols_A, a_row_f, A[i].address)
        A_rows.append(a_row_f)

    for i in range(cols_B):
        b_col_f = sint(size=cols_A)  # colsA == colsBt
        vldms(cols_A, b_col_f, Btp[i].address)
        Btp_rows.append(b_col_f)

    for i in range(rows_A):
        for j in range(cols_B):
            a_b_rowcol_f = sint(size=cols_A)
            vmulm(cols_A, a_b_rowcol_f, Btp_rows[j], A_rows[i])
            element = sint()
            sums(element, a_b_rowcol_f, cols_A)
            A_B[i][j] = element

    return A_B


# MATRIX-MATRIX MULTIPLICATION (TYPE:: cint & cint)
def multmat_cint2cint(A, B):
    from types import cint, Matrix, Array

    Btp = traspose(B)

    rows_A = len(A)
    cols_A = len(A[0])
    cols_B = len(B[0])

    A_rows = []
    Btp_rows = []

    A_B = cint.Matrix(rows_A, cols_B)

    for i in range(rows_A):
        a_row_f = cint(size=cols_A)
        vldmc(cols_A, a_row_f, A[i].address)
        A_rows.append(a_row_f)

    for i in range(cols_B):
        b_col_f = cint(size=cols_A)  # colsA == colsBt
        vldmc(cols_A, b_col_f, Btp[i].address)
        Btp_rows.append(b_col_f)

    for i in range(rows_A):
        for j in range(cols_B):
            a_b_rowcol_f = cint(size=cols_A)
            vmulc(cols_A, a_b_rowcol_f, Btp_rows[j], A_rows[i])
            element = cint()
            sumc(element, a_b_rowcol_f, cols_A)
            A_B[i][j] = element

    return A_B


# MATRIX-MATRIX ADDITION (TYPE:: any)
def add_matrices(A, B):
    from types import Matrix, sint, cint, sfix

    rows_A = len(A)
    cols_A = len(A[0])

    value_type = A.value_type

    C = Matrix(rows_A, cols_A, value_type)

    n = rows_A * cols_A

    if value_type != cint:
        value_type = sint

    a_f = value_type(size=n)
    b_f = value_type(size=n)
    c_f = value_type(size=n)

    if (value_type == sint):

        vldms(n, a_f, A.address)
        vldms(n, b_f, B.address)
        vadds(n, c_f, a_f, b_f)
        vstms(n, c_f, C.address)

    elif (value_type == cint):

        vldmc(n, a_f, A.address)
        vldmc(n, b_f, B.address)
        vaddc(n, c_f, a_f, b_f)
        vstmc(n, c_f, C.address)

    else:
        C = A

    return C


##########################################################################################
#################################### HELPER FUNCTIONS ####################################
##########################################################################################


# MATRIX TRASPOSE
def traspose(A):
    from types import Matrix

    rows_A = len(A)
    cols_A = len(A[0])

    Atp = Matrix(cols_A, rows_A, A.value_type)

    for j in range(cols_A):
        for i in range(rows_A):
            Atp[j][i] = A[i][j]

    return Atp


# FLATTEN MATRIX INTO AN ARRAY
def flatten_to_array(A):
    from types import Matrix, Array

    rows_A = len(A);
    cols_A = len(A[0]);

    A_f = Array(cols_A * rows_A, A.value_type)

    for j in range(cols_A):
        for i in range(rows_A):
            A_f[i * cols_A + j] = A[i][j]

    return A_f


# FLATTEN MATRIX INTO A 1 ROW MATRIX - ORDERED BY COLUMNs
def flatten_to_rowmatrix(A):
    from types import Matrix, Array

    rows_A = len(A);
    cols_A = len(A[0]);

    A_f = Matrix(1, cols_A * rows_A, A.value_type)

    index = 0
    for j in range(cols_A):
        for i in range(rows_A):
            A_f[0][index] = A[i][j]
            index += 1

    return A_f


# COMPARES 2 MATRIXES (TYPE:: sfix)
# A, B, input matrixes
# tolerance, the allowed difference in the mantissa
def sfix_matrix_equals(A, B, tolerance):
    from types import Matrix, sint, cint, MemValue
    from library import if_then, else_then, end_if

    A_rows = len(A)
    A_cols = len(A[0])
    B_rows = len(B)
    B_cols = len(B[0])
    A_length = len(A) * len(A[0])
    B_length = len(B) * len(B[0])  # = A_length

    if A_rows != B_rows or A_cols != B_cols:
        return 0

    a_f = sint(size=A_length)
    b_f = sint(size=B_length)
    vldms(A_length, a_f, A.address)
    vldms(B_length, b_f, B.address)

    d_f_sint = sint(size=A_length)
    d_f_cint = cint(size=A_length)

    vsubs(A_length, d_f_sint, a_f, b_f)
    vstartopen(A_length, d_f_sint)
    vstopopen(A_length, d_f_cint)

    D = Matrix(A_rows, A_cols, cint)
    vstmc(A_length, d_f_cint, D.address)

    zero = cint(0)
    diffs = MemValue(cint(zero))

    for i in range(A_rows):
        for j in range(A_cols):
            if_then((D[i][j] > cint(tolerance)))
            diffs.write(1)
            end_if()
            if_then((D[i][j] < cint(-1 * tolerance)))
            diffs.write(1)
            end_if()

    return diffs == cint(0)


# PAD FEATURES [channel][height][width] WITH ZEROS (TYPE:: any)
# X, input feature [channel][height][width]
# l, padding
def pad_feature_with_zeros_3d(X, l):
    from library import sfix, sint, cint, Matrix

    batch_size = len(X)
    rows = len(X[0])
    cols = len(X[0][0])

    padded_batch = []

    for k in range(batch_size):
        A = Matrix(rows + (2 * l), cols + (2 * l), X[0][0].value_type)
        for i in range(rows):
            for j in range(cols):
                A[i + l][j + l] = X[k][i][j]

        padded_batch.append(A)

    return padded_batch


# PAD FEATURES [height * width][channel] WITH ZEROS (TYPE:: any)
# the feature is a 3D feature converted into 2D matrix
# X, input feature [height * width][channel]
# l, padding
def pad_feature_with_zeros_2d(X, h, w, l):
    from library import sfix, sint, cint, Matrix

    rows = len(X)

    if rows != (w * h):  ## not tested for this case
        raise NotImplementedError('wrong input configuration on "pad_feature_with_zeros_2d"')

    cols = len(X[0])
    padded_X = Matrix((w + (2 * l)) * (h + (2 * l)), cols, sfix)

    for s in range(cols):

        initial_and_final_zeros = (w + (2 * l)) * l
        intertwined_zeros = l
        index = initial_and_final_zeros
        for i in range(h):
            index = index + intertwined_zeros
            for j in range(w):
                padded_X[index][s] = X[i * w + j][s]
                index += 1
            index = index + intertwined_zeros

    return padded_X


#################################################################################################
################ REARRANGE FEATURES/KERNELS FROM 3D/4D FORMAT into 2D MATRIXES ##################
#################################################################################################


# CONVERT 3D FEATURES [s][h][w] INTO 2D MATRIX [w * h][s]
# s = channels
# h = height
# w = width
def rearrange_3d_features_into_2d_matrix(input_features):
    from types import Matrix

    s = len(input_features)
    h = len(input_features[0])
    w = len(input_features[0][0])

    output_matrix_rows = w * h
    output_matrix_columns = s
    transformed_matrix = Matrix(output_matrix_rows, output_matrix_columns, input_features[0].value_type)
    for i in range(h):
        for j in range(w):
            for k in range(s):
                transformed_matrix[i * w + j][k] = input_features[k][i][j]

    return transformed_matrix


# CONVERT 2D MATRIX [w * h][s] INTO 3D FEATURES [s][h][w]
# s = channels
# h = height
# w = width
def rearrange_2d_matrix_into_3d_features(input_feature_matrix, h, w):
    from types import Matrix

    input_height = len(input_feature_matrix)
    input_width = len(input_feature_matrix[0])

    features = []
    for i in range(input_width):
        F = Matrix(h, w, input_feature_matrix.value_type)
        for j in range(input_height):
            F[j // w][j % w] = input_feature_matrix[j][i]
        features.append(F)

    return features


# CONVERT 4D KERNELS [s_][s][kh][kw] INTO 2D MATRIX [kw * kh][s * s_]
# s_ = output channels
# s = input channels
# kh = height
# kw = width
def rearrange_4d_kernels_into_2d_matrix(kernels):
    from types import Matrix

    s_ = len(kernels)
    s = len(kernels[0])

    kh = len(kernels[0][0])
    kw = len(kernels[0][0][0])

    output_matrix_rows = kh * kw
    output_matrix_columns = s_ * s
    transformed_matrix = Matrix(output_matrix_rows, output_matrix_columns, kernels[0][0].value_type)

    for i in range(kh):
        for j in range(kw):
            for k in range(s):
                for k_ in range(s_):
                    transformed_matrix[i * kw + j][k_ * s + k] = kernels[k_][k][i][j]

    return transformed_matrix


# CONVERT 2D MATRIX [kw * kh][s * s_] INTO 4D KERNELS [s_][s][kh][kw]
# s_ = output channels
# s = input channels
# kh = 2l+1
# kw = 2l+1
# :: Note this version requries squared kernels -> kh = kw = (2l+1)
def rearrange_2d_matrix_into_4d_kernels(kernels_matrix, l, s, s_):
    from types import Matrix

    kernels = []
    for k_ in range(s_):
        kernel_ki = []
        for k in range(s):
            kernel_ki.append(Matrix((2 * l) + 1, (2 * l) + 1, kernels_matrix.value_type))
        kernels.append(kernel_ki)

    input_height = len(kernels_matrix)  # (2*l+1)^2
    input_width = len(kernels_matrix[0])  # s*s'

    for i in range(input_width):
        for j in range(input_height):
            kernels[i // s][i % s][j // (2 * l + 1)][j % (2 * l + 1)] = kernels_matrix[j][i]
    return kernels


#################################################################################################
################ PRUNE FEATURES IN 2D MATRIX FORMAT ACCORDING STRIDE  ###########################
#################################################################################################

# TAKES FEATURES IN A 2D MATRIX [h * w][s] and PRUNES ROWS ACCORDING STRIDE
# h = height, w = width, s = channels
# :: only works for stride = 2 / untested for other strides
def prune_matrix_rows_according_to_stride(Atr, h, w, stride):
    from types import Matrix

    reduced_h = (h // stride)
    reduced_w = (w // stride)
    even_w = 0

    if (h % stride) > 0:
        reduced_h += 1

    if (w % stride) > 0:
        reduced_w += 1
        even_w = 1

    Atrpr_rows = reduced_h * reduced_w
    Atrpr_cols = len(Atr[0])

    Atrpr = Matrix(Atrpr_rows, Atrpr_cols, Atr.value_type)

    counter = 0
    jump = 0
    for i in range(Atrpr_rows):

        pos_i = (i * stride) + (jump * (stride - 1) * (w - even_w))
        counter += 1
        if counter == reduced_w:  ## here I jump the stride row-wise
            jump += 1
            counter = 0

        for j in range(Atrpr_cols):
            Atrpr[i][j] = Atr[pos_i][j]

    return Atrpr


#################################################################################################
################ TRANSFORM FEATURES/KERNELS IN 2D MATRIX FORMAT SO THAT THEY  ###################
################         CAN BE MULTIPLIED TO PERFORM A CONVOLUTION           ###################
#################################################################################################


# TRANSFORM FEATURES IN 2D MATRIX [h * w][s] INTO ANOTHER 2D MATRIX [h' * w'][kh * kw * s]
# h = (input) height, w = (input) width, s = (input) channels
# h' = (output) height, w' = (output) width
# kw = kernel width, kh = kernel height
# the output is another 2D Matrix [w' * h'] [kh * kw *s]
# There is no padding, so w' = (w - kw + 1) and h' = (h - kh + 1)
# The stride prunes rows accordingly for values >1
def transform_input_features(input_features, h, w, s, kh, kw, stride=1):
    from types import Matrix

    if stride > 2:  ## not tested for this case
        raise NotImplementedError('stride > 2 not expected')

    wp = (w - kw + 1)
    hp = (h - kh + 1)
    F_rows = wp * hp
    F_cols = kw * kh * s
    F = Matrix(F_rows, F_cols, input_features[0].value_type)

    for i in range(F_rows):
        for j in range(F_cols):
            sx = j // (kh * kw)
            shift_w = i % (w - kw + 1)
            shift_h = i // (w - kw + 1)
            hx = ((j - sx * kh * kw) // kw) + shift_h
            wx = ((j - sx * kh * kw) % kw) + shift_w
            hwx = (hx * w + wx)
            F[i][j] = input_features[hwx][sx]

    if stride > 1:
        pruned_transformed_matrix = prune_matrix_rows_according_to_stride(F, hp, wp, stride)
        return pruned_transformed_matrix
    else:
        return F


# TRANSFORM FEATURES IN 2D MATRIX [h * w][s] INTO ANOTHER 2D MATRIX [h * w][(2l+1)^2 * s]
# h = (input) height, w = (input) width, s = (input) channels
# (2l+1) = kernel width = kernel height
# the output is another 2D Matrix [w' * h'] [(2l+1)^2 * s]
# There is adjusted padding = l, so w' = w and h' = h
# The stride prunes rows accordingly for values >1
def transform_input_features_adjusted_padding(input_features, l, h, w, s, stride=1):
    from types import Matrix

    if stride > 2:  ## not tested for this case
        raise NotImplementedError('stride > 2 not expected')

    output_matrix_rows = w * h
    output_matrix_columns = (2 * l + 1) ** 2 * s
    transformed_matrix = Matrix(output_matrix_rows, output_matrix_columns, input_features[0].value_type)

    for i in range(h):
        for j in range(w):
            for di in range(-l, l + 1):
                for dj in range(-l, l + 1):
                    for k in range(s):
                        pos_i = i * w + j
                        pos_j = (di + l) * (2 * l + 1) * s + (dj + l) * s + k

                        idi = i + di
                        jdj = j + dj
                        if 0 <= idi < h and 0 <= jdj < w:
                            transformed_matrix[pos_i][pos_j] = input_features[idi * w + jdj][k]
                        else:
                            transformed_matrix[pos_i][pos_j] = 0

    if stride > 1:
        pruned_transformed_matrix = prune_matrix_rows_according_to_stride(transformed_matrix, h, w, stride)
        return pruned_transformed_matrix
    else:
        return transformed_matrix


# TRANSFORM KERNELS IN 2D MATRIX [kh * kw][s * s_] INTO ANOTHER 2D MATRIX [kh * kw * s][s_]
# kh = kernel height, kw = kernel width, s = input channels, s_ = output channels
def transform_kernels(kernels, kh, kw, s, s_):
    from types import Matrix

    output_matrix_rows = kh * kw * s
    output_matrix_columns = s_
    transformed_matrix = Matrix(output_matrix_rows, output_matrix_columns, kernels.value_type)

    for i in range(output_matrix_rows):
        for j in range(output_matrix_columns):
            i2 = (i % (kh * kw))
            j2 = (j * s) + (i // (kh * kw))
            transformed_matrix[i][j] = kernels[i2][j2]

    return transformed_matrix


# TRANSFORM KERNELS IN 2D MATRIX [kh * kw][s * s_] INTO ANOTHER 2D MATRIX [kh * kw * s][s_]
# kh = kernel height, kw = kernel width, s = input channels, s_ = output channels
# kh = kw = (2l+1)
def transform_kernels_adjusted_padding(kernels, l, s, s_):
    from types import Matrix

    output_matrix_rows = (2 * l + 1) ** 2 * s
    output_matrix_columns = s_
    transformed_matrix = Matrix(output_matrix_rows, output_matrix_columns, kernels.value_type)

    for i in range(output_matrix_rows):
        for j in range(output_matrix_columns):
            i2 = (i % ((2 * l + 1) ** 2))
            j2 = (j * s) + (i // ((2 * l + 1) ** 2))
            transformed_matrix[i][j] = kernels[i2][j2]

    return transformed_matrix


#################################################################################################
################                    INPUT-OUTPUT FUNCTIONS                    ###################
################                     CONNECTION WITH DBs                      ###################
#################################################################################################


# converts input list into matrix
def input_to_matrix(input, n, m):
    from library import Array, sint

    array = Array(n * m, sint).assign(input)
    return sint.Matrix(n, m, array.address)


# Obsolete due to new CTLOAD set of instructions
# input matrix triple from channel
def input_matrix_triple(rowsA, colsA, colsB, channel=0):
    from library import open_channel_with_return, input_shares, close_channel, sint

    open_channel_with_return(channel)

    inpA = [sint()] * rowsA * colsA
    input_shares(channel, *inpA)
    A = input_to_matrix(inpA, rowsA, colsA)

    inpB = [sint()] * colsA * colsB
    input_shares(channel, *inpB)
    B = input_to_matrix(inpB, colsA, colsB)

    inpC = [sint()] * rowsA * colsB
    input_shares(channel, *inpC)
    C = input_to_matrix(inpC, rowsA, colsB)

    close_channel(channel)

    return [A, B, C]


# Obsolete due to new CTLOAD set of instructions
# output matrix triple to channel
def output_matrix_triple(matrix_triple, channel=0):
    from library import open_channel_with_return, output_shares, close_channel

    A = matrix_triple[0].to_vector()
    B = matrix_triple[1].to_vector()
    C = matrix_triple[2].to_vector()

    open_channel_with_return(channel)

    output_shares(channel, *A)
    output_shares(channel, *B)
    output_shares(channel, *C)

    close_channel(channel)


def compare_with_matrix_from_file(matrix, file_path, tolerance):
    from library import sfix, Matrix

    file = open(file_path, "r")
    values = file.read().split(" ")
    if len(values) != len(matrix.to_vector()):
        return false

    current_value_position = 0
    matrix_from_file = Matrix(matrix.rows, matrix.columns, sfix)
    for i in range(matrix.rows):
        for j in range(matrix.columns):
            matrix_from_file[i][j] = sfix(float(values[current_value_position]))
            current_value_position += 1

    return sfix_matrix_equals(matrix, matrix_from_file, tolerance)



##################################   REGISTER-BASED VERSION OF FUNCTIONS    ##################################
##################################   more efficient and tested, some are unused now  #########################
##################################        DO NOT DELETE THESE FUNCTIONS     ##################################
##################################      TESTED WITH HELPER FUNCTIONS TEST   ##################################


# use this one,
# it is faster than the one above
def flatten_to_array_reg(A):
    from types import Array, sint

    rows_A = len(A)
    cols_A = len(A[0])

    n = rows_A * cols_A

    A_f = Array(n, A.value_type)

    a_f = sint(size=n)

    vldms(n, a_f, A.address)
    vstms(n, a_f, A_f.address)

    return A_f


# returns the transposed sint matrix, operates over register memory
def traspose_sint_reg(a_f, h, w):
    from types import sint

    n = h * w
    atp_f = sint(size=n)

    for row in range(0, h):
        for col in range(0, w):
            addsi(atp_f[row + (col * h)], a_f[(row * w) + col], 0)  # RISC addition cz movs doesnt work like this

    return atp_f


# returns the transposed cint matrix, operates over register memory # RETEST before use!!
def traspose_cint_reg(a_f, h, w):
    from types import cint

    n = h * w
    atp_f = cint(size=n)

    for row in range(0, h):
        for col in range(0, w):
            addci(atp_f[row + (col * h)], a_f[(row * w) + col], 0)  # RISC addition cz movs doesnt work like this

    return atp_f



##################################   INEFFICIENT FUNCTIONS or TESTING ONLY   ###################################
##################################       DO NOT DELETE THESE FUNCTIONS       ###################################

# mult 2 sint or 2 cint matrices without specialized matrix triples / inefficient / just for testing
def mult_matrixes(A, B):
    from types import Matrix

    rows_A = len(A)
    cols_A = len(A[0])
    rows_B = len(B)
    cols_B = len(B[0])

    C = Matrix(rows_A, cols_B, A.value_type)
    if cols_A == rows_B:
        # iterate through rows of A
        for i in range(rows_A):
            # iterate through columns of B
            for j in range(cols_B):
                # iterate through rows of B
                for k in range(rows_B):
                    C[i][j] += A[i][k] * B[k][j]

    return C


# mult 1 sint by 1 cint matrix / DEPRECATED / just for testing
def mult_matrixes_w_type(A, B, type):
    from types import Matrix

    rows_A = len(A)
    cols_A = len(A[0])
    rows_B = len(B)
    cols_B = len(B[0])

    C = Matrix(rows_A, cols_A, type)
    if cols_A == rows_B:
        # iterate through rows of A
        for i in range(rows_A):
            # iterate through columns of B
            for j in range(cols_B):
                # iterate through rows of B
                for k in range(rows_B):
                    C[i][j] += A[i][k] * B[k][j]

    return C


# add 2 matrixes /  DEPRECATED / just for testing
def add_matrixes_w_type(A, B, type):
    from types import Matrix

    rows_A = len(A)  # rows_A = rows_B
    cols_A = len(A[0])  # cols_A = cols_B

    C = Matrix(rows_A, cols_A, type)
    for i in range(rows_A):
        for j in range(cols_A):
            C[i][j] = A[i][j] + B[i][j]

    return C


########################################################################################################
########################################################################################################
##################################  NOT USED FUNCTIONS, TESTING ONLY ###################################
##################################   WILL BE DELETED IN THE FUTURE   ###################################
########################################################################################################
########################################################################################################


def convolutional_layer_as_matrix_mult(input_features, kernels, h, w, s, s_):
    l = 1
    transformed_input_features = transform_input_features_adjusted_padding(input_features, l, h, w, s)
    transformed_kernels = transform_kernels_adjusted_padding(kernels, l, s, s_)
    output_matrix = mult_matrixes(transformed_input_features, transformed_kernels)
    return output_matrix


# VECTOR-MATRIX MULTIPLICATION
# X is a Matrix [1][n]
# Y is a Matrix [n][m]
def multvecmat_sint2sint(X, Y, A, B, C):
    from types import sint, cint, Matrix

    rows_A = 1
    cols_A = len(X[0])
    rows_B = len(Y)  # =colsA
    cols_B = len(Y[0])
    rows_C = rows_A
    cols_C = cols_B

    n1 = rows_A * cols_A
    n2 = rows_B * cols_B
    n3 = rows_C * cols_C

    x_f = sint(size=n1)  # features
    a_f = sint(size=n1)  # triple A
    y_f = sint(size=n2)  # kernels
    b_f = sint(size=n2)  # triple B
    c_f = sint(size=n3)  # triple C

    epsilon = sint(size=n1)
    delta = sint(size=n2)
    epsilon_clear = cint(size=n1)
    delta_clear = cint(size=n2)

    vldms(n1, x_f, X.address)
    vldms(n2, y_f, Y.address)

    ############ - - MATRIX TRIPLE - - ############
    ###############################################
    vldms(n1, a_f, A.address)
    vldms(n2, b_f, B.address)
    vldms(n3, c_f, C.address)
    ####################################

    vsubs(n1, epsilon, x_f, a_f)
    vsubs(n2, delta, y_f, b_f)

    vstartopen(n1, epsilon)
    vstopopen(n1, epsilon_clear)
    vstartopen(n2, delta)
    vstopopen(n2, delta_clear)

    E = cint.Matrix(rows_A, cols_A)
    D = cint.Matrix(rows_B, cols_B)

    vstmc(n1, epsilon_clear, E.address)
    vstmc(n2, delta_clear, D.address)

    E_B = mult_cint2sint(E, B)  # sint class
    A_D = mult_sint2cint(A, D)  # sint class
    E_D = mult_cint2cint(E, D)  # cint class

    e_b_f = sint(size=n3)
    a_d_f = sint(size=n3)
    e_d_f = cint(size=n3)

    __z_f = sint(size=n3)  # intermediate value
    _z_f = sint(size=n3)  # intermediate value
    z_f = sint(size=n3)

    vldms(n3, e_b_f, E_B.address)
    vldms(n3, a_d_f, A_D.address)
    vldmc(n3, e_d_f, E_D.address)

    vadds(n3, __z_f, e_b_f, a_d_f)
    vadds(n3, _z_f, __z_f, c_f)
    vaddm(n3, z_f, _z_f, e_d_f)

    Z = sint.Matrix(rows_C, cols_C)
    vstms(n3, z_f, Z.address)

    return Z


# VECTOR-MATRIX MULTIPLICATION
# X_sfix is a Matrix [1][n]
# Y_sfix is a Matrix [n][m]
# calls the function above multvecmat_sint2sint
def multvecmat_sfix2sfix(X_sfix, Y_sfix, A, B, C, mode = Trunc_Mode.ON):
    from types import sfix, sint, Matrix

    rows_X = 1
    cols_X = len(X_sfix[0])
    rows_Y = len(Y_sfix)
    cols_Y = len(Y_sfix[0])

    x_f = sint(size=rows_X * cols_X)
    y_f = sint(size=rows_Y * cols_Y)

    vldms(rows_X * cols_X, x_f, X_sfix.address)
    vldms(rows_Y * cols_Y, y_f, Y_sfix.address)

    X_sint = Matrix(rows_X, cols_X, sint)
    Y_sint = Matrix(rows_Y, cols_Y, sint)

    vstms(rows_X * cols_X, x_f, X_sint.address)
    vstms(rows_Y * cols_Y, y_f, Y_sint.address)

    Z_sint = multvecmat_sint2sint(X_sint, Y_sint, A, B, C)

    if mode == Trunc_Mode.ON:
        Z_sfix = truncate_sfix_matrix(Z_sint, X_sfix[0][0])
    else:
        Z_sfix = sint_to_sfix_matrix(Z_sint)

    return Z_sfix

