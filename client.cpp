#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "sock.h"
#include "sockdist.h"
#include <pthread.h>
#include <time.h>
using namespace std;

#define dimension 10


//**********************************************//

//             Fonction des couleur             //

//**********************************************//

int rest= 20;
int rand_a_b(int a, int b){
    return rand()%(b-a) +a;
}


//*************************************************************//

//             Structure que le client va recevoir             //

//*************************************************************//

typedef struct {
  bool pilotageAttente;
  bool pilotag;
  char t[dimension+2][dimension+2];
}grille;

//**********************************************//

//             Déclarations utiles              //

//**********************************************//
grille g;
char t[256];
char c;
char rep[11];
int j;
int numPort;
char *NomHote;


int main(int argc , char * argv[]){
  //****************************************************//
  
  //             Vérification des arguments             //
  
  //***************************************************//
  while(argc != 3){
    cout<<"Nombre passé est insuffisant Veuillez entrer l'adresse de la hote   et leNumero Port "<<endl;
    exit(0);
  }
  
  numPort = atoi(argv[2]);
  if( numPort < 1024){
    cout<<"erreur veuillez entrer un ENTIER pour le num Port superieur de 1024"<<endl;
    exit(0);
  }
  NomHote = argv[1];
  
  j=0;
  srand(time(NULL));
  j =rand_a_b(31,37); 
  Sock brC(SOCK_STREAM,0); //Creation de la boite reseaux du client
  int descBrC;
  if(brC.good()){
    descBrC=brC.getsDesc();
  }
  else{
    cout<<"erreur BR client"<<endl;
    exit(0);
  }
  SockDist brPub(NomHote,short(numPort));//Identification du serveur
  struct sockaddr_in * adrBrPub =brPub.getAdrDist();//Contient les info sur le serveur
  int lgAdrBrPub=sizeof(struct sockaddr_in);
  int erlude=connect(descBrC,(struct sockaddr *)adrBrPub,lgAdrBrPub);//demander la connexion
  if(erlude < 0){
    perror("connect()");
    cout<<"Serveur n'est pas en marche ou saturé.Veuillez ressayer plus tard"<<endl;
    exit(0);
  }
  if(recv(descBrC,&t,sizeof(t),0)==-1){
    perror("erreur recv");
    exit(0);
  }
  if(strcmp(t,"Nombre de client max atteint")==0){//Le cas ou le serveur est saturé
    cout<<"Serveur n'est plus en marche ou saturé,Veuillez ressayer plus tard"<<endl;
    exit(0);
  }
  
  system("clear");
  cout<<"\033[1;"<<j<<"\t\tm**********Bienvenu dans notre site*********\n\n\t****"<<t<<"****\033[0m"<<endl;
  
  cin>>c;
  while(c != 'o' && c != 'n'){
    cout<<"\033[1;"<<j<<"mveuillez tapez o pour piloter , n pour visionner\033[0m"<<endl;
    cin>>c;
  }
  if(send(descBrC,&c,sizeof(char),0)==-1){
    perror("erreur send");
    exit(0);
  }
  int r=recv(descBrC,&rep,sizeof(rep),0);
  if(r==-1){
    perror("erreur recv");
    exit(0);
  }
  if(r==0){
    cout<<"\033[1;"<<j<<"m**********Serveur n'est plus en marche************** \033[0m"<<endl;
    close(descBrC);
    exit(0);
  }
  
  if(rep[0]=='V'){//Le cas ou le client veut visionner
    cout<<"\033[1;"<<j<<"m----------------->connecté en tant que visionneur\033[0m"<<endl<<endl;      
    while(rest != 0){
      rest=recv(descBrC,&g,sizeof(g),0);
      if(rest != 0){
	for(int z=0;z<(dimension+2);z++) {
	  for(int t=0;t<(dimension+2);t++) {
	    cout<<"\033[1;"<<j<<"m "<<g.t[z][t]<<"\033[0m";
	  }
	  cout<<endl;
	}
	cout<<"\033[1;"<<j<<"m************************************************ \033[0m"<<endl;
	char c[2];
	c[0]='O';
	for(int t=0;t<3;t++){
	  send(descBrC,&c,strlen(c)+1,0);
	  sleep(1);
	}
      }
      else{
	cout<<"\033[1;"<<j<<"m**********Serveur n'est plus en marche************** \033[0m"<<endl;
	close(descBrC);
	exit(0);
      }
    }
  }
  if(rep[0]=='P'){//Le cas ou le client veut piloter
    while(rest!=0){
      rest=recv(descBrC,&g,sizeof(g),0);
      if(rest != 0){
	if(g.t[0][0]=='E'){
	  cout<<"\033[1;"<<j<<"m*************Merci de votre visite*************\033[0m"<<endl;
	  close(descBrC);
	  exit(0);
	}
	if(g.pilotag){
	  if(!g.pilotageAttente){
	    for(int z=0;z<(dimension+2);z++) {
	      for(int t=0;t<(dimension+2);t++) {
		cout<<"\033[1;"<<j<<"m "<<g.t[z][t]<<"\033[0m";
	      }
	      cout<<endl;
	    }
	    cout<<endl;
	    cout<<"\033[1;"<<j<<"m***veuillez saisir la direction*** \n\t8 pour Haut \n\t2 pour Bas \n\t4 pour Gauche \n\t6 pour Droite\n\t1 pour Diagonale bas-gauche\n\t3 pour Diagonale bas-droite\n\t7 pour Diagonale haut-gauche\n\t9 pour Diagonale haut-droite\n\t*****************************\n\t5 pour renitialiser la grille\n\tA tout moment tapper 0 pour quitter\033[0m"<<endl;
	    int direction[1]={0};
	    cin>>direction[0];
	    while((direction[0]!=8)&&(direction[0]!=2)&&(direction[0]!=4)&&(direction[0]!=6)&&(direction[0]!=0)&&(direction[0]!=5)&&(direction[0]!=7)&&(direction[0]!=9)&&(direction[0]!=1)&&(direction[0]!=3)){
	      cout<<"\033[1;"<<j<<"m***Touche saisie est incorrecte a nouveau :\n\t8 pour Haut \n\t2 pour Bas \n\t4 pour Gauche \n\t6 pour Droite\n\t1 pour Diagonale bas-gauche\n\t3 pour Diagonale bas-droite\n\t7 pour Diagonale haut-gauche\n\t9 pour Diagonale haut-droite\n\t***************************** \n\t5 pour renitialiser la grille\n\tA tout moment tapper 0 quitter\033[0m"<<endl;
	      cin>>direction[0];
	    }
	    if(send(descBrC,&direction,sizeof(direction),0)==-1){
	      perror("send");
	      exit(0);
	    }
	  }
	  else{
	    cout<<"\033[1;"<<j<<"m************************************************************************************ \033[0m"<<endl<<endl;
	    cout<<"\033[1;"<<j<<"mPilotage est demandé par un autre client,vous avez 1min avant la fin de votre piotage\033[0m"<<endl<<endl;
	    cout<<"\033[1;"<<j<<"m************************************************************************************ \033[0m"<<endl<<endl;
	    sleep(2);
	  }
	}
	else{
	  cout<<"\033[1;"<<j<<"m****************************************************************************** \033[0m"<<endl<<endl;
	  cout<<"\033[1;"<<j<<"m***                          Fin du pilotage                                ***\033[0m"<<endl<<endl;
	  cout<<"\033[1;"<<j<<"m****************************************************************************** \033[0m"<<endl<<endl;
	  exit(0);
	}
      }
      else{
	cout<<"\033[1;"<<j<<"mServeur n'est pas en marche \033[0m"<<endl<<endl;
	close(descBrC);
	exit(0);
      }
    }
  }
  if(rep[0]=='Q')//Le cas ou le client veut piloter et la grille est deja prise
   {
     cout<<"\033[1;"<<j<<"m************************** ************************************************* \033[0m"<<endl<<endl;
     cout<<"\033[1;"<<j<<"m             Pilotage est deja prise vous etes à présent visionneur          \033[0m"<<endl<<endl;
     cout<<"\033[1;"<<j<<"m**************************************************************************** \033[0m"<<endl<<endl;
     while(rest != 0){
       rest=recv(descBrC,&g,sizeof(g),0);
       if(rest != 0){
	 if(g.pilotag){
	   for(int z=0;z<(dimension+2);z++) {
	     for(int t=0;t<(dimension+2);t++) {
	       cout<<"\033[1;"<<j<<"m "<<g.t[z][t]<<"\033[0m";
	     }
	     cout<<endl;
	   }
	   cout<<"\033[1;"<<j<<"m************************************************ \033[0m"<<endl;
	   char c[2];
	   c[0]='O';
	   for(int t=0;t<3;t++){
	     send(descBrC,&c,strlen(c)+1,0);
	     sleep(1);
	   }
	 }
	 else{
	   cout<<"\033[1;"<<j<<"m************************************************************* ***************** \033[0m"<<endl<<endl;
	   cout<<"\033[1;"<<j<<"mPilotage est liberé vous pouvez à présent se reconnecter pour essayer de piloter\033[0m"<<endl<<endl;
	   cout<<"\033[1;"<<j<<"m******************************************************************************* \033[0m"<<endl<<endl;
	   close(descBrC);
	   exit(0);
	 }
       }
     }
   }
}
