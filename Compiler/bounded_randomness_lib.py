# Copyright (c) 2024, Technology Innovation Institute, Yas Island, Abu Dhabi, United Arab Emirates.

from Compiler.instructions import OSRAND, LOADSRAND, SRAND, vSRAND
from Compiler.types import sint


def store_randvals_to_db(bound, total):
    OSRAND(bound, total)
    return 0


def load_randvals_from_db(bound, total):
    LOADSRAND(bound, total)
    return 0


def get_next_randval(bound):
    value = sint()
    SRAND(bound, value)
    return value


def get_next_randvals(bound, total):
    values = []
    [values.append(sint()) for _ in range(total)]
    SRAND(bound, *values)
    return values


def get_next_randvals_vectorized(bound, total):
    values = sint(size=total)
    vSRAND(total, bound, values)
    return values
