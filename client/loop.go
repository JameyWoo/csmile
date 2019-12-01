package main

import (
	"bufio"
	"fmt"
	"os"
	//"reflect"
	"strings"
)

func main() {
	for {
		fmt.Print("~$ ")  // 命令提示符
		var cmd string
		reader := bufio.NewReader(os.Stdin)
		cmd, err := reader.ReadString('\n')  // 以换行作为结束符
		cmd = strings.Trim(cmd, " \r\n")  // 去掉换行符. 小心返回之后要赋值
		if err != nil {
			fmt.Println("read error = ", err)
		}

		// 按空格解析命令的各个部分
		fmt.Println(strings.TrimSpace(cmd))
		cmds := strings.Fields(strings.TrimSpace(cmd))  // 一个字符串数组
		// 查看类型
		//fmt.Println("type: " + reflect.TypeOf(cmds).String())
		fmt.Println(cmds)

		var bk bool  // 退出命令行
		switch cmds[0] {
		case "ls":
			fmt.Println("cmd: ls")
		case "cd":
			fmt.Println("cmd: cd")
		case "mkfile":
			fmt.Println("cmd: mkfile")
		case "mkdir":
			fmt.Println("cmd: mkdir")
		case "upload":
			fmt.Println("cmd: upload")
		case "download":
			fmt.Println("cmd: download")
		case "exit":  // 退出
			bk = true
		}
		if bk {
			break
		}
	}
}
