#!/usr/bin/env python2

# Copyright (c) 2024, Technology Innovation Institute, Yas Island, Abu Dhabi, United Arab Emirates.
# Copyright (c) 2017, The University of Bristol, Senate House, Tyndall Avenue, Bristol, BS8 1TH, United Kingdom.
# Copyright (c) 2021, COSIC-KU Leuven, Kasteelpark Arenberg 10, bus 2452, B-3001 Leuven-Heverlee, Belgium.


#     ===== Compiler usage instructions =====
#
# See documentation for details on the Compiler package

import os
from optparse import OptionParser
import Compiler


def main():
    usage = "usage: %prog [options] filename [args]"
    parser = OptionParser(usage=usage)
    parser.add_option("-n", "--nomerge",
                      action="store_false", dest="merge_opens", default=True,
                      help="don't attempt to merge open instructions")
    parser.add_option("-o", "--output", dest="outfile",
                      help="specify output file")
    parser.add_option("-a", "--asm-output", dest="asmoutfile",
                      help="asm output file for debugging")
    parser.add_option("-A",
                      action="store_true", dest="produce_asm", default=False,
                      help="produces .asm files in the *same* directory as the .mpc files"),
    parser.add_option("-d", "--debug", action="store_true", dest="debug",
                      help="keep track of trace for debugging")
    parser.add_option("-D", "--dead-code-elimination", action="store_true",
                      dest="dead_code_elimination", default=False,
                      help="eliminate instructions with unused result")
    parser.add_option("-r", "--noreorder", dest="reorder_between_opens",
                      action="store_false", default=True,
                      help="don't attempt to place instructions between start/stop opens")
    parser.add_option("-M", "--preserve-mem-order", action="store_true",
                      dest="preserve_mem_order", default=False,
                      help="preserve order of memory instructions; possible efficiency loss")
    parser.add_option("-u", "--noreallocate", action="store_true", dest="noreallocate",
                      default=False, help="don't reallocate")
    parser.add_option("-m", "--max-parallel-open", dest="max_parallel_open",
                      default=False, help="restrict number of parallel opens")
    parser.add_option("-P", "--profile", action="store_true", dest="profile",
                      help="profile compilation")
    parser.add_option("-s", "--stop", action="store_true", dest="stop",
                      help="stop on register errors (read before writes and double writes)")
    parser.add_option("-f", "--fdflag", action="store_false",
                      dest="fdflag", default=True,
                      help="de-activates under-over flow check for sfloats")
    parser.add_option("-F", "--fixflag", action="store_true",
                      dest="fixflag", default=False,
                      help="adjusts register sizes externally")
    options, args = parser.parse_args()

    if len(args) < 1:
        parser.print_help()
        return

    def modify_register_sizes(prog):
        config_file_path = 'src/config.h'
        reg_sizes = {
            'REG_MAXB_SIZE': prog.curr_tape.reg_counter['sb'],
            'REG_MAXC_SIZE': prog.curr_tape.reg_counter['c'],
            'REG_MAXI_SIZE': prog.curr_tape.reg_counter['sr'],
            'REG_MAXP_SIZE': prog.curr_tape.reg_counter['s']
        }

        if os.path.exists(config_file_path):
            replacements = {
                '#define ignore_memory_sizes': '#define ignore_memory_sizes 1',
                '#define REG_MAXB_SIZE': '#define REG_MAXB_SIZE ' + str(reg_sizes['REG_MAXB_SIZE']),
                '#define REG_MAXC_SIZE': '#define REG_MAXC_SIZE ' + str(reg_sizes['REG_MAXC_SIZE']),
                '#define REG_MAXI_SIZE': '#define REG_MAXI_SIZE ' + str(reg_sizes['REG_MAXI_SIZE']),
                '#define REG_MAXP_SIZE': '#define REG_MAXP_SIZE ' + str(reg_sizes['REG_MAXP_SIZE'])
            }

            with open(config_file_path, 'r') as file:
                lines = file.readlines()

            modified_lines = []
            for line in lines:
                for key, value in replacements.items():
                    if line.startswith(key):
                        line = value + '\n'
                        break
                modified_lines.append(line)

            with open(config_file_path, 'w') as file:
                file.writelines(modified_lines)

            print("Modification of register sizes in " +
                  config_file_path + " completed successfully.")

        else:
            print("Config file not found at " +
                  config_file_path + ". Skipping modification of register sizes.")

        return reg_sizes

    def compilation():
        prog = Compiler.run(args, options,
                            merge_opens=options.merge_opens,
                            debug=options.debug)
        prog.write_bytes(options.outfile)
        if options.fixflag:
            reg_sizes = modify_register_sizes(prog)
            log_filename = args[0] + '/' + prog.curr_tape.name + '.log'
            with open(log_filename, 'w') as log_file:
                log_file.write("Register Sizes:\n")
                sorted_reg_sizes = sorted(reg_sizes.items()) 
                for key, value in sorted_reg_sizes:
                    log_file.write("{}: {}\n".format(key, value))
            print("Register sizes logged in " + log_filename)

        if options.asmoutfile:
            for tape in prog.tapes:
                tape.write_str(options.asmoutfile + '-' + tape.name+'.asm')
        if options.produce_asm:
            for tape in prog.tapes:
                tape.write_str(args[0]+'/'+tape.name+'.asm')

    if options.profile:
        import cProfile
        p = cProfile.Profile().runctx('compilation()', globals(), locals())
        p.dump_stats(args[0] + '.prof')
        p.print_stats(2)
    else:
        compilation()


if __name__ == '__main__':
    main()
