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
		//fmt.Println(strings.TrimSpace(cmd))
		cmds := strings.Fields(strings.TrimSpace(cmd))  // 一个字符串数组
		// 查看类型
		//fmt.Println("type: " + reflect.TypeOf(cmds).String())
		//fmt.Println(cmds)

		var bk bool  // 退出命令行
		if len(cmds) == 0 {
			continue
		}
		var cmdStr string
		cmdStr = cmds[0]
		switch cmdStr {
		case "upload":
			if len(cmds) < 2 {
				fmt.Println("please input the path")
				break
			}
			handleUpload(conn, cmds[1])  // 将本地文件上传到服务器的当前工作目录
			//fmt.Println("cmd: upload")
		case "download":
			//fmt.Println("cmd: download")
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

// 一个将两段 []byte 合并的函数
func BytesCombine(pBytes ...[]byte) []byte {
	return bytes.Join(pBytes, []byte(""))
}

// 处理简单命令的函数
func handleSimple(conn net.Conn, cmd string) {
	// 将简单命令传递给server进行处理, 这些命令只需要传递给server, 然后server执行, 返回结果
	// 防止 粘包 的发生, 要控制好发送 []byte 的长度
	buf := []byte("simple")
	empty := make([]byte, 1024 - len(buf))
	buf = BytesCombine(buf, empty)
	//fmt.Println("len of buf: ", len(buf))
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

// 处理上传命令 upload
func handleUpload(conn net.Conn, filepath string) {
	//fmt.Println("handle upload")
	// 发送模式mod, 并防止 粘包
	buf := []byte("upload")
	empty := make([]byte, 1024 - len(buf))
	buf = BytesCombine(buf, empty)
	//fmt.Println("len of buf: ", len(buf))
	_, err := conn.Write(buf)  // 首先传递命令模式, 这里是simple cmd模式
	if err != nil {
		fmt.Println("write mod failed: ", err)
	}

	file, err := os.Open(filepath)
	if err != nil {
		fmt.Println("open error = ", err)
	}
	defer file.Close()

	// 首先需要传输文件的名字
	buf = []byte(file.Name())
	empty = make([]byte, 1024 - len(buf))
	buf = BytesCombine(buf, empty)
	//fmt.Println("len of buf: ", len(buf))
	_, err = conn.Write(buf)
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