# Copyright (c) 2024, Technology Innovation Institute, Yas Island, Abu Dhabi, United Arab Emirates.

from Compiler import types, instructions

class Program(object):
    def __init__(self, progname):
        types.program = self
        instructions.program = self
        self.curr_tape = None
        execfile(progname)
    def malloc(self, *args):
        pass
