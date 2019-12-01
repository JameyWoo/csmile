/*
第一版本的server, 现已弃用
 */

package tests

import (
	"fmt"
	"io"
	"net"
	"os"
)

// 处理一个客户端请求
func process(conn net.Conn) {
	// 这个 defer 真是方便
	defer conn.Close()

	// 读取名字
	buffer := make([]byte, 1024)
	n, err := conn.Read(buffer)
	fullName := string(buffer[:n])
	var filename string
	for i := len(fullName) - 1; i >= 0; i-- {
		if fullName[i]== '/' {
			break
		}
		filename += string(fullName[i])
	}
	var filename2 string
	for i := len(filename) - 1; i >= 0; i-- {
		filename2 += string(filename[i])
	}
	filename = filename2
	fmt.Println("filename: ", filename)

	fs, err := os.Create(filename)
	if err != nil {
		fmt.Println("failed to create file")
	}
	defer fs.Close()

	buf := make([]byte, 1024)

	for {
		// 同样以阻塞地方式等待读取数据(接收)
		n, err := conn.Read(buf)
		if err != nil {
			// 修改了报错逻辑, 免得正常传输也有报错
			if err == io.EOF {
				fmt.Println("file read over")
			} else {
				fmt.Println("conn.Read err =", err)
			}
			return
		}
		if n == 0 {
			fmt.Println("file read over", err)
			return
		}
		// 一次读取 n个字节, 然后写入到文件
		fs.Write(buf[:n])
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
