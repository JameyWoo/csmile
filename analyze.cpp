#include "analyze.h"

ofstream symtabOut("Output-symtab.txt", ios::out);

int isError   = 0;
int typeError = 0;
vector<string> fun_names;
static int location = 0;
map<string, string> fun_name2ret_type;

static void traverse(TreeNode *t, void (*preProc)(TreeNode *), void (*postProc)(TreeNode *)) {
    if (t != NULL) {
        preProc(t);
        for (int i = 0; i < MAX_CHILDREN; ++i) {
            traverse(t->child[i], preProc, postProc);
        }
        postProc(t);
        traverse(t->sibling, preProc, postProc);
    }
}

/**占位的吧这... */
static void nullProc(TreeNode *t) {
    if (t == NULL)
        return;
    else
        return;
}

static void insertNode(TreeNode *t) {
    if (t->nodekind == "FunDecl") {
        // 新的函数, 新的作用域
        newSymtabs();
        fun_names.push_back(t->child[1]->name);
        if (t->child[0]->nodekind == "int") {
            t->type                              = "Integer";
            fun_name2ret_type[t->child[1]->name] = "Integer";
        } else if (t->child[0]->nodekind == "void") {
            t->type                              = "Void";
            fun_name2ret_type[t->child[1]->name] = "Void";
        }
    }
    // 调用类型. 需要查符号表来赋值
    if (t->nodekind == "Call") {
        // t->nodekind =
        t->type = fun_name2ret_type[t->child[0]->name];
    }
    if (t->nodekind == "LocVarDecl") {  // 局部变量声明
        // 声明地时候只在符号表中添加, 检测到Id地时候才添加行号
        if (st_lookup(t->child[1]->name) == -1) {  // 符号表里没有
            st_insert(t->child[1]->name, -1, location++, 2);
        } else {
            // 符号表里有
            error("line " + to_string(t->child[1]->line) + " : 局部变量声明错误...");
            isError++;
        }
    } else if (t->nodekind == "ParamId") {
        // 作为参数地Id
        if (st_lookup(t->name) == -1) {  // 符号表里没有
            st_insert(t->name, t->line, location++, 1);
        } else {
            // 符号表里有
            error("line " + to_string(t->line) + " : 参数变量声明错误...");
            isError++;
        }
    } else if (t->nodekind == "Id") {
        if (st_lookup(t->name) == -1) {  // 符号表里没有
            st_insert(t->name, t->line, location++, 3);
        } else {
            // 符号表里有
            st_insert(t->name, t->line, -1, 3);
        }
    }
}

static void checkNode(TreeNode *t) {
    // symtabOut << t->nodekind << endl;
    // 添加对函数类型的生成
    // 由于是后序遍历, 函数的类型是最后检查的...
    if (t->nodekind == "FunDecl") {
        // error("func...");
        if (t->child[0]->nodekind == "int")
            t->type = "Integer";
        else if (t->child[0]->nodekind == "void")
            t->type = "Void";
    } else if (t->nodekind == "Id") {
        t->type = "Integer";
    } else if (t->nodekind == "CompareOp") {
        t->type = "Boolean";
    } else if (t->nodekind == "Const") {
        t->type = "Integer";
    } else if (t->nodekind == "MDOp") {
        if (t->child[0]->type == "Integer" && t->child[1]->type == "Integer") {
            t->type = "Integer";
        } else {
            typeError++;
            error("乘除类型错误");
        }
    } else if (t->nodekind == "PMOp") {
        if (t->child[0]->type == "Integer" && t->child[1]->type == "Integer") {
            t->type = "Integer";
        } else {
            typeError++;
            error("加减类型错误");
        }
    } else if (t->nodekind == "Input") {
        t->type = "Integer";
    }

    // 语句的类型判断
    if (t->nodekind == "Selection") {
        // if 选择
        if (t->child[0]->type != "Boolean") {
            typeError++;
            error("if 类型错误");
        } else {
            // symtabOut << "match" << endl;
        }
    } else if (t->nodekind == "Iteration") {
        // while 迭代
        if (t->child[0]->type != "Boolean") {
            typeError++;
            error("while 类型错误");
        }
    } else if (t->nodekind == "Assign") {
        if (t->child[1]->type != "Integer") {
            error("t->child[1]->nodekind" + t->child[1]->nodekind);
            if (t->child[1]->nodekind != "FunDecl") {
                typeError++;
                error("line " + to_string(t->line) + ": 赋值类型错误");
            }
        } else {
            t->type = "Integer";
        }
    }
    // symtabOut << "t->type = " << t->type << endl;
}

// 建立符号表
void buildSymtab(TreeNode *syntaxTree) {
    traverse(syntaxTree, insertNode, nullProc);
    if (not isError) {
        cout << "\nprinting symtab ..." << endl;
        printSymtab();
        cout << "print over !\n"
                  << endl;
        cout << "symtab build over, everything is ok! congratulations!" << endl
                  << endl;
    } else {
        cout << "you have " << isError << " errors in symtab, please check!" << endl
                  << endl;
    }
}

// 类型检查
void typeCheck(TreeNode *syntaxTree) {
    traverse(syntaxTree, nullProc, checkNode);
    if (typeError) {
        cout << "type check over, your have " << typeError << " errors, please check!" << endl
             << endl;
    } else {
        cout << "type check over, everything is ok! congratulations!" << endl
             << endl;
    }
}