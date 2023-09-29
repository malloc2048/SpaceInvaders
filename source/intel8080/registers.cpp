#include "cpu.h"

word Registers::read(byte reg) const {
    word data = 0;
    switch(reg) {
        case B:
            return b;
        case C:
            return c;
        case D:
            return d;
        case E:
            return e;
        case H:
            return h;
        case L:
            return l;
        case M:
            return 0;
        case A:
            return accumulator;
        case BC:
            return word((b << 8u) | c);
        case DE:
            return word((d << 8u) | e);
        case HL:
            return word((h << 8u) | l);
        case SP:
            return stack_pointer;
        default:
            return data;
    }
}

void Registers::write(byte reg, word data) {
    switch(reg) {
        case B:
            b = data & 0xffu;
            break;
        case C:
            c = data & 0xffu;
            break;
        case D:
            d = data & 0xffu;
            break;
        case E:
            e = data & 0xffu;
            break;
        case H:
            h = data & 0xffu;
            break;
        case L:
            l = data & 0xffu;
            break;
        case A:
            accumulator = data & 0xffu;
            break;
        case BC:
            b = (data >> 8u) & 0xffu;
            c = data & 0xffu;
            break;
        case DE:
            d = (data >> 8u) & 0xffu;
            e = data & 0xffu;
            break;
        case HL:
            h = (data >> 8u) & 0xffu;
            l = data & 0xffu;
            break;
        case SP:
            stack_pointer = data;
            break;
        default:
            break;
    }
}
