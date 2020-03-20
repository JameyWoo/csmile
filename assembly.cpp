#include "assembly.h"

ofstream assout("Output-assembly.txt", ios::out);
// ! 存储变量在栈中的位置, 用map
map<string, map<string, string>> fun2param;
map<string, map<string, string>> fun2var;
int LFB_id = 0, LFE_id = 0;

void debug(string);  // // debug信息输出的函数
void getParams(TreeNode* params, string func_name);
int genLocVarDecl(TreeNode* vars, string func_name);
void genOutput(TreeNode* output, map<string, string> var2stack);
void genInput(TreeNode* input, string left_loc);
void genStmt(TreeNode* stmts, map<string, string> var2stack);
// need代表是否是递归调用assign, 如果是, 那么需要将右值赋给寄存器%edx
void genAssign(TreeNode* assign, map<string, string> var2stack, bool need);
void genCalc(TreeNode* calc, map<string, string> var2stack);
void genIfElse(TreeNode* ifelse, map<string, string> var2stack);
void genReturn(TreeNode* returnStmt, map<string, string> var2stack);
void genCall(TreeNode* call, map<string, string> var2stack);
void genWhile(TreeNode* iter, map<string, string> var2stack);

int getId();  // 生成一个全局的id
int id = 0;

void genFunc(TreeNode*);  // 处理单个函数
void genAssembly(TreeNode* root);

/** 寄存器约定
 * %eax 用来做返回值
 * 在 output 函数中, %edx用来存储要输出的值
 * 在Assign中, %edx用来保存右边的assign
 */

void debug(string info) {
    cout << info << endl;
}

int getId() {
    return id++;
}

void getParams(TreeNode* params, string func_name) {
    if (params == NULL) return;
    // debug("params->child[0]->nodekind : " + params->child[0]->nodekind);
    if (params->child[0]->nodekind == "void") {
        // debug("param void");
        return;
    }

    vector<string> param_stack;
    map<string, string> var2stack;
    // 接下来是有参数的
    TreeNode* param = params->child[0];
    while (param != NULL) {
        // debug("param: " + param->nodekind);
        // debug(param->child[0]->nodekind + " " + param->child[1]->name);
        param_stack.push_back(param->child[1]->name);
        param = param->sibling;
    }

    // * 给参数分配栈空间
    int bias = 8;
    while (not param_stack.empty()) {
        var2stack[param_stack.front()] = to_string(bias) + "(%ebp)";
        param_stack.erase(param_stack.begin());
        bias += 4;
    }

    fun2param[func_name] = var2stack;
}

int genLocVarDecl(TreeNode* vars, string func_name) {
    if (vars == NULL) return 0;
    // debug(vars->child[0]->name);

    // ! 需要统计有多少个局部变量, 以分配初始的栈空间
    stack<string> var_stack;
    map<string, string> var2stack;
    int var_cnt   = 0;
    TreeNode* var = vars;
    while (var != NULL) {
        var_cnt += 1;
        // debug("var " + var->child[1]->name);
        var_stack.push(var->child[1]->name);
        var = var->sibling;
    }
    // debug("\tvar cnt: " + to_string(var_cnt));

    // * 给局部变量分配栈空间
    int bias = -4;
    while (not var_stack.empty()) {
        var2stack[var_stack.top()] = to_string(bias) + "(%ebp)";
        var_stack.pop();
        bias -= 4;
    }

    fun2var[func_name] = var2stack;
    return var_cnt;
}

void genOutput(TreeNode* output, map<string, string> var2stack) {
    // debug("in output: " + output->child[0]->nodekind);
    // debug("in output value: " + to_string(output->child[0]->val));
    if (output->child[0]->nodekind == "Const") {
        assout << "\t"
               << "movl\t$.LC0, %eax" << endl
               << "\t"
               << "movl"
               << "\t"
               << "$" << to_string(output->child[0]->val)
               << ", 4(%esp)" << endl
               << "\t"
               << "movl"
               << "\t"
               << "%eax, (%esp)"
               << "\n\t"
               << "call\tprintf" << endl;
    } else if (output->child[0]->nodekind == "Id") {
        // debug("id: " + output->child[0]->name);
        string id_loc = var2stack[output->child[0]->name];
        assout << "\tmovl\t" << id_loc << ", %edx" << endl
               << "\t"
               << "movl\t$.LC0, %eax" << endl
               << "\t"
               << "movl"
               << "\t"
               << "%edx"
               << ", 4(%esp)" << endl
               << "\t"
               << "movl"
               << "\t"
               << "%eax, (%esp)"
               << "\n\t"
               << "call\tprintf" << endl;
    } else if (output->child[0]->nodekind == "Call") {
        genCall(output->child[0], var2stack);
        // call 的结果在 %eax中
        assout << "\tmovl\t%eax, 4(%esp)" << endl
               << "\tmovl\t$.LC0, %eax" << endl
               << "\tmovl\t%eax, (%esp)" << endl
               << "\tcall\tprintf" << endl;
    }
}

void genInput(TreeNode* input, string left_loc) {
    debug(input->nodekind);
    assout << "\tmovl\t$.LC1, %eax" << endl
           << "\tleal\t" << left_loc << ", %edx" << endl
           << "\tmovl\t"
           << "%edx, 4(%esp)" << endl
           << "\tmovl\t%eax, (%esp)" << endl
           << "\tcall\t__isoc99_scanf" << endl;
}

void genStmt(TreeNode* stmts, map<string, string> var2stack) {
    if (stmts == NULL) return;
    TreeNode* stmt = stmts;
    while (stmt != NULL) {
        // debug(stmt->nodekind);

        if (stmt->nodekind == "Output") {
            genOutput(stmt, var2stack);
        } else if (stmt->nodekind == "Assign") {
            genAssign(stmt, var2stack, false);
        } else if (stmt->nodekind == "Selection") {
            genIfElse(stmt, var2stack);
        } else if (stmt->nodekind == "ReturnStmt") {
            genReturn(stmt, var2stack);
        } else if (stmt->nodekind == "Call") {
            genCall(stmt, var2stack);
        } else if (stmt->nodekind == "Iteration") {
            genWhile(stmt, var2stack);
        }

        stmt = stmt->sibling;
    }
}

void solveCompare(TreeNode* root, string reg, map<string, string> var2stack) {
    // debug(root->nodekind);
    // TODO: 最好有一个通用处理的函数, 专门计算 exp等
    if (root->nodekind == "Const") {
        assout << "\tmovl\t$" << to_string(root->val) << ", " << reg << endl;
    } else if (root->nodekind == "Id") {
        assout << "\tmovl\t" << var2stack[root->name] << ", " << reg << endl;
    }
}

void genIfElse(TreeNode* ifelse, map<string, string> var2stack) {
    debug("ifelse->child[0]: " + ifelse->child[0]->nodekind);
    // debug("ifelse->child[0]: " + ifelse->child[1]->nodekind);
    // debug("ifelse->child[0]: " + ifelse->child[2]->nodekind);
    // ifelse->child[0] 是条件判断
    // ifelse->child[1] 是 if 的条件成立之后的的stmt
    // ifelse->child[2] 是 else 之后的stmt
    if (ifelse->child[0]->op == "==") {
        debug(" == ");
        TreeNode* left = ifelse->child[0]->child[0];
        solveCompare(left, "%eax", var2stack);
        // 左边的结果放到 %eax, 右边的结果放到 %edx
        TreeNode* right = ifelse->child[0]->child[1];
        solveCompare(right, "%edx", var2stack);
        assout << "\tcmpl\t%edx, %eax" << endl;

        string L1 = ".L" + to_string(getId());
        string L2 = ".L" + to_string(getId());
        assout << "\tjne\t" << L1 << endl;
        genStmt(ifelse->child[1], var2stack);
        assout << "\tjmp\t" << L2 << endl;
        assout << L1 << ":" << endl;
        debug("else: " + ifelse->child[2]->nodekind);
        genStmt(ifelse->child[2], var2stack);
        assout << L2 << ":" << endl;
    }
    // TODO: 其他的比较
}

void genReturn(TreeNode* returnStmt, map<string, string> var2stack) {
    debug("return: " + returnStmt->child[0]->nodekind);
    if (returnStmt->child[0]->nodekind == "Id") {
        assout << "\tmovl\t" << var2stack[returnStmt->child[0]->name] << ", %eax" << endl;
    } else if (returnStmt->child[0]->nodekind == "Const") {
        assout << "\tmovl\t$" << to_string(returnStmt->child[0]->val) << ", %eax" << endl;
    } else if (returnStmt->child[0]->nodekind == "Call") {
        debug("return call: " + returnStmt->child[0]->nodekind);
        genCall(returnStmt->child[0], var2stack);
    }
    // TODO: 除了直接返回Id, 还有其他返回形式. 如 a + b, input() 这种
}

void genCall(TreeNode* call, map<string, string> var2stack) {
    // ! 结果在 %eax中
    if (call == NULL) return;
    debug("call: " + call->child[0]->nodekind);
    // 解析 Call 的参数
    // call->child[0] 是这个函数的名字
    // call->child[1] 如果不为 NULL 的话, 是 Args 类型的
    // 解析args, arg可以是expression, 种类比较多. 考虑是不是要综合一个整体的
    TreeNode* args = call->child[1];
    if (args == NULL) {
        // TODO: call 没有参数的情况
        assout << "\tcall\t" << call->child[0]->name << endl;
        return;
    }
    debug("args: " + args->child[0]->nodekind);
    // debug("args: " + args->child[0]->sibling->nodekind);

    // 传递参数
    stack<TreeNode*> call_stack;
    TreeNode* arg = args->child[0];
    int cnt       = 0;
    while (arg != NULL) {
        cnt += 1;
        call_stack.push(arg);
        arg = arg->sibling;
    }
    // 调整栈的位置, 防止参数过多
    cnt -= 1;
    assout << "\tsubl\t$" << 4 * cnt << ", %esp" << endl;
    while (not call_stack.empty()) {
        string stack_loc = to_string(4 * cnt) + "(%esp)";
        TreeNode* tmp    = call_stack.top();
        debug("arg type: " + tmp->nodekind);
        string left = "";
        if (tmp->nodekind == "Const") {
            left += "$" + to_string(tmp->val);
        } else if (tmp->nodekind == "Id") {
            assout << "\tmovl\t" << var2stack[tmp->name] << ", %edx" << endl;
            left += "%edx";
        } else if (tmp->nodekind == "PMOp" || tmp->nodekind == "MDOp") {
            // 一个计算的表达式
            genCalc(tmp, var2stack);
            left += "%edx";
        }
        assout << "\tmovl\t" << left << ", " << stack_loc << endl;
        // TODO: 支持更多的参数类型

        call_stack.pop();
        cnt -= 1;
    }
    // 调用函数
    assout << "\tcall\t" << call->child[0]->name << endl;
}

void genWhile(TreeNode* iter, map<string, string> var2stack) {
    // 循环
    string L1 = ".L" + to_string(getId());
    string L2 = ".L" + to_string(getId());

    if (iter->child[0]->op == "==") {
        // debug(" == ");
        assout << L1 << ":" << endl;
        TreeNode* left = iter->child[0]->child[0];
        solveCompare(left, "%eax", var2stack);
        // 左边的结果放到 %eax, 右边的结果放到 %edx
        TreeNode* right = iter->child[0]->child[1];
        solveCompare(right, "%edx", var2stack);
        assout << "\tcmpl\t%edx, %eax" << endl;
        assout << "\tjne\t" << L2 << endl;
        genStmt(iter->child[1], var2stack);
        assout << "\tjmp\t" << L1 << endl;

        assout << L2 << ":" << endl;
    } else if (iter->child[0]->op == "<") {
        assout << L1 << ":" << endl;
        // 左边的结果放到 %eax, 右边的结果放到 %edx
        TreeNode* left = iter->child[0]->child[0];
        TreeNode* right = iter->child[0]->child[1];
        solveCompare(left, "%eax",var2stack);
        solveCompare(right, "%edx", var2stack);
        assout << "\tcmpl\t %edx, %eax" << endl;
        assout << "\tjge\t" << L2 << endl;
        
        genStmt(iter->child[1], var2stack);
        assout << "\tjmp\t" << L1 << endl;

        assout << L2 << ":" << endl;
    } else if (iter->child[0]->op == ">") {
        assout << L1 << ":" << endl;
        // 左边的结果放到 %eax, 右边的结果放到 %edx
        TreeNode* left = iter->child[0]->child[0];
        TreeNode* right = iter->child[0]->child[1];
        solveCompare(left, "%eax",var2stack);
        solveCompare(right, "%edx", var2stack);
        assout << "\tcmpl\t %edx, %eax" << endl;
        assout << "\tjle\t" << L2 << endl;
        
        genStmt(iter->child[1], var2stack);
        assout << "\tjmp\t" << L1 << endl;

        assout << L2 << ":" << endl;
    }
    // TODO: 其他的比较
}

void genAssign(TreeNode* assign, map<string, string> var2stack, bool need) {
    // debug("assign " + assign->child[0]->name);
    string left_loc = var2stack[assign->child[0]->name];
    if (assign->child[1]->nodekind == "Const") {
        assout << "\tmovl\t$"
               << to_string(assign->child[1]->val)
               << ", " << left_loc << endl;
    } else if (assign->child[1]->nodekind == "Id") {
        // debug("assign->child[1]->name: " + assign->child[1]->name);
        string right_loc = var2stack[assign->child[1]->name];
        // 先将该值赋值给某个寄存器, 然后再将寄存器赋值给左值
        assout << "\tmovl\t" << right_loc << ", %edx" << endl
               << "\tmovl\t%edx, " << left_loc << endl;
    } else if (assign->child[1]->nodekind == "Assign") {
        genAssign(assign->child[1], var2stack, true);
        assout << "\tmovl\t%edx, " << left_loc << endl;
    } else if (assign->child[1]->nodekind == "PMOp" || assign->child[1]->nodekind == "MDOp") {
        // debug("assign->child[1]->nodekind: " + assign->child[1]->nodekind);
        // 计算(加减乘除)
        // debug("assign->child[1]->nodekind: " + assign->child[1]->nodekind);
        genCalc(assign->child[1], var2stack);
        assout << "\tmovl\t%edx, " << left_loc << endl;
    } else if (assign->child[1]->nodekind == "Input") {
        genInput(assign->child[1], left_loc);  // 第二个参数是赋值的地址
    } else if (assign->child[1]->nodekind == "Call") {
        // call 有一种是在赋值中的, 有一种是普通的没有被赋值的调用
        genCall(assign->child[1], var2stack);
        // 返回值
        assout << "\tmovl\t%eax, " << left_loc << endl;
    }
    if (need) {
        assout << "\tmovl\t" << left_loc << ", %edx" << endl;
    }
}

void genCalc(TreeNode* calc, map<string, string> var2stack) {
    // TODO: 出了一个bug, 要注意, 相同优先级是要先左后右来计算的
    // TODO: 有 bug, 多重计算的时候, 寄存器会混淆
    // * 将计算结果保存到 %edx 中
    // * 递归地先将左值计算, 存储到%edx中, 将右值计算, 存储到%ecx中. %eax做暂时的中介.

    // debug("calc->nodekind: " + calc->nodekind);
    // debug("calc->op: " + calc->op);
    // 计算
    debug("calc->child[1]->nodekind: " + calc->child[1]->nodekind);

    string left_loc;
    string right_loc;
    // 是否有左值也是表达式的情况?
    // 将左值放入 %eax
    if (calc->child[0]->nodekind == "MDOp" || calc->child[0]->nodekind == "PMOp") {
        genCalc(calc->child[0], var2stack);
        assout << "\tmovl\t%edx, %eax" << endl;
    } else if (calc->child[0]->nodekind == "Id") {
        left_loc = var2stack[calc->child[0]->name];
        assout << "\tmovl\t" << left_loc << ", %eax" << endl;
    } else if (calc->child[0]->nodekind == "Const") {
        left_loc = "$" + to_string(calc->child[1]->val);
        assout << "\tmovl\t" << left_loc << ", %eax" << endl;
    }
    left_loc = "%eax";

    // 如果是计算结点, 那么递归地调用, 得到的数放入寄存器 %ecx
    // 如果是 Id, 那么得到栈地址
    // 如果是 Const, 那么生成 $num
    if (calc->child[1]->nodekind == "MDOp" || calc->child[1]->nodekind == "PMOp") {
        genCalc(calc->child[1], var2stack);
        assout << "\tmovl\t%edx, %ecx" << endl;
        right_loc = "%ecx";
    } else if (calc->child[1]->nodekind == "Id") {
        right_loc = var2stack[calc->child[1]->name];
    } else if (calc->child[1]->nodekind == "Const") {
        right_loc = "$" + to_string(calc->child[1]->val);
    }
    if (calc->op == "/") {
        assout << "\tmovl\t" << left_loc << ", %eax" << endl
               << "\tmovl\t%eax, %edx" << endl
               << "\tsarl\t$31, %edx" << endl
               << "\tidivl\t" << right_loc << endl
               << "\tmovl\t%eax, %edx" << endl;
    } else {
        assout << "\tmovl\t" << left_loc << ", %edx" << endl;

        if (calc->op == "+") {
            assout << "\taddl\t" << right_loc << ", %edx" << endl;
        } else if (calc->op == "-") {
            assout << "\tsubl\t" << right_loc << ", %edx" << endl;
        } else if (calc->op == "*") {
            assout << "\timull\t" << right_loc << ", %edx" << endl;
        }
    }
}

void genFunc(TreeNode* func) {
    // ! 函数需要分析参数的栈情况, 写入符号表. 因此这个符号表非常重要.
    string func_name = func->child[1]->name;
    // debug("func name: " + func_name);
    // debug(func->child[2]->nodekind);  // 函数的参数
    // debug(func->child[3]->nodekind);  // 函数的内容

    assout << "\t.global\t" << func->child[1]->name << endl
           << "\t.type\t" << func->child[1]->name << ", @function" << endl
           << func->child[1]->name << ":" << endl
           << ".LFB" + to_string(LFB_id) << ":" << endl
           << "\t"
           << "pushl"
           << "\t"
           << "%ebp\n"
           << "\tmovl\t%esp, %ebp" << endl;

    int stack_size = 16;

    // 对参数生成对应的符号表
    getParams(func->child[2], func->child[1]->name);
    TreeNode* statement = NULL;

    // debug(func->child[3]->child[0]->nodekind);
    // 在 func->child[3] 中, 如果有临时变量声明, 那么是它的child[0], 否则child[0]是正式的内容, 所以要判断
    if (func->child[3]->child[0]->nodekind == "LocVarDecl") {
        // ! 局部变量声明, 更新符号表. 这个时候要保存每个变量在栈中的位置
        int ext = genLocVarDecl(func->child[3]->child[0], func->child[1]->name);
        stack_size += ext;
        statement = func->child[3]->child[1];

        // genStmt(func->child[3]->child[1], var2stack);          // 生成statement
    } else if (func->child[3]->child[0] != NULL) {  // 如果没有变量声明
        // // debug("before statement");
        statement = func->child[3]->child[0];
    }
    assout << "\t"
           << "subl"
           << "\t$" << stack_size << ", %esp" << endl;
    map<string, string> var2stack;
    for (auto x : fun2param[func_name]) {
        var2stack[x.first] = x.second;
    }
    for (auto x : fun2var[func_name]) {
        if (var2stack.count(x.first)) {
            cout << "error ! param & loc_var repeat!" << endl;
        } else {
            var2stack[x.first] = x.second;
        }
    }

    // ! 这个是最麻烦的
    genStmt(statement, var2stack);

    // // debug(func->child[3]->child[0]->nodekind);
    // // debug(func->child[3]->child[1]->nodekind);

    assout << "\t"
           << "leave" << endl
           << "\t"
           << "ret" << endl;

    assout << ".LFE" + to_string(LFE_id) + ":" << endl
           << "\t.size\t" + func->child[1]->name << ", .-" << func->child[1]->name
           << endl;
}

void genAssembly(TreeNode* root) {
    // 汇编代码的头部
    assout << "\t.section\t.rodata" << endl
           << ".LC0:" << endl
           << "\t"
           << ".string \"%d\\n\"" << endl
           << ".LC1:" << endl
           << "\t"
           << ".string \"%d\"" << endl
           << "\t"
           << ".text" << endl;

    // 函数部分, 遍历每个 function, 单独处理
    TreeNode* func = root;
    while (func != NULL) {
        genFunc(func);
        func = func->sibling;
        LFB_id++;
        LFE_id++;
    }

    assout << "\t.ident	\"GCC: (Ubuntu/Linaro 4.6.3-1ubuntu5) 4.6.3\"" << endl
           << "\t.section"
           << "\t"
           << ".note.GNU-stack,\"\",@progbits " << endl;

    cout << "the end..." << endl;
}
