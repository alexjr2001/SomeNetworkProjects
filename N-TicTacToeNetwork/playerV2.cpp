 /* Client code in C */
 
  #include <sys/types.h>
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include <unistd.h>
  #include <iostream>
  #include <string>
  #include <thread>

  using namespace std;

  bool turn=0;
  bool playing=0;       //Is he playing?
  char sign=' ';        //Player's sign
  int N=5; //N en raya
  void readServer (int P_socket) {
    int n;
    string name;
    int size_name;
    string ans;
    string buff_envio;
      while (1) {
        char buff[256];
        n = read (P_socket, buff, 1);
        if(buff[0]=='1'){       //It's your turn
            turn=1;
        }
        else if(buff[0]=='D'){        //Receive the dimension of the table in case the another player request the match
          n=read(P_socket,buff,2);
          N=atoi(buff);
        }
        else if(buff[0]=='T'){          //Print the table
            n=read(P_socket,buff,(N+N-1)*N);
            for(int i=0;i<(N+N-1)*N;i++){
              if(i%(N+N-1)==0) cout<<endl;
              cout<<buff[i];
            }
            cout<<endl;
        }
        else if(buff[0]=='W'){        //The game has ended
          n = read (P_socket, buff, 1);
          if(buff[0]=='T') cout<<"WIIIN";
          else if(buff[0]=='E') cout<<"DRAAW";
          else cout<<"LOSE";
          playing=0;
          turn=0;
        }
        else if(buff[0]=='R'){      //Response after request a match
          n = read (P_socket, buff, 1);
          if(buff[0]=='1'){             //Got accepted
            cout << "ACCEPTED\n";
            cout << "Choose your sign: ";
            cin >> sign; 

            int NRaya;
            cout<< "\nDimension: ";       //Dimension of the table
            cin>>NRaya;
            N=NRaya;
            buff_envio.clear();
            buff_envio="D";
            if(N<10){      //In case the table is one character
              buff_envio+="0";
            }
            buff_envio+=to_string(N);
            write(P_socket,buff_envio.c_str(),buff_envio.size());           //Send the size of the table  
            buff_envio.clear();

            playing=1;
          }
          else if(buff[0]=='2'){        //In case u were rejected
            cout<<"You have been rejected\n";
            playing=0;
          }
          else{                   
            cout<<"The player is in another match\n";
            playing=0;
          }
        }
        else if(buff[0]=='J'){        //Request for playing with u
          n = read (P_socket, buff, 1);     //Read size of the name opponent
          size_name=atoi(buff);
          n = read (P_socket, buff, size_name);
          name=buff;
          cout<<"\n¿Do you wanna play with "<<name<<"?(Y/N)\n";
          cin >> ans;
          if(ans=="Y"){
            cout << "Choose your sign: ";
            cin >> sign; 
            playing=1;
          }
          buff_envio="J";         //Accept the game with J
          buff_envio+=ans;
          buff_envio+=to_string(size_name);
          buff_envio+=name;
          write(P_socket,buff_envio.c_str(),buff_envio.size());
          buff_envio.clear();
        }
        bzero(buff,256);
      }
      //Close connection with socket
      shutdown(P_socket, SHUT_RDWR);
      close(P_socket);
 
  }

  int main(void)
  {
    struct sockaddr_in stSockAddr;
    int Res;
    int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);   //Creating socket
    int n;
 
    if (-1 == SocketFD)
    {
      perror("cannot create socket");
      exit(EXIT_FAILURE);
    }
 
    memset(&stSockAddr, 0, sizeof(struct sockaddr_in));
 
    stSockAddr.sin_family = AF_INET;
    stSockAddr.sin_port = htons(45026); //Port
    Res = inet_pton(AF_INET, "127.0.0.1", &stSockAddr.sin_addr);
 
    if (0 > Res)
    {
      perror("error: first parameter is not a valid address family");
      close(SocketFD);
      exit(EXIT_FAILURE);
    }
    else if (0 == Res)      //No valid ip address
    {
      perror("char string (second parameter does not contain valid ipaddress");
      close(SocketFD);
      exit(EXIT_FAILURE);
    }
 
    if (-1 == connect(SocketFD, (const struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in)))    //No connection
    {
      perror("connect failed");
      close(SocketFD);
      exit(EXIT_FAILURE);
    }


    string bff="N";
    string nickname;
    string opponent;
    cout<<"Nickname: ";
    cin>>nickname;
    bff+=to_string(nickname.size());
    bff+=nickname;
    n = write(SocketFD, bff.c_str(), bff.size());     //size of Nickname + Nickname for the server
    bff.clear();
  
    char buffer[256];
    
    thread (readServer,SocketFD).detach(); //detach()
    for (;;) {
        if(playing==0){
          bff="P";      //Play with someone else
          cout<<"\nWho you wanna play with?\n";
          cin >> opponent;          
          bff+=to_string(opponent.size())+opponent;
          n = write(SocketFD, bff.c_str(), bff.size());   //P + sizeOpponent + nameOpponent
          bff.clear();
          playing=1;
        }
        else if(turn==1 && sign!=' '){        //Choose position to put your sign
            printf("\nPosition: ");
            cin >> bff; //scanf("%s", msj);
            bff+= sign;
            n = write(SocketFD, bff.c_str(), bff.size()); //g++ -o cli.exe -pthread client.cpp
            /* perform read write operations ... */
            bff.clear();
            turn=0;
        } 
    }
    shutdown(SocketFD, SHUT_RDWR);
    close(SocketFD);
    return 0;
  }
