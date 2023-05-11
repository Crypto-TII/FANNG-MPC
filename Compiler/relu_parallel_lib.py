from Compiler.types import sint, sfix
from Compiler.rabbit_lib import rabbit_sint, Mode as rabbit_mode
from Compiler.instructions import vsubsfi, vldms, vstms, vmuls


class Gradient:
    OFF, ON = range(2)

class Mode:
    SFIX, SINT = range(2)

def relu_2d_parallel(matrix, gradient=Gradient.OFF, mode=Mode.SFIX):
    from library import Matrix

    if mode is not Mode.SINT and mode is not Mode.SFIX:
        raise NotImplementedError

    matrix_as_vector = matrix.to_vector()
    relu_x, relu_g = relu_2d_parallel_with_gradient(matrix_as_vector, len(matrix_as_vector))
    matrix_relu_x = Matrix(matrix.rows, matrix.columns, matrix.value_type, relu_x.address)
    matrix_relu_g = sint.Matrix(matrix.rows, matrix.columns, relu_g.address)

    if gradient == Gradient.OFF:
        return matrix_relu_x

    return matrix_relu_x, matrix_relu_g

def relu_2d_parallel_with_gradient(matrix_as_vector, size):
    local_type = matrix_as_vector.value_type
    relu_X = local_type.Array(size)
    relu_G = sint.Array(size)

    if local_type == sint:
        relu_X, relu_G = relu_parallel(matrix_as_vector)  # comparison with 0 happens here
    elif local_type == sfix:
        relu_X, relu_G = relu_sfix_parallel(matrix_as_vector)  # comparison with 0 happens here

    return relu_X, relu_G

def relu_parallel(vector):

    size = len(vector)

    ### START: rabbit[i] = rabbit_sint(matrix[i], 0, rabbit_mode.DABITS_LTZ)

    rabbit = sint.Array(size)

    for i in range(size):
        if vector.value_type == sfix:
            rabbit[i] = rabbit_sint(vector[i].v, 0, rabbit_mode.DABITS_LTZ)
        elif vector.value_type == sint:
            rabbit[i] = rabbit_sint(vector[i], 0, rabbit_mode.DABITS_LTZ)

    rabbit_ = sint(size=size)
    vldms(size, rabbit_, rabbit.address)

    ### END: rabbit[i] = rabbit_sint(matrix[i], 0, rabbit_mode.DABITS_LTZ)

    ### START: c[i] = 1 - rabbit[i]

    c = sint(size=size)
    vsubsfi(size, c, rabbit_, 1)

    c_vector = sint.Array(size)
    vstms(size, c, c_vector.address)

    ### FINISH: c[i] = 1 - rabbit[i]

    ### START: result = c[i] * matrix[i] + 0

    m = sint(size=size)
    vldms(size, m, vector.address)

    result = sint(size=size)
    vmuls(size, result, c, m)

    ### FINISH: result = c[i] * matrix[i] + 0

    result_vector = sint.Array(size)
    vstms(size, result, result_vector.address)

    return result_vector, c_vector


def relu_sfix_parallel(vector):
    result_vector, c = relu_parallel(vector)
    sfix_result_vector = sfix.Array(len(result_vector))
    for i in range(len(result_vector)):
        sfix_result_vector[i] = sfix(result_vector[i])
    return sfix_result_vector, c


def relu_3d_parallel(matrix_3D, gradient=Gradient.OFF, mode=Mode.SFIX):
    if mode is not Mode.SINT and mode is not Mode.SFIX:
        raise NotImplementedError

    relu_X = []
    relu_G = []
    dimensions = len(matrix_3D)
    for i in range(dimensions):
        X, G = relu_2d_parallel(matrix_3D[i], Gradient.ON, mode)
        relu_X.append(X)
        relu_G.append(G)

    if gradient == Gradient.ON:
        return relu_X, relu_G

    return relu_X

