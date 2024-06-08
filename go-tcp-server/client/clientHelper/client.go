package client

import (
	"bufio"
	"fmt"
	"io"
	"net"
	"os"
	"strings"
)

// func main() {
//     conn, err := net.Dial("tcp", "localhost:8080")
//     if err != nil {
//         fmt.Println(err)
//         return 
//     }
//     defer conn.Close()
//
//     terminate := make(chan bool)
//
//     go handleServerEvents(conn, terminate)
//     handleUserMessages(conn, terminate)
//
// }

func handleServerEvents(conn net.Conn, terminate chan bool) {
    buffer := make([]byte, 1024)
    for {
        bytesReceived, err := conn.Read(buffer)
        if err != nil {
            if err == io.EOF {
                fmt.Println("The server has disconnected") 
                terminate <- true
                fmt.Println("Terminate siganl sent")
                return
            }
            fmt.Println(err)
        }
        if bytesReceived > 0 {
            fmt.Println(string(buffer[:bytesReceived]))
        }
    }
}

func handleUserMessages(conn net.Conn, terminate chan bool) {
    var userName string
    fmt.Print("Please enter your name: ")
    fmt.Scanln(&userName)
    fmt.Println("Enter you message: ")
    
    reader := bufio.NewReader(os.Stdin)

    for {
        select {
        case <-terminate:
            fmt.Println("Sever disconnected")
            return 
        default:
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
            _, err = conn.Write([]byte(formattedMsg))
            if err != nil {
                fmt.Println(err)
                return
            }
        }
    }
}
