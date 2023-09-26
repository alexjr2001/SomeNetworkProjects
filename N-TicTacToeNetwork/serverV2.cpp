    /*TicToeServer*/  
    
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

    using namespace std;
    map<int,string> players;    //Store all the players id, and nickname
    map<int,string> playing;
    vector<string> table;

    bool play=0;
    int moves=0;
    int N=5; //N en raya

    void Ntable(){                //Draw the table
        table.clear();
        for(int i=0; i<N-1;i++){
          table.push_back("_");
          for(int j=0;j<N-1;j++){
            table[i]+="|_";
          }
        }
        table.push_back(" ");
        for(int i=0;i<N-1;i++){
          table[N-1]+="| ";
        }
    }

    bool verifyWinner(){      //Verify winner 
      bool win=0;
      
      for(int i=0;i<N;i++){     //Line horizontally
          win=1;
          for(int j=0;win && j<N+N-3;j+=2){
              if(table[i][j]!=table[i][j+2] || table[i][j]=='_' || table[i][j]==' '){
                 win=0;
              }
          }
          if(win){
           return win;
          }
      }

      for(int i=0;i<N+N-3;i+=2){  //Vertically
          win=1;
          for(int j=0;win && j<N-1;j++){
              if(table[j][i]!=table[j+1][i] || table[j][i]=='_'){
                 win=0;
              }
          }
          if(win){
            return win;
          }
      }
      
      //Diagonal
      win=1;
      for(int i=0, j=0;win&&i<N-1&&j<N+N-3;i++,j+=2){       
        if(table[i][j]!=table[i+1][j+2] || table[i][j]=='_'){
          win=0;
        }
      }
      if(win){
        return win;
      }
      win=1;
      for(int i=N-1, j=0;win&&i>0&&j<N+N-3;i--,j+=2){
        if(table[i][j]!=table[i-1][j+2] || table[i][j]=='_'){
          win=0;
        }
      }
      return win;
    }

    void readClient (int P_socket) {
      int n; 
      int row;
      int col;
      char sign;

      string opponent;
      int len_op;

      string buff_send;

      while (1) {
        char buff[256];
        bzero(buff,256);
        n = read (P_socket, buff, 1);
        if(buff[0]=='P'){       //Question for playing
          n = read (P_socket, buff, 1);
          len_op=atoi(buff);
          n = read (P_socket, buff, len_op);
          opponent=buff;
          bool encontrado=0;
          for(map<int,string>::iterator it=playing.begin();!encontrado && it!=playing.end();++it){      //In every play to which he has asked
            if(it->second==opponent){     //Answer to the match request
              buff_send="R";
              buff_send+="0";
              write(P_socket,buff_send.c_str(),buff_send.size());
              buff_send.clear();
              encontrado=true;
              break;
            }
          }
          if(!encontrado){      //Reques for playing with someone
            buff_send="J";
            buff_send+=to_string(players.find(P_socket)->second.size());
            buff_send+=players.find(P_socket)->second;
            
            for(map<int,string>::iterator it=players.begin();it!=players.end();++it){
              if(it->second==opponent){
                write(it->first,buff_send.c_str(),buff_send.size());
                break;
              }
            }
            buff_send.clear();
          }
        }
        else if(buff[0]=='D'){        //Send the dimension to the correct user
          n=read(P_socket,buff,2);
          N=atoi(buff);
          buff_send.clear();
          buff_send="D";
          if(N<10){
              buff_send+="0";
          }
          buff_send+=to_string(N);
          for(map<int,string>::iterator it=playing.begin(); it!=playing.end();++it){
              write(it->first,buff_send.c_str(),buff_send.size());
          }

          buff_send.clear();
          Ntable();
        }
        else if(buff[0]=='J'){      //Response to request
          buff_send="R";
          n=read (P_socket, buff, 1);
          if(buff[0]=='Y'){
            n=read (P_socket, buff, 1);
            len_op=atoi(buff);
            n=read (P_socket, buff, len_op);
            opponent=buff;
            bzero(buff,256);

            for(map<int,string>::iterator it=players.begin();it!=players.end();++it){
              if(it->second==opponent){
                buff_send+="1";
                write(it->first,buff_send.c_str(),buff_send.size());
                buff_send.clear();
                playing.insert(make_pair(it->first,opponent));
              }
              else if(P_socket==it->first){
                playing.insert(make_pair(P_socket,it->second));
              }
            }
            if(playing.size()==2){      //In case you were accepted
                buff[0]='1';
                write(playing.begin()->first,buff,1);
                bzero(buff,256);
            }
          }
          else{             //In case you were rejected
            buff_send+="2";
            n=read (P_socket, buff, 1);
            len_op=atoi(buff);
            n=read (P_socket, buff, len_op);
            opponent=buff;
            bzero(buff,256);
            
            for(map<int,string>::iterator it=players.begin();it!=players.end();++it){
              if(it->second==opponent){
                write(it->first,buff_send.c_str(),buff_send.size());
                break;
              }
            }
            buff_send.clear();
          }
          
        }
        else{         
          //Which is the chosen sign
          row=atoi(buff);
          n = read (P_socket, buff, 1);
          col =atoi(buff);
          n = read (P_socket, buff, 1);
          sign = buff[0];
          table[row][col*2]=sign;
          bzero(buff,256);
          //Send the table updates
          string bff_table="T";
          for(int i=0;i<N;i++){
            bff_table+=table[i];
          }
          //cout<<bff_table;
          for(map<int,string>::iterator it=playing.begin();it!=playing.end();++it){
            write(it->first,bff_table.c_str(),bff_table.size());
            if(P_socket!=it->first){
              buff[0]='1';
              write(it->first,buff,1);
            } 
          }
          bff_table.clear();
          
          moves++;
          if(verifyWinner()){
            string bff;
            for(map<int,string>::iterator it=playing.begin();it!=playing.end();++it){
              bff="W";
              if(it->first==P_socket){  //You win
                bff+="T";
                write(it->first,bff.c_str(),bff.size());
                bff.clear();
              }
              else{           //You lose
                bff+="F";
                write(it->first,bff.c_str(),bff.size());
                bff.clear();
              }  
            }
            playing.clear();
            Ntable();
            moves=0;
          }
          //DRAW
          else if(moves==N*N+1){
            string bff;
            for(map<int,string>::iterator it=playing.begin();it!=playing.end();++it){
              bff="W";
              bff+="E";
              write(it->first,bff.c_str(),bff.size());
              bff.clear();
            }
            playing.clear();
            Ntable();
            moves=0;
          }
        }
        bzero(buff,256);
      }
      shutdown(P_socket, SHUT_RDWR);
      close(P_socket);
    }


    int main(){
        Ntable();     //Initialize table
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
    
        stSockAddr.sin_family = AF_INET;
        stSockAddr.sin_port = htons(45026);
        stSockAddr.sin_addr.s_addr = INADDR_ANY;
    
        if(-1 == bind(SocketFD,(const struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in)))
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

        string nickname;
        for(;;)
        {
          int ConnectFD = accept(SocketFD, NULL, NULL);     //Connect with socket
          if(0 > ConnectFD)
          {
              perror("error accept failed");
              close(SocketFD);
              exit(EXIT_FAILURE);
          }
          
          char buff[256];
          int len_nickname;
          bzero(buff,256);
          n = read (ConnectFD, buff, 2);
          if(buff[0]=='N'){       //Insert your nickname
              len_nickname = atoi(&buff[1]);
              bzero(buff,256);
              read(ConnectFD,buff,len_nickname);
              players.insert(make_pair(ConnectFD,buff));
              
              
              bzero(buff,256);

          }
          bzero(buff,256);

          thread(readClient,ConnectFD).detach();      //Threads for reading requests
        }
        close(SocketFD);
        return 0;
    }