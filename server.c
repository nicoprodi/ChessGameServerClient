#include <sys/types.h>

#include <sys/socket.h>

#include <sys/time.h>

#include <netinet/in.h>

#include <unistd.h>

#include <errno.h>

#include <stdio.h>

#include <arpa/inet.h>

#include <string.h>

#include <stdlib.h>



/* portul folosit */



#define PORT 2728



extern int errno;		/* eroarea returnata de unele apeluri */



/* functie de convertire a adresei IP a clientului in sir de caractere */

char * conv_addr (struct sockaddr_in address)

{

  static char str[25];

  char port[7];



  /* adresa IP a clientului */

  strcpy (str, inet_ntoa (address.sin_addr));	

  /* portul utilizat de client */

  bzero (port, 7);

  printf("%s \n", str);

  sprintf (port, ":%d", ntohs (address.sin_port));	

  strcat (str, port);

  return (str);

}



#define REGEN 6

#define REGEA 60

#define REGINAN 5

#define REGINAA 50

#define PIONN 1

#define PIONA 10

#define TURAN 2

#define TURAA 20

#define CALN 3

#define CALA 30

#define NEBUNN 4

#define NABUNA 40







int checkMove(int x1, int x2, int y1, int y2, int t[10][10]);

void move(int x1, int x2, int y1, int y2, int t[10][10]);

void createTable(int t[10][10]);

void play(int client1, int client2);

void convert(int t[10][10], char* x);

int stoi(char x);



/* programul */

int

main ()

{

  struct sockaddr_in server;	/* structurile pentru server si clienti */

  struct sockaddr_in from, from2;

  int sd, client, client2;		/* descriptori de socket */

  int optval=1; 			/* optiune folosita pentru setsockopt()*/ 

  int len, len2;			/* lungimea structurii sockaddr_in */

  pid_t pid;



  /* creare socket */

  if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)

    {

      perror ("[server] Eroare la socket().\n");

      return errno;

    }



  /*setam pentru socket optiunea SO_REUSEADDR */ 

  setsockopt(sd, SOL_SOCKET, SO_REUSEADDR,&optval,sizeof(optval));



  /* pregatim structurile de date */

  bzero (&server, sizeof (server));



  /* umplem structura folosita de server */

  server.sin_family = AF_INET;

  server.sin_addr.s_addr = htonl (INADDR_ANY);

  conv_addr(server);

  server.sin_port = htons (PORT);



  /* atasam socketul */

  if (bind (sd, (struct sockaddr *) &server, sizeof (struct sockaddr)) == -1)

    {

      perror ("[server] Eroare la bind().\n");

      return errno;

    }



  /* punem serverul sa asculte daca vin clienti sa se conecteze */

  if (listen (sd, 5) == -1)

    {

      perror ("[server] Eroare la listen().\n");

      return errno;

    }

  



  printf ("[server] Asteptam la portul %d...\n", PORT);

  fflush (stdout);

        

  /* servim in mod concurent clientii... */

  while (1)

    {

	  /* a venit un client, acceptam conexiunea */

	  client = accept (sd, (struct sockaddr *) &from, &len);

	  if(client < 0)

	  {

	  	perror("[server] Eroare la accept()");

	  	continue;

	  }

	  else

	  {

	  	write(client, "Astept jucatorul nr 2\n", 22);

	  	client2 = accept (sd, (struct sockaddr *) &from2, &len2);

	  	pid = fork();

	  	if(pid == 0)

	  	{

	  		write(client, "Jucator 2 gasit\n", 16);

	  		write(client2, "Sunteti jucatorul nr 2\n", 24);

	  		write(client2, "Sunteti cu piesele negre", 25);

	  		play(client, client2);

	  		break;

	  	}

	  }

    }				

}				





void play(int client1, int client2)

{

  

  int i, j, t[10][10];

  int x1, x2, y1, y2, ok=0;

  char s1[50];

  char x[150];

  

  createTable(t);

  convert(t, x);

  printf("%s", x);

  

  write(client1, "start alb\0", 12);

  write(client2, "start negru\0", 12);

  

  while(1)

  {

  	

  	memset(x, '\0', 150);

  	convert(t, x); 

  	write(client1, x, strlen(x));

  	

  	ok=0;

  	while(ok==0)

  	{

  		ok=1;

  		

  		memset(s1, '\0', 50);

  		read(client1, s1, sizeof(s1));

  		x1=stoi(s1[0]);	

  		x2=stoi(s1[1]);

  		y1=stoi(s1[2]);

  		y2=stoi(s1[3]);

  		

  		if(t[x1][x2]>=10)

  		{

  			ok=checkMove(x1, x2, y1, y2, t);

  			if(ok==0)

  				write(client1, "Miscare incorecta\n", 18);

  		}else

  		{

  			write(client1, "Nu sunt piesele tale\n", 21);

  			ok=0;

  		}

  		printf(" Client 1 %d %d %d %d\n", x1, x2, y1, y2);

  		



    	}

    	

    	move(x1, x2, y1, y2, t);

    	convert(t, x);

    	write(client2, x, strlen(x));

    	ok=0;

    	while(ok==0)

    	{

    		ok=1;

   		

    		memset(s1, '\0', 50);

    		read(client2, s1, sizeof(s1));

  		x1=stoi(s1[0]);

  		x2=stoi(s1[1]);

  		y1=stoi(s1[2]);

  		y2=stoi(s1[3]);

  		

  		if(t[x1][x2]<10)

  		{

  			ok=checkMove(x1, x2, y1, y2, t);

  			if(ok==0)

  				write(client2, "Miscare incorecta\n", 18);

  		}else

  		{

  			write(client2, "Nu sunt piesele tale\n", 21);

  			ok=0;

  		}

  		

  		printf(" Client 2 %d %d %d %d\n", x1, x2, y1, y2);

  	}

  	move(x1, x2, y1, y2, t);

    	

  }

  

  printf("Int %d %d %d %d\n", x1, x2, y1, y2);

  

  return ;

}



int checkMove(int x1, int x2, int y1, int y2, int t[10][10])

{

	int i;

	

	if(t[x1][x2]>=10)				//alb

	{

		if(t[x1][x2]==10)			//pion

		{

			if(t[x1-1][x2]==0 && x1-1==y1 && x2==y2)

				return 1;

			if(( t[x1-1][x2-1]!=0 && t[x1-1][x2-1]<10 && x1-1==y1 && x2-1==y2) || (t[x1-1][x2+1]<10 && x1-1==y1 && x2+1==y2 && t[x1-1][x2+1]!=0))

				return 1;

		}

		if(t[x1][x2]==20)			//tura

		{

			if(x1==y1)

			{

				if(x2<y2)

					for(i=x2+1; i<y2; i++)

					{

						if(t[x1][i]!=0)

							return 0;

						if(i==y2)

							return 1;

					}

				if(t[x1][i]<10 || t[x1][i]==0)

					return 1;

				

				if(x2>y2)

					for(i=x2-1; i>y2; i=i-1)

					{

						if(t[x1][i]!=0)

							return 0;

						if(i==y2)

							return 1;

					}

				if(t[x1][i]<10 || t[x1][i]==0)

					return 1;

			

			}

			

			if(x2==y2)

			{

				if(x1<y1)

					for(i=x1+1; i<y1; i++)

					{

						if(t[i][x2]!=0)

							return 0;

						if(i==y1)

							return 1;

					}

				if(t[i][x2]<10 || t[i][x2]==0)

					return 1;

				

				if(x1>y1)

					for(i=x1-1; i>y1; i=i-1)

					{

						if(t[i][x2]!=0)

							return 0;

						if(i==y1)

							return 1;

					}

				if(t[i][x2]<10 || t[i][x2]==0)

					return 1;

			

			}

		}

		

		

		

		

	}	

	else						//negru

	{

		if(t[x1][x2]==1)			//pion

		{

			if(t[x1+1][x2]==0 && x1+1==y1 && x2==y2)

				return 1;

			if(( t[x1+1][x2-1]!=0 && t[x1+1][x2-1]>=10 && x1+1==y1 && x2-1==y2) || (t[x1+1][x2+1]>=10 && x1+1==y1 && x2+1==y2 && t[x1+1][x2+1]!=0))

				return 1;

		}

		

		if(t[x1][x2]==2)			//tura

		{

			if(x1==y1)

			{

				if(x2<y2)

					for(i=x2+1; i<y2; i++)

					{

						if(t[x1][i]!=0)

							return 0;

						if(i==y2)

							return 1;

					}

				if(t[x1][i]>10 || t[x1][i]==0)

					return 1;

				

				if(x2>y2)

					for(i=x2-1; i>y2; i=i-1)

					{

						if(t[x1][i]!=0)

							return 0;

						if(i==y2)

							return 1;

					}

				if(t[x1][i]>10 || t[x1][i]==0)

					return 1;

			

			}

			

			if(x2==y2)

			{

				if(x1<y1)

					for(i=x1+1; i<y1; i++)

					{

						if(t[i][x2]!=0)

							return 0;

						if(i==y1)

							return 1;

					}

				if(t[i][x2]>10 || t[i][x2]==0)

					return 1;

				

				if(x1>y1)

					for(i=x1-1; i>y1; i=i-1)

					{

						if(t[i][x2]!=0)

							return 0;

						if(i==y1)

							return 1;

					}

				if(t[i][x2]>10 || t[i][x2]==0)

					return 1;

			

			}

		}

	

	

	

	}	

	

	return 0;

}



void move(int x1, int x2, int y1, int y2, int t[10][10])

{

	t[y1][y2]=t[x1][x2];

	t[x1][x2]=0;

}



void convert(int t[10][10], char* x)

{

	int i, j, count=0;

	x[count]='\n';

	count++;

	for(i=1; i<=8; i++)

	{

	    for(j=1; j<=8; j++)

	    {

	       if(t[i][j]==10)

	       {   

	          x[count]='P';

	          count++;

	          x[count]=' ';

	          count++;

	       }

	       

	       if(t[i][j]==20)

	       {   

	          x[count]='T';

	          count++;

	          x[count]=' ';

	          count++;

	       }

	       

	       if(t[i][j]==30)

	       {   

	          x[count]='C';

	          count++;

	          x[count]=' ';

	          count++;

	       }

	       

	       if(t[i][j]==40)

	       {   

	          x[count]='N';

	          count++;

	          x[count]=' ';

	          count++;

	       }

	       

	       if(t[i][j]==50)

	       {   

	          x[count]='R';

	          count++;

	          x[count]=' ';

	          count++;

	       }

	       

	       if(t[i][j]==60)

	       {   

	          x[count]='Z';

	          count++;

	          x[count]=' ';

	          count++;

	       }

	       

	       if(t[i][j]==1)

	       {   

	          x[count]='p';

	          count++;

	          x[count]=' ';

	          count++;

	       }

	       

	       if(t[i][j]==2)

	       {   

	          x[count]='t';

	          count++;

	          x[count]=' ';

	          count++;

	       }

	       

	       if(t[i][j]==3)

	       {   

	          x[count]='c';

	          count++;

	          x[count]=' ';

	          count++;

	       }

	       

	       if(t[i][j]==4)

	       {   

	          x[count]='n';

	          count++;

	          x[count]=' ';

	          count++;

	       }

	       

	       if(t[i][j]==5)

	       {   

	          x[count]='r';

	          count++;

	          x[count]=' ';

	          count++;

	       }

	       

	       if(t[i][j]==6)

	       {   

	          x[count]='z';

	          count++;

	          x[count]=' ';

	          count++;

	       }

	       

	       

	       

	       if(t[i][j]==0)

	       {

	       	  x[count]='.';

	          count++;

	          x[count]=' ';

	          count++;

	       }

	      

	    }

	    x[count]='\n';

	    count++;  

	}

	x[count-1]='\0';

}



void createTable(int t[10][10])

{

	int i, j;

	for(i=0; i<=9; i++)

	    for(j=0; j<=9; j++)

	    	t[i][j]=0;

	

	for(i=0; i<=9; i++)

	{

	   t[i][0]=-1;

	   t[0][i]=-1;

	   t[i][9]=-1;

	   t[9][i]=-1;

	}

	for(i=1; i<=8; i++)

	{

	    t[2][i]=1;

	    t[7][i]=10;	

	}

	t[1][1]=2;

	t[1][8]=2;

	t[8][1]=20;

	t[8][8]=20;

	

	t[1][2]=3;

	t[1][7]=3;

	t[8][2]=30;

	t[8][7]=30;

	

	t[1][3]=4;

	t[1][6]=4;

	t[8][3]=40;

	t[8][6]=40;

	

	t[1][4]=5;

	t[1][5]=6;

	

	t[8][4]=50;

	t[8][5]=60;

}



int stoi(char x)

{

	

return (int)(x-'0');

}







