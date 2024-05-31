# Copyright (c) 2024, Technology Innovation Institute, Yas Island, Abu Dhabi, United Arab Emirates.

T = 99999

def ineq(a, b):
    return a < b


def vector_permutation(v, factor):
    return v


def matrix_permutation(u, factor):
    return u


def obtain_random_factor(n):
    return 0


def exchange_row_matrix(i, j, u):
    for k in range(len(u[0])):
        a = u[j][k]
        u[j][k] = u[i][k]
        u[i][k] = a


def exchange_vector(i, j, v):
    a = v[j]
    v[j] = v[i]
    v[i] = a
    return v


def ternary_operator(c, a, b):
    return c * (a-b) + b


def dijkstra_optimized(weights, source):

    n = len(weights)
    distance = [T] * n
    alpha = [0] * n
    vertex_id = [int(i) for i in range(n)]
    distance[source] = 0

    factor = obtain_random_factor(0)
    p_weights = matrix_permutation(weights, factor)
    p_vertex_id = vector_permutation(vertex_id, factor)

    for i in range(n):
        d_prime = T
        v = int(0)
        for j in range(n-1, i-1, -1):
            c = ineq(distance[j], d_prime)
            v = ternary_operator(c, j, v)
            d_prime = ternary_operator(c, distance[j], d_prime)
        v_open = v#.reveal()

        exchange_row_matrix(i, v_open, p_weights)
        exchange_vector(i, v_open, distance)
        exchange_vector(i, v_open, p_vertex_id)

        for j in range(i+1, n):
            a = distance[i] + p_weights[i][j]

            c = ineq(a,  distance[j])
            distance[j] = ternary_operator(c, a, distance[j])
            alpha[j] = ternary_operator(c, p_vertex_id[i], alpha[j])
    return alpha


test_matrix = [[T, 7, 5, 1, T], [T, T, 2, 4, T], [
    T, T, T, T, 9], [T, T, 2, T, 2], [T, T, T, T, T]]

result_vector = dijkstra_optimized(test_matrix, 0)

print (result_vector)


