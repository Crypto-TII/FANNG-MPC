# Copyright (c) 2024, Technology Innovation Institute, Yas Island, Abu Dhabi, United Arab Emirates.

from Compiler.instructions import LOADCT, CTRIPLE, CT_DYN, vstms
from Compiler.types import sint
from Compiler.offline_triple_lib import TripleType


# ########### Custom Matrix Triple Types Below this Line ###########
#             Types need to match Base_Matrix_Triple.cpp

def load_matrix_triples_from_db(triple_type, total):
    LOADCT(triple_type.triple_type_id, total)


def get_specific_matrix_triple(triple_type):
    values = []
    [values.append(sint()) for _ in range(triple_type.get_total_shares())]
    CT_DYN(triple_type.triple_type_id, *values)
    return triple_type.format_matrix_triple(values)


def get_specific_matrix_triple_as_vector(triple_type):
    A, B, C = get_specific_matrix_triple(triple_type)
    A_l = A[0]
    B_l = B[0]
    C_l = C[0]

    A_l.size = len(A)
    B_l.size = len(B)
    C_l.size = len(C)

    return A_l, B_l, C_l


def get_specific_matrix_triple_as_matrix(triple_type):
    A, B, C = get_specific_matrix_triple_as_vector(triple_type)

    A_l = sint.Matrix(triple_type.rows_A, triple_type.columns_A)
    B_l = sint.Matrix(triple_type.rows_B, triple_type.columns_B)
    C_l = sint.Matrix(triple_type.rows_A, triple_type.columns_C)

    vstms(A.size, A, A_l.address)
    vstms(B.size, B, B_l.address)
    vstms(C.size, C, C_l.address)

    return A_l, B_l, C_l


def set_ONLY_TYPE(triple_type):
    TripleType.ONLY_TYPE = triple_type


def get_next_matrix_triple():
    values = []
    [values.append(sint())
     for _ in range(TripleType.ONLY_TYPE.get_total_shares())]
    CTRIPLE(*values)
    return TripleType.ONLY_TYPE.format_matrix_triple(values)


def get_next_matrix_triple_as_vector():
    A, B, C = get_next_matrix_triple()
    A_l = A[0]
    B_l = B[0]
    C_l = C[0]

    A_l.size = len(A)
    B_l.size = len(B)
    C_l.size = len(C)

    return A_l, B_l, C_l


def get_next_matrix_triple_as_matrix():
    A, B, C = get_next_matrix_triple_as_vector()

    A_l = sint.Matrix(triple_type.rows_A, triple_type.columns_A)
    B_l = sint.Matrix(triple_type.rows_B, triple_type.columns_B)
    C_l = sint.Matrix(triple_type.rows_A, triple_type.columns_C)

    vstms(A.size, A, A_l.address)
    vstms(B.size, B, B_l.address)
    vstms(C.size, C, C_l.address)

    return A_l, B_l, C_l
