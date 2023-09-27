# SomeNetworkProjects

## Description
This is a repository which gathers two projects about network to comprehend how it works, the sockets, buffers and how the server manage all the data traffic between clients. Both have a similar implementation in spite of their own goals. As I said, it has sockets and also, we use threads in the server to listen different user request at the same time.

### N-TicTacToe
The first one and more interesting is N-TicTacToeNetwork where we can create users with nicknames and they can play Tic Tac Toe with a matrix of any dimension what makes it graspable and instructive is the control that the server has, performing the right connections and giving answers when a player is on a game or has rejected the other user.

#### How to use
As we use files such as socket.h from sys library we must prefer a Linux environment, in this example and during the whole project we have been operating on WSL. How to play? Once we open one terminal for the server and as many terminals as it is necessary to have one terminal for each player, we compile the serverV2.cpp file and playerV2.cpp file, we run the server file and then the players' one. We should see this:
<p align="center">
<br><br><img width="249" alt="image" src="https://github.com/alexjr2001/SomeNetworkProjects/assets/63054183/423c2bbb-ab71-4108-9f1c-7e225955c6b9">
<br>Fig. 1 Asking for a nickname and then for an opponent.
<br><br><img width="326" alt="image" src="https://github.com/alexjr2001/SomeNetworkProjects/assets/63054183/a2d21329-17c6-4ee9-9e75-b4ad973f49bd">
<br>Fig. 2 The opponent receives the request, replies and chooses a sign (It must insert two inputs "Y" because the previous input "Who you want to play with?" hasn't been satisfied).
<br><br><img width="224" alt="image" src="https://github.com/alexjr2001/SomeNetworkProjects/assets/63054183/865385bf-34a3-499b-9df1-fe493efcf881">
<br>Fig. 3 The player who requests the game needs to choose a sign and the dimension of the table.
<br><br><img width="181" alt="image" src="https://github.com/alexjr2001/SomeNetworkProjects/assets/63054183/155a7977-ae22-44f1-80fe-526cfd96b14f">
<br>Fig. 4 After setting everything they will be able to put the index position in continuous numbers (Remember the table index begins in 00).
<br><br><img width="365" alt="image" src="https://github.com/alexjr2001/SomeNetworkProjects/assets/63054183/55363711-ef5c-43db-a5ed-a036ac9d1b48">
<br>Fig. 5 In every turn the opponent will receive the updated table.
</p>

### Actions-Chat
This mini-project is a simulation of a chat where you can sign-up and talk or send files with other users. It is really simple to use if you already understood the previous and more complex project. You just press "M" to send messages, and "F" to send files in your directory, besides, you can ask the list of connected users.

## Compilation and Execution
Three terminals needed. 

Firstly, the server terminal:
- N-TicTacToe
```
g++ serverV2.cpp -o s.exe
./s.exe
```
- Actions
```
g++ server.cpp -o s.exe
./s.exe
```

And secondly the players' terminals:
- N-TicTacToe
```
g++ playerV2.cpp -o p.exe
./p.exe
```
- Actions
```
g++ client.cpp -o c.exe
./c.exe
```

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
