from Compiler.instructions import GC, OGC, LOADGC, EGC


class Circuit:
    ONLY_ANDS = 66001
    ANDS_XORS = 66002


def persist_circuits(circuit_id, total):
    OGC(circuit_id, total)
