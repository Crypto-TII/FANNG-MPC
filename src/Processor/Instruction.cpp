/*
Copyright (c) 2024, Technology Innovation Institute, Yas Island, Abu Dhabi, United Arab Emirates.
Copyright (c) 2017, The University of Bristol, Senate House, Tyndall Avenue, Bristol, BS8 1TH, United Kingdom.
Copyright (c) 2021, COSIC-KU Leuven, Kasteelpark Arenberg 10, bus 2452, B-3001 Leuven-Heverlee, Belgium.


*/

#include "Processor/Instruction.h"
#include "Exceptions/Exceptions.h"
#include "Local/Float.h"
#include "Offline/offline_data.h"
#include "Processor/Processor.h"
#include "Tools/Crypto.h"
#include "Tools/parse.h"
#include "Tools/util_containers.h"

#include <algorithm>
#include <limits>
#include <map>
#include <mutex>
#include <sstream>
#include <stdlib.h>
#include <unistd.h>

#include "Mod2Engine/Mod2Maurer.h"
#include "Mod2Engine/Mod2_Thread.h"
#include "OT/OT_Thread_Data.h"

extern OT_Thread_Data OTD;
extern Mod2_Thread_Data MTD;

extern Base_Circuits Global_Circuit_Store;
extern vector<sacrificed_data> SacrificeD;

using namespace std;

template <class SRegint, class SBit> void Instruction<SRegint, SBit>::parse(istream &s)
{
    n = 0;
    start.resize(0);
    r[0] = 0;
    r[1] = 0;
    r[2] = 0;
    r[3] = 0;

    int pos = s.tellg();
    opcode = get_int(s);
    size = opcode >> 9;
    opcode &= 0x1FF;

    if (size == 0)
        size = 1;

    parse_operands(s, pos);
}

void BaseInstruction::parse_operands(istream &s, int pos)
{
    int num_var_args = 0;
    switch (opcode)
    {
    // instructions with 3 register (or 3 integer) operands
    case ADDC:
    case ADDS:
    case ADDM:
    case SUBC:
    case SUBS:
    case SUBML:
    case SUBMR:
    case MULC:
    case MULM:
    case DIVC:
    case MODC:
    case ANDC:
    case XORC:
    case ORC:
    case SHLC:
    case SHRC:
    case LTINT:
    case GTINT:
    case EQINT:
    case ADDINT:
    case SUBINT:
    case MULINT:
    case DIVINT:
    case MODINT:
    case ADDSINT:
    case ADDSINTC:
    case SUBSINT:
    case SUBSINTC:
    case SUBCINTS:
    case MULSINT:
    case MULSINTC:
    case DIVSINT:
    case SAND:
    case XORSB:
    case ANDSB:
    case ORSB:
    case ANDSINT:
    case ANDSINTC:
    case ORSINT:
    case ORSINTC:
    case XORSINT:
    case XORSINTC:
    case ANDINT:
    case ORINT:
    case XORINT:
    case SHLINT:
    case SHRINT:
    case MREVC:
    case MREVS:
    case MBITDECC:
    case MBITDECINT:
        r[0] = get_int(s);
        r[1] = get_int(s);
        r[2] = get_int(s);
        break;
        // instructions with 4 register (or integer) operands
    case RUN_TAPE:
    case MADDC:
    case MADDS:
    case MADDM:
    case MSUBC:
    case MSUBS:
    case MSUBML:
    case MSUBMR:
    case MMULC:
    case MMULM:
    case MDIVC:
    case MMODC:
    case MEVALCC:
    case MEVALSC:
        r[0] = get_int(s);
        r[1] = get_int(s);
        r[2] = get_int(s);
        n = get_int(s);
        break;
        // instructions with 2 register operands
    case LDMCI:
    case LDMSI:
    case STMCI:
    case STMSI:
    case MOVC:
    case MOVS:
    case MOVINT:
    case MOVSB:
    case LDMINTI:
    case STMINTI:
    case LEGENDREC:
    case DABIT:
    case CONVINT:
    case LTZINT:
    case EQZINT:
    case RAND:
    case DIGESTC:
    case NEG:
    case NEGB:
    case CONVSINTSREG:
    case CONVREGSREG:
    case CONVSREGSINT:
    case CONVSUREGSINT:
    case CONVSINTSBIT:
    case CONVSBITSINT:
    case OPENSINT:
    case OPENSBIT:
    case LDMSINTI:
    case STMSINTI:
    case MOVSINT:
    case INVSINT:
    case INVINT:
    case EQZSINT:
    case LTZSINT:
    case PEEKINT:
    case PEEKSINT:
    case PEEKC:
    case PEEKS:
    case PEEKSBIT:
    case POKEINT:
    case POKESINT:
    case POKEC:
    case POKES:
    case POKESBIT:
    case RPEEKINT:
    case RPEEKSINT:
    case RPEEKC:
    case RPEEKS:
    case RPEEKSBIT:
    case RPOKEINT:
    case RPOKESINT:
    case RPOKEC:
    case RPOKES:
    case RPOKESBIT:
    case NEWC:
    case NEWS:
    case NEWINT:
    case NEWSINT:
        r[0] = get_int(s);
        r[1] = get_int(s);
        break;
        // instructions with 1 register operand
    case PRINT_REG:
    case LDTN:
    case LDARG:
    case STARG:
    case PUSHINT:
    case PUSHSINT:
    case PUSHC:
    case PUSHS:
    case PUSHSBIT:
    case POPINT:
    case POPSINT:
    case POPC:
    case POPS:
    case POPSBIT:
    case GETSPINT:
    case GETSPSINT:
    case GETSPC:
    case GETSPS:
    case GETSPSBIT:
    case PRINT_CHAR_REGINT:
    case PRINT_CHAR4_REGINT:
    case PRINT_INT:
    case PRINT_IEEE_FLOAT:
    case CALLR:
    case JMPR:
    case DELETEC:
    case DELETES:
    case DELETEINT:
    case DELETESINT:
    case RANDC:
    case RANDINT:
    case RANDSINT:
    case RANDFLOAT:
    case RANDSBIT:
        r[0] = get_int(s);
        break;
    case OTRIPLE:
        n = get_int(s);
        break;
    case LOADTRIPLE:
        n = get_int(s);
        break;
    // instructions with 2 registers + 1 integer operand
    case OSRAND:
        n = get_int(s);
        m = get_int(s);
        break;
    case LOADSRAND:
        n = get_int(s);
        m = get_int(s);
        break;
    case ODABIT:
        n = get_int(s);
        break;
    case LOADDABIT:
        n = get_int(s);
        break;
    case ADDCI:
    case ADDSI:
    case SUBCI:
    case SUBSI:
    case SUBCFI:
    case SUBSFI:
    case MULCI:
    case MULSI:
    case DIVCI:
    case MODCI:
    case ANDCI:
    case XORCI:
    case ORCI:
    case SHLCI:
    case SHRCI:
    case SHLSINT:
    case SHRSINT:
    case NOTC:
    case CONVMODP:
    case BITSINT:
        r[0] = get_int(s);
        r[1] = get_int(s);
        n = get_int(s);
        break;
        // instructions with 2 registers + 1 integer operand
    case SINTBIT:
        r[0] = get_int(s);
        r[1] = get_int(s);
        r[2] = get_int(s);
        n = get_int(s);
        break;
        // instructions with 1 register + 1 integer operand
    case LDI:
    case LDSI:
    case LDMC:
    case LDMS:
    case STMC:
    case STMS:
    case LDMINT:
    case STMINT:
    case LDINT:
    case INPUT_CLEAR:
    case OUTPUT_CLEAR:
    case INPUT_INT:
    case OPEN_CHANNEL:
    case OUTPUT_INT:
    case LDMSINT:
    case STMSINT:
    case LDSINT:
    case LDSBIT:
        r[0] = get_int(s);
        n = get_int(s);
        break;
        // instructions with 1 reg + 1 player + 1 integer operand
    case PRIVATE_INPUT:
    case PRIVATE_OUTPUT:
        r[0] = get_int(s);
        p = get_int(s);
        m = get_int(s);
        break;
        // instructions with 2 reg + 1 player + 1 integer
    case MPRIVATE_INPUT:
    case MPRIVATE_OUTPUT:
        r[0] = get_int(s);
        r[1] = get_int(s);
        p = get_int(s);
        m = get_int(s);
        break;
        // instructions with 1 reg + 2 integer operand
    case PRINT_FIX:
    case JMPNE:
    case JMPEQ:
        r[0] = get_int(s);
        n = get_int(s);
        m = get_int(s);
        break;
        // instructions with 1 integer operand
    case PRINT_CHAR4:
    case PRINT_CHAR:
    case JMP:
    case CALL:
    case START_CLOCK:
    case STOP_CLOCK:
    case CLOSE_CHANNEL:
    case PRINT_MEM:
    case JOIN_TAPE:
    case GC:
    case EGC:
    case LF:
        n = get_int(s);
        break;
    case OGC:
        n = get_int(s);
        m = get_int(s);
        break;
    case LOADGC:
        n = get_int(s);
        m = get_int(s);
        break;
    // instructions with no operand
    case CLEAR_MEMORY:
    case CLEAR_REGISTERS:
    case RESTART:
    case CRASH:
    case RETURN:
        break;
    // instructions with 4 register operands
    case MUL2SINT:
        get_vector(4, start, s);
        break;
    // instructions with 5 register operands
    case PRINT_FLOAT:
        get_vector(5, start, s);
        break;
        // open instructions instructions with variable length args
    case BIT:
    case TRIPLE:
    case SQUARE:
    case STARTOPEN:
    case STOPOPEN:
        num_var_args = get_int(s);
        get_vector(num_var_args, start, s);
        break;
        // As above, but with a trailing int argument
    case OUTPUT_SHARES:
    case INPUT_SHARES:
        // subtract player/channel number from the number of arguments
        num_var_args = get_int(s) - 1;
        p = get_int(s);
        get_vector(num_var_args, start, s);
        break;
    case REQBL:
        n = get_int(s);
        if (n > 0 && gfp::pr() < bigint(1) << (n - 1))
        {
            cout << "Tape requires prime of bit length " << n << endl;
            throw invalid_params();
        }
        break;
        // below the new summation bytecodes
    case SUMS:
        r[0] = get_int(s);
        r[1] = get_int(s);
        n = get_int(s);
        break;
    case SUMC:
        r[0] = get_int(s);
        r[1] = get_int(s);
        n = get_int(s);
        break;
    case LOADCT:
        n = get_int(s);
        m = get_int(s);
        break;
    case CTRIPLE:
        n = get_int(s);
        get_vector(n, start, s);
        break;
    case CT_DYN:
        n = get_int(s);
        m = get_int(s);
        get_vector(n - 1, start, s);
        break;
    case SRAND:
        n = get_int(s);
        m = get_int(s);
        get_vector(n - 1, start, s);
        break;
    default:
        ostringstream os;
        os << "Invalid instruction :'( " << hex << showbase << opcode << " at " << dec << pos
           << " the opcode is: " << opcode << " the end";

        throw Invalid_Instruction(os.str());
    }
}

RegType BaseInstruction::get_reg_type() const
{
    switch (opcode)
    { // List here commands which write to a specific type of register or a direct memory access
    case LDMINT:
    case LDMINTI:
    case MOVINT:
    case POPINT:
    case PEEKSINT:
    case RPEEKSINT:
    case PUSHINT:
    case PUSHSINT:
    case POKEINT:
    case RPOKEINT:
    case POKESINT:
    case RPOKESINT:
    case GETSPINT:
    case GETSPSINT:
    case GETSPS:
    case GETSPC:
    case GETSPSBIT:
    case LDTN:
    case LDARG:
    case INPUT_INT:
    case RAND:
    case LDINT:
    case CONVMODP:
    case ADDINT:
    case SUBINT:
    case MULINT:
    case DIVINT:
    case MODINT:
    case LTZINT:
    case LTINT:
    case GTINT:
    case EQINT:
    case EQZINT:
    case STMINT:
    case STMSINT:
    case STMSINTI:
    case LDMSINT:
    case LDMSINTI:
    case MOVSINT:
    case LDSINT:
    case ADDSINT:
    case ADDSINTC:
    case SUBSINT:
    case SUBSINTC:
    case SUBCINTS:
    case MULSINT:
    case MULSINTC:
    case DIVSINT:
    case SHLSINT:
    case SHRSINT:
    case NEG:
    case SAND:
    case SINTBIT:
    case CONVSINTSREG:
    case CONVREGSREG:
    case OPENSINT:
    case OPENSBIT:
    case ANDSINT:
    case ANDSINTC:
    case ORSINT:
    case ORSINTC:
    case XORSINT:
    case XORSINTC:
    case INVSINT:
    case ANDINT:
    case ORINT:
    case XORINT:
    case INVINT:
    case SHLINT:
    case SHRINT:
    case MUL2SINT:
    case OPEN_CHANNEL:
    case NEWC:
    case NEWS:
    case NEWINT:
    case NEWSINT:
    case DELETEC:
    case DELETES:
    case DELETEINT:
    case DELETESINT:
    case RANDINT:
    case RANDSINT:
    case RANDFLOAT:
        return INT;
    case MOVSB:
    case XORSB:
    case ANDSB:
    case ORSB:
    case NEGB:
    case LTZSINT:
    case EQZSINT:
    case BITSINT:
    case POPSBIT:
    case PUSHSBIT:
    case PEEKSBIT:
    case RPEEKSBIT:
    case POKESBIT:
    case RPOKESBIT:
    case CONVSINTSBIT:
    case LDSBIT:
    case RANDSBIT:
        return SBIT;
    case OTRIPLE:
    case LOADTRIPLE:
    case ODABIT:
    case LOADDABIT:
    case STARG:
    case REQBL:
    case RUN_TAPE:
    case JOIN_TAPE:
    case CRASH:
    case CLEAR_MEMORY:
    case CLEAR_REGISTERS:
    case PRINT_MEM:
    case PRINT_REG:
    case PRINT_CHAR:
    case PRINT_CHAR4:
    case PRINT_CHAR_REGINT:
    case PRINT_CHAR4_REGINT:
    case PRINT_FLOAT:
    case PRINT_FIX:
    case PRINT_INT:
    case PRINT_IEEE_FLOAT:
    case CLOSE_CHANNEL:
    case OUTPUT_SHARES:
    case OUTPUT_INT:
    case PRIVATE_OUTPUT:
    case MPRIVATE_OUTPUT:
    case JMP:
    case JMPNE:
    case JMPEQ:
    case STARTOPEN:
    case START_CLOCK:
    case STOP_CLOCK:
    case CALL:
    case CALLR:
    case JMPR:
    case RETURN:
    case GC:
    case OGC:
    case LOADGC:
    case EGC:
    case LF:
        return NONE;
    case SUMS:
    case SUMC:
    case LOADCT:
    case CTRIPLE:
    case CT_DYN:
    case SRAND:
    case DABIT:
        return DUAL;
    default:
        return MODP;
    }
}

/* This does an overestimate as it counts ALL values, even
 * if they are reading. But the reg_type looks only at
 * the RETURN value type. So we will overcount some register
 * usage. If we got the exact register usage it would cost
 * more logic, and would save little in terms of memeory
 *
 * The trick is that if a register is read it must be
 * written so if we only count the instructions which
 * write, and then take the max register in that
 * instruction it will be OK
 *
 * So if we had
 *      blah with c0,c1,s0,s1 registers
 *      c5 <- add c0, c1
 *      s3 <- add c5, s1
 * Then the max registers *should* be
 *    c : 5    s: 3
 * But we actually count
 *    c : 5    s: 5
 * due to the c5 read in the last instruction. But we only
 * need a max and 3<5 so all is OK
 *
 * Dual is a weird one to catch the different write types
 * of the DABIT instruction
 *
 */
int BaseInstruction::get_max_reg(RegType reg_type) const
{
    if ((get_reg_type() == reg_type) || (get_reg_type() == DUAL && (reg_type == SBIT || reg_type == MODP)))
    {
        if (start.size())
            return *max_element(start.begin(), start.end()) + size;
        else
            return *max_element(r, r + 3) + size;
    }
    return 0;
}

template <class SRegint, class SBit> ostream &operator<<(ostream &s, const Instruction<SRegint, SBit> &instr)
{
    // Is it vectorized?
    if (instr.size != 1)
    {
        s << "V";
    }

    // Then the main opcode
    switch (instr.opcode)
    {
    case LDI:
        s << "LDI";
        break;
    case LDSI:
        s << "LDSI";
        break;
    case LDMC:
        s << "LDMC";
        break;
    case LDMS:
        s << "LDMS";
        break;
    case STMC:
        s << "STMC";
        break;
    case STMS:
        s << "STMS";
        break;
    case LDMCI:
        s << "LDMCI";
        break;
    case LDMSI:
        s << "LDMSI";
        break;
    case STMCI:
        s << "STMCI";
        break;
    case STMSI:
        s << "STMSI";
        break;
    case MOVC:
        s << "MOVC";
        break;
    case MOVS:
        s << "MOVS";
        break;
    case LDMINT:
        s << "LDMINT";
        break;
    case MOVSB:
        s << "MOVSB";
        break;
    case STMINT:
        s << "STMINT";
        break;
    case LDMINTI:
        s << "LDMINTI";
        break;
    case STMINTI:
        s << "STMINTI";
        break;
    case NEWC:
        s << "NEWC";
        break;
    case DELETEC:
        s << "DELETEC";
        break;
    case NEWS:
        s << "NEWS";
        break;
    case DELETES:
        s << "DELETES";
        break;
    case NEWINT:
        s << "NEWINT";
        break;
    case DELETEINT:
        s << "DELETEINT";
        break;
    case NEWSINT:
        s << "NEWSINT";
        break;
    case DELETESINT:
        s << "DELETESINT";
        break;
    case PUSHINT:
        s << "PUSHINT";
        break;
    case PUSHSINT:
        s << "PUSHSINT";
        break;
    case PUSHS:
        s << "PUSHS";
        break;
    case PUSHC:
        s << "PUSHC";
        break;
    case PUSHSBIT:
        s << "PUSHSBIT";
        break;
    case POPINT:
        s << "POPINT";
        break;
    case POPSINT:
        s << "POPSINT";
        break;
    case POPS:
        s << "POPS";
        break;
    case POPC:
        s << "POPC";
        break;
    case POPSBIT:
        s << "POPSBIT";
        break;
    case PEEKINT:
        s << "PEEKINT";
        break;
    case PEEKSINT:
        s << "PEEKSINT";
        break;
    case PEEKS:
        s << "PEEKS";
        break;
    case PEEKC:
        s << "PEEKC";
        break;
    case PEEKSBIT:
        s << "PEEKSBIT";
        break;
    case RPEEKINT:
        s << "RPEEKINT";
        break;
    case RPEEKSINT:
        s << "RPEEKSINT";
        break;
    case RPEEKS:
        s << "RPEEKS";
        break;
    case RPEEKC:
        s << "RPEEKC";
        break;
    case RPEEKSBIT:
        s << "RPEEKSBIT";
        break;
    case POKEINT:
        s << "POKEINT";
        break;
    case POKESINT:
        s << "POKESINT";
        break;
    case POKES:
        s << "POKES";
        break;
    case POKEC:
        s << "POKEC";
        break;
    case POKESBIT:
        s << "POKESBIT";
        break;
    case RPOKEINT:
        s << "RPOKEINT";
        break;
    case RPOKESINT:
        s << "RPOKESINT";
        break;
    case RPOKES:
        s << "RPOKES";
        break;
    case RPOKEC:
        s << "RPOKEC";
        break;
    case RPOKESBIT:
        s << "RPOKESBIT";
        break;
    case GETSPINT:
        s << "GETSPINT";
        break;
    case GETSPSINT:
        s << "GETSPSINT";
        break;
    case GETSPS:
        s << "GETSPS";
        break;
    case GETSPC:
        s << "GETSPC";
        break;
    case GETSPSBIT:
        s << "GETSPSBIT";
        break;
    case MOVINT:
        s << "MOVINT";
        break;
    case LDTN:
        s << "LDTN";
        break;
    case LDARG:
        s << "LDARG";
        break;
    case REQBL:
        s << "REQBL";
        break;
    case STARG:
        s << "STARG";
        break;
    case CALL:
        s << "CALL";
        break;
    case RETURN:
        s << "RETURN";
        break;
    case CALLR:
        s << "CALLR";
        break;
    case JMPR:
        s << "JMPR";
        break;
    case RUN_TAPE:
        s << "RUN_TAPE";
        break;
    case JOIN_TAPE:
        s << "JOIN_TAPE";
        break;
    case CRASH:
        s << "CRASH";
        break;
    case RESTART:
        s << "RESTART";
        break;
    case CLEAR_MEMORY:
        s << "CLEAR_MEMORY";
        break;
    case CLEAR_REGISTERS:
        s << "CLEAR_REGISTERS";
        break;
    case ADDC:
        s << "ADDC";
        break;
    case ADDS:
        s << "ADDS";
        break;
    case ADDM:
        s << "ADDM";
        break;
    case ADDCI:
        s << "ADDCI";
        break;
    case ADDSI:
        s << "ADDSI";
        break;
    case SUBC:
        s << "SUBC";
        break;
    case SUBS:
        s << "SUBS";
        break;
    case SUBML:
        s << "SUBML";
        break;
    case SUBMR:
        s << "SUBMR";
        break;
    case SUBCI:
        s << "SUBCI";
        break;
    case SUBSI:
        s << "SUBSI";
        break;
    case SUBCFI:
        s << "SUBCFI";
        break;
    case SUBSFI:
        s << "SUBSFI";
        break;
    case MULC:
        s << "MULC";
        break;
    case MULM:
        s << "MULM";
        break;
    case MULCI:
        s << "MULCI";
        break;
    case MULSI:
        s << "MULSI";
        break;
    case DIVC:
        s << "DIVC";
        break;
    case DIVCI:
        s << "DIVCI";
        break;
    case MODC:
        s << "MODC";
        break;
    case MODCI:
        s << "MODCI";
        break;
    case LEGENDREC:
        s << "LEGENDREC";
        break;
    case DIGESTC:
        s << "DIGESTC";
        break;
    case MADDC:
        s << "MADDC";
        break;
    case MADDS:
        s << "MADDS";
        break;
    case MADDM:
        s << "MADDM";
        break;
    case MSUBC:
        s << "MSUBC";
        break;
    case MSUBS:
        s << "MSUBS";
        break;
    case MSUBML:
        s << "MSUBML";
        break;
    case MSUBMR:
        s << "MSUBMR";
        break;
    case MMULC:
        s << "MMULC";
        break;
    case MMULM:
        s << "MMULM";
        break;
    case MDIVC:
        s << "MDIVC";
        break;
    case MMODC:
        s << "MMODC";
        break;
    case MREVC:
        s << "MREVC";
        break;
    case MREVS:
        s << "MREVS";
        break;
    case MEVALCC:
        s << "MEVALCC";
        break;
    case MEVALSC:
        s << "MEVALSC";
        break;
    case MBITDECC:
        s << "MBITDECC";
        break;
    case MBITDECINT:
        s << "MBITDECINT";
        break;
    case OUTPUT_CLEAR:
        s << "OUTPUT_CLEAR";
        break;
    case INPUT_CLEAR:
        s << "INPUT_CLEAR";
        break;
    case OUTPUT_SHARES:
        s << "OUTPUT_SHARES";
        break;
    case INPUT_SHARES:
        s << "INPUT_SHARES";
        break;
    case PRIVATE_INPUT:
        s << "PRIVATE_INPUT";
        break;
    case PRIVATE_OUTPUT:
        s << "PRIVATE_OUTPUT";
        break;
    case MPRIVATE_INPUT:
        s << "MPRIVATE_INPUT";
        break;
    case MPRIVATE_OUTPUT:
        s << "MPRIVATE_OUTPUT";
        break;
    case OUTPUT_INT:
        s << "OUTPUT_INT";
        break;
    case INPUT_INT:
        s << "INPUT_INT";
        break;
    case OPEN_CHANNEL:
        s << "OPEN_CHANNEL";
        break;
    case CLOSE_CHANNEL:
        s << "CLOSE_CHANNEL";
        break;
    case STARTOPEN:
        s << "STARTOPEN";
        break;
    case STOPOPEN:
        s << "STOPOPEN";
        break;
    case BIT:
        s << "BIT";
        break;
    case SQUARE:
        s << "SQUARE";
        break;
    case DABIT:
        s << "DABIT";
        break;
    case OTRIPLE:
        s << "OTRIPLE";
        break;
    case LOADTRIPLE:
        s << "LOADTRIPLE";
        break;
    case TRIPLE:
        s << "TRIPLE";
        break;
    case ANDC:
        s << "ANDC";
        break;
    case XORC:
        s << "XORC";
        break;
    case ORC:
        s << "ORC";
        break;
    case ANDCI:
        s << "ANDCI";
        break;
    case XORCI:
        s << "XORCI";
        break;
    case ORCI:
        s << "ORCI";
        break;
    case NOTC:
        s << "NOTC";
        break;
    case SHLC:
        s << "SHLC";
        break;
    case SHRC:
        s << "SHLR";
        break;
    case SHLCI:
        s << "SHLCI";
        break;
    case SHRCI:
        s << "SHRCI";
        break;
    case JMP:
        s << "JMP";
        break;
    case JMPNE:
        s << "JMPNE";
        break;
    case JMPEQ:
        s << "JMPEQ";
        break;
    case EQZINT:
        s << "EQZINT";
        break;
    case LTZINT:
        s << "LTZINT";
        break;
    case LTINT:
        s << "LTINT";
        break;
    case GTINT:
        s << "GTINT";
        break;
    case EQINT:
        s << "EQINT";
        break;
    case LDINT:
        s << "LDINT";
        break;
    case ADDINT:
        s << "ADDINT";
        break;
    case SUBINT:
        s << "SUBINT";
        break;
    case MULINT:
        s << "MULINT";
        break;
    case DIVINT:
        s << "DIVINT";
        break;
    case MODINT:
        s << "MODINT";
        break;
    case CONVINT:
        s << "CONVINT";
        break;
    case CONVMODP:
        s << "CONVMODP";
        break;
    case PRINT_MEM:
        s << "PRINT_MEM";
        break;
    case PRINT_REG:
        s << "PRINT_REG";
        break;
    case PRINT_CHAR:
        s << "PRINT_CHAR";
        break;
    case PRINT_CHAR4:
        s << "PRINT_CHAR4";
        break;
    case PRINT_CHAR_REGINT:
        s << "PRINT_CHAR_REGINT";
        break;
    case PRINT_CHAR4_REGINT:
        s << "PRINT_CHAR4_REGINT";
        break;
    case PRINT_FLOAT:
        s << "PRINT_FLOAT";
        break;
    case PRINT_FIX:
        s << "PRINT_FIX";
        break;
    case PRINT_INT:
        s << "PRINT_INT";
        break;
    case PRINT_IEEE_FLOAT:
        s << "PRINT_IEEE_FLOAT";
        break;
    case RAND:
        s << "RAND";
        break;
    case START_CLOCK:
        s << "START_CLOCK";
        break;
    case STOP_CLOCK:
        s << "STOP_CLOCK";
        break;
    case RANDC:
        s << "RANDC";
        break;
    case RANDINT:
        s << "RANDINT";
        break;
    case RANDSINT:
        s << "RANDSINT";
        break;
    case RANDFLOAT:
        s << "RANDFLOAT";
        break;
    case RANDSBIT:
        s << "RANDSBIT";
        break;
    case OSRAND:
        s << "OSRAND";
        break;
    case LOADSRAND:
        s << "LOADSRAND";
        break;
    case ODABIT:
        s << "ODABIT";
        break;
    case LOADDABIT:
        s << "LOADDABIT";
        break;
    case LDMSINT:
        s << "LDMSINT";
        break;
    case LDMSINTI:
        s << "LDMSINTI";
        break;
    case STMSINT:
        s << "STMSINT";
        break;
    case STMSINTI:
        s << "STMSINTI";
        break;
    case MOVSINT:
        s << "MOVSINT";
        break;
    case LDSINT:
        s << "LDSINT";
        break;
    case ADDSINT:
        s << "ADDSINT";
        break;
    case ADDSINTC:
        s << "ADDSINTC";
        break;
    case SUBSINT:
        s << "SUBSINT";
        break;
    case SUBSINTC:
        s << "SUBSINTC";
        break;
    case SUBCINTS:
        s << "SUBCINTS";
        break;
    case MULSINT:
        s << "MULSINT";
        break;
    case MULSINTC:
        s << "MULSINTC";
        break;
    case DIVSINT:
        s << "DIVSINT";
        break;
    case SHLSINT:
        s << "SHLSINT";
        break;
    case SHRSINT:
        s << "SHRSINT";
        break;
    case NEG:
        s << "NEG";
        break;
    case SAND:
        s << "SAND";
        break;
    case XORSB:
        s << "XORSB";
        break;
    case ANDSB:
        s << "ANDSB";
        break;
    case ORSB:
        s << "ORSB";
        break;
    case NEGB:
        s << "NEGB";
        break;
    case LDSBIT:
        s << "LDSBIT";
        break;
    case LTZSINT:
        s << "LTZSINT";
        break;
    case EQZSINT:
        s << "EQZSINT";
        break;
    case BITSINT:
        s << "BITSINT";
        break;
    case SINTBIT:
        s << "SINTBIT";
        break;
    case CONVSINTSREG:
        s << "CONVSINTSREG";
        break;
    case CONVREGSREG:
        s << "CONVREGSREG";
        break;
    case CONVSREGSINT:
        s << "CONVSREGSINT";
        break;
    case CONVSUREGSINT:
        s << "CONVSUREGSINT";
        break;
    case CONVSINTSBIT:
        s << "CONVSINTSBIT";
        break;
    case CONVSBITSINT:
        s << "CONVSBITSINT";
        break;
    case OPENSINT:
        s << "OPENSINT";
        break;
    case OPENSBIT:
        s << "OPENSBIT";
        break;
    case ANDSINT:
        s << "ANDSINT";
        break;
    case ANDSINTC:
        s << "ANDSINTC";
        break;
    case ORSINT:
        s << "ORSINT";
        break;
    case ORSINTC:
        s << "ORSINTC";
        break;
    case XORSINT:
        s << "XORSINT";
        break;
    case XORSINTC:
        s << "XORSINTC";
        break;
    case INVSINT:
        s << "INVSINT";
        break;
    case ANDINT:
        s << "ANDINT";
        break;
    case ORINT:
        s << "ORINT";
        break;
    case XORINT:
        s << "XORINT";
        break;
    case INVINT:
        s << "INVINT";
        break;
    case SHLINT:
        s << "SHLINT";
        break;
    case SHRINT:
        s << "SHRINT";
        break;
    case MUL2SINT:
        s << "MUL2SINT";
        break;
    case GC:
        s << "GC";
        break;
    case OGC:
        s << "OGC";
        break;
    case LOADGC:
        s << "LOADGC";
        break;
    case EGC:
        s << "EGC";
        break;
    case LF:
        s << "LF";
        break;
        // new bytecode for summation
    case SUMS:
        s << "SUMS";
        break;
    case SUMC:
        s << "SUMC";
        break;
        // new bytcodes for triple loading
    case LOADCT:
        s << "LOADCT";
        break;
    case CTRIPLE:
        s << "CTRIPLE";
        break;
    case CT_DYN:
        s << "CT_DYN";
        break;
    case SRAND:
        s << "SRAND";
        break;
    default:
        s << instr.opcode;
        throw Invalid_Instruction("Verbose Opcode Not Known");
    }

    if (instr.size != 1)
    {
        s << " " << instr.size;
    }
    s << "  ";

    // Now the arguments
    switch (instr.opcode)
    {
    // instructions with 3 cint register operands */
    case ADDC:
    case SUBC:
    case MULC:
    case DIVC:
    case MODC:
    case ANDC:
    case XORC:
    case ORC:
    case SHLC:
    case SHRC:
        s << "c_" << instr.r[0] << " ";
        s << "c_" << instr.r[1] << " ";
        s << "c_" << instr.r[2] << " ";
        break;
        // instructions with 3 sint register operands */
    case ADDS:
    case SUBS:
        s << "s_" << instr.r[0] << " ";
        s << "s_" << instr.r[1] << " ";
        s << "s_" << instr.r[2] << " ";
        break;
        // instructions with 3 rint register operands */
    case ADDINT:
    case SUBINT:
    case MULINT:
    case DIVINT:
    case MODINT:
    case ANDINT:
    case ORINT:
    case XORINT:
    case SHLINT:
    case SHRINT:
    case LTINT:
    case GTINT:
    case EQINT:
    case MREVC:
    case MREVS:
    case MBITDECINT:
        s << "r_" << instr.r[0] << " ";
        s << "r_" << instr.r[1] << " ";
        s << "r_" << instr.r[2] << " ";
        break;
        // instructions with 3 sregint register operands */
    case ADDSINT:
    case SUBSINT:
    case MULSINT:
    case DIVSINT:
    case ANDSINT:
    case ORSINT:
    case XORSINT:
        s << "sr_" << instr.r[0] << " ";
        s << "sr_" << instr.r[1] << " ";
        s << "sr_" << instr.r[2] << " ";
        break;
        // instructions with 4 regint register operands */
    case MADDC:
    case MADDS:
    case MADDM:
    case MSUBC:
    case MSUBS:
    case MSUBML:
    case MSUBMR:
    case MMULC:
    case MMULM:
    case MDIVC:
    case MMODC:
        s << "r_" << instr.r[0] << " ";
        s << "r_" << instr.r[1] << " ";
        s << "r_" << instr.r[2] << " ";
        s << "r_" << instr.n << " ";
        break;
        // instructions with 3 sbit register operands */
    case XORSB:
    case ANDSB:
    case ORSB:
        s << "sb_" << instr.r[0] << " ";
        s << "sb_" << instr.r[1] << " ";
        s << "sb_" << instr.r[2] << " ";
        break;
        // instructions with 2 sint + 1 cint register operands */
    case ADDM:
    case SUBML:
    case MULM:
        s << "s_" << instr.r[0] << " ";
        s << "s_" << instr.r[1] << " ";
        s << "c_" << instr.r[2] << " ";
        break;
        // instructions with 2 sregint and 1 rint operand
    case ADDSINTC:
    case SUBSINTC:
    case MULSINTC:
    case ANDSINTC:
    case ORSINTC:
    case XORSINTC:
        s << "sr_" << instr.r[0] << " ";
        s << "sr_" << instr.r[1] << " ";
        s << "r_" << instr.r[2] << " ";
        break;
        // instructions with 2 sregint and 1 sbit operand
    case SAND:
        s << "sr_" << instr.r[0] << " ";
        s << "sr_" << instr.r[1] << " ";
        s << "sb_" << instr.r[2] << " ";
        break;
        // instructions with 1 sbit and 1 sregint operands
    case LTZSINT:
    case EQZSINT:
    case PEEKSBIT:
    case RPEEKSBIT:
        s << "sb_" << instr.r[0] << " ";
        s << "sr_" << instr.r[1] << " ";
        break;
        // instructions with 1 sbit and 1 sint operands
    case CONVSINTSBIT:
        s << "sb_" << instr.r[0] << " ";
        s << "s_" << instr.r[1] << " ";
        break;
        // instructions with 1 sbit and 1 integer operands
    case LDSBIT:
        s << "sb_" << instr.r[0] << " ";
        s << instr.n << " ";
        break;
        // instructions with 1 sint and 1 sbit operands
    case CONVSBITSINT:
        s << "s_" << instr.r[0] << " ";
        s << "sb_" << instr.r[1] << " ";
        break;
        // instructions with 1 sbit and 1 sregint and 1 integer operands
    case BITSINT:
        s << "sb_" << instr.r[0] << " ";
        s << "sr_" << instr.r[1] << " ";
        s << instr.n << " ";
        break;
        // instructions with 1 sregint and 1 sbit and 1 integer operands
    case SINTBIT:
        s << "sr_" << instr.r[0] << " ";
        s << "sr_" << instr.r[1] << " ";
        s << "sb_" << instr.r[2] << " ";
        s << instr.n << " ";
        break;
        // instructions with 1 sint + 1 cint + 1 sint register operands */
    case SUBMR:
        s << "s_" << instr.r[0] << " ";
        s << "c_" << instr.r[1] << " ";
        s << "s_" << instr.r[2] << " ";
        break;
        // instructions with 1 sregint + 1 rint + 1 sregint operand
    case SUBCINTS:
        s << "sr_" << instr.r[0] << " ";
        s << "r_" << instr.r[1] << " ";
        s << "sr_" << instr.r[2] << " ";
        break;
        // instructions with 1 cint + 1 rint register operand
    case LDMCI:
    case CONVINT:
    case STMCI:
    case PEEKC:
    case RPEEKC:
        s << "c_" << instr.r[0] << " ";
        s << "r_" << instr.r[1] << " ";
        break;
        // instructions with 1 sint + 1 rint register operand
    case LDMSI:
    case STMSI:
    case PEEKS:
    case RPEEKS:
        s << "s_" << instr.r[0] << " ";
        s << "r_" << instr.r[1] << " ";
        break;
        // instructions with 1 sregint + 1 rint register operand
    case LDMSINTI:
    case CONVREGSREG:
    case PEEKSINT:
    case RPEEKSINT:
        s << "sr_" << instr.r[0] << " ";
        s << "r_" << instr.r[1] << " ";
        break;
        // instructions with 1 sregint + 1 sint register operand
    case CONVSINTSREG:
        s << "sr_" << instr.r[0] << " ";
        s << "s_" << instr.r[1] << " ";
        break;
        // instructions with 1 sint + 1 sregint register operand
    case CONVSREGSINT:
    case CONVSUREGSINT:
        s << "s_" << instr.r[0] << " ";
        s << "sr_" << instr.r[1] << " ";
        break;
        // instructions with 1 sint + 1 sbit register operand
    case DABIT:
        s << "s_" << instr.r[0] << " ";
        s << "sb_" << instr.r[1] << " ";
        break;
        // instructions with 1 rint + 1 sregint register operand
    case OPENSINT:
    case POKESINT:
    case RPOKESINT:
        s << "r_" << instr.r[0] << " ";
        s << "sr_" << instr.r[1] << " ";
        break;
        // instructions with 1 rint + 1 cint register operand
    case POKEC:
    case RPOKEC:
        s << "r_" << instr.r[0] << " ";
        s << "c_" << instr.r[1] << " ";
        break;
        // instructions with 1 rint + 1 sint register operand
    case POKES:
    case RPOKES:
        s << "r_" << instr.r[0] << " ";
        s << "s_" << instr.r[1] << " ";
        break;
        // instructions with 1 rint + 1 sbit register operand
    case OPENSBIT:
    case POKESBIT:
    case RPOKESBIT:
        s << "r_" << instr.r[0] << " ";
        s << "sb_" << instr.r[1] << " ";
        break;
        // instructions with 2 cint register operands
    case MOVC:
    case LEGENDREC:
    case DIGESTC:
        s << "c_" << instr.r[0] << " ";
        s << "c_" << instr.r[1] << " ";
        break;
        // instructions with 2 sint register operands
    case MOVS:
        s << "s_" << instr.r[0] << " ";
        s << "s_" << instr.r[1] << " ";
        break;
        // instructions with 2 rint register operands
    case MOVINT:
    case LDMINTI:
    case RAND:
    case STMINTI:
    case STMSINTI:
    case LTZINT:
    case EQZINT:
    case PEEKINT:
    case RPEEKINT:
    case POKEINT:
    case RPOKEINT:
    case INVINT:
    case NEWC:
    case NEWS:
    case NEWINT:
    case NEWSINT:
        s << "r_" << instr.r[0] << " ";
        s << "r_" << instr.r[1] << " ";
        break;
        // instructions with 2 srint operands
    case NEG:
    case MOVSINT:
    case INVSINT:
        s << "sr_" << instr.r[0] << " ";
        s << "sr_" << instr.r[1] << " ";
        break;
        // instructions with 2 sbit operands
    case NEGB:
    case MOVSB:
        s << "sb_" << instr.r[0] << " ";
        s << "sb_" << instr.r[1] << " ";
        break;
        // instructions with 1 cint register operands
    case PRINT_REG:
    case POPC:
    case PUSHC:
    case RANDC:
        s << "c_" << instr.r[0] << " ";
        break;
        // instructions with 1 sint register operands
    case POPS:
    case PUSHS:
        s << "s_" << instr.r[0] << " ";
        break;
        // instructions with 1 sbit register operands
    case POPSBIT:
    case PUSHSBIT:
    case RANDSBIT:
        s << "sb_" << instr.r[0] << " ";
        break;
    // instructions for Beaver triples
    case OTRIPLE:
        s << instr.n << " ";
        break;
    case LOADTRIPLE:
        s << instr.n << " ";
        break;
    // instructions for bounded randomness
    case OSRAND:
        s << instr.n << " ";
        s << instr.m << " ";
        break;
    case LOADSRAND:
        s << instr.n << " ";
        s << instr.m << " ";
        break;
    // instructions for persistent dabits
    case ODABIT:
        s << instr.n << " ";
        break;
    case LOADDABIT:
        s << instr.n << " ";
        break;
    // instructions with 1 srint register operands
    case POPSINT:
    case PUSHSINT:
    case RANDSINT:
        s << "sr_" << instr.r[0] << " ";
        break;
        // instructions with 1 rint register operands
    case DELETEC:
    case DELETES:
    case DELETEINT:
    case DELETESINT:
    case PRINT_INT:
    case PRINT_IEEE_FLOAT:
    case PRINT_CHAR_REGINT:
    case PRINT_CHAR4_REGINT:
    case PUSHINT:
    case POPINT:
    case STARG:
    case LDTN:
    case LDARG:
    case GETSPINT:
    case GETSPSINT:
    case GETSPS:
    case GETSPC:
    case GETSPSBIT:
    case CALLR:
    case JMPR:
    case RANDINT:
    case RANDFLOAT:
        s << "r_" << instr.r[0] << " ";
        break;
        // instructions with 2 cint + 1 integer operand
    case ADDCI:
    case SUBCI:
    case SUBCFI:
    case MULCI:
    case DIVCI:
    case MODCI:
    case ANDCI:
    case XORCI:
    case ORCI:
    case SHLCI:
    case NOTC:
        s << "c_" << instr.r[0] << " ";
        s << "c_" << instr.r[1] << " ";
        s << instr.n << " ";
        break;
        // instructions with 2 sint + 1 integer operand
    case ADDSI:
    case SUBSI:
    case SUBSFI:
    case MULSI:
    case SHRCI:
        s << "s_" << instr.r[0] << " ";
        s << "s_" << instr.r[1] << " ";
        s << instr.n << " ";
        break;
        // instructions with 2 srint + 1 integer operand
    case SHLSINT:
    case SHRSINT:
        s << "sr_" << instr.r[0] << " ";
        s << "sr_" << instr.r[1] << " ";
        s << instr.n << " ";
        break;
        // instructions with 1 rint + 1 cint + 1 integer operand
    case CONVMODP:
        s << "r_" << instr.r[0] << " ";
        s << "c_" << instr.r[1] << " ";
        s << instr.n << " ";
        break;
        // instructions with 1 rint + 1 integer operand
    case LDINT:
    case LDMINT:
    case STMINT:
    case INPUT_INT:
    case OPEN_CHANNEL:
    case OUTPUT_INT:
        s << "r_" << instr.r[0] << " ";
        s << instr.n << " ";
        break;
        // instructions with 1 sint + 1 integer operand
    case LDSI:
    case LDMS:
    case STMS:
        s << "s_" << instr.r[0] << " ";
        s << instr.n << " ";
        break;
        // instructions with 1 cint + 1 integer operand
    case LDI:
    case LDMC:
    case STMC:
    case INPUT_CLEAR:
    case OUTPUT_CLEAR:
        s << "c_" << instr.r[0] << " ";
        s << instr.n << " ";
        break;
        // instructions with 1 srint + 1 integer operand
    case LDSINT:
    case LDMSINT:
    case STMSINT:
        s << "sr_" << instr.r[0] << " ";
        s << instr.n << " ";
        break;
        // instructions with 1 sint + 1 player + 1 integer operand
    case PRIVATE_INPUT:
    case PRIVATE_OUTPUT:
        s << "s_" << instr.r[0] << " ";
        s << instr.p << " ";
        s << instr.m << " ";
        break;
        // instructions with 1 sint + 1 player + 2 integer operand
    case MPRIVATE_INPUT:
    case MPRIVATE_OUTPUT:
        s << "r_" << instr.r[0] << " ";
        s << "r_" << instr.r[1] << " ";
        s << instr.p << " ";
        s << instr.m << " ";
        break;
        // instructions with 1 sint + 2 integer operands
    case PRINT_FIX:
        s << "c_" << instr.r[0] << " ";
        s << instr.n << " ";
        s << instr.m << " ";
        break;
        // instructions with 1 rint + 2 integer operands
    case JMPEQ:
    case JMPNE:
        s << "r_" << instr.r[0] << " ";
        s << instr.n << " ";
        s << instr.m << " ";
        break;
        // instructions with 1 integer operand
    case PRINT_CHAR:
    case PRINT_CHAR4:
    case REQBL:
    case JMP:
    case START_CLOCK:
    case STOP_CLOCK:
    case CLOSE_CHANNEL:
    case PRINT_MEM:
    case JOIN_TAPE:
    case CALL:
    case GC:
    case EGC:
    case LF:
        s << instr.n << " ";
        break;
        // instructions with 2 integer operand
    case OGC:
        s << instr.n << " ";
        s << instr.m << " ";
        break;
    case LOADGC:
        s << instr.n << " ";
        s << instr.m << " ";
        break;
    // instructions with no operand
    case RESTART:
    case CRASH:
    case CLEAR_MEMORY:
    case CLEAR_REGISTERS:
    case RETURN:
        break;
        // Four integer operands
    case RUN_TAPE:
        s << instr.r[0] << " ";
        s << instr.r[1] << " ";
        s << instr.r[2] << " ";
        s << instr.n << " ";
        break;
        // Weird ones
    case MEVALCC:
        s << "c_" << instr.r[0] << " ";
        s << "r_" << instr.r[1] << " ";
        s << "r_" << instr.r[2] << " ";
        s << "c_" << instr.n << " ";
        break;
    case MEVALSC:
        s << "s_" << instr.r[0] << " ";
        s << "r_" << instr.r[1] << " ";
        s << "r_" << instr.r[2] << " ";
        s << "c_" << instr.n << " ";
        break;
    case MBITDECC:
        s << "r_" << instr.r[0] << " ";
        s << "c_" << instr.r[1] << " ";
        s << "r_" << instr.r[2] << " ";
        break;
        // Various variable length instructions
    case MUL2SINT:
        for (unsigned int i = 0; i < instr.start.size(); i++)
        {
            s << "sr_" << instr.start[i] << " ";
        }
        break;
    case PRINT_FLOAT:
    case STOPOPEN:
        for (unsigned int i = 0; i < instr.start.size(); i++)
        {
            s << "c_" << instr.start[i] << " ";
        }
        break;
    case STARTOPEN:
    case TRIPLE:
    case SQUARE:
    case BIT:
        for (unsigned int i = 0; i < instr.start.size(); i++)
        {
            s << "s_" << instr.start[i] << " ";
        }
        break;
    case OUTPUT_SHARES:
    case INPUT_SHARES:
        s << instr.p << " ";
        for (unsigned int i = 0; i < instr.start.size(); i++)
        {
            s << "s_" << instr.start[i] << " ";
        }
        break;
    case SUMS: // 2 sint + 1 integer operand
        s << "s_" << instr.r[0] << " ";
        s << "s_" << instr.r[1] << " ";
        s << instr.n << " ";
        break;
    case SUMC: // 2 cint + 1 integer operand
        s << "c_" << instr.r[0] << " ";
        s << "c_" << instr.r[1] << " ";
        s << instr.n << " ";
        break;
    case LOADCT:
        s << instr.n << " ";
        s << instr.m << " ";
        break;
    case CTRIPLE:
        s << instr.n << " ";
        for (unsigned int i = 0; i < instr.start.size(); i++)
        {
            s << "s_" << instr.start[i] << " ";
        }
        break;
    case CT_DYN:
        s << instr.n << " ";
        s << instr.m << " ";
        for (unsigned int i = 0; i < instr.start.size(); i++)
        {
            s << "s_" << instr.start[i] << " ";
        }
        break;
    case SRAND:
        s << instr.n << " ";
        s << instr.m << " ";
        for (unsigned int i = 0; i < instr.start.size(); i++)
        {
            s << "s_" << instr.start[i] << " ";
        }
        break;
    default:
        throw Invalid_Instruction("Cannot parse operands in verbose mode");
    }

    return s;
}

template <class SRegint, class SBit>
void Instruction<SRegint, SBit>::execute_using_sacrifice_data([[maybe_unused]] Player &P,
                                                              Processor<SRegint, SBit> &Proc,
                                                              offline_control_data &OCD) const
{
    int thread = Proc.get_thread_num();
    unsigned int num = size * start.size();
    if (opcode == TRIPLE)
    {
        if ((num % 3) != 0)
        {
            throw bad_value();
        }
        num /= 3;
    }
    else if (opcode == SQUARE)
    {
        if ((num & 1) != 0)
        {
            throw bad_value();
        }
        num /= 2;
    }

#ifdef BENCH_OFFLINE
    switch (opcode)
    {
    case TRIPLE:
        P.triples += num;
        break;
    case SQUARE:
        P.squares += num;
        break;
    case BIT:
        P.bits += num;
        break;
    }
#endif

    // Check to see if we have to wait
    if (opcode != SRAND && opcode != OSRAND)
    {
        Wait_For_Preproc(opcode, num, thread, OCD);
    }
    list<Share> la, lb, lc;

    unsigned int reg_num, count;
    switch (opcode)
    {
    // ============== Persisted Beaver Triples Instructions
    case OTRIPLE: {
        int left_over = n;
        int current_batch_size = beaver_triples_batch_size;
        while (left_over > 0)
        {
            num = min(current_batch_size, left_over);
            Wait_For_Preproc(opcode, num, thread, OCD);
            OCD.mul_mutex[thread].lock();
            Split_Lists(la, SacrificeD[thread].TD.ta, num);
            Split_Lists(lb, SacrificeD[thread].TD.tb, num);
            Split_Lists(lc, SacrificeD[thread].TD.tc, num);
            OCD.mul_mutex[thread].unlock();
            Proc.store_beaver_triples(la, lb, lc, num, P);
            left_over -= num;
        }
    }
    break;
    case TRIPLE: {
        vector<Share> values = Proc.obtain_beaver_triples(num, P);
        unsigned int differential = (num * 3) - values.size();
        if (differential > 0)
        {
            unsigned int expected_count = differential / 3;
            Wait_For_Preproc(opcode, expected_count, thread, OCD);
            OCD.mul_mutex[thread].lock();
            Split_Lists(la, SacrificeD[thread].TD.ta, expected_count);
            Split_Lists(lb, SacrificeD[thread].TD.tb, expected_count);
            Split_Lists(lc, SacrificeD[thread].TD.tc, expected_count);
            OCD.mul_mutex[thread].unlock();
            Randomness_Utils::combine_triple_shares(values, la, lb, lc);
        }
        reg_num = 0;
        count = 0;
        for (unsigned int j = 0; j < values.size(); j += 3)
        {
            Proc.get_Sp_ref(start[reg_num] + count) = values.at(j);
            Proc.get_Sp_ref(start[reg_num + 1] + count) = values.at(j + 1);
            Proc.get_Sp_ref(start[reg_num + 2] + count) = values.at(j + 2);
            count++;
            if (count == size)
            {
                count = 0;
                reg_num += 3;
            }
        }
    }
    break;
    case SQUARE: {
        OCD.sqr_mutex[thread].lock();
        Split_Lists(la, SacrificeD[thread].SD.sa, num);
        Split_Lists(lb, SacrificeD[thread].SD.sb, num);
        OCD.sqr_mutex[thread].unlock();
        reg_num = 0;
        count = 0;
        for (list<Share>::const_iterator zz = la.begin(); zz != la.end(); ++zz)
        {
            Proc.get_Sp_ref(start[reg_num] + count) = *zz;
            count++;
            if (count == size)
            {
                count = 0;
                reg_num += 2;
            }
        }
        reg_num = 1;
        count = 0;
        for (list<Share>::const_iterator zz = lb.begin(); zz != lb.end(); ++zz)
        {
            Proc.get_Sp_ref(start[reg_num] + count) = *zz;
            count++;
            if (count == size)
            {
                count = 0;
                reg_num += 2;
            }
        }
    }
    break;
    case BIT: {
        OCD.bit_mutex[thread].lock();
        Split_Lists(lb, SacrificeD[thread].BD.bb, num);
        OCD.bit_mutex[thread].unlock();
        reg_num = 0;
        count = 0;
        for (list<Share>::const_iterator zz = lb.begin(); zz != lb.end(); ++zz)
        {
            Proc.get_Sp_ref(start[reg_num] + count) = *zz;
            count++;
            if (count == size)
            {
                count = 0;
                reg_num++;
            }
        }
    }
    break;
    // ============== Persisted Randomness Instructions
    case OSRAND: {
        int left_over = m;
        int current_batch_size = bounded_randomness_batch_size;
        while (left_over > 0)
        {
            int batch_l =
                ((int)(current_batch_size > left_over)) * (left_over - current_batch_size) + current_batch_size;
            num = batch_l * n;
            Wait_For_Preproc(opcode, num, thread, OCD);
            OCD.bit_mutex[thread].lock();
            Split_Lists(lb, SacrificeD[thread].BD.bb, num);
            OCD.bit_mutex[thread].unlock();
            Proc.store_bounded_randomness(lb, n, batch_l, P);
            left_over -= batch_l;
        }
    }
    break;
    case SRAND: {
        vector<Share> values = Proc.obtain_bounded_randomness(m, num, P);
        unsigned int differential = num - values.size();
        if (differential > 0)
        {

            unsigned int expected_bits = differential * m;
            Wait_For_Preproc(opcode, expected_bits, thread, OCD);
            OCD.bit_mutex[thread].lock();
            Split_Lists(lb, SacrificeD[thread].BD.bb, expected_bits);
            OCD.bit_mutex[thread].unlock();
            Randomness_Utils::combine_shares(values, lb, m, differential);
        }
        reg_num = 0;
        count = 0;
        for (unsigned int j = 0; j < values.size(); j++)
        {
            Share &sp_ref = Proc.get_Sp_ref(start[reg_num] + count);
            sp_ref = values.at(j);
            count++;
            if (count == size)
            {
                count = 0;
                reg_num++;
            }
        }
    }
    break;
    default: {
        throw bad_value();
    }
    break;
    }
}

void Mult_Bit(aBit &z, const aBit &x, const aBit &y, Player &P, int online_thread_num)
{
    OTD.check();
    aTriple T = OTD.aAD.get_aAND(online_thread_num);
#ifdef BENCH_OFFLINE
    P.aands += 1;
#endif

    Mult_Bit(z, x, y, T, P);
}

void Mult_Bit(Share2 &z, const Share2 &x, const Share2 &y, Player &P, int online_thread_num)
{
    MTD.check();
    vector<Share2> T(3);
    MTD.get_Single_Triple(T, online_thread_num);
    Mult_Bit(z, x, y, T, P);

#ifdef BENCH_OFFLINE
    P.mod2s += 1;
#endif
}

template <class SRegint, class SBit>
bool Instruction<SRegint, SBit>::execute(Processor<SRegint, SBit> &Proc, Player &P, Machine<SRegint, SBit> &machine,
                                         offline_control_data &OCD) const
{
    Timer instr_timer;
    stringstream ss;
    if (machine.verbose > 0)
    {
        ss << *this;
        printf("Thread %d : %s", Proc.get_thread_num(), ss.str().c_str());
        if (machine.verbose > 1)
        {
            instr_timer.reset();
        }
        else
        {
            printf("\n");
        }
        fflush(stdout);
    }
    bool restart = false;

    Proc.increment_PC();

    int r[3] = {this->r[0], this->r[1], this->r[2]};
    int n = this->n;

    /* First deal with instructions we want to deal with outside the main
     * loop for vectorized instructions
     */

    // Deal the offline data input routines as these need thread locking
    if (opcode == SQUARE || opcode == BIT || opcode == OTRIPLE || opcode == TRIPLE || opcode == DABIT ||
        opcode == ODABIT || opcode == PRIVATE_OUTPUT || opcode == PRIVATE_INPUT || opcode == STARTOPEN ||
        opcode == STOPOPEN || opcode == MPRIVATE_OUTPUT || opcode == MPRIVATE_INPUT || opcode == OSRAND ||
        opcode == SRAND)
    {
        if (opcode == SQUARE || opcode == BIT || opcode == OTRIPLE || opcode == TRIPLE || opcode == OSRAND ||
            opcode == SRAND)
        {
            // Deal with offline data
            execute_using_sacrifice_data(P, Proc, OCD);
        }
        else if (opcode == DABIT)
        {
            // Extract daBit
            if (size > Proc.dabits.size() && !Proc.is_dabits_mem_announced)
            {
                cout << "Not Enough Dabits Elements -- Switching to Offline Factory "
                        "Generation"
                     << endl;
                Proc.is_dabits_mem_announced = true;
            }
            for (unsigned int i = 0; i < size; i++)
            {
                Proc.write_daBit(r[0] + i, r[1] + i, P);
            }

#ifdef BENCH_OFFLINE
            P.dabits += size;
#endif
        }
        else if (opcode == ODABIT)
        {
            std::cout << "Executes ODABIT" << std::endl;
            Proc.store_daBits(n, P);

            std::cout << "Added ODABITs to DB" << std::endl;
        }
        else if (opcode == PRIVATE_OUTPUT || opcode == PRIVATE_INPUT || opcode == MPRIVATE_OUTPUT ||
                 opcode == MPRIVATE_INPUT)
        {
            // Private input/output
            if (Proc.get_thread_num() != 0)
            {
                throw IO_thread();
            }
            if (opcode == PRIVATE_OUTPUT)
            {
                Proc.iop.private_output(p, r[0], m, Proc, P, machine, OCD, size);
            }
            else if (opcode == PRIVATE_INPUT)
            {
                Proc.iop.private_input(p, r[0], m, Proc, P, machine, OCD, size);
            }
            else if (opcode == MPRIVATE_OUTPUT)
            {
                Proc.iop.mprivate_output(p, r[0], m, Proc.read_Ri(r[1]), Proc, P, machine, OCD, size);
            }
            else
            {
                Proc.iop.mprivate_input(p, r[0], m, Proc.read_Ri(r[1]), Proc, P, machine, OCD, size);
            }
        }
        else if (opcode == STARTOPEN || opcode == STOPOPEN)
        {
            if (opcode == STARTOPEN && size < open_values_workaround_batch_size)
                Proc.POpen_Start(start, size, P);
            else if (opcode == STOPOPEN && size < open_values_workaround_batch_size)
                Proc.POpen_Stop(start, size, P);
            // bug workaround to solve problem opening huge vectors (CEP-786)
            else if (opcode == STARTOPEN)
                Proc.POpen_Start_Batched(start, size, P);
            else if (opcode == STOPOPEN)
                Proc.POpen_Stop_Batched(start, size, P);
        }
    }
    else
    {
        // Need to copy as we might need to alter this in the loop
        //   But it should not be that big in any case here
        vector<int> c_start = start;
        SBit aB;
        word t;

        // Loop here to cope with vectorization, if an instruction is not vectorizable
        // then size=1 in any case :-)
        for (unsigned int i = 0; i < size; i++)
        {
            switch (opcode)
            {
            case LDI:
                Proc.temp.ansp.assign(n);
                Proc.write_Cp(r[0], Proc.temp.ansp);
                break;
            case LDSI: {
                Proc.temp.ansp.assign(n);
                Proc.get_Sp_ref(r[0]).assign(Proc.temp.ansp, P.get_mac_keys());
            }
            break;
            case LDMC:
                Proc.write_Cp(r[0], machine.Mc.read(n, machine.verbose));
                n++;
                break;
            case LDMS:
                Proc.write_Sp(r[0], machine.Ms.read(n, machine.verbose));
                n++;
                break;
            case LDMINT:
                Proc.write_Ri(r[0], machine.Mr.read(n, machine.verbose).get());
                n++;
                break;
            case LDMCI:
                Proc.write_Cp(r[0], machine.Mc.read(Proc.read_Ri(r[1]), machine.verbose));
                break;
            case LDMSI:
                Proc.write_Sp(r[0], machine.Ms.read(Proc.read_Ri(r[1]), machine.verbose));
                break;
            case LDMINTI:
                Proc.write_Ri(r[0], machine.Mr.read(Proc.read_Ri(r[1]), machine.verbose).get());
                break;
            case STMC:
                machine.Mc.write(n, Proc.read_Cp(r[0]), machine.verbose, Proc.get_PC());
                n++;
                break;
            case STMS:
                machine.Ms.write(n, Proc.read_Sp(r[0]), machine.verbose, Proc.get_PC());
                n++;
                break;
            case STMINT:
                machine.Mr.write(n, Integer(Proc.read_Ri(r[0])), machine.verbose, Proc.get_PC());
                n++;
                break;
            case STMCI:
                machine.Mc.write(Proc.read_Ri(r[1]), Proc.read_Cp(r[0]), machine.verbose, Proc.get_PC());
                break;
            case STMSI:
                machine.Ms.write(Proc.read_Ri(r[1]), Proc.read_Sp(r[0]), machine.verbose, Proc.get_PC());
                break;
            case STMINTI:
                machine.Mr.write(Proc.read_Ri(r[1]), Integer(Proc.read_Ri(r[0])), machine.verbose, Proc.get_PC());
                break;
            case MOVC:
                Proc.write_Cp(r[0], Proc.read_Cp(r[1]));
                break;
            case MOVS:
                Proc.write_Sp(r[0], Proc.read_Sp(r[1]));
                break;
            case MOVINT:
                Proc.write_Ri(r[0], Proc.read_Ri(r[1]));
                break;
            case MOVSB:
                Proc.write_sbit(r[0], Proc.read_sbit(r[1]));
                break;
            case PUSHINT:
                Proc.push_int(Proc.read_Ri(r[0]));
                break;
            case PUSHSINT:
                Proc.push_srint(Proc.read_srint(r[0]));
                break;
            case PUSHC:
                Proc.push_Cp(Proc.read_Cp(r[0]));
                break;
            case PUSHS:
                Proc.push_Sp(Proc.read_Sp(r[0]));
                break;
            case PUSHSBIT:
                Proc.push_sbit(Proc.read_sbit(r[0]));
                break;
            case POPINT:
                Proc.pop_int(Proc.get_Ri_ref(r[0]));
                break;
            case POPSINT:
                Proc.pop_srint(Proc.get_srint_ref(r[0]));
                break;
            case POPC:
                Proc.pop_Cp(Proc.get_Cp_ref(r[0]));
                break;
            case POPS:
                Proc.pop_Sp(Proc.get_Sp_ref(r[0]));
                break;
            case POPSBIT:
                Proc.pop_sbit(Proc.get_sbit_ref(r[0]));
                break;
            case PEEKINT:
                Proc.peek_int(Proc.get_Ri_ref(r[0]), Proc.read_Ri(r[1]));
                break;
            case PEEKSINT:
                Proc.peek_srint(Proc.get_srint_ref(r[0]), Proc.read_Ri(r[1]));
                break;
            case PEEKC:
                Proc.peek_Cp(Proc.get_Cp_ref(r[0]), Proc.read_Ri(r[1]));
                break;
            case PEEKS:
                Proc.peek_Sp(Proc.get_Sp_ref(r[0]), Proc.read_Ri(r[1]));
                break;
            case PEEKSBIT:
                Proc.peek_sbit(Proc.get_sbit_ref(r[0]), Proc.read_Ri(r[1]));
                break;
            case RPEEKINT:
                Proc.rpeek_int(Proc.get_Ri_ref(r[0]), Proc.read_Ri(r[1]));
                break;
            case RPEEKSINT:
                Proc.rpeek_srint(Proc.get_srint_ref(r[0]), Proc.read_Ri(r[1]));
                break;
            case RPEEKC:
                Proc.rpeek_Cp(Proc.get_Cp_ref(r[0]), Proc.read_Ri(r[1]));
                break;
            case RPEEKS:
                Proc.rpeek_Sp(Proc.get_Sp_ref(r[0]), Proc.read_Ri(r[1]));
                break;
            case RPEEKSBIT:
                Proc.rpeek_sbit(Proc.get_sbit_ref(r[0]), Proc.read_Ri(r[1]));
                break;
            case POKEINT:
                Proc.poke_int(Proc.read_Ri(r[0]), Proc.read_Ri(r[1]));
                break;
            case POKESINT:
                Proc.poke_srint(Proc.read_Ri(r[0]), Proc.read_srint(r[1]));
                break;
            case POKEC:
                Proc.poke_Cp(Proc.read_Ri(r[0]), Proc.read_Cp(r[1]));
                break;
            case POKES:
                Proc.poke_Sp(Proc.read_Ri(r[0]), Proc.read_Sp(r[1]));
                break;
            case POKESBIT:
                Proc.poke_sbit(Proc.read_Ri(r[0]), Proc.read_sbit(r[1]));
                break;
            case RPOKEINT:
                Proc.rpoke_int(Proc.read_Ri(r[0]), Proc.read_Ri(r[1]));
                break;
            case RPOKESINT:
                Proc.rpoke_srint(Proc.read_Ri(r[0]), Proc.read_srint(r[1]));
                break;
            case RPOKEC:
                Proc.rpoke_Cp(Proc.read_Ri(r[0]), Proc.read_Cp(r[1]));
                break;
            case RPOKES:
                Proc.rpoke_Sp(Proc.read_Ri(r[0]), Proc.read_Sp(r[1]));
                break;
            case RPOKESBIT:
                Proc.rpoke_sbit(Proc.read_Ri(r[0]), Proc.read_sbit(r[1]));
                break;
            case GETSPINT:
                Proc.getsp_int(Proc.get_Ri_ref(r[0]));
                break;
            case GETSPSINT:
                Proc.getsp_srint(Proc.get_Ri_ref(r[0]));
                break;
            case GETSPC:
                Proc.getsp_Cp(Proc.get_Ri_ref(r[0]));
                break;
            case GETSPS:
                Proc.getsp_Sp(Proc.get_Ri_ref(r[0]));
                break;
            case GETSPSBIT:
                Proc.getsp_sbit(Proc.get_Ri_ref(r[0]));
                break;
            case LDTN:
                Proc.write_Ri(r[0], Proc.get_thread_num());
                break;
            case LDARG:
                Proc.write_Ri(r[0], Proc.get_arg());
                break;
            case STARG:
                Proc.set_arg(Proc.read_Ri(r[0]));
                break;
            case ADDC:
#ifdef DEBUG
                Proc.temp.ansp.add(Proc.read_Cp(r[1]), Proc.read_Cp(r[2]));
                Proc.write_Cp(r[0], Proc.temp.ansp);
#else
                Proc.get_Cp_ref(r[0]).add(Proc.read_Cp(r[1]), Proc.read_Cp(r[2]));
#endif
                break;
            case ADDS:
#ifdef DEBUG
                Proc.temp.Sansp.add(Proc.read_Sp(r[1]), Proc.read_Sp(r[2]));
                Proc.write_Sp(r[0], Proc.temp.Sansp);
#else
                Proc.get_Sp_ref(r[0]).add(Proc.read_Sp(r[1]), Proc.read_Sp(r[2]));
#endif
                break;
            /// -------------------------------------- NEW BYTECODES -------------------------------------- ///
            case SUMS: {
                int first_item = r[1]++;
                int second_item = r[1]++;
                Proc.get_Sp_ref(r[0]).add(Proc.read_Sp(first_item), Proc.read_Sp(second_item));
                for (int u = 2; u < n; u++)
                {
                    Proc.get_Sp_ref(r[0]).add(Proc.read_Sp(r[0]), Proc.read_Sp(r[1]++));
                }
            }
            break;
            case SUMC: {
                int first_item = r[1]++;
                int second_item = r[1]++;
                Proc.get_Cp_ref(r[0]).add(Proc.read_Cp(first_item), Proc.read_Cp(second_item));
                for (int u = 2; u < n; u++)
                {
                    Proc.get_Cp_ref(r[0]).add(Proc.read_Cp(r[0]), Proc.read_Cp(r[1]++));
                }
            }
            break;
                /// -------------------------------- END OF NEW BYTECODES ------------------------------------ ///

            case ADDM:
#ifdef DEBUG
                Proc.temp.Sansp.add(Proc.read_Sp(r[1]), Proc.read_Cp(r[2]), P.get_mac_keys());
                Proc.write_Sp(r[0], Proc.temp.Sansp);
#else
                Proc.get_Sp_ref(r[0]).add(Proc.read_Sp(r[1]), Proc.read_Cp(r[2]), P.get_mac_keys());
#endif
                break;
            case SUBC:
#ifdef DEBUG
                Proc.temp.ansp.sub(Proc.read_Cp(r[1]), Proc.read_Cp(r[2]));
                Proc.write_Cp(r[0], Proc.temp.ansp);
#else
                Proc.get_Cp_ref(r[0]).sub(Proc.read_Cp(r[1]), Proc.read_Cp(r[2]));
#endif
                break;
            case SUBS:
#ifdef DEBUG
                Proc.temp.Sansp.sub(Proc.read_Sp(r[1]), Proc.read_Sp(r[2]));
                Proc.write_Sp(r[0], Proc.temp.Sansp);
#else
                Proc.get_Sp_ref(r[0]).sub(Proc.read_Sp(r[1]), Proc.read_Sp(r[2]));
#endif
                break;
            case SUBML:
#ifdef DEBUG
                Proc.temp.Sansp.sub(Proc.read_Sp(r[1]), Proc.read_Cp(r[2]), P.get_mac_keys());
                Proc.write_Sp(r[0], Proc.temp.Sansp);
#else
                Proc.get_Sp_ref(r[0]).sub(Proc.read_Sp(r[1]), Proc.read_Cp(r[2]), P.get_mac_keys());
#endif
                break;
            case SUBMR:
#ifdef DEBUG
                Proc.temp.Sansp.sub(Proc.read_Cp(r[1]), Proc.read_Sp(r[2]), P.get_mac_keys());
                Proc.write_Sp(r[0], Proc.temp.Sansp);
#else
                Proc.get_Sp_ref(r[0]).sub(Proc.read_Cp(r[1]), Proc.read_Sp(r[2]), P.get_mac_keys());
#endif
                break;
            case MULC:
#ifdef DEBUG
                Proc.temp.ansp.mul(Proc.read_Cp(r[1]), Proc.read_Cp(r[2]));
                Proc.write_Cp(r[0], Proc.temp.ansp);
#else
                Proc.get_Cp_ref(r[0]).mul(Proc.read_Cp(r[1]), Proc.read_Cp(r[2]));
#endif
                break;
            case MULM:
#ifdef DEBUG
                Proc.temp.Sansp.mul(Proc.read_Sp(r[1]), Proc.read_Cp(r[2]));
                Proc.write_Sp(r[0], Proc.temp.Sansp);
#else
                Proc.get_Sp_ref(r[0]).mul(Proc.read_Sp(r[1]), Proc.read_Cp(r[2]));
#endif
                break;
            case DIVC:
                if (Proc.read_Cp(r[2]).is_zero())
                    throw Processor_Error("Division by zero from register");
                Proc.temp.ansp.invert(Proc.read_Cp(r[2]));
                Proc.temp.ansp.mul(Proc.read_Cp(r[1]));
                Proc.write_Cp(r[0], Proc.temp.ansp);
                break;
            case MODC:
                to_bigint(Proc.temp.aa, Proc.read_Cp(r[1]));
                to_bigint(Proc.temp.aa2, Proc.read_Cp(r[2]));
                mpz_fdiv_r(Proc.temp.aa.get_mpz_t(), Proc.temp.aa.get_mpz_t(), Proc.temp.aa2.get_mpz_t());
                to_gfp(Proc.temp.ansp, Proc.temp.aa);
                Proc.write_Cp(r[0], Proc.temp.ansp);
                break;
            case LEGENDREC:
                to_bigint(Proc.temp.aa, Proc.read_Cp(r[1]));
                Proc.temp.aa = mpz_legendre(Proc.temp.aa.get_mpz_t(), gfp::pr().get_mpz_t());
                to_gfp(Proc.temp.ansp, Proc.temp.aa);
                Proc.write_Cp(r[0], Proc.temp.ansp);
                break;
            case DIGESTC: {
                stringstream o;
                to_bigint(Proc.temp.aa, Proc.read_Cp(r[1]));

                to_gfp(Proc.temp.ansp, Proc.temp.aa);
                Proc.temp.ansp.output(o, false);
                string s = Hash(o.str());
                // keep first n bytes
                istringstream is(s);
                Proc.temp.ansp.input(is, false);
                Proc.write_Cp(r[0], Proc.temp.ansp);
            }
            break;
            case DIVCI:
                if (n == 0)
                    throw Processor_Error("Division by immediate zero");
                to_gfp(Proc.temp.ansp, n % gfp::pr());
                Proc.temp.ansp.invert();
                Proc.temp.ansp.mul(Proc.read_Cp(r[1]));
                Proc.write_Cp(r[0], Proc.temp.ansp);
                break;
            case MODCI:
                to_bigint(Proc.temp.aa, Proc.read_Cp(r[1]));
                to_gfp(Proc.temp.ansp, mpz_fdiv_ui(Proc.temp.aa.get_mpz_t(), n));
                Proc.write_Cp(r[0], Proc.temp.ansp);
                break;
            case ADDCI:
                Proc.temp.ansp.assign(n);
#ifdef DEBUG
                Proc.temp.ansp.add(Proc.temp.ansp, Proc.read_Cp(r[1]));
                Proc.write_Cp(r[0], Proc.temp.ansp);
#else
                Proc.get_Cp_ref(r[0]).add(Proc.temp.ansp, Proc.read_Cp(r[1]));
#endif
                break;
            case ADDSI:
                Proc.temp.ansp.assign(n);
#ifdef DEBUG
                Proc.temp.Sansp.add(Proc.read_Sp(r[1]), Proc.temp.ansp, P.get_mac_keys());
                Proc.write_Sp(r[0], Proc.temp.Sansp);
#else
                Proc.get_Sp_ref(r[0]).add(Proc.read_Sp(r[1]), Proc.temp.ansp, P.get_mac_keys());
#endif
                break;
            case SUBCI:
                Proc.temp.ansp.assign(n);
#ifdef DEBUG
                Proc.temp.ansp.sub(Proc.read_Cp(r[1]), Proc.temp.ansp);
                Proc.write_Cp(r[0], Proc.temp.ansp);
#else
                Proc.get_Cp_ref(r[0]).sub(Proc.read_Cp(r[1]), Proc.temp.ansp);
#endif
                break;
            case SUBSI:
                Proc.temp.ansp.assign(n);
#ifdef DEBUG
                Proc.temp.Sansp.sub(Proc.read_Sp(r[1]), Proc.temp.ansp, P.get_mac_keys());
                Proc.write_Sp(r[0], Proc.temp.Sansp);
#else
                Proc.get_Sp_ref(r[0]).sub(Proc.read_Sp(r[1]), Proc.temp.ansp, P.get_mac_keys());
#endif
                break;
            case SUBCFI:
                Proc.temp.ansp.assign(n);
#ifdef DEBUG
                Proc.temp.ansp.sub(Proc.temp.ansp, Proc.read_Cp(r[1]));
                Proc.write_Cp(r[0], Proc.temp.ansp);
#else
                Proc.get_Cp_ref(r[0]).sub(Proc.temp.ansp, Proc.read_Cp(r[1]));
#endif
                break;
            case SUBSFI:
                Proc.temp.ansp.assign(n);
#ifdef DEBUG
                Proc.temp.Sansp.sub(Proc.temp.ansp, Proc.read_Sp(r[1]), P.get_mac_keys());
                Proc.write_Sp(r[0], Proc.temp.Sansp);
#else
                Proc.get_Sp_ref(r[0]).sub(Proc.temp.ansp, Proc.read_Sp(r[1]), P.get_mac_keys());
#endif
                break;
            case MULCI:
                Proc.temp.ansp.assign(n);
#ifdef DEBUG
                Proc.temp.ansp.mul(Proc.temp.ansp, Proc.read_Cp(r[1]));
                Proc.write_Cp(r[0], Proc.temp.ansp);
#else
                Proc.get_Cp_ref(r[0]).mul(Proc.temp.ansp, Proc.read_Cp(r[1]));
#endif
                break;
            case MULSI:
                Proc.temp.ansp.assign(n);
#ifdef DEBUG
                Proc.temp.Sansp.mul(Proc.read_Sp(r[1]), Proc.temp.ansp);
                Proc.write_Sp(r[0], Proc.temp.Sansp);
#else
                Proc.get_Sp_ref(r[0]).mul(Proc.read_Sp(r[1]), Proc.temp.ansp);
#endif
                break;
            case ANDC:
#ifdef DEBUG
                Proc.temp.ansp.AND(Proc.read_Cp(r[1]), Proc.read_Cp(r[2]));
                Proc.write_Cp(r[0], Proc.temp.ansp);
#else
                Proc.get_Cp_ref(r[0]).AND(Proc.read_Cp(r[1]), Proc.read_Cp(r[2]));
#endif
                break;
            case XORC:
#ifdef DEBUG
                Proc.temp.ansp.XOR(Proc.read_Cp(r[1]), Proc.read_Cp(r[2]));
                Proc.write_Cp(r[0], Proc.temp.ansp);
#else
                Proc.get_Cp_ref(r[0]).XOR(Proc.read_Cp(r[1]), Proc.read_Cp(r[2]));
#endif
                break;
            case ORC:
#ifdef DEBUG
                Proc.temp.ansp.OR(Proc.read_Cp(r[1]), Proc.read_Cp(r[2]));
                Proc.write_Cp(r[0], Proc.temp.ansp);
#else
                Proc.get_Cp_ref(r[0]).OR(Proc.read_Cp(r[1]), Proc.read_Cp(r[2]));
#endif
                break;
            case ANDCI:
                Proc.temp.aa = n;
#ifdef DEBUG
                Proc.temp.ansp.AND(Proc.read_Cp(r[1]), Proc.temp.aa);
                Proc.write_Cp(r[0], Proc.temp.ansp);
#else
                Proc.get_Cp_ref(r[0]).AND(Proc.read_Cp(r[1]), Proc.temp.aa);
#endif
                break;
            case XORCI:
                Proc.temp.aa = n;
#ifdef DEBUG
                Proc.temp.ansp.XOR(Proc.read_Cp(r[1]), Proc.temp.aa);
                Proc.write_Cp(r[0], Proc.temp.ansp);
#else
                Proc.get_Cp_ref(r[0]).XOR(Proc.read_Cp(r[1]), Proc.temp.aa);
#endif
                break;
            case ORCI:
                Proc.temp.aa = n;
#ifdef DEBUG
                Proc.temp.ansp.OR(Proc.read_Cp(r[1]), Proc.temp.aa);
                Proc.write_Cp(r[0], Proc.temp.ansp);
#else
                Proc.get_Cp_ref(r[0]).OR(Proc.read_Cp(r[1]), Proc.temp.aa);
#endif
                break;
            case NOTC:
                to_bigint(Proc.temp.aa, Proc.read_Cp(r[1]));
                mpz_com(Proc.temp.aa.get_mpz_t(), Proc.temp.aa.get_mpz_t());
                Proc.temp.aa2 = 1;
                Proc.temp.aa2 <<= n;
                Proc.temp.aa += Proc.temp.aa2;
                to_gfp(Proc.temp.ansp, Proc.temp.aa);
                Proc.write_Cp(r[0], Proc.temp.ansp);
                break;
            case SHLC:
                to_bigint(Proc.temp.aa, Proc.read_Cp(r[2]));
                if (Proc.temp.aa > 63)
                {
                    cout << "SHLC : error ";
                    Proc.read_Cp(r[2]).output(cout, true);
                    cout << endl;
                    throw not_implemented();
                }
#ifdef DEBUG
                Proc.temp.ansp.SHL(Proc.read_Cp(r[1]), Proc.temp.aa);
                Proc.write_Cp(r[0], Proc.temp.ansp);
#else
                Proc.get_Cp_ref(r[0]).SHL(Proc.read_Cp(r[1]), Proc.temp.aa);
#endif
                break;
            case SHRC:
                to_bigint(Proc.temp.aa, Proc.read_Cp(r[2]));
                if (Proc.temp.aa > 63)
                {
                    cout << "SHLR : error ";
                    Proc.read_Cp(r[2]).output(cout, true);
                    cout << endl;
                    throw not_implemented();
                }
#ifdef DEBUG
                Proc.temp.ansp.SHR(Proc.read_Cp(r[1]), Proc.temp.aa);
                Proc.write_Cp(r[0], Proc.temp.ansp);
#else
                Proc.get_Cp_ref(r[0]).SHR(Proc.read_Cp(r[1]), Proc.temp.aa);
#endif
                break;
            case SHLCI:
#ifdef DEBUG
                Proc.temp.ansp.SHL(Proc.read_Cp(r[1]), Proc.temp.aa);
                Proc.write_Cp(r[0], Proc.temp.ansp);
#else
                Proc.get_Cp_ref(r[0]).SHL(Proc.read_Cp(r[1]), n);
#endif
                break;
            case SHRCI:
#ifdef DEBUG
                Proc.temp.ansp.SHR(Proc.read_Cp(r[1]), Proc.temp.aa);
                Proc.write_Cp(r[0], Proc.temp.ansp);
#else
                Proc.get_Cp_ref(r[0]).SHR(Proc.read_Cp(r[1]), n);
#endif
                break;
            case JMP:
                Proc.relative_jump((signed int)n);
                break;
            case JMPNE:
                if (Proc.read_Ri(r[0]) != n)
                {
                    Proc.relative_jump((signed int)m);
                }
                break;
            case JMPEQ:
                if (Proc.read_Ri(r[0]) == n)
                {
                    Proc.relative_jump((signed int)m);
                }
                break;
            case CALL:
                Proc.push_int(Proc.get_PC());
                Proc.relative_jump((signed int)n);
                break;
            case CALLR:
                Proc.push_int(Proc.get_PC());
                Proc.jump(Proc.read_Ri(r[0]));
                break;
            case JMPR:
                Proc.jump(Proc.read_Ri(r[0]));
                break;
            case RETURN:
                long ret_pos;
                if (Proc.stack_int.size() >= 1)
                {
                    Proc.pop_int(ret_pos);
                }
                else
                {
                    ret_pos = Proc.program_size();
                }
                Proc.jump(ret_pos);
                break;

            case EQZINT:
                if (Proc.read_Ri(r[1]) == 0)
                    Proc.write_Ri(r[0], 1);
                else
                    Proc.write_Ri(r[0], 0);
                break;
            case LTZINT:
                if (Proc.read_Ri(r[1]) < 0)
                    Proc.write_Ri(r[0], 1);
                else
                    Proc.write_Ri(r[0], 0);
                break;
            case LTINT:
                if (Proc.read_Ri(r[1]) < Proc.read_Ri(r[2]))
                    Proc.write_Ri(r[0], 1);
                else
                    Proc.write_Ri(r[0], 0);
                break;
            case GTINT:
                if (Proc.read_Ri(r[1]) > Proc.read_Ri(r[2]))
                    Proc.write_Ri(r[0], 1);
                else
                    Proc.write_Ri(r[0], 0);
                break;
            case EQINT:
                if (Proc.read_Ri(r[1]) == Proc.read_Ri(r[2]))
                    Proc.write_Ri(r[0], 1);
                else
                    Proc.write_Ri(r[0], 0);
                break;
            case LDINT:
                Proc.write_Ri(r[0], n);
                break;
            case ADDINT:
                Proc.get_Ri_ref(r[0]) = Proc.read_Ri(r[1]) + Proc.read_Ri(r[2]);
                break;
            case SUBINT:
                Proc.get_Ri_ref(r[0]) = Proc.read_Ri(r[1]) - Proc.read_Ri(r[2]);
                break;
            case MULINT:
                Proc.get_Ri_ref(r[0]) = Proc.read_Ri(r[1]) * Proc.read_Ri(r[2]);
                break;
            case DIVINT:
                Proc.get_Ri_ref(r[0]) = Proc.read_Ri(r[1]) / Proc.read_Ri(r[2]);
                break;
            case MODINT:
                Proc.get_Ri_ref(r[0]) = Proc.read_Ri(r[1]) % Proc.read_Ri(r[2]);
                break;
            case CONVINT:
                Proc.get_Cp_ref(r[0]).assign(Proc.read_Ri(r[1]));
                break;
            case CONVMODP:
                to_signed_bigint(Proc.temp.aa, Proc.read_Cp(r[1]), n);
                Proc.write_Ri(r[0], Proc.temp.aa.get_si());
                break;
            case NEWC:
                Proc.write_Ri(r[0], machine.Mc.New(Proc.read_Ri(r[1])));
                break;
            case NEWS:
                Proc.write_Ri(r[0], machine.Ms.New(Proc.read_Ri(r[1])));
                break;
            case NEWINT:
                Proc.write_Ri(r[0], machine.Mr.New(Proc.read_Ri(r[1])));
                break;
            case NEWSINT:
                Proc.write_Ri(r[0], machine.Msr.New(Proc.read_Ri(r[1])));
                break;
            case DELETEC:
                machine.Mc.Delete(Proc.read_Ri(r[0]));
                break;
            case DELETES:
                machine.Ms.Delete(Proc.read_Ri(r[0]));
                break;
            case DELETEINT:
                machine.Mr.Delete(Proc.read_Ri(r[0]));
                break;
            case DELETESINT:
                machine.Msr.Delete(Proc.read_Ri(r[0]));
                break;
            case PRINT_MEM:
                if (P.whoami() == 0)
                {
                    stringstream ss;
                    ss << "Mem[" << n << "] = " << machine.Mc.read(n, machine.verbose) << endl;
                    machine.get_IO().debug_output(ss);
                }
                break;
            case PRINT_REG:
                if (P.whoami() == 0)
                {
                    stringstream ss;
                    ss << Proc.read_Cp(r[0]);
                    machine.get_IO().debug_output(ss);
                }
                break;
            case PRINT_FIX:
                if (P.whoami() == 0)
                {
                    gfp v = Proc.read_Cp(r[0]);
                    mpf_class res;
                    convert_from_fix(res, v, m, n);
                    stringstream ss;
                    ss << res;
                    machine.get_IO().debug_output(ss);
                }
                break;
            case PRINT_FLOAT:
                if (P.whoami() == 0)
                {
                    gfp v = Proc.read_Cp(c_start[0]);
                    gfp p = Proc.read_Cp(c_start[1]);
                    gfp z = Proc.read_Cp(c_start[2]);
                    gfp s = Proc.read_Cp(c_start[3]);
                    gfp e = Proc.read_Cp(c_start[4]);

                    mpf_class res;
                    convert_from_float(res, v, p, z, s);

                    stringstream ss;
                    if (e.is_zero())
                    {
                        ss << res;
                    }
                    else
                    {
                        ss << "NaN";
                    }
                    machine.get_IO().debug_output(ss);
                }
                break;
            case PRINT_INT:
                if (P.whoami() == 0)
                {
                    stringstream ss;
                    ss << Proc.read_Ri(r[0]);
                    machine.get_IO().debug_output(ss);
                }
                break;
            case PRINT_IEEE_FLOAT:
                if (P.whoami() == 0)
                {
                    long y = Proc.read_Ri(r[0]);
                    double x = convert_to_double(y);
                    // Now print the double
                    stringstream ss;
                    ss.precision(numeric_limits<double>::digits10 + 2);
                    ss << x;
                    machine.get_IO().debug_output(ss);
                }
                break;
            case PRINT_CHAR4:
                if (P.whoami() == 0)
                {
                    stringstream ss;
                    ss << string((char *)&n, sizeof(n));
                    machine.get_IO().debug_output(ss);
                }
                break;
            case PRINT_CHAR:
                if (P.whoami() == 0)
                {
                    stringstream ss;
                    ss << string((char *)&n, 1);
                    machine.get_IO().debug_output(ss);
                }
                break;
            case PRINT_CHAR_REGINT:
                if (P.whoami() == 0)
                {
                    stringstream ss;
                    ss << string((char *)&(Proc.read_Ri(r[0])), 1);
                    machine.get_IO().debug_output(ss);
                }
                break;
            case PRINT_CHAR4_REGINT:
                if (P.whoami() == 0)
                {
                    stringstream ss;
                    ss << string((char *)&(Proc.read_Ri(r[0])), sizeof(int));
                    machine.get_IO().debug_output(ss);
                }
                break;
            case RAND:
                Proc.write_Ri(r[0], Proc.get_random_word() % (1 << Proc.read_Ri(r[1])));
                break;
            case START_CLOCK:
                machine.start_timer(n);
                break;
            case STOP_CLOCK:
                machine.stop_timer(n);
                break;
            case RANDC:
                Proc.write_Cp(r[0], Proc.get_random_gfp());
                break;
            case RANDINT:
                Proc.write_Ri(r[0], Proc.get_random_word());
                break;
            case RANDSINT: {
                SRegint a;
                a.randomize(Proc.online_thread_num, P);
                Proc.write_srint(r[0], a);
            }
            break;
            case RANDFLOAT:
                Proc.write_Ri(r[0], Proc.get_random_float());
                break;
            case RANDSBIT: {
                SBit a;
                a.randomize(Proc.online_thread_num, P);
                Proc.write_sbit(r[0], a);
            }
            break;
            case REQBL:
                break;
            case RUN_TAPE:
                machine.run_tape(r[0], r[2], r[1], n);
                break;
            case JOIN_TAPE:
                machine.Lock_Until_Finished_Tape(n);
                break;
            case CRASH:
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wimplicit-fallthrough"
                // Note deliberately no "break" to enable CRASH to call RESTART
                // if the IO.crash returns
                machine.get_IO().crash(Proc.get_PC() - 1, Proc.get_thread_num());
#pragma GCC diagnostic pop
            case RESTART:
                if (Proc.get_thread_num() != 0)
                {
                    throw IO_thread();
                }
                restart = true;
                break;
            case CLEAR_MEMORY:
                machine.Mc.clear_memory();
                machine.Ms.clear_memory();
                machine.Mr.clear_memory();
                machine.Msr.clear_memory();
                break;
            case CLEAR_REGISTERS:
                Proc.clear_registers();
                break;
            case OUTPUT_SHARES:
                if (Proc.get_thread_num() != 0)
                {
                    throw IO_thread();
                }
                for (unsigned int j = 0; j < c_start.size(); j++)
                {
                    machine.get_IO().output_share(Proc.get_Sp_ref(c_start[j]), p);
                }
                break;
            case INPUT_SHARES:
                if (Proc.get_thread_num() != 0)
                {
                    throw IO_thread();
                }
                for (unsigned int j = 0; j < c_start.size(); j++)
                {
                    Proc.get_Sp_ref(c_start[j]) = machine.get_IO().input_share(p);
                }
                break;
            case INPUT_CLEAR:
                if (Proc.get_thread_num() != 0)
                {
                    throw IO_thread();
                }
                Proc.get_Cp_ref(r[0]) = machine.get_IO().public_input_gfp(n);
                break;
            case INPUT_INT:
                if (Proc.get_thread_num() != 0)
                {
                    throw IO_thread();
                }
                Proc.get_Ri_ref(r[0]) = machine.get_IO().public_input_int(n);
                break;
            case OUTPUT_CLEAR:
                if (Proc.get_thread_num() != 0)
                {
                    throw IO_thread();
                }
                machine.get_IO().public_output_gfp(Proc.read_Cp(r[0]), n);
                break;
            case OUTPUT_INT:
                if (Proc.get_thread_num() != 0)
                {
                    throw IO_thread();
                }
                machine.get_IO().public_output_int(Proc.read_Ri(r[0]), n);
                break;
            case OPEN_CHANNEL:
                if (Proc.get_thread_num() != 0)
                {
                    throw IO_thread();
                }
                Proc.get_Ri_ref(r[0]) = machine.get_IO().open_channel(n);
                break;
            case CLOSE_CHANNEL:
                if (Proc.get_thread_num() != 0)
                {
                    throw IO_thread();
                }
                machine.get_IO().close_channel(n);
                break;
                /* Now we add in the new instructions for sregint and sbit operations */
            case LDMSINT:
                Proc.write_srint(r[0], machine.Msr.read(n, machine.verbose));
                n++;
                break;
            case LDMSINTI:
                Proc.write_srint(r[0], machine.Msr.read(Proc.read_Ri(r[1]), machine.verbose));
                break;
            case STMSINT:
                machine.Msr.write(n, Proc.read_srint(r[0]), machine.verbose, Proc.get_PC());
                n++;
                break;
            case STMSINTI:
                machine.Msr.write(Proc.read_Ri(r[1]), Proc.read_srint(r[0]), machine.verbose, Proc.get_PC());
                break;
            case MOVSINT:
                Proc.write_srint(r[0], Proc.read_srint(r[1]));
                break;
            case LDSINT:
                Proc.write_srint(r[0], n);
                break;
            case LDSBIT:
                if ((n & 1) == 0)
                {
                    aB.assign_zero(P.whoami());
                }
                else
                {
                    aB.assign_one(P.whoami());
                }
                Proc.write_sbit(r[0], aB);
                break;
            case ADDSINT:
                Proc.get_srint_ref(r[0]).add(Proc.read_srint(r[1]), Proc.read_srint(r[2]), P, Proc.online_thread_num);
                break;
            case ADDSINTC:
                Proc.get_srint_ref(r[0]).add(Proc.read_srint(r[1]), Proc.read_Ri(r[2]), P, Proc.online_thread_num);
                break;
            case SUBSINT:
                Proc.get_srint_ref(r[0]).sub(Proc.read_srint(r[1]), Proc.read_srint(r[2]), P, Proc.online_thread_num);
                break;
            case SUBSINTC:
                Proc.get_srint_ref(r[0]).sub(Proc.read_srint(r[1]), Proc.read_Ri(r[2]), P, Proc.online_thread_num);
                break;
            case SUBCINTS:
                Proc.get_srint_ref(r[0]).sub(Proc.read_Ri(r[1]), Proc.read_srint(r[2]), P, Proc.online_thread_num);
                break;
            case MULSINT:
                Proc.get_srint_ref(r[0]).mul(Proc.read_srint(r[1]), Proc.read_srint(r[2]), P, Proc.online_thread_num);
                break;
            case MULSINTC:
                Proc.get_srint_ref(r[0]).mul(Proc.read_srint(r[1]), Proc.read_Ri(r[2]), P, Proc.online_thread_num);
                break;
            case MUL2SINT:
                mul(Proc.get_srint_ref(c_start[0]), Proc.get_srint_ref(c_start[1]), Proc.read_srint(c_start[2]),
                    Proc.read_srint(c_start[3]), P, Proc.online_thread_num);
                break;
            case DIVSINT:
                Proc.get_srint_ref(r[0]).div(Proc.read_srint(r[1]), Proc.read_srint(r[2]), P, Proc.online_thread_num);
                break;
            case SHLSINT:
                Proc.get_srint_ref(r[0]).SHL(Proc.read_srint(r[1]), n);
                break;
            case SHRSINT:
                Proc.get_srint_ref(r[0]).SHR(Proc.read_srint(r[1]), n);
                break;
            case NEG:
                Proc.get_srint_ref(r[0]).negate(Proc.read_srint(r[1]), P, Proc.online_thread_num);
                break;
            case SAND:
                Proc.get_srint_ref(r[0]).Bit_AND(Proc.read_srint(r[1]), Proc.read_sbit(r[2]), P,
                                                 Proc.online_thread_num);
                break;
            case ANDSINT:
                Proc.get_srint_ref(r[0]).Bitwise_AND(Proc.read_srint(r[1]), Proc.read_srint(r[2]), P,
                                                     Proc.online_thread_num);
                break;
            case ANDSINTC:
                Proc.get_srint_ref(r[0]).Bitwise_AND(Proc.read_srint(r[1]), Proc.read_Ri(r[2]));
                break;
            case ORSINT:
                Proc.get_srint_ref(r[0]).Bitwise_OR(Proc.read_srint(r[1]), Proc.read_srint(r[2]), P,
                                                    Proc.online_thread_num);
                break;
            case ORSINTC:
                Proc.get_srint_ref(r[0]).Bitwise_OR(Proc.read_srint(r[1]), Proc.read_Ri(r[2]));
                break;
            case XORSINT:
                Proc.get_srint_ref(r[0]).Bitwise_XOR(Proc.read_srint(r[1]), Proc.read_srint(r[2]));
                break;
            case XORSINTC:
                Proc.get_srint_ref(r[0]).Bitwise_XOR(Proc.read_srint(r[1]), Proc.read_Ri(r[2]));
                break;
            case INVSINT:
                Proc.get_srint_ref(r[0]).Bitwise_Negate(Proc.read_srint(r[1]));
                break;
            case ANDINT:
                Proc.get_Ri_ref(r[0]) = Proc.read_Ri(r[1]) & Proc.read_Ri(r[2]);
                break;
            case ORINT:
                Proc.get_Ri_ref(r[0]) = Proc.read_Ri(r[1]) | Proc.read_Ri(r[2]);
                break;
            case XORINT:
                Proc.get_Ri_ref(r[0]) = Proc.read_Ri(r[1]) ^ Proc.read_Ri(r[2]);
                break;
            case INVINT:
                Proc.get_Ri_ref(r[0]) = ~Proc.read_Ri(r[1]);
                break;
            case SHLINT:
                Proc.get_Ri_ref(r[0]) = Proc.read_Ri(r[1]) << Proc.read_Ri(r[2]);
                break;
            case SHRINT:
                Proc.get_Ri_ref(r[0]) = Proc.read_Ri(r[1]) >> Proc.read_Ri(r[2]);
                break;
            case XORSB:
                Proc.get_sbit_ref(r[0]).add(Proc.read_sbit(r[1]), Proc.read_sbit(r[2]));
                break;
            case ANDSB:
                Mult_Bit(Proc.get_sbit_ref(r[0]), Proc.read_sbit(r[1]), Proc.read_sbit(r[2]), P,
                         Proc.online_thread_num);
#if BENCH_OFFLINE
                P.aands++;
#endif
                break;
            case ORSB:
                Mult_Bit(aB, Proc.read_sbit(r[1]), Proc.read_sbit(r[2]), P, Proc.online_thread_num);
                Proc.get_sbit_ref(r[0]).add(Proc.read_sbit(r[1]), Proc.read_sbit(r[2]));
                Proc.get_sbit_ref(r[0]).add(aB);
#if BENCH_OFFLINE
                P.aands++;
#endif
                break;
            case NEGB:
                Proc.get_sbit_ref(r[0]).negate(Proc.read_sbit(r[1]));
                break;
            case LTZSINT:
                Proc.get_sbit_ref(r[0]) = Proc.read_srint(r[1]).less_than_zero();
                break;
            case EQZSINT:
                Proc.get_sbit_ref(r[0]) = Proc.read_srint(r[1]).equal_zero(P, Proc.online_thread_num);
                break;
            case BITSINT:
                Proc.get_sbit_ref(r[0]) = Proc.read_srint(r[1]).get_bit(n);
                break;
            case SINTBIT:
                Proc.get_srint_ref(r[0]) = Proc.read_srint(r[1]);
                Proc.get_srint_ref(r[0]).set_bit(n, Proc.read_sbit(r[2]));
                break;
            case CONVSINTSREG:
                Proc.convert_sint_to_sregint(r[1], r[0], P);
                break;
            case CONVSREGSINT:
                Proc.convert_sregint_to_sint(r[1], r[0], P);
                break;
            case CONVSUREGSINT:
                Proc.convert_suregint_to_sint(r[1], r[0], P);
                break;
            case CONVREGSREG:
                Proc.write_srint(r[0], Proc.read_Ri(r[1]));
                break;
            case CONVSINTSBIT:
                Proc.convert_sint_to_sbit(r[1], r[0], P, OCD);
                break;
            case CONVSBITSINT:
                Proc.convert_sbit_to_sint(r[1], r[0], P);
                break;
            case OPENSINT:
                P.OP2->Open_BitVector(t, Proc.read_srint(r[1]), P);
                Proc.write_Ri(r[0], t);
                break;
            case OPENSBIT:
                P.OP2->Open_Bit(t, Proc.read_sbit(r[1]), P);
                // AND with one, as for Share2's the sbits can be bigger
                // than one bit
                Proc.write_Ri(r[0], t & 1);
                break;
            case GC:
                Proc.apply_GC(n, P);
                break;
            case OGC:
                Proc.garble_circuit(n, m, P);
                break;
            case LOADGC:
                Proc.load_garbled_circuits(n, m, P);
                break;
            case EGC:
                Proc.evaluate_garbled_circuit(n, P);
                break;
            case LF:
                machine.LF_Table.apply_Function(n, Proc);
                break;
            // ============== DB Instructions
            case LOADSRAND:
                Proc.load_bounded_randomness(n, m, P);
                break;
            case LOADTRIPLE:
                Proc.load_beaver_triples(n, P);
                break;
            case LOADDABIT:
                std::cout << "Executes Load DABIT" << std::endl;
                Proc.load_daBits(n, P);
                Proc.is_dabits_mem_announced = false;
                std::cout << "Successfully Load DABITs to Memory" << std::endl;
                break;
            case LOADCT:
                Proc.load_matrix_triples(n, m, P);
                break;
            case CTRIPLE: {
                vector<Share> values = Proc.obtain_next_matrix_triple(c_start.size(), P);
                unsigned long invocations = c_start.size() / values.size();
                for (unsigned int k = 0; k < invocations; k++)
                {
                    for (unsigned int j = 0; j < values.size(); j++)
                    {
                        unsigned long c_start_j_position = (k)*values.size() + j;
                        Proc.get_Sp_ref(c_start[c_start_j_position]) = values.at(j);
                    }
                    if (invocations - 1 == k)
                    {
                        break;
                    }
                    else
                    {
                        values = Proc.obtain_next_matrix_triple(c_start.size(), P);
                    }
                }
            }
            break;
            case CT_DYN: {
                vector<Share> values = Proc.obtain_matrix_triple_by_triple_id(m, c_start.size(), P);
                for (unsigned int j = 0; j < c_start.size(); j++)
                {
                    Proc.get_Sp_ref(c_start[j]) = values.at(j);
                }
            }
            break;
            case MADDC: {
                unsigned int i = Proc.read_Ri(r[0]);
                unsigned int j = Proc.read_Ri(r[1]);
                unsigned int k = Proc.read_Ri(r[2]);
                for (unsigned int t = 0; t < Proc.read_Ri(n); t++)
                {
                    Proc.temp.ansp.add(machine.Mc.read(j + t, machine.verbose),
                                       machine.Mc.read(k + t, machine.verbose));
                    machine.Mc.write(i + t, Proc.temp.ansp, machine.verbose, Proc.get_PC());
                }
            }
            break;
            case MADDS: {
                unsigned int i = Proc.read_Ri(r[0]);
                unsigned int j = Proc.read_Ri(r[1]);
                unsigned int k = Proc.read_Ri(r[2]);
                for (unsigned int t = 0; t < Proc.read_Ri(n); t++)
                {
                    Proc.temp.Sansp.add(machine.Ms.read(j + t, machine.verbose),
                                        machine.Ms.read(k + t, machine.verbose));
                    machine.Ms.write(i + t, Proc.temp.Sansp, machine.verbose, Proc.get_PC());
                }
            }
            break;
            case MADDM: {
                unsigned int i = Proc.read_Ri(r[0]);
                unsigned int j = Proc.read_Ri(r[1]);
                unsigned int k = Proc.read_Ri(r[2]);
                for (unsigned int t = 0; t < Proc.read_Ri(n); t++)
                {
                    Proc.temp.Sansp.add(machine.Ms.read(j + t, machine.verbose),
                                        machine.Mc.read(k + t, machine.verbose), P.get_mac_keys());
                    machine.Ms.write(i + t, Proc.temp.Sansp, machine.verbose, Proc.get_PC());
                }
            }
            break;
            case MSUBC: {
                unsigned int i = Proc.read_Ri(r[0]);
                unsigned int j = Proc.read_Ri(r[1]);
                unsigned int k = Proc.read_Ri(r[2]);
                for (unsigned int t = 0; t < Proc.read_Ri(n); t++)
                {
                    Proc.temp.ansp.sub(machine.Mc.read(j + t, machine.verbose),
                                       machine.Mc.read(k + t, machine.verbose));
                    machine.Mc.write(i + t, Proc.temp.ansp, machine.verbose, Proc.get_PC());
                }
            }
            break;
            case MSUBS: {
                unsigned int i = Proc.read_Ri(r[0]);
                unsigned int j = Proc.read_Ri(r[1]);
                unsigned int k = Proc.read_Ri(r[2]);
                for (unsigned int t = 0; t < Proc.read_Ri(n); t++)
                {
                    Proc.temp.Sansp.sub(machine.Ms.read(j + t, machine.verbose),
                                        machine.Ms.read(k + t, machine.verbose));
                    machine.Ms.write(i + t, Proc.temp.Sansp, machine.verbose, Proc.get_PC());
                }
            }
            break;
            case MSUBML: {
                unsigned int i = Proc.read_Ri(r[0]);
                unsigned int j = Proc.read_Ri(r[1]);
                unsigned int k = Proc.read_Ri(r[2]);
                for (unsigned int t = 0; t < Proc.read_Ri(n); t++)
                {
                    Proc.temp.Sansp.sub(machine.Ms.read(j + t, machine.verbose),
                                        machine.Mc.read(k + t, machine.verbose), P.get_mac_keys());
                    machine.Ms.write(i + t, Proc.temp.Sansp, machine.verbose, Proc.get_PC());
                }
            }
            break;
            case MSUBMR: {
                unsigned int i = Proc.read_Ri(r[0]);
                unsigned int j = Proc.read_Ri(r[1]);
                unsigned int k = Proc.read_Ri(r[2]);
                for (unsigned int t = 0; t < Proc.read_Ri(n); t++)
                {
                    Proc.temp.Sansp.sub(machine.Mc.read(j + t, machine.verbose),
                                        machine.Ms.read(k + t, machine.verbose), P.get_mac_keys());
                    machine.Ms.write(i + t, Proc.temp.Sansp, machine.verbose, Proc.get_PC());
                }
            }
            break;
            case MMULC: {
                unsigned int i = Proc.read_Ri(r[0]);
                unsigned int j = Proc.read_Ri(r[1]);
                unsigned int k = Proc.read_Ri(r[2]);
                for (unsigned int t = 0; t < Proc.read_Ri(n); t++)
                {
                    Proc.temp.ansp.mul(machine.Mc.read(j + t, machine.verbose),
                                       machine.Mc.read(k + t, machine.verbose));
                    machine.Mc.write(i + t, Proc.temp.ansp, machine.verbose, Proc.get_PC());
                }
            }
            break;
            case MMULM: {
                unsigned int i = Proc.read_Ri(r[0]);
                unsigned int j = Proc.read_Ri(r[1]);
                unsigned int k = Proc.read_Ri(r[2]);
                for (unsigned int t = 0; t < Proc.read_Ri(n); t++)
                {
                    Proc.temp.Sansp.mul(machine.Ms.read(j + t, machine.verbose),
                                        machine.Mc.read(k + t, machine.verbose));
                    machine.Ms.write(i + t, Proc.temp.Sansp, machine.verbose, Proc.get_PC());
                }
            }
            break;
            case MDIVC: {
                unsigned int i = Proc.read_Ri(r[0]);
                unsigned int j = Proc.read_Ri(r[1]);
                unsigned int k = Proc.read_Ri(r[2]);
                for (unsigned int t = 0; t < Proc.read_Ri(n); t++)
                {
                    gfp num = machine.Mc.read(k + t, machine.verbose);
                    if (num.is_zero())
                        throw Processor_Error("Division by zero from register");
                    Proc.temp.ansp.invert(num);
                    Proc.temp.ansp.mul(machine.Mc.read(j + t, machine.verbose));
                    machine.Mc.write(i + t, Proc.temp.ansp, machine.verbose, Proc.get_PC());
                }
            }
            break;
            case MMODC: {
                unsigned int i = Proc.read_Ri(r[0]);
                unsigned int j = Proc.read_Ri(r[1]);
                unsigned int k = Proc.read_Ri(r[2]);
                for (unsigned int t = 0; t < Proc.read_Ri(n); t++)
                {
                    to_bigint(Proc.temp.aa, machine.Mc.read(j + t, machine.verbose));
                    to_bigint(Proc.temp.aa2, machine.Mc.read(k + t, machine.verbose));
                    mpz_fdiv_r(Proc.temp.aa.get_mpz_t(), Proc.temp.aa.get_mpz_t(), Proc.temp.aa2.get_mpz_t());
                    to_gfp(Proc.temp.ansp, Proc.temp.aa);
                    machine.Mc.write(i + t, Proc.temp.ansp, machine.verbose, Proc.get_PC());
                }
            }
            break;
            case MREVC: {
                unsigned int i = Proc.read_Ri(r[0]);
                unsigned int j = Proc.read_Ri(r[1]);
                unsigned int len = Proc.read_Ri(r[2]);
                // Copy first in case i=j
                vector<gfp> A(len);
                for (unsigned int t = 0; t < len; t++)
                {
                    A[t] = machine.Mc.read(len - 1 + j - t, machine.verbose);
                }
                for (unsigned int t = 0; t < len; t++)
                {
                    machine.Mc.write(i + t, A[t], machine.verbose, Proc.get_PC());
                }
            }
            break;
            case MREVS: {
                unsigned int i = Proc.read_Ri(r[0]);
                unsigned int j = Proc.read_Ri(r[1]);
                unsigned int len = Proc.read_Ri(r[2]);
                // Copy first in case i=j
                vector<Share> A(len);
                for (unsigned int t = 0; t < len; t++)
                {
                    A[t] = machine.Ms.read(len - 1 + j - t, machine.verbose);
                }
                for (unsigned int t = 0; t < len; t++)
                {
                    machine.Ms.write(i + t, A[t], machine.verbose, Proc.get_PC());
                }
            }
            break;
            case MEVALCC: {
                unsigned int i = Proc.read_Ri(r[1]);
                unsigned int len = Proc.read_Ri(r[2]);
                gfp x = Proc.read_Cp(n), xpow = 1, sum = 0;
                for (unsigned int t = 0; t < len; t++)
                {
                    sum = sum + machine.Mc.read(i + t, machine.verbose) * xpow;
                    xpow = xpow * x;
                }
                Proc.write_Cp(r[0], sum);
            }
            break;
            case MEVALSC: {
                unsigned int i = Proc.read_Ri(r[1]);
                unsigned int len = Proc.read_Ri(r[2]);
                gfp x = Proc.read_Cp(n), xpow = 1;
                Share sum(P.whoami());
                for (unsigned int t = 0; t < len; t++)
                {
                    sum = sum + machine.Ms.read(i + t, machine.verbose) * xpow;
                    ;
                    xpow = xpow * x;
                }
                Proc.write_Sp(r[0], sum);
            }
            break;
            case MBITDECC: {
                unsigned int i = Proc.read_Ri(r[0]);
                gfp zero, one, x = Proc.read_Cp(r[1]);
                unsigned int len = Proc.read_Ri(r[2]);
                zero.assign_zero();
                one.assign_one();
                bigint val;
                to_bigint(val, x);
                for (unsigned int t = 0; t < len; t++)
                {
                    if (isOdd(val))
                    {
                        machine.Mc.write(i + t, one, machine.verbose, Proc.get_PC());
                    }
                    else
                    {
                        machine.Mc.write(i + t, zero, machine.verbose, Proc.get_PC());
                    }
                    val >>= 1;
                }
            }
            break;
            case MBITDECINT: {
                unsigned int i = Proc.read_Ri(r[0]);
                unsigned int val = Proc.read_Ri(r[1]);
                unsigned int len = Proc.read_Ri(r[2]);
                for (unsigned int t = 0; t < len; t++)
                {
                    machine.Mr.write(i + t, val & 1, machine.verbose, Proc.get_PC());
                    val >>= 1;
                }
            }
            break;
            default:
                printf("Invalid opcode=%d. Since it is not implemented\n", opcode);
                throw not_implemented();
                break;
            }
            if (size > 1)
            {
                r[0]++;
                r[1]++;
                r[2]++;
                for (unsigned int j = 0; j < c_start.size(); j++)
                {
                    c_start[j]++;
                }
            }
        }
    }
    if (machine.verbose > 1)
    {
        long long time = instr_timer.elapsed_since_last_start();
        printf(BENCH_TEXT_BOLD);
        if (time > 10000)
        {
            printf(BENCH_COLOR_RED "++");
        }
        else if (time > 1000)
        {
            printf(BENCH_COLOR_BLUE "+ ");
        }
        else
        {
            printf(BENCH_COLOR_GREEN "  ");
        }
        printf("\t{\"instruction_timer\": %lld}", time);
        printf("\n" BENCH_ATTR_RESET);
        fflush(stdout);
    }
    return restart;
}

template ostream &operator<<(ostream &s, const Instruction<aBitVector, aBit> &instr);

template class Instruction<aBitVector, aBit>;

template class Instruction<aBitVector2, Share2>;
