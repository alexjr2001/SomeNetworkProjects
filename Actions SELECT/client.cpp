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
  #include <fstream>
  #include <string>
  #include <thread>

  using namespace std;
 
  void readServer (int P_socket) {
    int n;
    int tam_destination;
    int tam_msg;
    string destination;
    string msg;
    while (1) {
      char buff[256];
      n = read (P_socket, buff, 1);
      if(buff[0]=='M'||buff[0]=='m'){     //Message reception
        n = read (P_socket, buff, 1);
        tam_destination=atoi(buff);
        n = read (P_socket, buff, 2);
        tam_msg=atoi(buff);
        bzero(buff,256);
        n = read (P_socket, buff, tam_destination);
        destination=buff;
        bzero(buff,256);
        n = read (P_socket, buff, tam_msg);
        msg=buff;
        cout << "\nMessage from " << destination << ": " << msg << endl;
        bzero(buff, 256);
      }
      else if(buff[0]=='L'||buff[0]=='l'){      //List of the users
        n = read (P_socket, buff, 3);
        tam_msg=atoi(buff);
        n = read (P_socket, buff, tam_msg);
        msg=buff;
        cout << "\nUsers: \n" << msg << endl;
      }
      else if(buff[0]=='R'){          //Close the session
        n = read (P_socket, buff, 2);
        tam_msg=atoi(buff);
        n = read (P_socket, buff, tam_msg);
        msg=buff;
        cout << msg << endl;
      }
      else if(buff[0]=='F'){      //Print the file in the same path
        int tam_nameF;
        string nameF;
        int tam_file;
        string tam_file9;

        n = read (P_socket, buff, 1);
        tam_destination=atoi(buff);
        n = read (P_socket, buff, tam_destination);
        destination=buff;
        bzero(buff,256);
        n = read (P_socket, buff, 1);
        tam_nameF=atoi(buff);
        bzero(buff,256);
        n = read (P_socket, buff, tam_nameF);
        nameF=buff;
        bzero(buff,256);
        n = read (P_socket, buff, 9);
        tam_file=atoi(buff);
        bzero(buff,256);

        ofstream MyFile(nameF);             //Receiving the file   
        cout<<nameF <<" from "<<destination;
        
        for(int i=0;i<tam_file/256;i++){
            n = read (P_socket, buff, 256);
            MyFile << buff;
            bzero(buff,256);
        }
        
        n = read (P_socket, buff, tam_file%256);
        MyFile << buff;
        MyFile << " RECEIVED";

        
        // Close the file
        MyFile.close();
      }
      bzero(buff, 256);
    }
  }

  int main(void)
  {
    struct sockaddr_in stSockAddr;
    int Res;
    int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    int n;
 
    if (-1 == SocketFD)
    {
      perror("cannot create socket");
      exit(EXIT_FAILURE);
    }
 
    memset(&stSockAddr, 0, sizeof(struct sockaddr_in));
 
    stSockAddr.sin_family = AF_INET;
    stSockAddr.sin_port = htons(45005); //45000
    Res = inet_pton(AF_INET, "127.0.0.1", &stSockAddr.sin_addr);
 
    if (0 > Res)
    {
      perror("error: first parameter is not a valid address family");
      close(SocketFD);
      exit(EXIT_FAILURE);
    }
    else if (0 == Res)
    {
      perror("char string (second parameter does not contain valid ipaddress");
      close(SocketFD);
      exit(EXIT_FAILURE);
    }
 
    if (-1 == connect(SocketFD, (const struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in)))
    {
      perror("connect failed");
      close(SocketFD);
      exit(EXIT_FAILURE);
    }

    //Insert nickname
    string bff="N";
    string nickname;
    cout<<"Nickname: ";
    cin>>nickname;
    bff+=to_string(nickname.size());
    bff+=nickname;
    n = write(SocketFD, bff.c_str(), bff.size());
    bff.clear();
    char buffer[256];
    
    thread (readServer,SocketFD).detach(); //detach()
    for (;;) {
        printf("\nAction: ");       //Which action you want to perform
        cin >> bff;//scanf("%s", msg);
        if(bff=="M" || bff=="m"){     //Send a message
          string name;
          string msg;
          cout<<"\nDestination: ";      //To who
          cin>> name;
          bff+=to_string(name.size());
          cout<<"\nMessage: ";          //What message, type
          cin>>msg;
          if(msg.size()<10){
            bff+="0";
          }
          bff+=to_string(msg.size());
          bff+=name;
          bff+=msg;
        }
        else if(bff=="F"){      //Send a file
          string nameFile;
          int sizeFile;
          string name;
          cout<<"\nDestination: ";      //To who
          cin>> name;
          bff+=to_string(name.size());
          bff+=name;
          cout<<"Name of file: ";       
          cin>>nameFile;
          bff+=to_string(nameFile.size());
          bff+=nameFile;
          ifstream File (nameFile,ifstream::binary);

          File.seekg (0, File.end);       //Size of the file into buffer we complete the buffer with 0000...
          sizeFile = File.tellg();
          File.seekg (0, File.beg);
          bff+="000000000";
          int i=0;
          while(i<to_string(sizeFile).size()){
            bff.pop_back();
            i++;
          }
          bff+=to_string(sizeFile);
          i=0;

          write(SocketFD, bff.c_str(), bff.size());

          //Break the file in chunks
          for(; i+256<sizeFile;i+=256){
            File.read (buffer,256);           //WE USE CHAR* BUFFER AND NOT BFF BECAUSE .READ THE FUNCTION ONLY ACCEPTS CHAR*
            n = write(SocketFD, buffer, 256);
            bzero(buffer,256);
          }
          //The remaining incomplete chunk
          File.read(buffer,sizeFile%256);
          bff.assign(buffer,sizeFile%256);
          bzero(buffer,256);
        }
        
        n = write(SocketFD, bff.c_str(), bff.size()); //g++ -o cli.exe -pthread client.cpp
        /* perform read write operations ... */
        bff.clear();
        /*int nn = read(SocketFD, buffer, 255);
        if (n < 0) perror("ERROR reading from socket");
        printf("Answer: [%s]\n", buffer);*/
    }
    shutdown(SocketFD, SHUT_RDWR);
    close(SocketFD);
    return 0;
  }
