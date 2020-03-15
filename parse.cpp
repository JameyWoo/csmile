#include "parse.h"
#include "scan.h"

ofstream out("Output-parseTree.txt", ios::out);

void error() {
    error_cnt += 1;
    cout << "ErrorToken >> " << ptoken.type << ": " << ptoken.value << endl;
}

Token match(string expected) {
    Token tmp = ptoken;
    // cout << "ptoken.type = " << ptoken.type << " ptoken.val = " << ptoken.value << endl;
    // cout << "expected: " << expected << endl;
    // 匹配一个类型
    if (ptoken.type == expected) {
        // cout << "match: " << ptoken.value << endl;
        if (ptoken.type != "EndFile") ptoken = getToken();
    } else {
        error();
    }
    return tmp;
}

TreeNode* param(TreeNode* k) {
    TreeNode* t = new TreeNode("Param");
    TreeNode* p = NULL;
    TreeNode* q = NULL;
    if (k == NULL && ptoken.type == "void") {
        p = new TreeNode("void");
        match("void");
    } else if (k == NULL && ptoken.type == "int") {
        p = new TreeNode("int");
        match("int");
    } else if (k != NULL) {
        p = k;
    }
    if (p != NULL) {
        t->child[0] = p;
        if (ptoken.type == "Id") {
            q            = new TreeNode("ParamId");
            q->name      = ptoken.value;
            q->line      = ptoken.line;
            t->child[1]  = q;
            t->child_cnt = 2;
            match("Id");
        }
        // TODO: 处理数组传参
    }
    return t;
}

TreeNode* param_list(TreeNode* k) {
    // TODO: 到这里暂停, 明天再写. 完善函数的操作
    TreeNode* t = param(k);  // 先读一个参数
    TreeNode* p = t;
    k           = NULL;
    while (ptoken.type == "Comma") {  // 如果读到逗号, 说明还有下一个参数
        match("Comma");
        TreeNode* q = param(k);
        if (q != NULL) {
            if (t == NULL) {  // ! 是否多余
                t = p = q;
            } else {
                p->sibling = q;
                p          = q;
            }
        }
    }
    return t;
}

TreeNode* params() {
    TreeNode* t = new TreeNode("Params");  // 根节点, 参数s
    TreeNode* p = NULL;
    if (ptoken.type == "void") {
        p = new TreeNode("void");
        match("void");
        if (ptoken.type == "RightBracket") {
            if (t != NULL) {
                t->child[0]  = p;
                t->child_cnt = 1;
            }
        } else {
            t->child[0]  = param_list(p);
            t->child_cnt = 1;
        }
    } else {
        t->child[0]  = param_list(p);
        t->child_cnt = 1;
    }
    return t;
}

TreeNode* local_declaration() {
    /**
     * 一系列的局部声明
     */
    TreeNode* t = NULL;
    TreeNode* p = NULL;
    TreeNode* q = NULL;
    while (ptoken.type == "int") {
        p            = new TreeNode("LocVarDecl");  // 局部变量声明, 只有int类型
        TreeNode* q1 = new TreeNode("int");
        p->child[0]  = q1;
        match("int");
        if (ptoken.type == "Id") {
            TreeNode* q2 = new TreeNode("Id");
            q2->name     = ptoken.value;
            q2->line     = ptoken.line;
            p->child[1]  = q2;
            match("Id");
            // TODO: 添加数组
            if (ptoken.type == "Semicolon") {
                match("Semicolon");
                p->child_cnt = 2;
            } else if (ptoken.type == "Assign") {
                match("Assign");
                Token tmp = match("Num");
                match("Semicolon");
                int val      = stoi(tmp.value);
                q2->val      = val;
                p->child_cnt = 2;
            } else {
                match("Semicolon");
            }
        }
        if (p != NULL) {
            if (t == NULL) {
                t = q = p;
            } else {
                q->sibling = p;
                q          = p;
            }
        }
    }
    return t;
}

TreeNode* selection_stmt() {
    TreeNode* t = new TreeNode("Selection");
    match("if");
    match("LeftBracket");
    if (t != NULL) {
        t->child[0] = expression();
    }
    match("RightBracket");
    bool has_bracket = false;
    if (ptoken.type == "LeftBigBkt") {
        has_bracket = true;
        match("LeftBigBkt");
    }
    t->child[1] = statement_list();
    if (has_bracket)
        match("RightBigBkt");
    if (ptoken.type == "else") {
        match("else");  // else 部分, 存在child[2]里
        // cout << "else 的开始部分*******************" << endl;
        if (t != NULL) {
            bool has_bracket_2 = false;
            if (ptoken.type == "LeftBigBkt") {
                // cout << "else 有括号***********************" << endl;
                has_bracket_2 = true;
                match("LeftBigBkt");
            } else {
                // cout << "else 无括号***********************" << endl;
            }
            t->child[2] = statement_list();
            if (has_bracket_2)
                match("RightBigBkt");
        }
    }
    return t;
}

TreeNode* iteration_stmt() {
    TreeNode* t = new TreeNode("Iteration");
    match("while");
    match("LeftBracket");
    if (t != NULL) {
        t->child[0] = expression();
    }
    match("RightBracket");
    match("LeftBigBkt");
    if (t != NULL) {
        t->child[1] = statement_list();
    }
    match("RightBigBkt");
    return t;
}

TreeNode* var() {
    TreeNode* t = NULL;
    TreeNode* p = NULL;
    if (ptoken.type == "Id") {
        p       = new TreeNode("Id");
        p->name = ptoken.value;
        p->line = ptoken.line;
        match("Id");
        // TODO: 添加数组
        t = p;  // 非数组时
    }
    return t;
}

TreeNode* args() {
    TreeNode* t = new TreeNode("Args");
    TreeNode *s = NULL, *p = NULL;
    if (ptoken.type != "RightBracket") {
        s = expression();
        p = s;
        while (ptoken.type == "Comma") {
            TreeNode* q;
            match("Comma");
            q = expression();
            if (q != NULL) {
                if (s == NULL) {
                    s = p = q;
                } else {
                    p->sibling = q;
                    p          = q;
                }
            }
        }
    }
    if (s != NULL) {
        t->child[0] = s;
    }
    return t;
}

TreeNode* call(TreeNode* k) {
    TreeNode* t = new TreeNode("Call");
    if (k != NULL) {
        t->child[0] = k;
    }
    match("LeftBracket");
    if (ptoken.type == "RightBracket") {
        match("RightBracket");
        return t;
    } else if (k != NULL) {
        t->child[1] = args();
        match("RightBracket");
    }
    return t;
}

TreeNode* factor(TreeNode* k) {
    TreeNode* t = NULL;
    if (k != NULL) {
        if (ptoken.type == "LeftBracket" && k->nodekind != "ArrayElem") {  // TODO: 数组元素
            t = call(k);
        } else {
            t = k;
        }
    } else {
        if (ptoken.type == "LeftBracket") {
            match("LeftBracket");
            t = expression();
            match("RightBracket");
        } else if (ptoken.type == "Id") {
            k = var();
            if (ptoken.type == "LeftBracket" && k->nodekind != "ArrayElem") {
                t = call(k);
            } else {
                t = k;
            }
        } else if (ptoken.type == "Num") {
            t = new TreeNode("Const");
            if ((t != NULL) && (ptoken.type == "Num")) {
                t->val = stoi(ptoken.value);
            }
            match("Num");
        } else {
            ptoken = getToken();
        }
    }
    return t;
}

TreeNode* term(TreeNode* k) {
    TreeNode* t = factor(k);
    k           = NULL;
    while (ptoken.type == "MDOP") {
        TreeNode* q = new TreeNode("MDOp");
        q->op       = ptoken.value;
        q->child[0] = t;
        match(ptoken.type);
        q->child[1] = term(k);
        t           = q;
    }
    return t;
}

TreeNode* additive_expression(TreeNode* k) {
    TreeNode* t = term(k);
    k           = NULL;
    while (ptoken.type == "PMOP") {  // 加减
        TreeNode* q = new TreeNode("PMOp");
        q->op       = ptoken.value;
        q->child[0] = t;
        match("PMOP");
        q->child[1] = term(k);
        t           = q;
    }
    return t;
}

TreeNode* simple_expression(TreeNode* k) {
    // 这里会处理只有一个整数的情况
    TreeNode* t = additive_expression(k);
    k           = NULL;
    if (ptoken.type == "CompareOp") {  // 比较
        TreeNode* q = new TreeNode("CompareOp");
        q->line     = ptoken.line;
        q->op       = ptoken.value;
        q->child[0] = t;
        t           = q;
        match(ptoken.type);
        t->child[1] = additive_expression(k);
        return t;
    }
    return t;
}

TreeNode* expression() {
    // cout << "expression token: " << ptoken.type << endl;
    if (ptoken.type == "input") {  // 如果是输入
        TreeNode* input = new TreeNode("Input");
        match("input");
        match("LeftBracket");
        match("RightBracket");
        return input;
    } else if (ptoken.type == "output") {
        TreeNode* output = new TreeNode("Output");
        match("output");
        match("LeftBracket");
        if (output != NULL) {
            output->child[0] = expression();
        }
        match("RightBracket");
        return output;
    }
    TreeNode* t = var();
    if (t == NULL) {
        t = simple_expression(t);  // ! 开头不是Id的情况
    } else {                       // Id 开头
        TreeNode* p = NULL;
        if (ptoken.type == "Assign") {
            p = new TreeNode("Assign");
            match("Assign");
            p->child[0] = t;
            p->child[1] = expression();  // 赋值的另一边可能是一个表达式
            return p;
        } else {
            t = simple_expression(t);
        }
    }

    return t;
}

TreeNode* return_stmt() {
    TreeNode* t = new TreeNode("ReturnStmt");
    match("return");
    // cout << "exp 之前 -- 0" << endl;
    if (ptoken.type == "Semicolon") {
        match("Semicolon");
        return t;
    } else {
        if (t != NULL) {
            // cout << "exp 之前" << endl;
            t->child[0]  = expression();
            t->child_cnt = 1;
        }
    }
    match("Semicolon");
    return t;
}

TreeNode* expression_stmt() {
    TreeNode* t = NULL;
    t           = expression();
    match("Semicolon");
    return t;
}

TreeNode* statement() {
    TreeNode* t = NULL;
    if (ptoken.type == "if") {
        t = selection_stmt();
    } else if (ptoken.type == "while") {
        t = iteration_stmt();
    } else if (ptoken.type == "return") {
        t = return_stmt();
    } else if (ptoken.type == "Id") {
        t = expression_stmt();
    } else if (ptoken.type == "intput" || ptoken.type == "output") {
        t = expression_stmt();
    }
    return t;
}

TreeNode* statement_list() {
    /**
     * 一系列的语句
     */
    TreeNode* t = statement();
    TreeNode* p = t;
    // 判断哪些token可以做为statement的开始
    while (ptoken.type == "if" || ptoken.type == "Id" || ptoken.type == "while" ||
           ptoken.type == "return" || ptoken.type == "semicolon" || ptoken.type == "input" ||
           ptoken.type == "output") {
        TreeNode* q;
        // cout << "ptoken.type == " << ptoken.type << endl;
        q = statement();
        if (q != NULL) {
            if (t == NULL) {
                t = p = q;
            } else {
                p->sibling = q;
                p          = q;
            }
        }
    }
    return t;
}

// TODO: 是否要区分一下声明且赋值与普通声明
TreeNode* compound_stmt() {                  // * 解析函数体
    TreeNode* t = new TreeNode("Compound");  // 函数体
    match("LeftBigBkt");
    t->child[0] = local_declaration();  // 声明都在前面, 以sibling连接
    t->child[1] = statement_list();     // 下面是各种语句, 以sibling连接
    if (t->child[0] == NULL) {
        t->child[0] = t->child[1];
        t->child[1] = NULL;
    }
    t->child_cnt = 2;
    match("RightBigBkt");
    return t;
}

TreeNode* declaration() {
    /**
     * 对声明(函数/变量)的递归下降分析
     */
    TreeNode* t;
    TreeNode* p;
    // cout << ptoken.type << ": " << ptoken.value << endl;
    if (ptoken.type == "int") {
        p = new TreeNode("int");
        match("int");
    } else {
        p = new TreeNode("void");
        match("void");
    }
    if (p != NULL && ptoken.type == "Id") {  // 变量/函数名
        TreeNode* q = new TreeNode("Id");
        q->name     = ptoken.value;
        q->line     = ptoken.line;
        match("Id");
        // TODO: 需要增加数组
        if (ptoken.type == "Semicolon") {  // 检测到分号, 变量声明
            t            = new TreeNode("VarDecl");
            t->child[0]  = p;
            t->child[1]  = q;
            t->child_cnt = 2;  // 记录子节点的个数
            match("Semicolon");
        } else if (ptoken.type == "Assign") {  // 初始化变量并赋值
            match("Assign");
            Token tmp = match("Num");
            match("Semicolon");
            int val      = stoi(tmp.value);
            t            = new TreeNode("VarAssign");
            t->child[0]  = p;
            t->child[1]  = q;
            t->child_cnt = 2;
            q->val       = val;
        } else if (ptoken.type == "LeftBracket") {  // 左括号, 说明是函数
            match("LeftBracket");
            t           = new TreeNode("FunDecl");
            t->child[0] = p;
            t->child[1] = q;
            t->child[2] = params();  // ! 解析函数的参数
            match("RightBracket");
            t->child[3]  = compound_stmt();
            t->child_cnt = 4;
        } else {
            error();
        }
    }
    return t;
}

TreeNode* declaration_list() {
    /**
     * 不断检测声明
     * 包括函数声明和变量声明.
     */
    TreeNode* t = declaration();  // 检测一个声明
    TreeNode* p = t;
    // while (ptoken.type != "int" && ptoken.type != "void" && ptoken.type != "EndFile") {
    //     ptoken = getToken();
    //     if (ptoken.type == "EndFile") break;
    // }
    while (ptoken.type == "int" || ptoken.type == "void") {  // 检测到int整形, void函数
        // cout << "声明的类型 ................... : " << ptoken.type << endl;
        TreeNode* q;
        q = declaration();
        if (q != NULL) {
            if (t == NULL) {  // ! 似乎不正常, 前面t为NULL说明没有检测到decl, 可以报错
                t = p = q;
            } else {
                p->sibling = q;  // *每个声明是并列的, 用兄弟结点连接.
                p          = q;
            }
        }
    }
    match("EndFile");
    // cout << "hehe\n";
    return t;
}

// 前序遍历获取语法树打印
void preOrder(TreeNode* t, string indent) {
    // if (t != NULL)
    //     out << "t->nodekind = " << t->nodekind << endl;
    if (t == NULL) {
        out << "NULL!" << endl;
        return;
    }
    if (t->nodekind == "FunDecl") {
        out << indent << "FunctionDeclaration {\n"
            << indent << "  type: FunctionDeclaration\n"
            << indent << "  id: " << t->child[1]->name << endl
            << indent << "  params: [\n";
        TreeNode* param = t->child[2]->child[0];
        while (param != NULL) {
            if (param->nodekind == "void") {
                out << indent << "    " << param->nodekind << endl;
            } else {
                out << indent << "    " << param->child[0]->nodekind << ": " << param->child[1]->name << endl;
            }
            param = param->sibling;
        }
        out << indent << "  ]\n"
            << indent << "  body: BlockStatement {\n"
            << indent << "    type: BlockStatement\n"
            << indent << "    body: [\n";
        preOrder(t->child[3], indent + "      ");
        out << indent << "    ]\n"
            << indent << "  }\n"
            << indent << "}\n";
        if (t->sibling != NULL) preOrder(t->sibling, indent);
        return;
    }
    if (t->nodekind == "Compound") {
        if (t->child[1] != NULL) {
            TreeNode* loc = t->child[0];
            out << indent << loc->nodekind << ":{\n";
            while (loc != NULL) {
                out << indent << "  " << loc->child[0]->nodekind << ": " << loc->child[1]->name << endl;
                loc = loc->sibling;
            }
            out << indent << "}\n";
        }
        TreeNode* stmt = NULL;
        if (t->child[1] == NULL) {
            stmt = t->child[0];
        } else
            stmt = t->child[1];
        while (stmt != NULL) {
            preOrder(stmt, indent);
            stmt = stmt->sibling;
        }
        return;
    }
    if (t->nodekind == "ReturnStmt") {
        out << indent << t->nodekind << ": {\n";
        preOrder(t->child[0], indent + "  ");
        out << indent << "}\n";
        return;
    }
    if (t->nodekind == "Id") {
        out << indent << t->nodekind << ": " << t->name << endl;
    } else if (t->nodekind == "Const") {
        out << indent << t->nodekind << ": " << t->val << endl;
    } else if (t->nodekind != "Input") {
        out << indent << t->nodekind << ": {\n";
    } else {  // ReturnStmt 会出现在这里输出
        out << indent << t->nodekind << endl;
    }

    for (int i = 0; i < MAX_CHILDREN; ++i) {
        // out << "i = " << i << endl;
        if (t->child[i] == NULL) break;
        blank += 2;
        if (i == 2 && t->nodekind == "Selection") {
            if (t->child[2] != NULL) {
                out << indent << "Else:\n";
            }
        }
        preOrder(t->child[i], indent + "  ");
        blank -= 2;
    }
    if (t->nodekind != "Id" && t->nodekind != "Input" && t->nodekind != "Const") {
        out << indent << "}\n";
    }
    // Assign 的sibling会重复
    // out << "t->nodekind = " << t->nodekind << endl;
    if (t->sibling != NULL) {
        if (t->nodekind != "Assign") {
            preOrder(t->sibling, indent);
        }
    }
}

void printParse(TreeNode* t) {
    out << "Program:{\n"
        << "  type: program,\n"
        << "  body: [\n";
    string indent = "    ";
    preOrder(t, indent);
    out << "  ]\n"
        << "}\n";
    out.close();
    // ifstream in("parse.txt", ios::in);
    // 输出到控制台
    // string s;
    // while (getline(in, s)) {
    //     cout << s << endl;
    // }
    cout << "error cnt: " << error_cnt << endl;
}

TreeNode* parse(bool print) {
    TreeNode* t;
    ptoken = getToken();          // 获取第一个token, 开始语法分析
    t      = declaration_list();  // 获取声明列表, c-由一系列整数/函数声明组成
    if (print) {
        printParse(t);
        cout << "parse print over !" << endl;
    }
    return t;
}