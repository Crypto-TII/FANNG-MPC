# Copyright (c) 2024, Technology Innovation Institute, Yas Island, Abu Dhabi, United Arab Emirates.

from Compiler.instructions import bitsint, GC, EGC, vldms, vstmc, vadds, vsubmr, vaddsi, vldmsint, vbitsint
from types import cint, sregint, regint, sbit, sint, program
from Compiler.library import get_random_from_dabit_list, combine_dabit, extract_sbits, get_dabit_list, \
    get_random_p_from_dabit, get_random_from_dabits_trunc_mixed
from Compiler.comparison import less_than_eq_gc_sint, less_than_eq_gc_generic

from Compiler.AdvInteger import PRandInt
from math import floor, log
from Compiler.bounded_randomness_lib import get_next_randval, get_next_randvals_vectorized


class Circuit:
    NO_CIRCUIT = 0
    DEFAULT = 1
    ONLY_ANDS = 66001
    ANDS_XORS = 66002


class Garbling:
    NUMBER_OF_TYPES = 2
    OFFLINE_GARBLING, ONLINE_GARBLING = range(NUMBER_OF_TYPES)


# Cannot be NO_CIRCUIT or DEFAULT
DEFAULT_CIRCUIT = Circuit.ONLY_ANDS
DEFAULT_GARBLING = Garbling.ONLINE_GARBLING


class Mode:
    NUMBER_OF_TYPES = 8
    RABBIT_SLACK, RABBIT_LIST, RABBIT_FP, RABBIT_CONV, RABBIT_LESS_THAN, DABITS_LTZ, DABITS_TRUNC_LTZ, NONE = range(
        NUMBER_OF_TYPES)


def get_concrete_circuit(circuit):
    if circuit == Circuit.DEFAULT:
        return DEFAULT_CIRCUIT
    else:
        return circuit


def less_than_eq_custom_sregint(a, b, less_than_id=DEFAULT_CIRCUIT, garbling=DEFAULT_GARBLING):
    sregint.push(a)
    sregint.push(b)
    if garbling == Garbling.OFFLINE_GARBLING:
        EGC(less_than_id)
    elif garbling == Garbling.ONLINE_GARBLING:
        GC(less_than_id)
    answer = sregint.pop()
    return answer


def less_than_eq_custom_sbit(a, b, less_than_id=DEFAULT_CIRCUIT, garbling=DEFAULT_GARBLING):
    answer = less_than_eq_custom_sregint(a, b, less_than_id, garbling)
    a = sbit()
    bitsint(a, answer, 0)
    return a


def less_than_eq_custom(a, b, less_than_id=DEFAULT_CIRCUIT, garbling=DEFAULT_GARBLING):
    a = less_than_eq_custom_sbit(a, b, less_than_id, garbling)
    return sint(a)


# x- y <= 0
# bc of domain switching this method returns <= instead of <
# rabbit implemented with 64 bits with slack kappa
def rabbit_slack(x, y, circuit, kappa=program.security):
    M_2 = 2 ** program.bit_length
    k_2 = program.bit_length

    m = k_2  # bits analyzed in comparison --> could be 63. Will allow to use Circuit
    ll = k_2 + kappa  # size of the inputs in P
    # size of slack Randomness (kappa + non_dabits bits, if any e.g. 40 + 1)
    k_total = ll - m

    R = (M_2 // 2) - 1
    r_kappa = sint()

    PRandInt(r_kappa, k_total)
    r_p, _, r_2, R_2 = get_random_from_dabit_list(m)

    z = x - y + R

    a = (r_p + z) + r_kappa * (2 ** m)
    _a = a.reveal()
    b = (_a + M_2 - R) % M_2
    circuit_l = get_concrete_circuit(circuit)
    if circuit_l == Circuit.NO_CIRCUIT:
        w_1 = less_than_eq_gc_sint(_a, R_2, m)
        w_2 = less_than_eq_gc_sint(b, R_2, m)
    else:
        a_mod = regint(_a % (2 ** m))
        a_sign = regint(_a >> 63)
        a_64 = sregint(a_mod + (2 ** 63) * a_sign)

        b_mod = regint(b % (2 ** m))
        b_sign = regint(b >> 63)
        b_64 = sregint(b_mod + (2 ** 63) * b_sign)

        w_1 = less_than_eq_custom(a_64, r_2, less_than_id=circuit_l)
        w_2 = less_than_eq_custom(b_64, r_2, less_than_id=circuit_l)

    # (done over the integers, using signs hence we need to switch)
    T = b - cint(M_2 - R)
    w_3 = cint(T < 0)

    return 1 - (w_1 - w_2 + w_3)


# x - y <= 0
# rabbit implemented with 64 bits no slack
def rabbit_list(x, y):
    M = program.P
    k = int(floor(log(M, 2))) + 1
    R = (M - 1) // 2

    r_p, R_p, r_2, R_2 = get_random_p_from_dabit(M - 1, k)

    z = x - y
    a = (r_p + z)
    _a = a.reveal()
    b = (_a + M - R)

    w_1 = less_than_eq_gc_sint(_a, R_2, k)
    w_2 = less_than_eq_gc_sint(b, R_2, k)
    # (done over the integers, using signs hence we need to switch)
    w_3 = b + R < cint(M - R) + R

    return w_1 - w_2 + w_3


# x- y <= 0
# above (the prime)
# rabbit implemented with 64 bits no slack
def rabbit_fp(x, y, above=0):
    M = program.P
    k = int(floor(log(M, 2))) + 1
    R = (M - 1) // 2

    if above == 1:
        r_p, R_p, r_2, R_2 = get_random_from_dabit_list(k)
    else:
        R_p, R_2 = get_dabit_list(k)
        # algorithmically not needed. This is to be able to have k elements in the vector. But could be substituted
        # by a 0 on the first entrance on the vector.
        R_p[k - 1] = R_p[k - 1] * 0
        R_2[k - 1] = R_2[k - 1] & sbit(0)

        r_p, _ = combine_dabit(R_p, R_2, k)

    z = x - y

    a = (r_p + z)
    _a = a.reveal()
    b = (_a + M - R)
    w_1 = less_than_eq_gc_sint(_a, R_2, k)
    w_2 = less_than_eq_gc_sint(b, R_2, k)
    # (done over the integers, using signs hence we need to switch)
    w_3 = b + R < cint(M - R) + R

    return w_1 - w_2 + w_3


# x- y < 0
# rabbit implemented with 64 bits no slack (looses 1 bit of precision)
def rabbit_conversion(x, y):
    z = x - y
    _z = sregint(z)
    bit = sbit()
    bitsint(bit, _z, program.bit_length - 1)
    return sregint(bit)


# x <= y
# above (the prime)
# rabbit implemented with 64 bits no slack
def rabbit_less_than(x, y):
    k = program.bit_length
    X_bits = extract_sbits(x, k)
    Y_bits = extract_sbits(y, k)
    return sregint(less_than_eq_gc_generic(X_bits, Y_bits, k))


# Aly et a. PPML
# Optimized mod2m of an sint, Catrina flow with Rabbit and dabits
# returns an sint
def dabits_mod2m(x, k, m, kappa, circuit, ):
    r_kappa = sint()
    k_total = k + kappa - m  # size of the mask
    r_p, _, r_2, R_2 = get_random_from_dabit_list(m)
    PRandInt(r_kappa, k_total)

    r_kappa = r_kappa * (2 ** m)
    r_p = r_p + 1
    r_total = r_kappa + r_p

    c = (2 ** (k - 1)) + x + r_total
    c_public = c.reveal()
    c_mod = c_public % (2 ** m)
    circuit_l = get_concrete_circuit(circuit)

    if circuit == Circuit.NO_CIRCUIT:
        u = less_than_eq_gc_sint(c_mod, R_2, m)
    else:
        u = less_than_eq_custom(sregint(c_mod), r_2, less_than_id=circuit_l)

    a = c_mod - r_p + (2 ** m) * u

    return a


# Optimized trunc of an sint, Catrina flow with Rabbit and dabits
# returns an sint
def dabits_trunc(a, k, m, kappa, circuit):
    a_prime = dabits_mod2m(a, k, m, kappa, circuit)
    d = a - a_prime

    return d / (2 ** m)


# Optimized LTZ of an sint, Catrina flow with Rabbit and dabits
# returns an sint
def dabits_LTZ(x, y, circuit):
    k = program.bit_length
    m = program.bit_length - 1
    kappa = program.security
    a = x - y
    s = dabits_trunc(a, k, m, kappa, circuit)
    return -s


# Optimized LTZ of an sint, Catrina flow with Rabbit and dabits
# combined with a truncation operation of the input
# returns two sints
def dynamic_dabits_trunc_LTZ(a, k, m_c, m_t, kappa, circuit):
    """ 
     trunc_LTZ with explicit parameters k, m_c, m_t, kappa, circuit
    """
    two_to_k = 2**(k-1)
    two_to_m_t = 2**(m_t)
    two_to_m_c = 2**(m_c)

    r_kappa = get_next_randval(k + kappa - m_c)
    r_p, R_p, r_2, R_2 = get_random_from_dabit_list(m_c)
    r_prime, _ = combine_dabit(R_p, R_2, m_t)
    r_total = two_to_m_c * r_kappa + r_p

    c = a + two_to_k + r_total
    c_public = c.reveal()

    # Truncation
    if m_t == 0:
        a_t = a
    else:
        c_prime = c_public % two_to_m_t
        a_prime = c_prime - r_prime
        a_t = (a - a_prime) / two_to_m_t

    # Comparison
    c_public = c_public + 1
    c_mod = c_public % two_to_m_c
    circuit_l = get_concrete_circuit(circuit)
    if circuit == Circuit.NO_CIRCUIT:
        u = less_than_eq_gc_sint(c_mod, R_2, m_c)
    else: 
        u = less_than_eq_custom(sregint(c_mod), r_2, less_than_id= circuit_l)
   
    a_dprime = c_mod - (r_p + 1) + two_to_m_c * u
    b = -(a - a_dprime) / two_to_m_c

    return a_t, b


# Optimized LTZ of an sint, Catrina flow with Rabbit and dabits
# combined with a truncation operation of the input.
# Supports vectorization and return two sints
def dynamic_dabits_trunc_LTZ_vectorized(a, k, m_c, m_t, kappa, total, circuit):
    """ 
     vectorized trunc_LTZ with explicit parameters k, m_c, m_t, kappa, circuit
    """
    two_to_k = 2**(k-1)
    two_to_m_t = 2**(m_t)
    two_to_m_c = 2**(m_c)

    r_kappa = get_next_randvals_vectorized(k + kappa - m_c, total)
    r_p, r_2, R_2, r_prime = get_random_from_dabits_trunc_mixed(
        m_c, m_t, total)

    r_kappa_shift = two_to_m_c * r_kappa
    r_total = sint(size=total)
    vadds(total, r_total, r_kappa_shift, r_p)

    c = a + two_to_k + r_total
    c_public = c.reveal()

    # Truncation
    if m_t == 0:
        a_t = a
    else:
        c_prime = c_public % two_to_m_t
        a_prime = sint(size=total)
        vsubmr(total, a_prime, c_prime, r_prime)
        a_t = (a - a_prime) / two_to_m_t

    # Comparison
    c_public = c_public + 1
    c_mod = c_public % two_to_m_c

    c_mod_list = cint.Array(total)
    vstmc(total, c_mod, c_mod_list.address)

    circuit_l = get_concrete_circuit(circuit)
    if circuit_l == Circuit.NO_CIRCUIT:
        u_list = sint.Array(total)
    else:
        u_list = sregint.Array(total)
    
    for i in range(total):
        if circuit_l == Circuit.NO_CIRCUIT:
            u_list[i] = less_than_eq_gc_sint(c_mod_list[i], R_2[i], m_c)
        else:
            u_list[i] = less_than_eq_custom_sregint(
                sregint(c_mod_list[i]), r_2[i], less_than_id=circuit_l)

    if circuit == Circuit.NO_CIRCUIT:
        u = sint.Array(total)
        vldms(total, u, u_list.address)
    else:
        u_sregint = sregint(size=total)
        u_sbit = sbit(size=total)
        vldmsint(total, u_sregint, u_list.address)
        vbitsint(total, u_sbit, u_sregint, 0)
        u = sint(u_sbit, size=total)

    r_p_plus_1 = sint(size=total)
    vaddsi(total, r_p_plus_1, r_p, 1)

    a_dprime = c_mod - r_p_plus_1 + two_to_m_c * u
    b = -(a - a_dprime) / two_to_m_c

    return a_t, b


def dabits_trunc_LTZ(a, m_t, circuit=Circuit.DEFAULT):
    """ 
     Probabilistic truncation a_t = [a/2^{m_t} + u]
     where Pr[u = 1] = (a % 2^{m_t}) / 2^{m_t} and
     Rabbit Comparison LTZ: b = a < 0
     Supports vectorization
    """
    kappa = program.security
    k = program.bit_length
    m_c = program.bit_length - 1
    total = a.size
    if total == 1:
        a_t, b = dynamic_dabits_trunc_LTZ(a, k, m_c, m_t, kappa, circuit)
    else:
        a_t, b = dynamic_dabits_trunc_LTZ_vectorized(
            a, k, m_c, m_t, kappa, total, circuit)
    return a_t, b


## FACADE Methods ##

# rabbit main facade
# x and y are sints (if not choose RABBIT_LESS_THAN mode),
# note that conv returns x<=y instead of x<y
# returns a sregint
def rabbit(x, y, mode=Mode.DABITS_LTZ):
    """
     the difference of the inputs is bounded to 2^63 on either side (+/-)
     If need be to compare absolute numbers in 2^62<x<2^64 use mode less_than
    """
    c = 0
    if mode == Mode.RABBIT_SLACK:
        c = rabbit_slack(x, y, Circuit.DEFAULT)
    elif mode == Mode.RABBIT_LIST:
        c = rabbit_list(x, y)
    elif mode == Mode.RABBIT_FP:
        c = rabbit_fp(x, y)
    elif mode == Mode.RABBIT_CONV:
        c = 1 - rabbit_conversion(y, x)
    elif mode == Mode.RABBIT_LESS_THAN:
        c = rabbit_less_than(x, y)
    elif mode == Mode.DABITS_LTZ:
        c = 1 - dabits_LTZ(y, x, Circuit.DEFAULT)
    return c


# rabbit facade
# x and y are sints and returns x<=y
# returns a sint
def rabbit_sint(x, y, mode=Mode.DABITS_LTZ):
    _z = rabbit(x, y, mode)
    if isinstance(_z, sregint):
        bit = sbit()
        bitsint(bit, _z, 0)
        return sint(bit)
    elif isinstance(_z, sbit):
        return sint(_z)
    elif isinstance(_z, sint):
        return _z
    else:
        raise NotImplementedError


# trunc LTZ facade
# a is an sint, m_t is public and returns a/2^{m_t} and a<=0
# returns two sints
def trunc_LTZ(a, m_t, mode=Mode.DABITS_TRUNC_LTZ):
    """ 
     Probabilistic truncation a_t = [a/2^{m_t} + u]
     where Pr[u = 1] = (a % 2^{m_t}) / 2^{m_t} and
     Rabbit Comparison LTZ: b = a < 0
    """
    if mode == Mode.DABITS_TRUNC_LTZ:
        a_t, _b = dabits_trunc_LTZ(a, m_t, Circuit.DEFAULT)
    else:
        raise NotImplementedError

    return a_t, _b
