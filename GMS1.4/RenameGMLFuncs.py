from ida_domain import Database
from ida_typeinf import tinfo_t


def check_is_YYGMLFuncs(db: Database, ea: int) -> bool:
    check1 = (db.is_valid_ea(db.bytes.get_dword_at(ea)) and
              db.is_valid_ea(db.bytes.get_dword_at(ea + 4)))
    if not check1:
        return False
    return db.functions.get_at(db.bytes.get_dword_at(ea + 4)) is not None


def is_global_func(demangled: str) -> bool:
    return demangled.startswith("gml_GlobalScript_") or demangled.startswith("gml_Script_")


def name_gml_func(demangled: str) -> str:
    if is_global_func(demangled):
        return f"?{demangled.replace('@', '_')}@@YAAEAUYYRValue@@PEAVCInstance@@0AEAU1@HPEAPEAU1@@Z"
    else:
        return f"?{demangled.replace('@', '_')}@@YAXPEAVCInstance@@0@Z"


def main():
    with Database.open() as db:
        print("Renaming GML Functions...")
        print(f"Current database: {db.path}")
        yyvar_type = db.types.get_by_name("YYVAR")
        if not yyvar_type or not db.types.get_by_name("YYRValue") or not db.types.get_by_name("YYRValue"):
            print("Type not found, run the IDC first.")
            return
        current_ea = db.current_ea
        if not db.is_valid_ea(current_ea):
            print("ea not valid.")
            return

        temp_global_func_type = tinfo_t("YYRValue& temp_func"
                                        " (CInstance *pSelf, CInstance *pOther,"
                                        " YYRValue& _result, int _count, YYRValue** _args);")
        temp_object_func_type = tinfo_t("void temp_func(CInstance *pSelf, CInstance *pOther);")
        print(f"Interpreting array at {hex(current_ea)} as g_GMLFuncs...")
        db.names.set_name(current_ea, "?g_GMLFuncs@@3PAUYYGMLFuncs@@A")
        while check_is_YYGMLFuncs(db, current_ea):
            name = db.bytes.get_cstring_at(db.bytes.get_dword_at(current_ea))
            func = db.functions.get_at(db.bytes.get_dword_at(current_ea + 4))

            func_type = temp_global_func_type if is_global_func(name) else temp_object_func_type
            db.types.apply_at(func_type, func.start_ea)
            db.functions.set_name(func, name_gml_func(name))

            print(f"GMLFunc {name} at {hex(current_ea)} processed.")
            current_ea += 8


if __name__ == "__main__":
    main()
