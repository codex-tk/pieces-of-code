package main

import "net/http"

type Server struct {
	*Router
	middlewares  []Middleware
	startHandler HandlerFunc
}

func NewServer() *Server {
	r := &Router{make(map[string]map[string]HandlerFunc)}
	s := &Server{Router: r}
	s.middlewares = []Middleware{
		logHandler,
		recoverHandler,
		staticFileHandler,
		parseFromHandler,
		parseJsonBodyHandler}
	return s
}

func (s *Server) Run(addr string) {
	s.startHandler = s.handler()
	for i := len(s.middlewares) - 1; i >= 0; i-- {
		s.startHandler = s.middlewares[i](s.startHandler)
	}
	if err := http.ListenAndServe(addr, s); err != nil {
		panic(err)
	}
}

func (s *Server) ServeHTTP(w http.ResponseWriter, req *http.Request) {
	c := &Context{
		Params:         make(map[string]interface{}),
		ResponseWriter: w,
		Request:        req,
	}
	for k, v := range req.URL.Query() {
		c.Params[k] = v[0]
	}
	s.startHandler(c)
}

func (s *Server) Use(middlewares ...Middleware) {
	s.middlewares = append(s.middlewares, middlewares...)
}
