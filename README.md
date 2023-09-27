# SomeNetworkProjects

## Description
This is a repository which gathers two projects about network to comprehend how it works, the sockets, buffers and how the server manage all the data traffic between clients. Both have a similar implementation in spite of their own goals.
### N-TicTacToe
The first one and more interesting is N-TicTacToeNetwork where we can create users with nicknames and they can play Tic Tac Toe with a matrix of any dimension what makes it graspable and instructive is the control that the server has, performing the right connections and giving answers when a player is on a game or has rejected the other user.

### Actions-Chat
This mini-project is a simulation of a chat where you can sign-up and talk or send files with other users. 




## Used libraries
```
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <unistd.h>
    #include <vector>
    #include <map>
    #include <thread>
    #include <string>
    #include <iostream>
```
