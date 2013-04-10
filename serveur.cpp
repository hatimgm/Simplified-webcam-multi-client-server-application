#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "sock.h"
#include "sockdist.h"
#include <pthread.h>
using namespace std;

#define dimension 10


//**********************************************//

//             Fonction des couleur             //

//**********************************************//

int rand_a_b(int a, int b){
  return rand()%(b-a) +a;
}

//**********************************************//

//             Structure de la grille           //

//**********************************************//


typedef struct{ 
  bool pilotageAttente ;
  bool pilotag;
  char t[dimension+2][dimension+2];
}grille;

//**********************************************//

//             Déclarations utiles              //

//**********************************************//

int nbC;                       //Nombre des clients
int numPort;                   //Numero du port
grille g;                      //Grille g
pthread_t p[200];              //un tableau des pthread pour traiter les clients visionneur
pthread_t pilote;              //pthread pour le pilote
pthread_t piloteA;            
pthread_mutex_t mutex_modif=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_modif=PTHREAD_COND_INITIALIZER;
int i; 
int tmp=0;
int posi[2]={5,5};             //Position initiale de la grille
int descripteur[200];          //Tableau des descripteurs des visionneur
int descripteurP;
int descripteurPA;
int dir[1];                   
int p1=0;
char t[256]={"veuillez tapez o pour piloter , n pour visionner"};
char c;
time_t temp_actu;              //le temps actuel
time_t temp_fin = 2147483647;  



//*************************************************************//

//          Fonction d'initialisation de la grille             //

//*************************************************************//
grille initialiserGrille(){
  grille gri;
  gri.pilotageAttente = false;
  gri.pilotag=false;
  for(int j=0;j<(dimension+2);j++){
    for(int k=0;k<(dimension+2) ;k++){
      switch(j){
      case 0:
	gri.t[j][k]='*';
	break;
      case 5:
	if(k==5)
	  gri.t[j][k]='X';
	else
	  if(k==0 ||k==dimension +1)
	    gri.t[j][k]='*';
	  else
	    gri.t[j][k]=' ';
	break;
      case (dimension+1):
	gri.t[j][k]='*';
	break;
      default :
	switch(k){
	case 0:
	  gri.t[j][k]='*';
	  break;
	case (dimension+1):
	  gri.t[j][k]='*';
	  break;
	default:
	  gri.t[j][k]=' ';
	}
      }
    }
  }
  return gri;
}




//***************************************************************************//

//             Fonction des executer par le pthread des visonneurs           //

//***************************************************************************//

void *envoi(void *para){
  int descCourant = tmp;
  tmp++;
  if(send(descripteur[descCourant],&g,sizeof(g),0)==-1){
    perror("erreur 1 send"); 
  }
  char c[2];
  char ta[10];
  while(1){
    for(int t=0;t<3;t++){
      recv(descripteur[descCourant],&c,100*sizeof(c),0);
      ta[t]=c[0];
      c[0]='P';
    }
    if(ta[0]!='P'||ta[1]!='P'||ta[2]!='P'){
      pthread_mutex_lock(&mutex_modif);
      pthread_cond_wait(&cond_modif,&mutex_modif);
      pthread_mutex_unlock(&mutex_modif);
      send(descripteur[descCourant],&g,sizeof(g),0);
    }
    else {
      cout<<"Le visionneur  "<<(descCourant+1)<< " s'est deconnecte"<<endl;
      close(descripteur[descCourant]);
      pthread_exit(NULL);
    }    
  }
}


//***************************************************************************//

//             Fonction des executer par le pthread du pilote                //

//***************************************************************************//




void *pilotage(void *param){
  while(time(&temp_actu)<temp_fin){
    if(g.pilotageAttente == true){
      temp_fin =(time(&temp_fin)+60);
      g.pilotageAttente = false;
    }
    if(send(descripteurP,&g,sizeof(g),0)==-1) {
      perror("erreur 2 send");
      close(descripteurP);
      pthread_exit(NULL);
    }
    g.pilotageAttente = false;
    p1=recv(descripteurP,&dir,sizeof(dir),0);
    if(p1!=0){
      switch(dir[0]){
      case 8://Cas lorsque le pilote veut piloter vers le haut
	posi[0]--;
	if(posi[0]!=0)
	  g.t[posi[0]][posi[1]]='X';
	else
	  posi[0]++;
	break;
      case 2://Cas lorsque le pilote veut piloter vers le bas
	posi[0]++;
	if(posi[0]!=dimension+1)
	  g.t[posi[0]][posi[1]]='X';
	else
	  posi[0]--;
	break;
      case 4://Cas lorsque le pilote veut piloter vers la gauche
	posi[1]--;
	if(posi[1]!=0)
	  g.t[posi[0]][posi[1]]='X';
	else
	  posi[1]++;
	break;
      case 6://Cas lorsque le pilote veut piloter vers la droite
	posi[1]++;
	if(posi[1]!=dimension+1)
	  g.t[posi[0]][posi[1]]='X';
	else
	  posi[1]--;
	break;
      case 7://Cas lorsque le pilote veut piloter ver Diagonale haut-gauche
	posi[0]--;
	posi[1]--;
	if((posi[1]!=0) && (posi[0]!=0 ))
	  g.t[posi[0]][posi[1]]='X';
	else{
	  posi[0]++;
	  posi[1]++;
	}
	break;
      case 9://Cas lorsque le pilote veut piloter ver Diagonale haut-droite
	posi[0]--;
	posi[1]++;
	if((posi[1]!=dimension +1) && (posi[0]!=0))
	  g.t[posi[0]][posi[1]]='X';
	else{
	  posi[0]++;
	  posi[1]--;
	}
	break;
      case 1://Cas lorsque le pilote veut piloter ver Diagonale Bas-gauche
	posi[0]++;
	posi[1]--;
	if((posi[1]!=0) && (posi[0]!=dimension+1 ))
	  g.t[posi[0]][posi[1]]='X';
	else{
	  posi[0]--;
	  posi[1]++;
	}
	break;
      case 3://Cas lorsque le pilote veut piloter ver Diagonale Bas-droite
	posi[0]++;
	posi[1]++;
	if((posi[1]!=dimension+1) && (posi[0]!=dimension+1 ))
	  g.t[posi[0]][posi[1]]='X';
	else{
	  posi[0]--;
	  posi[1]--;
	}
	break;
      case 0:{//Cas lorsque le pilote veut quitter
	char tmpg=g.t[0][0];
	g.pilotag=false;
	g.pilotageAttente = false;
	send(descripteurPA,&g,sizeof(g),0);
	g.t[0][0]='E';
	if(send(descripteurP,&g,sizeof(g),0)==-1) {
	  perror("erreur send");
	  exit(0);
	}
	close(descripteurP);
	g.t[0][0]=tmpg;
	cout<<"Pilote s'est deconnecté "<<endl;
	pthread_exit(NULL);
	break;
      }
      case 5://Cas lorsque le pilote veut initialiser la grille
	bool p=g.pilotageAttente;
	bool k=g.pilotag;
	g=initialiserGrille();
	g.pilotag=k;
	g.pilotageAttente=p;
	posi[0]=posi[1]=5;
	break;
      }
      pthread_mutex_lock(&mutex_modif);
      pthread_cond_broadcast(&cond_modif);
      pthread_mutex_unlock(&mutex_modif);
    }
    else{//Lorsque le client s'est déconnecté
      g. pilotag=false;
      temp_fin = 2147483647;
      close(descripteurP);
      cout<<"Pilote s'est deconnecté "<<endl;
      pthread_exit(NULL);
    }
  }
  temp_fin = 2147483647;
  g.pilotag=false;
  g.pilotageAttente=false;
  send(descripteurP,&g,sizeof(g),0);
  send(descripteurPA,&g,sizeof(g),0);
  cout<<"delais dépassé---------------Pilote s'est deconnecté"<<endl;
  pthread_exit(NULL);
}

int main(int argc , char *argv[]){

//***************************************************************************//

//                    Verification des arguments d'entrée                    //

//***************************************************************************// 
  
  while(argc != 3){
    cout<<"le nombre d'argument est insuffisant\nVeuillez entrer le nombre Max des clients   et  Numero Port "<<endl;
    exit(0);
  }
  nbC = atoi(argv[1]);
  numPort = atoi(argv[2]);
  if( nbC == 0){
    cout<<"erreur veuillez entrer un ENTIER different de 0"<<endl;
    exit(0);
  }
  if( numPort < 1024){
    cout<<"erreur veuillez entrer un ENTIER pour le num Port superieur de 1024"<<endl;
    exit(0);
  }
  
  //intialiser les variable necessaires
  i=0;
  g=initialiserGrille();
  struct sockaddr_in brCv;
  socklen_t lgbrCv;
  
  //Creation de la boite reseaux publique
  Sock brPub(SOCK_STREAM,short(numPort),0);
  int descBrPub;
  if(brPub.good()){
    descBrPub=brPub.getsDesc();
    system("clear");
    cout<<"\t\033[1;31m*************Le serveur est en marche*************\033[0m"<<endl<<endl;
  }
  else{
    cout<<"erreur boite reseau serveur"<<endl;
    exit(0);
  }
  
  cout<<"\t\t\t\033[1;32mAttente des clients\033[0m"<<endl<<endl<<endl;
  while(1){
    if(i<nbC){
      time_t now = time (NULL);
      struct tm tm_now = *localtime (&now);
      char s_now[sizeof "JJ/MM/AAAA HH:MM:SS"];
      strftime (s_now, sizeof s_now, "%d/%m/%Y %H:%M:%S", &tm_now);
      listen(descBrPub,5);//Attendre les demandes de connections 5 est le nombre maximun que file d'attente peut supporter
      lgbrCv =sizeof(struct sockaddr_in);
      descripteur[i]=accept(descBrPub,(struct sockaddr *)&brCv,&lgbrCv);//accpeter la connexion
      cout<<"Client s'est connecté :"<<s_now;
      if(send(descripteur[i],&t,strlen(t)+1,0)==-1){//Envoyer un msg aux client connecté pour savoir si il veut piloter ou visionner
	perror("message non envoyé send");
	exit(0);
      }
      int pop=20;
      pop=recv(descripteur[i],&c,sizeof(c),0);
      if(pop!=0){
	char a[11];
	switch(c){
	case 'n'://le cas ou le client veut juste visionner
	  cout<<"---------------->Visionneur"<<endl;
	  a[0]='V';
	  if(send(descripteur[i],a,strlen(a)+1,0)==-1){
	    perror("message non envoyé send");
	    exit(0);
	  }
	  pthread_create(&p[i],NULL,envoi,(void *)&descripteur[i]);//Creation d'un pthread pour le client visonneur
	  i++;//incrementer le nombre des client sur le site
	  break;
	case 'o'://Le cas ou le client veut piloter et que la grille est libre
	  if(!g.pilotag) {
	    cout<<"---------------->Pilote"<<endl;
	    g.pilotag = true;//Rendre la grille inaccessible pour les client qui veulent piloter
	    a[0]='P';
	    descripteurP=descripteur[i];				
	    int okP=send(descripteurP,&a,sizeof(a),0);
	    pthread_create(&pilote,NULL,pilotage,NULL);//Thread qui s'occupe du pilote
	    
	  }
	  else {//Le cas le client veut piloter et que le grille est deja prise
	    cout<<"---------------->Pilote en attente"<<endl;
	    g.pilotageAttente=true;
	    descripteurPA=descripteur[i];
	    send(descripteurP,&g,sizeof(g),0);
	    a[0]='Q';
	    int okV1=send(descripteur[i],&a,sizeof(a),0);
	    pthread_create(&p[i],NULL,envoi,NULL);//Creation d'un pthread pour le client visonneur
	    i++;
	   
	  }
	  break;
	} 
      }
      else{//Le cas ou le client s'est deconnecté
	if(i!=0){
	  i--;
	}
	  cout<<"---------------->Le client s'est déconnecté"<<endl;
	  close(descripteur[i]);
	}
      }
    else{//Le cas ou nombre des client max est atteint
      cout<<"---------------->nombre atteint"<<endl;
      int nbatteint=accept(descBrPub,(struct sockaddr *)&brCv,&lgbrCv);
      char nbatteintmsg[256] = {"Nombre de client max atteint"} ;
      int refus = send(nbatteint,&nbatteintmsg,sizeof(nbatteintmsg),0);
      close(nbatteint);
    }
  }
}
