from ida_domain import Database


def name_func_var(name: str) -> str:
    return f"?g_FUNC_{name.replace('@','_')}@@3UYYVAR@@A "


def main():
    with Database.open() as db:
        print("Recovering g_Funcs...")
        print(f"Current database: {db.path}")
        yyvar_type = db.types.get_by_name("YYVAR")
        if not yyvar_type:
            print("YYVAR type not found, run the IDC first.")
            return
        current_ea = db.current_ea
        if not db.is_valid_ea(current_ea):
            print("ea not valid.")
            return

        print(f"Interpreting array at {hex(current_ea)} as g_Funcs...")
        db.names.set_name(current_ea, "?g_Funcs@@3PAPEAUYYVAR@@A")
        while (var_ea := db.bytes.get_qword_at(current_ea)) != 0:
            var_name = name_func_var(db.bytes.get_cstring_at(db.bytes.get_qword_at(var_ea)))
            db.types.apply_at(yyvar_type, var_ea)
            db.names.set_name(var_ea, var_name)
            print(f"Func {var_name} at {hex(current_ea)} processed.")
            current_ea += 8


if __name__ == "__main__":
    main()
