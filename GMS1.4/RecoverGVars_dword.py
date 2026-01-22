import idc
from ida_domain import Database


def name_var(name: str) -> str:
    return f"?g_VAR_{name.replace('@','_')}@@3UYYVAR@@A "


def name_VARID_enum(name: str) -> str:
    return f"kVARID_{name}"


def main():
    with Database.open() as db:
        print("Recovering g_Vars...")
        print(f"Current database: {db.path}")
        yyvar_type = db.types.get_by_name("YYVARVar")
        VARIDs_type = db.types.get_by_name("VARIDs")
        if not yyvar_type or not VARIDs_type:
            print("Type not found, run the IDC first.")
            return
        current_ea = db.current_ea
        if not db.is_valid_ea(current_ea):
            print("ea not valid.")
            return

        var_name_id = {}
        print(f"Interpreting array at {hex(current_ea)} as g_Vars...")
        db.names.set_name(current_ea, "?g_Vars@@3PAPEAUYYVAR@@A")
        while (var_ea := db.bytes.get_dword_at(current_ea)) != 0:
            var_name = db.bytes.get_cstring_at(db.bytes.get_dword_at(var_ea))
            var_id = db.bytes.get_dword_at(var_ea + 4)
            var_name_mangled = name_var(var_name)
            db.types.apply_at(yyvar_type, var_ea)
            db.names.set_name(var_ea, var_name_mangled)
            if var_id != 0xFFFFFFFF:
                var_name_id[var_name] = var_id
            print(f"Var {var_name} (id:{var_id}) at {hex(current_ea)} processed.")
            current_ea += 4

        VARIDs_type_id = VARIDs_type.get_tid()
        for var_name, var_id in var_name_id.items():
            idc.add_enum_member(VARIDs_type_id, name_VARID_enum(var_name), var_id)
        idc.add_enum_member(VARIDs_type_id, "internal_var", 0xFFFFFFFF)


if __name__ == "__main__":
    main()
