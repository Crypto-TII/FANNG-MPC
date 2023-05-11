from Compiler.instructions import LOADCT, CTRIPLE, CT_DYN
from Compiler.types import sint


class TripleType:
    triple_type_id = 0
    rows_A = 0
    columns_A = 0

    rows_B = 0
    columns_B = 0

    rows_C = 0
    columns_C = 0

    ONLY_TYPE = None

    def __init__(self, triple_type_id, rows_A_, columns_A_, rows_B_, columns_B_, rows_C_, columns_C_):

        self.triple_type_id = triple_type_id

        self.rows_A = rows_A_
        self.columns_A = columns_A_

        self.rows_B = rows_B_
        self.columns_B = columns_B_

        self.rows_C = rows_C_
        self.columns_C = columns_C_

    def get_total_shares(self):
        return self.rows_A * self.columns_A + self.rows_B * self.columns_B + self.rows_C * self.columns_C

    def format_matrix_triple(self, values):
        a_total = self.rows_A * self.columns_A
        b_total = self.rows_B * self.columns_B
        c_total = self.rows_C * self.columns_C
        A = []
        B = []
        C = []
        for i, value in enumerate(values):
            if (i < a_total):
                A.append(value)
            elif (i < a_total+b_total):
                B.append(value)
            elif (i < a_total+b_total+c_total):
                C.append(value)
            else:
                raise NotImplementedError
        return A, B, C

def get_NN_triple_dimensions(w, h, s, kh, kw, s_, stride, padding):

    w_in = w + (padding * 2)
    h_in = h + (padding * 2)

    w_out = (w_in - kw + 1)
    h_out = (h_in - kh + 1)

    if stride == 2:

        w_out_p = (w_out // stride)
        h_out_p = (h_out // stride)

        if (w_in % stride) > 0:
            w_out_p += 1

        if (h_in % stride) > 0:
            h_out_p += 1

        w_out = w_out_p
        h_out = h_out_p

    A = [w_in * h_in, s]
    B = [kh * kw, s * s_]
    C = [w_out * h_out, s_]
    return A, B, C


class NNTripleType(TripleType):
    def __init__(self, triple_type_id, w, h, s, kh, kw, s_, stride, padding):

        self.triple_type_id = triple_type_id

        A, B, C = get_NN_triple_dimensions(
            w, h, s, kh, kw, s_, stride, padding)

        self.rows_A = A[0]
        self.columns_A = A[1]
        self.rows_B = B[0]
        self.columns_B = B[1]
        self.rows_C = C[0]
        self.columns_C = C[1]


class TRIPLE_TYPES:
    LENET_1_1 = TripleType(777777, 25, 2, 9, 4, 9, 2)
    PRUNED_RESNET_TRIPLE = NNTripleType(777778, 5, 5, 2, 3, 3, 2, 1, 0)

    # ########### Custom Matrix Triple Types Below this Line ###########
    #             Types need to match Base_Matrix_Triple.cpp


def load_triples_from_db(triple_type, total):
    LOADCT(triple_type.triple_type_id, total)


def get_specific_matrix_triple(triple_type):
    values = []
    [values.append(sint()) for _ in range(triple_type.get_total_shares())]
    CT_DYN(triple_type.triple_type_id, *values)
    return triple_type.format_matrix_triple(values)


def set_ONLY_TYPE(triple_type):
    TripleType.ONLY_TYPE = triple_type


def get_next_matrix_triple():
    values = []
    [values.append(sint())
     for _ in range(TripleType.ONLY_TYPE.get_total_shares())]
    CTRIPLE(*values)
    return TripleType.ONLY_TYPE.format_matrix_triple(values)



