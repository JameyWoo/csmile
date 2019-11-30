package main

import (
	"fmt"
	"net"
)

// 处理一个客户端请求
func process(conn net.Conn) {
	// 这个 defer 真是妙啊
	defer conn.Close()

	for {
		// 同样以阻塞地方式等待读取数据(接收)
		nbyte, err := conn.Read()
	}
}
// todo
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
