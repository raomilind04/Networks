package main

import (
	"fmt"
	"net"
)

func main() {
    socket, err := net.Listen("tcp", ":8080")
    if err != nil {
       fmt.Println(err)
        return 
    } else {
        fmt.Println("Server listing on port 8080")
    }

    for {
        conn, err := socket.Accept()
        if err != nil {
            fmt.Println(err)
            return
        }
        go handleConnections(conn)
    }

}

func handleConnections(conn net.Conn) {
    buffer := make([]byte, 1024)
    bytesRead, err := conn.Read(buffer)
    if err != nil {
        fmt.Println(err)
        return 
    }
    fmt.Printf("[Read %d bytes] %s", bytesRead, buffer)
}
