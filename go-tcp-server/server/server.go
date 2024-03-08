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
    var connections[] net.Conn
    for {
        conn, err := socket.Accept()
        if err != nil {
            fmt.Println(err)
            return
        }
        connections = append(connections, conn)
        go handleConnections(conn, &connections)
    }

}

func handleConnections(conn net.Conn, connections *[]net.Conn) {
    buffer := make([]byte, 1024)
    for {
        bytesRead, err := conn.Read(buffer)
        if err != nil {
            fmt.Println(err)
            return 
        }
        fmt.Printf("[Read %d bytes] %s \n", bytesRead, buffer[:bytesRead])
        handleUpdates(conn, buffer[:bytesRead], connections)
    }
}

func handleUpdates(senderConn net.Conn, msg []byte, connections *[]net.Conn) {
    for _, conn := range *connections {
        if senderConn == conn {
            continue
        }
        conn.Write(msg)
    }
}
