package main

import (
	"context"
	"log"
	"net"

	"github.com/gwonhyeong/pieces-of-codes/golang_grpc/services"
	"google.golang.org/grpc"
)

//.\protoc.exe -I . .\services.proto --go_out=plugins=grpc:.

type server struct {
	//services.UnimplementedConfigStoreServer
}

func (s *server) Get(ctx context.Context, in *services.ConfigRequest) (*services.ConfigResponse, error) {
	log.Printf("Received profile: %v", in.GetProfile())
	return &services.ConfigResponse{JsonConfig: `"{"main":"http://google.com"}"`}, nil
}

func main() {
	lis, err := net.Listen("tcp", ":8088")
	if err != nil {
		log.Fatalf("failed to listen: %v", err)
	}

	s := grpc.NewServer()
	services.RegisterConfigStoreServer(s, &server{})
	if err := s.Serve(lis); err != nil {
		log.Fatalf("failed to serve: %v", err)
	}
}
