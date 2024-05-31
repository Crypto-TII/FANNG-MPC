# Copyright (c) 2024, Technology Innovation Institute, Yas Island, Abu Dhabi, United Arab Emirates.

from Compiler.types import sint
from Compiler.library import if_then, end_if, print_ln, print_str
T = 99999


def test_print_vector_secret(V):
    [print_str("%s ", v.reveal()) for v in V]
    print_ln(" ")


def ineq(a, b):
    return a < b


def vector_permutation(v, factor):
    return v


def matrix_permutation(u, factor):
    return u


def obtain_random_factor(n):
    return 0


def exchange_elements(c, a, b):
    aux_a = ternary_operator(c, b, a)
    aux_b = ternary_operator(c, a, b)
    return aux_a, aux_b


def exchange_row_matrix(i, j, u):

    for h in range(len(u)):
        c = h == j
        for k in range(len(u[0])):
            u[i][k], u[h][k] = exchange_elements(c, u[i][k], u[h][k])
    return u


def exchange_vector(i, j, v):
    for h in range(len(v)):
        c = h == j
        v[i], v[h] = exchange_elements(c, v[i], v[h])
    return v


def ternary_operator(c, if_true, if_false):
    return c * (if_true - if_false) + if_false


def dijkstra_optimized(weights, source):
    n = len(weights)
    distance = [T] * n
    alpha = [T] * n
    vertex_id = [i for i in range(n)]
    distance[source] = sint(0)

    factor = obtain_random_factor(0)
    p_weights = matrix_permutation(weights, factor)
    p_vertex_id = vector_permutation(vertex_id, factor)

    for i in range(n):
        d_prime = T
        v = sint(0)
        for j in range(n-1, i-1, -1):
            c = ineq(distance[j], d_prime)
            v = ternary_operator(c, j, v)
            d_prime = ternary_operator(c, distance[j], d_prime)
        v_open = None
        if isinstance(v, sint):            
            v_open = v.reveal()
        else: 
            v_open = v
        exchange_row_matrix(i, v_open, p_weights)
        exchange_vector(i, v_open, distance)
        exchange_vector(i, v_open, p_vertex_id)

        for j in range(i+1, n):
            a = distance[i] + p_weights[i][j]
            c = ineq(a,  distance[j])
            distance[j] = ternary_operator(c, a, distance[j])
            alpha[j] = ternary_operator(c, p_vertex_id[i], alpha[j])
    return alpha
