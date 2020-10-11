package main

import (
	"context"
	"log"
	"time"

	"github.com/gwonhyeong/pieces-of-codes/golang_grpc/services"
	"google.golang.org/grpc"
)

func main() {
	conn, err := grpc.Dial("localhost:8088", grpc.WithInsecure(), grpc.WithBlock())
	if err != nil {
		log.Fatalf("did not connect: %v", err)
	}
	defer conn.Close()
	c := services.NewConfigStoreClient(conn)

	ctx, cancel := context.WithTimeout(context.Background(), time.Second)
	defer cancel()

	r, err := c.Get(ctx, &services.ConfigRequest{Profile: "dev"})
	if err != nil {
		log.Fatalf("could not request: %v", err)
	}

	log.Printf("Config: %v", r)
}
