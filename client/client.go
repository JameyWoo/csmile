package main

import (
	"fmt"
	"net"
	"os"
)

func main() {
	conn, err := net.Dial("tcp", "222.240.54.75:8888")
	defer conn.Close()
	if err != nil {
		fmt.Println("dial error")
		return
	}
	var path string
	fmt.Println("input the path")
	_, err = fmt.Scan(&path)
	if err != nil {
		fmt.Println("scan err")
	}
	file, err := os.Open(path)
	if err != nil {
		fmt.Println("open error = ", err)
	}
	defer file.Close()

	// 首先需要传输文件的名字
	_, err = conn.Write([]byte(file.Name()))
	if err != nil {
		fmt.Println("write name error")
		return
	}
	// 再连续地传输文件
	for {
		buffer := make([]byte, 1024)
		n, err := file.Read(buffer)  // 读取文件到buffer中
		if err != nil {
			fmt.Println("read error")
		}
		if n == 0 {  // 是否读取完了文件
			fmt.Println("read over")
			break
		}

		_, err = conn.Write(buffer)
		if err != nil {
			fmt.Println("write err")
		}
	}
}
