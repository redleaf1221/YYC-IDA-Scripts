from ida_domain import Database
from ida_domain.bytes import ByteFlags

r_value_kinds = {
    0: "real",
    1: "string",
    2: "array",
    3: "ptr",
    4: "vec3",
    5: "undefined",
    6: "object",
    7: "int32",
    8: "vec4",
    9: "vec44",
    10: "int64",
    11: "accessor",
    12: "null",
    13: "bool",
    14: "iterator",
    15: "ref"
}


def check_is_RValue(db: Database, ea: int) -> bool:
    # Usually yyvar is behind consts, they should be already named.
    return ((db.bytes.get_dword_at(ea + 12) in r_value_kinds) and
            not db.bytes.has_any_flags_at(ea, ByteFlags.NAME))


def name_r_value(db: Database, value_ea: int, kind: int) -> str:
    if kind == 0:
        # real
        db.comments.set_at(value_ea, str(db.bytes.get_double_at(value_ea)))
        name = (f"rv_{r_value_kinds[kind]}_"
                f"{str(db.bytes.get_double_at(value_ea)).replace('.', '_').replace('-', 'neg')}"
                f"_p{hex(value_ea)[2:]}")
    elif kind == 7 or kind == 10:
        # int
        db.comments.set_at(value_ea, str(db.bytes.get_qword_at(value_ea)))
        name = f"rv_{r_value_kinds[kind]}_{db.bytes.get_qword_at(value_ea)}_p{hex(value_ea)[2:]}"
    elif kind == 13:
        # bool
        name = (f"rv_{r_value_kinds[kind]}_"
                f"{bool(db.bytes.get_qword_at(value_ea))}_a{hex(value_ea)[2:]}")
    else:
        # any other guy~
        name = f"rv_{r_value_kinds[kind]}_v{db.bytes.get_qword_at(value_ea)}_p{hex(value_ea)[2:]}"
    db.names.set_name(value_ea, name)
    return name


def main():
    with Database.open() as db:
        print("Parsing RValue consts...")
        print(f"Current database: {db.path}")
        r_value_type = db.types.get_by_name("RValue")
        d_value_type = db.types.get_by_name("DValue")
        dl_value_type = db.types.get_by_name("DLValue")
        if not r_value_type or not d_value_type or not dl_value_type:
            print("Type not found, run the IDC first.")
            exit(0)
        current_ea = db.current_ea
        if not db.is_valid_ea(current_ea):
            print("ea not valid.")
            exit(0)

        print(f"Setting const type starting from {hex(current_ea)}...")
        while check_is_RValue(db, current_ea):
            kind = db.bytes.get_dword_at(current_ea + 12)

            name = name_r_value(db, current_ea, kind)
            if kind == 0:
                # real
                db.types.apply_at(d_value_type, current_ea)
            elif kind == 10:
                # int64
                db.types.apply_at(dl_value_type, current_ea)
            else:
                # any other guy~
                db.types.apply_at(r_value_type, current_ea)
            print(f"RValue {name} at {hex(current_ea)} processed.")
            current_ea += 16


if __name__ == "__main__":
    main()
