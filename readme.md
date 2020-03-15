关于符号表的分析

要生成汇编代码, 需要先考虑单个函数
一个函数中, 在生成符号表的时候, 需要记录
1. 其传递进入的参数的数量
2. 局部变量的数量

需要解决的问题:
1. 参数和局部变量如何分配栈地址
2. 如何分配寄存器(需要有什么约定?)(关键是复杂的语法结构, 比如多重的a = b = c = d = e这种或 a = a + (b + c + (d + e))这种)
3. 进行函数调用时的栈空间变化


steps:
1. 首先解决单函数的情况
2. 各函数之间是独立的, 所以可以用一个函数来生成每个函数的代码



要生成汇编代码, 其实不用有中间代码. 因为汇编代码本身就可以作为一种中间代码
但是为什么要有中间代码呢? 

有更多优化的空间, 提取出源代码的特征. 这时候可以详细地获取

一篇好文章: https://zhuanlan.zhihu.com/p/37537046
分析了leave, call, ret 指令背后的过程

关于栈帧
1. 寄存器分为调用者保存的寄存器和被调用者保存的寄存器
2. 可以默认保存所有寄存器, 之后恢复, 而不根据实际情况选择保存
3. 栈帧结构(自高地址到低地址): 参数2->参数1->返回地址->原ebp->保存的寄存器->局部变量 ... 
4. 调用函数时, 不能两个都是栈偏移, 需要用到寄存器