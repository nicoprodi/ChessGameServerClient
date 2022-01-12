/* cliTCP.c - Exemplu de client TCP 

   Trimite un nume la server; primeste de la server "Hello nume".

         

   Autor: Lenuta Alboaie  <adria@infoiasi.ro> (c)2009



*/

#include <sys/types.h>

#include <sys/socket.h>

#include <netinet/in.h>

#include <errno.h>

#include <unistd.h>

#include <stdio.h>

#include <stdlib.h>

#include <netdb.h>

#include <string.h>



extern int errno;







int port;



int main (int argc, char *argv[])

{

  int sd;			

  struct sockaddr_in server;	

  char msg[100];		





  if (argc != 3)

    {

      printf ("[client] Sintaxa: %s <adresa_server> <port>\n", argv[0]);

      return -1;

    }



  /* stabilim portul */

  port = atoi (argv[2]);



  /* cream socketul */

  if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)

    {

      perror ("[client] Eroare la socket().\n");

      return errno;

    }

  

 

  server.sin_family = AF_INET;

  server.sin_addr.s_addr = inet_addr(argv[1]);

  server.sin_port = htons (port);

  

  if (connect (sd, (struct sockaddr *) &server,sizeof (struct sockaddr)) == -1)

    {

      perror ("[client]Eroare la connect().\n");

      return errno;

    }

  

  int ok=0;

  char x[100];

  char t[150];

  



  memset(msg, ' ', 100);

  read (sd, msg, 150);

  printf ("[client]Mesajul primit este: %s\n", msg);

  read (sd, msg, 150);

  printf ("[client]Mesajul primit este: %s\n", msg);

  read (sd, msg, 150);

  printf ("[client]Mesajul primit este: %s\n", msg);

  

  if(strstr(msg, "start"))

  	ok=1;





  if(ok==1)

  {

  	read(sd, t, 150);

  	printf("Tabla %s\n", t);

  	if(strstr(msg, "alb"))

  	{

  		printf("Sunteti cu piesele albe. Incepeti:\n");

  		while(1)

  		{

  			scanf("%s", x);

  			write(sd, x, strlen(x));

  			

  			memset(msg, '\0', 99);

  			read(sd, msg, 150);

  			printf("[server] %s\n", msg);

  		}	

  			

  	}

  	else

  	if(strstr(msg, "negru"))

  	{

  		while(1)

  		{

  			scanf("%s", x);

  			write(sd, x, strlen(x));

  

  			memset(msg, '\0', 99);

  			read(sd, msg, 150);

  			printf("[server]%s\n", msg);

  		}

  	}

  }

  

  

  close (sd);

}
