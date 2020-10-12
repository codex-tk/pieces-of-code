package main

import (
	"fmt"
	"net/http"

	"github.com/gin-gonic/gin"
)

func main() {
	r := &Router{make(map[string]map[string]HandlerFunc)}
	r.HandleFunc("GET", "/", logHandler(func(c *Context) {
		fmt.Fprintln(c.ResponseWriter, "welcome!")
	}))

	r.HandleFunc("GET", "/about", func(c *Context) {
		fmt.Fprintln(c.ResponseWriter, "about")
	})

	r.HandleFunc("GET", "/users/:id", func(c *Context) {
		fmt.Fprintf(c.ResponseWriter, "retrieve user %v\n",
			c.Params["id"])
	})

	r.HandleFunc("GET", "/users/:user_id/addresses/:address_id", func(c *Context) {
		fmt.Fprintf(c.ResponseWriter, "retrieve user %v’s address %v\n",
			c.Params["user_id"], c.Params["address_id"])
	})

	r.HandleFunc("POST", "/users", func(c *Context) {
		fmt.Fprintf(c.ResponseWriter, "create user\n")
	})

	r.HandleFunc("POST", "/users/:user_id/addresses", func(c *Context) {
		fmt.Fprintf(c.ResponseWriter, "create user %v’s address\n",
			c.Params["user_id"])
	})
	http.ListenAndServe(":8080", r)
	/*
		http.HandleFunc("/", func(w http.ResponseWriter, r *http.Request) {
			fmt.Fprintln(w, "welcome")
		})
		http.ListenAndServe(":8080", nil)
	*/
}

func gin_main() {
	r := gin.Default()
	r.GET("/ping", func(c *gin.Context) {
		c.JSON(200, gin.H{
			"message": "pong",
		})
	})
	r.Run(":80")
}
