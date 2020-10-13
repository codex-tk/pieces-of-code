package main

import (
	"fmt"

	"github.com/gin-gonic/gin"
)

type User struct {
	Id        string
	AddressId string
}

func main() {
	s := NewServer()
	s.HandleFunc("GET", "/", func(c *Context) {
		fmt.Fprintln(c.ResponseWriter, "welcome!")
	})

	s.HandleFunc("GET", "/about", func(c *Context) {
		fmt.Fprintln(c.ResponseWriter, "about")
	})

	s.HandleFunc("GET", "/users/:id", func(c *Context) {
		user := User{Id: c.Params["id"].(string)}
		/*
			fmt.Fprintf(c.ResponseWriter, "retrieve user %v\n",
				c.Params["id"])
		*/
		c.RenderJson(user)
	})

	s.HandleFunc("GET", "/users/:user_id/addresses/:address_id", func(c *Context) {
		fmt.Fprintf(c.ResponseWriter, "retrieve user %v’s address %v\n",
			c.Params["user_id"], c.Params["address_id"])
	})

	s.HandleFunc("POST", "/users",
		func(c *Context) {
			fmt.Println(c.Params)
			fmt.Fprintf(c.ResponseWriter, "create user\n")
		})

	s.HandleFunc("POST", "/users/:user_id/addresses", func(c *Context) {
		fmt.Fprintf(c.ResponseWriter, "create user %v’s address\n",
			c.Params["user_id"])
	})
	s.HandleFunc("GET", "/public/", func(c *Context) {
		fmt.Fprintf(c.ResponseWriter, "static Handler")
	})
	s.Run(":8080")
	/*
		r := &Router{make(map[string]map[string]HandlerFunc)}
		r.HandleFunc("GET", "/", logHandler(func(c *Context) {
			fmt.Fprintln(c.ResponseWriter, "welcome!")
		}))

		r.HandleFunc("GET", "/about", func(c *Context) {
			fmt.Fprintln(c.ResponseWriter, "about")
		})

		r.HandleFunc("GET", "/users/:id", parseFromHandler(func(c *Context) {
			fmt.Println(c.Params)
			fmt.Fprintf(c.ResponseWriter, "retrieve user %v\n",
				c.Params["id"])
		}))

		r.HandleFunc("GET", "/users/:user_id/addresses/:address_id", func(c *Context) {
			fmt.Fprintf(c.ResponseWriter, "retrieve user %v’s address %v\n",
				c.Params["user_id"], c.Params["address_id"])
		})

		r.HandleFunc("POST", "/users", logHandler(recoverHandler(parseJsonBodyHandler(
			func(c *Context) {
				fmt.Println(c.Params)
				fmt.Fprintf(c.ResponseWriter, "create user\n")
			}))))

		r.HandleFunc("POST", "/users/:user_id/addresses", func(c *Context) {
			fmt.Fprintf(c.ResponseWriter, "create user %v’s address\n",
				c.Params["user_id"])
		})
		r.HandleFunc("GET", "/public/", logHandler(staticFileHandler(func(c *Context) {
			fmt.Fprintf(c.ResponseWriter, "static Handler")
		})))
		http.ListenAndServe(":8080", r)
	*/
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
