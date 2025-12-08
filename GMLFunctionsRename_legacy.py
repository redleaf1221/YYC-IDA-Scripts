import idaapi
import idautils
import idc

def find_and_rename_functions(target_func_ea):
    """
    查找所有对目标函数的调用，并根据字符串参数重命名对应的函数
    """
    if target_func_ea == idaapi.BADADDR:
        print("错误: 无效地址")
        return
    
    print(f"开始分析对 0x{target_func_ea:X} 的调用...")
    
    ref_count = 0
    renamed_count = 0
    
    for xref in idautils.CodeRefsTo(target_func_ea, 0):
        # 检查是否为call指令
        if idc.print_insn_mnem(xref).lower() not in ['call', 'jmp']:
            continue
            
        ref_count += 1
        
        # 向前回溯查找参数设置
        string_addr = None
        func_addr = None
        string = ""
        
        # 从call指令开始向前查找
        ea = xref
        
        for i in range(5):
            ea = idc.prev_head(ea)
            if ea == idaapi.BADADDR:
                break

            insn = idaapi.insn_t()
            if idaapi.decode_insn(insn, ea):
                mnem = idc.print_insn_mnem(ea).lower()
                
                if mnem == 'lea':
                    op1 = idc.print_operand(ea, 0)
                    op2 = insn.ops[1]
                    
                    if 'rcx' in op1.lower():
                        if op2.type in [idaapi.o_mem, idaapi.o_displ, idaapi.o_imm]:
                            pointed_addr = idc.get_operand_value(ea, 1)
                            pointed_str = idc.get_strlit_contents(pointed_addr)
                            if pointed_str:
                                pointed_str_utf8 = pointed_str.decode("utf-8")
                                if pointed_str_utf8:
                                    string = pointed_str_utf8
                    elif 'rdx' in op1.lower() or 'edx' in op1.lower():
                        if op2.type in [idaapi.o_mem, idaapi.o_displ, idaapi.o_imm]:
                            func_addr_t = idc.get_operand_value(ea, 1)
                            if func_addr_t and func_addr_t != idaapi.BADADDR:
                                func = idaapi.get_func(func_addr_t)
                                if func:
                                    func_addr = func_addr_t
            
            # 如果已经找到了两个参数，就停止搜索
            if string and func_addr and func_addr != 0:
                break
        
        # 如果找到了字符串和函数地址，进行重命名
        if string and func_addr and func_addr != 0:
            new_name = string.strip()

            print(f"sub_{func_addr:X} 重命名为 {new_name}")
            
            # 重命名函数
            if idc.set_name(func_addr, new_name, idc.SN_NOWARN):
                renamed_count += 1
        
    
    print(f"\n分析完成:")
    print(f"  找到 {ref_count} 个调用")
    print(f"  成功重命名 {renamed_count} 个函数")

# 主执行部分
if __name__ == "__main__":
    # 获取当前光标位置的函数
    ea = idc.get_screen_ea()
    
    # 尝试获取函数起始地址
    func = idaapi.get_func(ea)
    if func:
        target_ea = func.start_ea
        func_name = idc.get_name(target_ea)
        print(f"分析函数: {func_name} (0x{target_ea:X})")
        find_and_rename_functions(target_ea)
    else:
        print("错误: 光标不在函数内")