package main

import (
	"fmt"
	"runtime"
	"sync"
	"sync/atomic"
	"time"
)

func main() {
	mutexMain()
}

func bufferedChanMain() {
	ch := bufferedChan()
	for {
		v, ok := <-ch
		if ok == false {
			break
		}
		fmt.Println("Receive", v)
		time.Sleep(1 * time.Millisecond)
	}
}

func bufferedChan() chan int {
	ch := make(chan int, 3)
	/*
		go func(ch chan<- int) {
			for i := 0; i < 10; i++ {
				fmt.Println("Send ", i)
				ch <- i
			}
			close(ch)
		}(ch)
	*/
	go func() {
		for i := 0; i < 10; i++ {
			fmt.Println("Send ", i)
			ch <- i
		}
		close(ch)
	}()
	return ch
}

func simpleChan() {
	done := make(chan bool)
	go doneReceiver(done)
	b := <-done
	fmt.Println("Done", b)
}

func doneReceiver(done chan<- bool) {
	time.Sleep(1 * time.Second)
	done <- true
}

func fib(c, q chan int) {
	x, y := 0, 1
	for {
		select {
		case c <- x:
			x, y = y, x+y
		case <-q:
			fmt.Println("Quit")
			return
		default:
			fmt.Println("Defalut")
			time.Sleep(10 * time.Millisecond)
		}
	}
}

func fibChan() {
	c := make(chan int)
	q := make(chan int)
	go func() {
		for i := 0; i < 10; i++ {
			fmt.Println(<-c)
		}
		q <- 0
	}()
	fib(c, q)
}

type counter struct {
	i    int32
	mu   sync.Mutex
	once sync.Once
}

func (c *counter) increment() {
	/*
		c.once.Do(
			func() {
				c.i = -100
			})
		c.mu.Lock()
		defer c.mu.Unlock()
		c.i++
	*/
	atomic.AddInt32(&c.i, 1)
}

func doneWithChan() {
	runtime.GOMAXPROCS(runtime.NumCPU())

	var c counter = counter{i: 0}
	done := make(chan struct{})

	for i := 0; i < 1000; i++ {
		go func() {
			for j := 0; j < 100; j++ {
				c.increment()
			}
			done <- struct{}{}
		}()
	}
	for i := 0; i < 1000; i++ {
		<-done
	}
	fmt.Println(c.i)
}

func mutexMain() {
	runtime.GOMAXPROCS(runtime.NumCPU())

	var c counter = counter{i: 0}
	w := sync.WaitGroup{}

	for i := 0; i < 1000; i++ {
		w.Add(1)
		go func() {
			defer w.Done()
			for j := 0; j < 100; j++ {
				c.increment()
			}
		}()
	}
	w.Wait()
	fmt.Println(c.i)
}
