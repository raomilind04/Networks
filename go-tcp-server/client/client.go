package main

import (
	"bufio"
	"fmt"
	"net"
	"os"
	"strings"
)

func main() {
    conn, err := net.Dial("tcp", "localhost:8080")
    if err != nil {
        fmt.Println(err)
        return 
    }
    go handleServerEvents(conn)
    handleUserMessages(conn)

}

func handleServerEvents(conn net.Conn) {
    buffer := make([]byte, 1024)
    for {
        bytesReceived, err := conn.Read(buffer)
        if err != nil {
            fmt.Println(err)
        }
        if bytesReceived > 0 {
            fmt.Println(string(buffer[:bytesReceived]))
        }
    }
}

func handleUserMessages(conn net.Conn) {
    var userName string
    fmt.Print("Please enter your name: ")
    fmt.Scanln(&userName)
    

    fmt.Println("Enter you message: ")
    
    reader := bufio.NewReader(os.Stdin)

    for {
        var msg string
        msg, err := reader.ReadString('\n')
        if err != nil {
            fmt.Println(err)
            return 
        }
        
        msg = strings.TrimSpace(msg)

        if strings.ToUpper(msg) == "EXIT" {
            break
        }
        formattedMsg := fmt.Sprintf("%s: %s", userName, msg)
        conn.Write([]byte(formattedMsg))
    }
       
}
