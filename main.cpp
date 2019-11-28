#include <iostream>
#include "globals.h"
#include "utils.h"
#include "scan.h"
#include "parse.h"
#include "analyze.h"

using namespace std;

ifstream read;

int main(int argc, char *argv[]) {
    if (argc != 2) {  // 判断参数, 给出用法提示
        cout << "Usage:" << endl;
        cout << "./main <file_name>" << endl;
        return 0;
    }
    read.open(string(argv[1]), ios::in);
    readin();
    scanning = true;
    TreeNode* root = parse();
    buildSymtab(root);
    typeCheck(root);
}