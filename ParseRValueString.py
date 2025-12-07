import ida_hexrays
from ida_domain import Database
from ida_idaapi import ea_t


class find_call_visitor_t(ida_hexrays.ctree_visitor_t):
    def __init__(self, target_ea, **kwargs):
        ida_hexrays.ctree_visitor_t.__init__(self, ida_hexrays.CV_FAST)
        self.target_ea = target_ea
        self.arg1_addr = 0
        self.arg2_addr = 0
        self.found = False

    def visit_expr(self, expr):
        if expr.op == ida_hexrays.cot_call and expr.ea == self.target_ea:
            self.found = True
            arg: ida_hexrays.carg_t
            if expr.a.size() >= 2:
                if expr.a[0].ea == 0xFFFFFFFFFFFFFFFF:
                    return True
                it: ida_hexrays.cexpr_t = expr.a[0]
                while it.obj_ea == 0xFFFFFFFFFFFFFFFF:
                    it = it.x
                self.arg1_addr = it.obj_ea
                it: ida_hexrays.cexpr_t = expr.a[1]
                while it.obj_ea == 0xFFFFFFFFFFFFFFFF:
                    it = it.x
                self.arg2_addr = it.obj_ea
            return True
        return False


def fetch_args(db: Database, call_ea: ea_t, target_ea: ea_t) -> list:
    c_func = ida_hexrays.decompile_func(db.functions.get_at(call_ea))
    visitor = find_call_visitor_t(call_ea)
    visitor.apply_to(c_func.body, None)
    return [visitor.arg1_addr, visitor.arg2_addr]


def main():
    with Database.open() as db:
        print("Parsing RValue const strings...")
        print(f"Current database: {db.path}")
        r_value_type = db.types.get_by_name("RValue")
        if not r_value_type:
            print("Type not found, run the IDC first.")
            return
        func = db.functions.get_function_by_name("?YYConstString@@YAXPEAURValue@@PEBD@Z")
        if not func:
            print("YYConstString not found, apply the signature first.")
            return
        for call in db.xrefs.to_ea(func.start_ea):
            arg_val = fetch_args(db, call.from_ea, call.to_ea)
            if arg_val[0] == 0 or arg_val[1] == 0:
                continue
            value_ea = arg_val[0]
            const_string = db.bytes.get_cstring_at(arg_val[1])
            db.types.apply_at(r_value_type, value_ea)
            db.names.set_name(value_ea, f"rv_string_v{const_string}_p{hex(value_ea)[2:]}")
            db.comments.set_at(value_ea, const_string)
            print(f"call from {hex(call.from_ea)}, RValue{hex(value_ea)} -> {const_string}")


if __name__ == "__main__":
    main()
