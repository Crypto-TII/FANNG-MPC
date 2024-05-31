# Copyright (c) 2024, Technology Innovation Institute, Yas Island, Abu Dhabi, United Arab Emirates.

from Compiler.types import sint, sfix
from Compiler.rabbit_lib import trunc_LTZ, rabbit_sint, Mode as rabbit_mode

class Gradient:
    OFF, ON = range(2)


class Mode:
    SFIX, SINT = range(2)


class relu_response:
    def __init__(self, x, c):
        self.__x = x
        self.__c = c

    def get_value(self):
        return self.__x

    def get_gradient(self):
        return self.__c

    def reveal(self):
        return relu_response(self.__x.reveal(), self.__c.reveal())

    def __equals__(self, other):
        return (self.get_value() == other.get_value()) * (self.get_gradient() == other.get_gradient())

    def __ne__(self, other):
        return 1 - (self.get_value() == other.get_value()) * (self.get_gradient() == other.get_gradient())

    # in this case, for complex types, a register of type string would make sense.
    def print_reg(self):
        return self.__x * self.__c


def relu(x, gradient=Gradient.OFF):
    c = 1 - rabbit_sint(x, 0, rabbit_mode.DABITS_LTZ)
    result = c * x + 0
    if gradient == Gradient.ON:
        return relu_response(result, c)
    elif gradient == Gradient.OFF:
        return result


# returns RELU(x) on sfix as in pytorch
def relu_sfix(x, gradient=Gradient.OFF):
    result = relu(x.v, gradient)

    if gradient == Gradient.ON:
        return relu_response(sfix(result.get_value()), result.get_gradient())
    elif gradient == Gradient.OFF:
        return sfix(result)


# X is a 2D matrix
# returns RELU(X) as in pytorch
# returns Matrices as it is the requirement
def relu_2d(X, gradient=Gradient.OFF, mode=Mode.SFIX):
    rows_X = len(X)
    cols_X = len(X[0])

    if mode == Mode.SINT:
        local_type = sint
    elif mode == Mode.SFIX:
        local_type = sfix
    else:
        raise NotImplementedError

    relu_X = local_type.Matrix(rows_X, cols_X)
    relu_G = sint.Matrix(rows_X, cols_X)

    for i in range(rows_X):
        for j in range(cols_X):
            if mode == Mode.SINT:
                local_result = relu(X[i][j], gradient)  # comparison with 0 happens here
            elif mode == Mode.SFIX:
                local_result = relu_sfix(X[i][j], gradient)  # comparison with 0 happens here

            if gradient == Gradient.ON:
                relu_X[i][j] = local_result.get_value()
                relu_G[i][j] = local_result.get_gradient()
            elif gradient == Gradient.OFF:
                relu_X[i][j] = local_result

    if gradient == Gradient.ON:
        return relu_X, relu_G
    elif gradient == Gradient.OFF:
        return relu_X
    

# X is a 3D matrix
# returns RELU(X) as in pytorch
# returns Matrices as it is the requirement
def relu_3d(X, gradient=Gradient.OFF, mode=Mode.SFIX):
    dimensions = len(X)
    rows_X = len(X[0])
    cols_X = len(X[0][0])

    relu_X = []
    relu_G = []
    if mode == Mode.SINT:
        local_type = sint
    elif mode == Mode.SFIX:
        local_type = sfix
    else:
        raise NotImplementedError

    for k in range(dimensions):
        Y = local_type.Matrix(rows_X, cols_X)
        G = sint.Matrix(rows_X, cols_X)
        for i in range(rows_X):
            for j in range(cols_X):
                local_result = None
                if mode == Mode.SINT:
                    local_result = relu(X[k][i][j], gradient)  # comparison with 0 happens here
                elif mode == Mode.SFIX:
                    local_result = relu_sfix(X[k][i][j], gradient)  # comparison with 0 happens here

                if gradient == Gradient.ON:
                    Y[i][j] = local_result.get_value()
                    G[i][j] = local_result.get_gradient()
                elif gradient == Gradient.OFF:
                    Y[i][j] = local_result
        relu_X.append(Y)
        if gradient == Gradient.ON:
            relu_G.append(G)
    if gradient == Gradient.ON:
        return relu_X, relu_G
    elif gradient == Gradient.OFF:
        return relu_X


# returns RELU(x/2^{m}) on sint 
# m denotes 'batch' number of truncations together
# supports vectorization
def relu_trunc(x, m, gradient=Gradient.OFF):
    x_t, b = trunc_LTZ(x, m)
    c = 1 - b
    result = c * x_t + 0
    if gradient == Gradient.ON:
        return relu_response(result, c)
    elif gradient == Gradient.OFF:
        return result  


# returns RELU(x/2^{m_t}) on sfix 
# m_t denotes 'batch' number of truncations together
# supports vectorization
def relu_trunc_sfix(x, batch, gradient=Gradient.OFF):
    m_t = (x.f) * batch
    result = relu_trunc(x.v, m_t, gradient)

    if gradient == Gradient.ON:
        return relu_response(sfix(result.get_value()), result.get_gradient())
    elif gradient == Gradient.OFF:
        return sfix(result)
    
    
# X is a 2D matrix
# returns RELU(X/2^{m_t}) as in pytorch
# returns Matrices as it is the requirement
def relu_trunc_2d(X, batch, gradient=Gradient.OFF, mode=Mode.SFIX):
    rows_X = len(X)
    cols_X = len(X[0])

    if mode == Mode.SINT:
        local_type = sint
        local_function = relu_trunc
    elif mode == Mode.SFIX:
        local_type = sfix
        local_function = relu_trunc_sfix
    else:
        raise NotImplementedError

    relu_X = local_type.Matrix(rows_X, cols_X)
    relu_G = sint.Matrix(rows_X, cols_X)

    for i in range(rows_X):
        for j in range(cols_X):
            local_result = local_function(X[i][j], batch, gradient)  # truncation and comparison with 0 happens here

            if gradient == Gradient.ON:
                relu_X[i][j] = local_result.get_value()
                relu_G[i][j] = local_result.get_gradient()
            elif gradient == Gradient.OFF:
                relu_X[i][j] = local_result

    if gradient == Gradient.ON:
        return relu_X, relu_G
    elif gradient == Gradient.OFF:
        return relu_X
  

# X is a 3D matrix
# returns RELU(X/2^{m_t}) as in pytorch
# returns Matrices as it is the requirement
def relu_trunc_3d(X, batch, gradient=Gradient.OFF, mode=Mode.SFIX):
    dimensions = len(X)
    rows_X = len(X[0])
    cols_X = len(X[0][0])

    relu_X = []
    relu_G = []
    if mode == Mode.SINT:
        local_type = sint
        local_function = relu_trunc
    elif mode == Mode.SFIX:
        local_type = sfix
        local_function = relu_trunc_sfix
    else:
        raise NotImplementedError

    for k in range(dimensions):
        Y = local_type.Matrix(rows_X, cols_X)
        G = sint.Matrix(rows_X, cols_X)
        for i in range(rows_X):
            for j in range(cols_X):
                local_result = local_function(X[k][i][j], batch, gradient)  # truncation and comparison with 0 happens here

                if gradient == Gradient.ON:
                    Y[i][j] = local_result.get_value()
                    G[i][j] = local_result.get_gradient()
                elif gradient == Gradient.OFF:
                    Y[i][j] = local_result
        relu_X.append(Y)
        if gradient == Gradient.ON:
            relu_G.append(G)
    if gradient == Gradient.ON:
        return relu_X, relu_G
    elif gradient == Gradient.OFF:
        return relu_X
