# Copyright (c) 2024, Technology Innovation Institute, Yas Island, Abu Dhabi, United Arab Emirates.

from Compiler.instructions import ODABIT, LOADDABIT, dabit, vdabit
from Compiler.types import sbit, sint


def store_dabits_to_db(size):
    ODABIT(size)


def load_dabits_from_db(size):
   LOADDABIT(size)


# Returns a dabit in the form of bp, b2.
# Vectorizable, returns inputs in the form:
# (bp[i], b2[i])
def get_next_dabit(size=None):
    bp = sint(size=size)
    b2 = sbit(size=size)
    if size is None:
        dabit(bp, b2)
    else:
        vdabit(size, *(bp, b2))
    return bp, b2


# Returns 'size' number of dabits in List form
# (bp[i], b2[i]) forms the ith dabit
# for i=0 ,.., size-1
def get_dabits_list(size):
    bp = []
    b2 = []
    [bp.append(sint()) for _ in range(size)]
    [b2.append(sbit()) for _ in range(size)]
    [dabit(bp[i], b2[i]) for i in range(size)]
    return bp, b2


# Returns 'size' number of dabits in List of tuples form
# with next_dabit[i] as the ith dabit of form (bp, b2),
# for i=0,..,size-1
def get_dabits_tuple(size):
    bp, b2 = get_dabits_list(size)
    next_dabit = []
    [next_dabit.append([bp[i], b2[i]]) for i in range(size)]
    return next_dabit
