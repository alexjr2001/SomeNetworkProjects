  /* Server code in C */
 
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
  #include <fstream>

  using namespace std;
 
  vector<int> clients;

  map<int,string> list;  //List of connected users

  void readClient (int P_socket) {
    int n, m;
    int size_destination;
    int size_msg;
    string destination;
    string msg;

    string buff_send;
    int sendSocket; //Who receives
    string nameSrc; //Source
    while (1) {
      char buff[256];
      bzero(buff,256);
      n = read (P_socket, buff, 1);
      if(buff[0]=='M'||buff[0]=='m'){     //Sending a message
        n = read (P_socket, buff, 1);
        size_destination=atoi(buff);
        n = read (P_socket, buff, 2);
        size_msg=atoi(buff);     
        bzero(buff,256);
        n = read (P_socket, buff, size_destination);
        destination=buff;
        bzero(buff,256);
        n = read (P_socket, buff, size_msg);
        msg=buff;
        for (map<int,string>::iterator it = list.begin(); it != list.end(); ++it) {   //Looking for destination
          if(it->second==destination){
            sendSocket=it->first;
          }
          else if(it->first==P_socket){
            nameSrc=it->second;
          }
        }
        buff_send="M"+to_string(nameSrc.size());
        if(msg.size()<10){  //In case length is less than 10
            buff_send+="0";
        }
        buff_send+= to_string(msg.size())+nameSrc+msg;
        m = write(sendSocket, buff_send.c_str(), buff_send.size());
        bzero(buff,256);
      }
      else if(buff[0]=='l' ||buff[0]=='L'){     //Print list of users 
        string buff_envio2="L";
        buff_send="";
        for (map<int,string>::iterator it = list.begin(); it != list.end(); ++it) {
          if(it->first!=P_socket){
            buff_send+=it->second;
            buff_send+="\n";
          }
          else{
            sendSocket=it->first;
          }
        }
        if(buff_send.size()<10) buff_envio2+="00";
        else if(buff_send.size()<100) buff_envio2+="0";
        buff_envio2+=to_string(buff_send.size());
        buff_envio2+=buff_send;
        m = write(sendSocket, buff_envio2.c_str(), buff_envio2.size()); 
        buff_envio2.clear();
      }
      else if(buff[0]=='R'||buff[0]=='r'){      //Close the session
        buff_send = "R";
        int nameSize = (list.find(P_socket)->second).size();
        buff_send += to_string(nameSize+15);
        buff_send += list.find(P_socket)->second;
        buff_send += " left the session";
        list.erase(P_socket);
        for (map<int,string>::iterator it = list.begin(); it != list.end(); ++it) {
          m = write(it->first, buff_send.c_str(), buff_send.size()); 
        }
      }
      else if(buff[0]=='F'||buff[0]=='f'){    //Send a file
        int size_nameF;     //File size
        string nameF;       //File name
        int size_file;
        string size_file9;

        n = read (P_socket, buff, 1);
        size_destination=atoi(buff);
        n = read (P_socket, buff, size_destination);
        destination=buff;
        bzero(buff,256);
        n = read (P_socket, buff, 1);
        size_nameF=atoi(buff);
        bzero(buff,256);
        n = read (P_socket, buff, size_nameF);
        nameF=buff;
        bzero(buff,256);
        n = read (P_socket, buff, 9);
        size_file9=buff;
        size_file=atoi(buff);
        bzero(buff,256);

        for (map<int,string>::iterator it = list.begin(); it != list.end(); ++it) {     //Who are you sending to
          if(it->second==destination){
            sendSocket=it->first;
          }
          else if(it->first==P_socket){
            nameSrc=it->second;
          }
        }

        //We send the file for complete chunks
        buff_send="F"+to_string(nameSrc.size())+nameSrc+to_string(nameF.size())+nameF+size_file9;
        m = write(sendSocket, buff_send.c_str(), buff_send.size());
        for(int i=0;i<size_file/256;i++){
            n = read (P_socket, buff, 256);
            m = write(sendSocket, buff, 256);
            bzero(buff,256);
        }
        
        //In case, it remains an incomplete chunks
        n = read (P_socket, buff, size_file%256);
        m = write(sendSocket, buff, 256);
        bzero(buff,256);
      }
      buff_send.clear();
    }
    shutdown(P_socket, SHUT_RDWR);
    close(P_socket);
  }

  int main(void)
  {
    struct sockaddr_in stSockAddr;
    int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    char buffer[256];
    int n;
 
    if(-1 == SocketFD)
    {
      perror("can not create socket");
      exit(EXIT_FAILURE);
    }
 
    memset(&stSockAddr, 0, sizeof(struct sockaddr_in));
 
    stSockAddr.sin_family = AF_INET;        //Mi estructura
    stSockAddr.sin_port = htons(45005);
    stSockAddr.sin_addr.s_addr = INADDR_ANY;
 
    if(-1 == bind(SocketFD,(const struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in)))  //Bind -> socket y mi estructura
    {
      perror("error bind failed");
      close(SocketFD);
      exit(EXIT_FAILURE);
    }
 
    if(-1 == listen(SocketFD, 10))
    {
      perror("error listen failed");
      close(SocketFD);
      exit(EXIT_FAILURE);
    }


    socklen_t addrlen;
    fd_set fds, readfds;
    FD_ZERO (&fds);
    FD_SET (SocketFD, &fds);
    int fdmax = SocketFD;

    string nickname;
    for(;;)       //No debe ser infinito, debe terminar.
    {
      
      readfds = fds;
      if (select (fdmax + 1, &readfds, NULL, NULL, NULL) == -1)
            perror ("select");
            // Some sockets are ready. Examine readfds
      for (int fd = 0; fd < (fdmax + 1); fd++) {
            if (FD_ISSET (fd, &readfds)) {  // fd is ready for reading 
                if (fd == SocketFD) {  // request for new connection
                    int ConnectFD = accept(SocketFD, NULL, NULL);     //fdnew
                    if(0 > ConnectFD)                                 //fdnew  
                    {
                      perror("error accept failed");
                      close(SocketFD);
                      exit(EXIT_FAILURE);
                    }

                    FD_SET (ConnectFD, &fds); 
                    if (ConnectFD > fdmax) 
                        fdmax = ConnectFD;

                    clients.push_back(ConnectFD);

                    //Insert nickname
                    char buff[256];       
                    int len_nickname;
                    bzero(buff,256);
                    n = read (ConnectFD, buff, 2);
                    if(buff[0]=='N'){
                      len_nickname = atoi(&buff[1]);
                      bzero(buff,256);
                      read(ConnectFD,buff,len_nickname);
                      list.insert(make_pair(ConnectFD,buff));
                      for (map<int,string>::iterator it = list.begin(); it != list.end(); ++it) {
                        cout<<it->first<<" "<<it->second<<endl;
                      }
                    }
                    bzero(buff,256); 
                }   
            }
            else  // data from an existing connection, receive it
            {   
                
              thread(readClient,fd).detach();
            }
      }
    
    }
    list.clear();
    close(SocketFD);
    return 0;
  }
