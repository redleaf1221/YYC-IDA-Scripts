from ida_domain import Database
from ida_domain.types import TypeApplyFlags


def main():
    with Database.open() as db:
        print("Recovering g_Vars...")
        print(f"Current database: {db.path}")
        temp_type = db.types.get_by_name("YYRValue *__fastcall gml_GlobalScript_check_board_valid(CInstance *pSelf, CInstance *pOther, YYRValue *_result, __int64 _count);")
        print(f"temp_type: {temp_type}")


if __name__ == "__main__":
    main()
