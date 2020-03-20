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

void getParams(TreeNode* params, string func_name) {
    if (params == NULL) return;
    // debug("params->child[0]->nodekind : " + params->child[0]->nodekind);
    if (params->child[0]->nodekind == "void") {
        // debug("param void");
        return;
    }

    stack<string> param_stack;
    map<string, string> var2stack;
    // 接下来是有参数的
    TreeNode* param = params->child[0];
    while (param != NULL) {
        // debug("param: " + param->nodekind);
        // debug(param->child[0]->nodekind + " " + param->child[1]->name);
        param_stack.push(param->child[1]->name);
        param = param->sibling;
    }

    // * 给参数分配栈空间
    int bias = 8;
    while (not param_stack.empty()) {
        var2stack[param_stack.top()] = to_string(bias) + "(%ebp)";
        param_stack.pop();
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
        }

        stmt = stmt->sibling;
    }
}

void genIfElse(TreeNode* ifelse, map<string, string> var2stack) {
    debug("ifelse->child[0]: " + ifelse->child[0]->nodekind);
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
    }
    if (need) {
        assout << "\tmovl\t" << left_loc << ", %edx" << endl;
    }
}

void genCalc(TreeNode* calc, map<string, string> var2stack) {
    // debug("calc->nodekind: " + calc->nodekind);
    // debug("calc->op: " + calc->op);
    // 计算
    // debug("calc->child[1]->nodekind: " + calc->child[1]->nodekind);

    string left_loc  = var2stack[calc->child[0]->name];
    string right_loc = var2stack[calc->child[1]->name];
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
