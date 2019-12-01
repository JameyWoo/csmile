package main

import (
	"bufio"
	"bytes"
	"fmt"
	"io"
	"net"
	"os"
	"strings"
)

func main() {
	// 先连接到唯一的客户端
	conn, err := net.Dial("tcp", "121.199.49.224:8888")
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
		case "upload":
			fmt.Println("cmd: upload")
		case "download":
			fmt.Println("cmd: download")
		case "exit":  // 退出
			bk = true
		default:
			handleSimple(conn, cmd)
		}
		if bk {
			break
		}
	}
}

func BytesCombine(pBytes ...[]byte) []byte {
	return bytes.Join(pBytes, []byte(""))
}

func handleSimple(conn net.Conn, cmd string) {
	// 将简单命令传递给server进行处理, 这些命令只需要传递给server, 然后server执行, 返回结果
	// todo: 暂时只实现 ls, 之后补充其他命令(或者这一类命令全部交给server实现)
	buf := []byte("simple")
	empty := make([]byte, 1024 - len(buf))
	buf = BytesCombine(buf, empty)
	fmt.Println("len of buf: ", len(buf))
	_, err := conn.Write(buf)  // 首先传递命令模式, 这里是simple cmd模式
	if err != nil {
		fmt.Println("write mod failed: ", err)
	}
	_, err = conn.Write([]byte(cmd))  // 发送命令
	if err != nil {
		fmt.Println("write cmd failed: ", err)
	}
	//读取字符串
	for {
		buf := make([]byte, 1024)
		n, err := conn.Read(buf)
		if err != nil {
			if err != io.EOF {
				fmt.Println("err: ", err)
			}
			return
		}
		if n == 0 {
			return
		}
		getStdout := string(buf[:n])
		if getStdout == "write over" {  // 读取到了结束标志
			return
		}
		fmt.Print(getStdout)
	}
}