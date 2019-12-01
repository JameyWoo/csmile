package main

import (
	"bufio"
	"fmt"
	"net"
	"os"
	"strings"
)

func main() {
	// 先连接到唯一的客户端
	conn, err := net.Dial("tcp", "222.240.54.75:8888")
	defer conn.Close()
	if err != nil {
		fmt.Println("dial error")
		return
	}
	// 循环输入命令和执行, 读入 exit 退出
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
		// 有些命令直接传过去让server处理, 有的需要相互处理
		case "ls":
			fallthrough
			//fmt.Println("cmd: ls")
		case "cd":
			fallthrough
			//fmt.Println("cmd: cd")
		case "mkfile":
			fallthrough
			//fmt.Println("cmd: mkfile")
		case "mkdir":
			fallthrough
		case "simple cmd":
			fmt.Println("pass one time")
			passToServer(conn, cmd)
			//fmt.Println("cmd: mkdir")
		case "upload":
			fmt.Println("cmd: upload")
		case "download":
			fmt.Println("cmd: download")
		case "exit":  // 退出
			bk = true
		default:
			fmt.Println("error cmd!!!")
		}
		if bk {
			break
		}
	}
}

func passToServer(conn net.Conn, cmd string) {
	// 将简单命令传递给server进行处理, 这些命令只需要传递给server, 然后server执行, 返回结果
	// todo: 暂时只实现 ls, 之后补充其他命令
	_, err := conn.Write([]byte("simple"))  // 首先传递命令模式, 这里是simple cmd模式
	if err != nil {
		fmt.Println("write mod failed: ", err)
	}
	_, err = conn.Write([]byte(cmd))
	if err != nil {
		fmt.Println("write cmd failed: ", err)
	}
}