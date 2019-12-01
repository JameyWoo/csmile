package main

import (
	"fmt"
	"io"
	"net"
	"os"
	"os/exec"
	"strings"
)

func main() {
	fmt.Println("a ftp simulator by socket")
	listen, err := net.Listen("tcp", "0.0.0.0:8888")
	if err != nil {
		fmt.Println("listen error")
	}
	defer listen.Close()

	// 循环处理
	for {
		// 以阻塞的方式等待请求, 返回一个连接
		conn, err := listen.Accept()
		if err != nil {
			fmt.Println("accept error")
		}
		fmt.Println("addr:", conn.RemoteAddr())
		// 新建一个协程, 异步处理这个连接
		go process(conn)
	}
}

// 处理一个连接
func process(conn net.Conn) {
	fmt.Println("start to process")
	// server 也需要不断接收命令
	for {
		buf := make([]byte, 1024)
		n, err := conn.Read(buf)
		if err != nil {
			fmt.Println("read error: ", err)
			break
		}
		mod := string(buf[:n])  // 读取的第一个是模式
		fmt.Println("mod: -" + mod + "-")
		fmt.Println("mod simple")
		processSimple(conn)
	}
}

func processSimple(conn net.Conn) {
	fmt.Println("process")
	buf := make([]byte, 1024)
	n, err := conn.Read(buf)
	if err != nil {
		fmt.Println("read error: ", err)
	}
	cmd := string(buf[:n])
	fmt.Println("cmd: +" + cmd + "+")
	cmds := strings.Fields(strings.TrimSpace(cmd))  // 一个字符串数组
	fmt.Println("args:", cmds)
	// 单独执行 cd 命令
	if cmds[0] == "cd" {
		var path string
		if len(cmds) == 2 {
			if cmds[1][0] != '/' {
				// 如果是相对路径, 那么进行组合
				path, err = os.Getwd()
				if err != nil {
					fmt.Println("err: ", err)
				}
				// todo: 判断cmds[1]最后一个字符不是 /
				path += "/" + cmds[1]
			} else {
				path = cmds[1]
			}
		}
		os.Chdir(path)
	}
	command := exec.Command(cmds[0], cmds[1:]...)
	// 获取输出对象
	stdout, err := command.StdoutPipe()
	if err != nil {
		fmt.Println("err = ", err)
	}
	// 保证关闭输出流
	defer stdout.Close()
	// 运行命令
	if err := command.Start(); err != nil {
		fmt.Println("err = ", err)
	}
	for {
		buf := make([]byte, 1024)
		_, err := stdout.Read(buf)
		if err != nil {
			if err != io.EOF{
				fmt.Println("err: ", err)
			}
			break
		}
		_, err = conn.Write(buf)
		if err != nil {
			fmt.Println("err: ", err)
			break
		}
	}
	// 似乎最后还需要发送一个指令告诉他已经结束了
	_, err = conn.Write([]byte("write over"))
	if err != nil {
		fmt.Println("write over error: ", err)
	}
}