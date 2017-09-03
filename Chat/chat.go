package main

import (
	"bufio"
	"fmt"
	"log"
	"net"
	"strings"
)

type client struct {
	communication chan<- string
	name          string
}

type pvt struct {
	who client
	with string
	msg string
}

var (
	entering = make(chan client)
	leaving  = make(chan client)
	messages = make(chan string)
	check    = make(chan string)
	valid    = make(chan bool)
	private  = make(chan pvt)
)

func broadcaster() {
	clients := make(map[string]client)
	for {
		select {
		case msg := <-messages:
			for _, cli := range clients {
				cli.communication <- msg
			}

		case p := <- private:
			if(p.msg == ""){
				p.who.communication <- "You have entered private chat with " + p.with
			}else {
				p.who.communication <- p.who.name + ": " + p.msg
				clients[p.with].communication <- p.who.name + " pvt: " + p.msg
			}

		case name := <- check:
			if _, ok := clients[name]; ok {
				valid <- true
			}else {
				valid <- false
			}

		case cli := <-entering:
			clients[cli.name] = cli
			cli.communication <-"Welcome " + cli.name +"!"
			cli.communication <- "Clients that currently are online:"
			for _, c := range clients {
				cli.communication <- c.name
			}

		case cli := <-leaving:
			delete(clients, cli.name)
			close(cli.communication)
		}
	}
}

func handleConn(conn net.Conn) {
	ch := make(chan string)
	go clientWriter(conn, ch)

	input := bufio.NewScanner(conn)
	var name string
	var who client
	var with string
	var isPrivate bool

	for {
		ch <- "What would you like your display name to be?"
		input.Scan()
		name = input.Text()

		check <- name

		if  !<-valid {
			messages <- name + " has arrived"
			who = client{ch, name}
			entering <- who
			break
		}else {
			ch <- "Name is already taken."
		}
	}

	for input.Scan() {
		msg := input.Text()

		if(isPrivate){
			if msg == "end pvt"{
				isPrivate = false
				ch <- "You left the private chat."
			}else {
				private <- pvt{who, with, msg}
			}

		} else if(strings.HasPrefix(msg,"pvt ")){

			with = msg[4:]
			check <- with
			if <-valid {
				isPrivate = true
				private <- pvt{who, with, ""}
			}else{
				ch <- "'" + with + "' user does not exist."
			}

		}else {
			messages <- name + ": " + msg
		}
	}

	leaving <- who
	messages <- name + " has left"
	conn.Close()
}

func clientWriter(conn net.Conn, ch <-chan string) {
	for msg := range ch {
		fmt.Fprintln(conn, msg)
	}
}

func main() {
	listener, err := net.Listen("tcp", "localhost:8000")
	if err != nil {
		log.Fatal(err)
	}

	go broadcaster()
	for {
		conn, err := listener.Accept()
		if err != nil {
			log.Print(err)
			continue
		}
		go handleConn(conn)
	}
}