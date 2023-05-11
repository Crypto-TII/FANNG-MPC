from Compiler.instructions import bitsint, GC
from types import cint, sregint, regint, sbit, sint, program
from Compiler.library import get_random_from_dabit_list, combine_dabit, extract_sbits, get_dabit_list, \
    get_random_p_from_dabit
from Compiler.comparison import less_than_eq_gc_sint, less_than_eq_gc_generic

from Compiler.AdvInteger import PRandInt
from math import floor, log


class Circuit:
    NO_CIRCUIT = 0
    DEFAULT = 1
    ONLY_ANDS = 66001
    ANDS_XORS = 66002


# Cannot be NO_CIRCUIT or DEFAULT
DEFAULT_CIRCUIT = Circuit.ONLY_ANDS


class Mode:
    RABBIT_SLACK, RABBIT_LIST, RABBIT_FP, RABBIT_CONV, RABBIT_LESS_THAN, DABITS_LTZ, NONE = range(7)


def less_than_eq_custom(a, b):
    less_than_id = DEFAULT_CIRCUIT
    sregint.push(a)
    sregint.push(b)
    GC(less_than_id)
    answer = sregint.pop()
    a = sbit()
    bitsint(a, answer, 0)
    return sint(a)


# x- y <= 0
# bc of domain switching this method returns <= instead of <
# rabbit implemented with 64 bits with slack kappa
def rabbit_slack(x, y, circuit, kappa=program.security):
    M_2 = 2 ** program.bit_length
    k_2 = program.bit_length

    m = k_2  # bits analyzed in comparison --> could be 63. Will allow to use Circuit
    ll = k_2 + kappa  # size of the inputs in P
    k_total = ll - m  # size of slack Randomness (kappa + non_dabits bits, if any e.g. 40 + 1)

    R = (M_2 // 2) - 1
    r_kappa = sint()

    PRandInt(r_kappa, k_total)
    r_p, R_p, r_2, R_2 = get_random_from_dabit_list(m)

    z = x - y + R

    a = (r_p + z) + r_kappa * (2 ** m)
    _a = a.reveal()
    b = (_a + M_2 - R) % M_2

    if circuit == Circuit.NO_CIRCUIT:
        w_1 = less_than_eq_gc_sint(_a, R_2, m)
        w_2 = less_than_eq_gc_sint(b, R_2, m)
    elif circuit == Circuit.DEFAULT:
        a_mod = regint(_a % (2 ** m))
        a_sign = regint(_a >> 63)
        a_64 = sregint(a_mod + (2 ** 63) * a_sign)

        b_mod = regint(b % (2 ** m))
        b_sign = regint(b >> 63)
        b_64 = sregint(b_mod + (2 ** 63) * b_sign)

        w_1 = less_than_eq_custom(a_64, r_2)
        w_2 = less_than_eq_custom(b_64, r_2)
    else:
        raise NotImplementedError

    T = b - cint(M_2 - R)  # (done over the integers, using signs hence we need to switch)
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
    w_3 = b + R < cint(M - R) + R  # (done over the integers, using signs hence we need to switch)

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
        # algoritmically not needed. This is to be able to have k elements in the vector. but could be substituted
        # by a 0 on the first entrance on the vector.
        R_p[k - 1] = R_p[k - 1] * 0
        R_2[k - 1] = R_2[k - 1] & sbit(0)

        r_p, r_2 = combine_dabit(R_p, R_2, k)

    z = x - y

    a = (r_p + z)
    _a = a.reveal()
    b = (_a + M - R)
    w_1 = less_than_eq_gc_sint(_a, R_2, k)
    w_2 = less_than_eq_gc_sint(b, R_2, k)
    w_3 = b + R < cint(M - R) + R  # (done over the integers, using signs hence we need to switch)

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


##Aly et a. PPML
# Optimized mod2m of an sint, Catrina flow with Rabbit and dabits
# returns an sint
def dabits_mod2m(x, k, m, kappa, circuit):
    r_kappa = sint()
    k_total = k + kappa - m  # size of the mask
    r_p, R_p, r_2, R_2 = get_random_from_dabit_list(m)
    PRandInt(r_kappa, k_total)

    r_kappa = r_kappa * (2 ** m)
    r_p = r_p + 1
    r_total = r_kappa + r_p

    c = (2 ** (k - 1)) + x + r_total
    c_public = c.reveal()
    c_mod = c_public % (2 ** m)

    if circuit == Circuit.NO_CIRCUIT:
        u = less_than_eq_gc_sint(c_mod, R_2, m)
    elif circuit == Circuit.DEFAULT:
        u = less_than_eq_custom(sregint(c_mod), r_2)
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


## FACADE Methods for rabbit ##
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


#
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
