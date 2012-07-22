 
/* server.c 
*/
#include <sys/stat.h>
       #include <fcntl.h>
#include <netinet/in.h> // où la structure sockaddr_in et la fonction inet_ntoa sont définies

#include <sys/types.h> //la socket ...
#include <sys/socket.h> //où la fonction socket est définie
#include <arpa/inet.h> //où la fonction  htons et la fonction inet_ntoa sont définies
#include <string.h> //où la fonction memset est définie
#include <stdio.h> //pour les entrées sorties
#include <sys/types.h>
#include <stdlib.h>
       
#define MAXDATASIZE 200 // taille du bufer d'entree.........
#define MYPORT 3490    // ........
#define BACKLOG 10     // 

char *authentif(char *log,char *pass)
{
  if(strcmp(log,"ftp")==0 & strcmp(pass,"ensias")==0)
   return("ok");
  return("okb");
}

void get(int new_fd)
{
    char buf[200],buf_file[300];
    if (recv(new_fd, buf, 200, 0) == -1) { // .reception  msg........
        perror("recv");
        exit(1);
    }
     int fichier,cree,x;
     fichier=open(buf,O_RDONLY);
     if (fichier<0) printf("Erreur lors de l'ouerture du fichie '%s' ...!!\n",buf);
	while((x=read(fichier,buf_file,200)))
	{
	  send(new_fd, buf_file, x, 0);
	}
     close(fichier);
}

void put(new_fd)
{
        char buf[200];
        if (recv(new_fd, buf, 200, 0)== -1) { // .reception  msg........
        perror("recv");
        exit(1);
        }  
        
        int cree,numbytes;
        cree=open(buf, O_CREAT |O_RDWR,0666);
    	while (numbytes=recv(new_fd, buf, MAXDATASIZE-1, 0)) { // .reception  msg........
  	write(cree,buf,numbytes);
  	if(numbytes < MAXDATASIZE-1) break;
    	}
        close(cree);
}

int main(void)
{
    int x,sockfd, new_fd;  //   ........
   int numbytes;
    char buf[200];
    struct sockaddr_in my_addr;    // pour définir my_addr de type la structure sockaddr_in
    struct sockaddr_in their_addr; // pour définir their_addr de type la structure sockaddr_in
    int sin_size;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) { //  
        perror("socket");
        exit(1);
    }
    my_addr.sin_family = AF_INET;         // le type de famille de my_addr est ip v4 internet protocol 
    my_addr.sin_port = htons(MYPORT);     // le port à contacter est le bit de network que htons a converti depuis host byte order 
    my_addr.sin_addr.s_addr = INADDR_ANY; /* Toutes les adresses IP de la station */
    memset(&(my_addr.sin_zero), '\0', 8); /* le 8 va pointer sur &(my_addr.sin_zero) avec le constant bit '\0'.*/

    if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr))  == -1) { //associe la socket avec un port........
        perror("bind");
        exit(1);
    }
    if (listen(sockfd, BACKLOG) == -1) { //  echec de mettre un socket en attente de connexion
        perror("listen");
        exit(1);
    }
	while(1) {  // ........boucle infinie
        sin_size = sizeof(struct sockaddr_in);
if ((new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size)) == -1) { // echec de la connexion en acceptant un appel 
            perror("accept");
            continue;
        }
	if(fork() == 0){
	 //close(sockfd);
      char log[45],pass[45];
      if ((numbytes=recv(new_fd, log, 200, 0)) == -1) { // .reception  msg........
        perror("recv");
        exit(1);
      }
      if ((numbytes=recv(new_fd, pass, 200, 0)) == -1) { // .reception  msg........
        perror("recv");
        exit(1);
      }
      char ath[4];
      strcpy(ath,authentif(log,pass));
      send(new_fd,ath,10, 0);
      if(strcmp(ath,"okb")==0)
       goto fin;
deb:       
    printf("je suis en attente d'une opération\n");
    if ((numbytes=recv(new_fd, buf, 200, 0)) == -1) { // .reception  msg........
        perror("operation");
        exit(1);
    }
   printf("il s'agit de l'opération %s\n",buf);
   if(strcmp(buf,"bye")==0)
     goto fin;
   if(strcmp(buf,"get")==0)
    get(new_fd);
   else
   if(strcmp(buf,"put")==0)
    put(new_fd);
   else 
   if(strcmp(buf,"mget")==0)
    get(new_fd);
   else
   if(strcmp(buf,"mput")==0)
    put(new_fd);
    goto deb;
    fin:printf("server: got connection from %s\n",inet_ntoa(their_addr.sin_addr));
       
    }	
}

    return 0;
}
