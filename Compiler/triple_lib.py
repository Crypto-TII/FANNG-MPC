# Copyright (c) 2024, Technology Innovation Institute, Yas Island, Abu Dhabi, United Arab Emirates.

from Compiler.instructions import OTRIPLE, LOADTRIPLE, triple, vtriple
from Compiler.types import sint


def store_triples_to_db(size):
    OTRIPLE(size)
    return 0


def load_triples_from_db(size):
    LOADTRIPLE(size)
    return 0


# Returns a Beaver triple of the form s_i, s_j, s_k
# Vectorizable, returns three consecutive sint registers
# s_{3i}, s_{3i+1} and s_{3i+2}, for i=0,...,size-1
def get_next_triple(size=None):
    if size is None:
        next_triple = (sint(), sint(), sint())
    else:
        next_triple = (sint(size=3*size))
    triple(*next_triple)
    return next_triple


# Returns 'size' number of Beaver triples in List form
# (next_triple[3i],next_triple[3i+1],next_triple[3i+2])
# forms the ith triple, for i=0,...,size-1
def get_triples_list(size):
    next_triples = []
    [next_triples.append(sint()) for _ in range(3*size)]
    triple(*next_triples)
    return next_triples


# Returns 'size' number of Beaver triples in List of tuples form
# with next_triple[i] as the ith triple of form (a, b, c),
# for i=0,...,size-1
def get_triples_tuple(size):
    next_triples = []
    next_triples_list = get_triples_list(size)
    [next_triples.append(next_triples_list[i:i+3])
     for i in range(0, len(next_triples_list), 3)]
    return next_triples


# Returns a tuple of form (a, b, c), with each of a, b, and c
# being 'size'-sized registers and (a_{i}, b_{i}, c_{i})
# forms a triple, for i=0,...,size-1
def get_triples_vectorized(size):
    if size == 1:
        return get_next_triple()
    a = sint(size=size)
    b = sint(size=size)
    c = sint(size=size)
    next_triples = (a, b, c)
    vtriple(size, *next_triples)
    return next_triples
