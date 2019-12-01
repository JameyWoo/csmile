package main

import (
	"fmt"
	"net"
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
		// 新建一个协程, 异步处理这个连接
		go process(conn)
	}
}

// 处理一个连接
func process(conn net.Conn) {
	// server 也需要不断接收命令
	for {
		buf := make([]byte, 1024)
		n, err := conn.Read(buf)
		if err != nil {
			fmt.Println("read error: ", err)
			break
		}
		mod := string(buf[:n])  // 读取的第一个是模式
		switch mod {
		case "simple":
			// 简单命令模式

			fmt.Println("sim")
		case "upload":
			// 上传文件
		}
	}
}