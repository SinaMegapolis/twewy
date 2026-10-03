#!/usr/bin/env python3
"""perm_score.py -- fast, dependency-free scorer for a permuter search.

decomp-permuter is not present on this box, and its usual loop is fast because
it assembles two tiny objects and diffs text.  This box has no ARM-capable
objdump, but it does not need one: a function's compiled bytes live in its
object file, and the delinked *target* object is on disk too.  So the score is a
plain word-by-word comparison of the two functions' bytes.

Reading the ELF directly (rather than shelling out to objdiff-cli, which reloads
the whole project) is what makes this usable in a search loop -- roughly two
orders of magnitude faster per candidate than fndiff.py.

Two numbers are reported, because they answer different questions:

  strict  differing 4-byte words.  This is the goal; 0 is a byte-exact match.
  masked  the same, after clearing the bits that name registers.  A non-zero
          masked score means the opcode stream genuinely differs, i.e. the
          source shape is wrong and no amount of register reasoning will help.
          A zero masked score with non-zero strict means only register choice
          differs -- that is the case worth reasoning about by hand afterwards.

    python tools/perm_score.py <target.o> <current.o> <function> [--quiet]
"""

import struct
import sys

# ARM names up to three registers in one word: Rd/Rt in bits 0-3, Rn in 8-11
# and Rs in 12-15.  Masking only the first two understates how much of a diff is
# pure register renaming, so all three go, with the two encodings that use those
# bits for something else exempted below.
REG_MASK = 0x0000_FF0F


def read_elf(data):
    """Return (sections, symtab_entries).  Sections are (name, sh_addr,
    sh_offset, sh_size); symbols are (name, st_shndx, st_value, st_size).

    These are relocatable objects, so every section's sh_addr is 0 and a
    symbol's st_value is an offset *within* its own section.  Sections must
    therefore be looked up by index rather than by address range -- address
    ranges all overlap at 0.
    """
    if data[:4] != b"\x7fELF":
        raise ValueError("not an ELF file")
    if data[4] != 1:
        raise ValueError("expected ELF32")
    end = "<" if data[5] == 1 else ">"

    e_shoff, = struct.unpack_from(end + "I", data, 0x20)
    e_shentsize, e_shnum, e_shstrndx = struct.unpack_from(end + "HHH", data, 0x2E)

    headers = [
        struct.unpack_from(end + "IIIIIIIIII", data, e_shoff + i * e_shentsize)
        for i in range(e_shnum)
    ]

    shstr_off = headers[e_shstrndx][4]
    sections = []
    for h in headers:
        sh_name, sh_type, _fl, sh_addr, sh_offset, sh_size = h[0], h[1], h[2], h[3], h[4], h[5]
        if sh_type == 8:  # SHT_NOBITS (.bss) occupies no file bytes
            continue
        e = data.index(b"\x00", shstr_off + sh_name)
        sections.append((data[shstr_off + sh_name:e].decode("latin1"),
                         sh_addr, sh_offset, sh_size))

    symbols = []
    for h in headers:
        if h[1] != 2:  # SHT_SYMTAB
            continue
        sh_offset, sh_size, sh_link = h[4], h[5], h[6]
        str_off = headers[sh_link][4]
        for k in range(sh_size // 16):
            o = sh_offset + k * 16
            st_name, st_value, st_size, _info, _other, st_shndx = struct.unpack_from(
                end + "IIIBBH", data, o
            )
            e = data.index(b"\x00", str_off + st_name)
            symbols.append((data[str_off + st_name:e].decode("latin1"),
                            st_shndx, st_value, st_size))
    return sections, symbols


def function_bytes(path, func):
    data = open(path, "rb").read()
    sections, symbols = read_elf(data)
    for name, shndx, value, size in symbols:
        if name == func and size:
            if shndx >= len(sections):
                raise KeyError(f"{func}: bad section index {shndx}")
            sec_name, _addr, sec_off, _sec_size = sections[shndx]
            start = sec_off + value
            return sec_name, data[start:start + size]
    raise KeyError(f"symbol {func} not found")


def mask_word(w):
    """Clear register-numbering bits, keeping opcode and immediate bits.

    Two encodings use the low bits for something other than a register and are
    handled explicitly, because a blind mask would make a genuinely wrong branch
    target or register list look like a match:

      B/BL  (0b0_101_xxxx)  bits 0-23 are a PC-relative byte offset
      LDM/STM (0b0_100_xxxx) bits 0-15 are a register list
    """
    top = w & 0x0E00_0000
    if top == 0x0A00_0000:  # B / BL
        return w
    if top == 0x0800_0000:  # LDM / STM
        return w & 0xFFFF_0000
    return w & ~REG_MASK & 0xFFFF_FFFF


def compare(a, b):
    n = max(len(a), len(b)) // 4
    strict = masked = 0
    for i in range(n):
        wa = a[i * 4:i * 4 + 4]
        wb = b[i * 4:i * 4 + 4]
        wa = wa.ljust(4, b"\x00")
        wb = wb.ljust(4, b"\x00")
        if wa != wb:
            strict += 1
        if mask_word(struct.unpack("<I", wa)[0]) != mask_word(struct.unpack("<I", wb)[0]):
            masked += 1
    return strict, masked


def main():
    argv = [x for x in sys.argv[1:] if not x.startswith("--")]
    quiet = "--quiet" in sys.argv
    if len(argv) < 3:
        sys.stderr.write(__doc__)
        sys.exit(2)
    target_o, current_o, func = argv[0], argv[1], argv[2]

    try:
        _, tb = function_bytes(target_o, func)
    except KeyError as e:
        print(f"target: {e}")
        return
    try:
        _, cb = function_bytes(current_o, func)
    except KeyError as e:
        print(f"current: {e}")
        return

    strict, masked = compare(tb, cb)
    if not quiet:
        print(f"{strict} {masked}")
    else:
        print(strict)
    return strict


if __name__ == "__main__":
    main()
