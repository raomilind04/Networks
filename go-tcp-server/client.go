package main

import (
	"fmt"
	"net"
)

func main() {
    conn, err := net.Dial("tcp", "localhost:8080")
    if err != nil {
        fmt.Println(err)
        return 
    }
    
    _, err = conn.Write([]byte("Hello from the client"))
    if err != nil {
        fmt.Println(err)
        return 
    }
    for {
        buffer := make([]byte, 1024)
        _, err = conn.Read(buffer)
        fmt.Printf("From sever %s", buffer)
    }
}
