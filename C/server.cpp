#include <stdio.h> 
#include <stdlib.h> /* pour exit */ 
#include <string.h> /* pour memcpy */
#include <unistd.h> 
#include <sys/types.h> 
#include <sys/socket.h> /* pour les types de socket */ 
#include <netdb.h> /* pour la structure hostent */ 
#include <errno.h> 
#include <netinet/in.h> /* pour la conversion adresse reseau->format dot 
 ainsi que le conversion format local/format reseau */ 
#include <netinet/tcp.h> /* pour la conversion adresse reseau->format dot */ 
#include <arpa/inet.h> /* pour la conversion adresse reseau->format dot */ 
#include "aes/aes.h"
#include "sha/sha256.h"
#define PORT 50000 /* Port d'ecoute de la socket serveur */ 
#define MAXSTRING 100 /* Longueur des messages */ 
int creationDunVac();
void modifvac(char id[4] , char val[25] , int zone);
void findobj(char pseudo[MAXSTRING] );
int taille( char* ta);
char salt[10] , salthash[256] ; 
int main() 
{ 

uint8_t key[] = { 0x61, 0x3d, 0xeb, 0x10, 0x15, 0xca, 0x71, 0xbe, 0x2b, 0x73, 0xae, 0xf0, 0x85, 0x7d, 0x77, 0x81,
                      0x1f, 0x35, 0x2c, 0x07, 0x3b, 0x61, 0x08, 0xd7, 0x2d, 0x98, 0x10, 0xa3, 0x09, 0x14, 0xdf, 0xf4 };
uint8_t crypt [MAXSTRING];
struct AES_ctx ctx;
AES_init_ctx(&ctx, key);

 int hSocketEcoute, /* Handle de la socket d'écoute */ 
 hSocketService; /* Handle de la socket de service connectee au client */ 
 struct hostent * infosHost; /*Infos sur le host : pour gethostbyname */ 
 struct in_addr adresseIP; /* Adresse Internet au format reseau */ 
 struct sockaddr_in adresseSocket; 
 /* Structure de type sockaddr contenant les infos adresses - ici, cas de TCP */ 
 uint tailleSockaddr_in; 
 char msgClient[MAXSTRING], msgServeur[MAXSTRING] ,  msgServeurSend[MAXSTRING] , msgHeader[25] ,  msgid[4] ,conmsg[MAXSTRING], hash[64] ;

 int nbreRecv; 
/* 1. Création de la socket */ 
hSocketEcoute= socket(AF_INET, SOCK_STREAM, 0); 
 if (hSocketEcoute== -1) 
 { 
 printf("Erreur de creation de la socket %d\n", errno); 
 exit(1); 
 } 
 else printf("Creation de la socket OK\n"); 
/* 2. Acquisition des informations sur l'ordinateur local */ 
 if ( (infosHost = gethostbyname("moon"))==0) 
 { 
 printf("Erreur d'acquisition d'infos sur le host %d\n", errno); 
 exit(1); 
 } 
 else printf("Acquisition infos host OK\n"); 
 memcpy(&adresseIP, infosHost->h_addr, infosHost->h_length); 
 printf("Adresse IP = %s\n",inet_ntoa(adresseIP)); 
 /* Conversion de l'adresse contenue dans le structure in_addr 
 en une chaine comprehensible */ 
/* 3. Préparation de la structure sockaddr_in */ 
 memset(&adresseSocket, 0, sizeof(struct sockaddr_in)); 
 adresseSocket.sin_family = AF_INET; /* Domaine */
 adresseSocket.sin_port = htons(PORT); 
 /* conversion numéro de port au format réseau _*/ 
 memcpy(&adresseSocket.sin_addr, infosHost->h_addr,infosHost->h_length); 
/* 4. Le système prend connaissance de l'adresse et du port de la socket */ 
 if (bind(hSocketEcoute, (struct sockaddr *)&adresseSocket, 
 sizeof(struct sockaddr_in)) == -1) 
 { 
 printf("Erreur sur le bind de la socket %d\n", errno); 
 exit(1); 
 } 
 else printf("Bind adresse et port socket OK\n"); 
/* 5. Mise a l'ecoute d'une requete de connexion */
 if (listen(hSocketEcoute,SOMAXCONN) == -1) 
 { 
 printf("Erreur sur lel isten de la socket %d\n", errno); 
 close(hSocketEcoute); /* Fermeture de la socket */ 
 exit(1); 
 } 
 else printf("Listen socket OK\n"); 
/* 6. Acceptation d'une connexion */ 
 tailleSockaddr_in = sizeof(struct sockaddr_in); 
 if ( (hSocketService = 
 accept(hSocketEcoute, (struct sockaddr *)&adresseSocket, &tailleSockaddr_in) ) == -1) { 
 printf("Erreur sur l'accept de la socket %d\n", errno); 
 close(hSocketEcoute); exit(1); 
 } 
 else printf("Accept socket OK\n"); 



 
 while (true){
    memset(msgClient,'\0',sizeof(msgClient));
        // printf("recption d un msg");
        /* 9. Reception d'un second message client */ 
    if ((nbreRecv = recv(hSocketService, msgClient, 4, 0)) == -1) 
    /* pas message urgent */ 
    { 
    printf("Erreur sur le recv de la socket %d\n", errno); 
    close(hSocketEcoute); /* Fermeture de la socket */ 
    close(hSocketService); /* Fermeture de la socket */ 
    exit(1); 
    }        
     printf("nbr recu = %s\n", msgClient);
    int nbrToRead = atoi(msgClient);
    if ((nbreRecv = recv(hSocketService, msgClient, nbrToRead, 0)) == -1) 
    /* pas message urgent */ 
    { 
    printf("Erreur sur le recv de la socket %d\n", errno); 
    close(hSocketEcoute); /* Fermeture de la socket */ 
    close(hSocketService); /* Fermeture de la socket */ 
    exit(1); 
    } 
        printf("Message recu = %s\n", msgClient); 
        memcpy(crypt,msgClient, MAXSTRING);
        AES_ECB_decrypt(&ctx,crypt); 
        memcpy(msgClient,crypt, MAXSTRING);
        /* 10. Envoi de l'ACK du serveur au client */ 
        // sprintf(msgServeur,"ACK pour votre message : <%s>", msgClient); 
        printf("\n\nla reponse envoyer par le client est %s\n",msgClient);
        strncpy(msgHeader,msgClient,23);
        printf("val du header %s \n",msgHeader);

        // regarder si ca commence par con:
        memcpy(conmsg,&msgClient[0],4);
        conmsg[4]= '\0';
        if( strcmp("con:",conmsg) == 0){//con:alex@pass
           // printf("il y a une tentative de co \n");
            // oui regarder pseudo -> @ 
            for (int i = 4; i < MAXSTRING-4 ; i++)
            {
                if( msgClient[i] == '@')
                    break;
                conmsg[i-4] = msgClient[i];
            }
            printf("pseudo decouper : %s \n",conmsg);
            
            // chercher le peseudo dans le fichierr 
            findobj(conmsg);
            // recup le sel recup de hash
            printf("val du salt %s\n ",salt);
            printf("val hash salt : %s \n",salthash);
            // hasher le mdp 
            int nbrcarpseudo =  strlen(conmsg) +1 ;
            for (int i = ( 4 + nbrcarpseudo ); i < MAXSTRING ; i++) {
                
                conmsg[i-4 - nbrcarpseudo] = msgClient[i];
                if( msgClient[i] == '\0')
                    break;

            }
            
            printf("lke mdp est  : %s \n",conmsg);
            strcat(conmsg,salt);


             Sha256Context ctx;
             SHA256_HASH hasher;
            Sha256Initialise( &ctx );
            Sha256Update( &ctx, (uint8_t*)conmsg, strlen(conmsg) );
            Sha256Finalise( &ctx, &hasher );
            for (int i = 0; i < 64; i = i+2)
                sprintf(&hash[i], "%02x", hasher.bytes[i/2]);
        
            printf("le hash est  %s1 \n",hash);
            printf("l autre est %s",salthash);
            // comparer les hashs si oui alors  
            if (strcmp(hash , salthash) == 0 )      
                strcpy( msgServeur ,"123456789987654321" ) ;
            else
                strcpy(msgServeur,"ERROR\0");
            
             
        }
        
        else if(strcmp("tok:123456789987654321@",msgHeader) == 0){
            memcpy(msgServeur,&msgClient[23],9);
            // creation ->
            if(strcmp(msgServeur,"creation") == 0 ){   
                int id_tmp ;
                char id_tmpp[4];
                memset(msgServeur,'\0',sizeof(msgServeur));
                id_tmp = creationDunVac();      
                snprintf(id_tmpp, 6, "%d", id_tmp);
                strcpy(msgServeur,id_tmpp);
                printf("vla %s \n",msgServeur);
            }
            else{
            // modification -> 
                memset(msgServeur,'\0',sizeof(msgServeur));
                memcpy(msgServeur,&msgClient[23],2);
                    // nom 
                
                if(msgServeur[0] == 'm' &&msgServeur[1] == 'n' ){
                memcpy(msgid,&msgClient[25],4); 
                msgid[4] = '\0'; 
                memcpy(msgServeur,&msgClient[29],25); 
                modifvac(msgid,msgServeur,0);
                }

                if(msgServeur[0] == 'm' &&msgServeur[1] == 'p' ){
                memcpy(msgid,&msgClient[25],4); 
                msgid[4] = '\0'; 
                memcpy(msgServeur,&msgClient[29],25); 
                modifvac(msgid,msgServeur,1);
                }
                if(msgServeur[0] == 'm' &&msgServeur[1] == 'd' ){
                memcpy(msgid,&msgClient[25],4); 
                msgid[4] = '\0'; 
                memcpy(msgServeur,&msgClient[29],25); 
                modifvac(msgid,msgServeur,2);
                }
                if(msgServeur[0] == 'm' &&msgServeur[1] == 'e' ){
                memcpy(msgid,&msgClient[25],4); 
                msgid[4] = '\0'; 
                memcpy(msgServeur,&msgClient[29],25); 
                modifvac(msgid,msgServeur,2);
                }
            
            }
           

        }
        else{
            strcpy(msgServeur,"ERROR\0");
        }


        printf("val client %s \n",msgClient);
        printf("val msg %s \n",msgServeur);    
        memset(msgServeurSend,'\0',sizeof(msgServeurSend));
        memcpy(crypt,msgServeur, MAXSTRING);
        AES_ECB_encrypt(&ctx, crypt);
        memcpy(msgServeur,crypt, MAXSTRING);
        int nbrcar = taille(msgServeur);
        snprintf(msgServeurSend, 6, "%4d", nbrcar);
        strcat(msgServeurSend,msgServeur);

    if (send(hSocketService, msgServeurSend, 4+nbrcar, 0) == -1) 
    { 
        printf("Erreur sur le send de la socket %d\n", errno); 
        close(hSocketEcoute); /* Fermeture de la socket */ 
        close(hSocketService); /* Fermeture de la socket */ 
        exit(1); 
    } 

    
    else printf("Send socket OK\n");
 }
 
 
/* 11. Fermeture des sockets */ 
close(hSocketService); /* Fermeture de la socket */ 
 printf("Socket connectee au client fermee\n"); 
close(hSocketEcoute); /* Fermeture de la socket */ 
 printf("Socket serveur fermee\n"); 
 return 0; 
} 


///////////////////////////////////////////////////////////
int taille( char* ta){
   int taille = 0 ;
   while (ta[taille] != '\0') {
      taille++;
   }
   return taille ;
}
   
///////////////////////////////////
///// ouverture du fichier + création d une ligne pour un nouveau vac + return id vac (int)
//////////////////////////////////
int creationDunVac(){
    char val_last_id[5] , c ; 
    int last_id ;
    FILE *fp , *fp_tmp;
    fp = fopen("./current.vac","r");
    fseek(fp, 0, SEEK_SET); 
    fread(val_last_id, sizeof(val_last_id), 1, fp);
    last_id = atoi(val_last_id);
    fp_tmp = fopen("./tmp.vac","w");
    fprintf(fp_tmp,"%4d;XXXXXXXXXXXXXXXXXXXXXXXXXXXXXX;XXXXXXXXXXXXXXXXXXXXXXXXXXXXXX;00/00/0000;N\n",++last_id);
    fseek(fp, 0, SEEK_SET); 
    while ( ( c = getc(fp)) != EOF ) { 
        fputc(c, fp_tmp);  
    }
    fclose(fp);
    fclose(fp_tmp);
    remove("./current.vac");
    rename("./tmp.vac","./current.vac");
    return last_id ;
}
///////////////////////////////////
//////id , valeur plus la zonne de mémoir et le fichier sera mit a  jour avec la bonne valeur
///////////////////////////////////
void modifvac(char id[5] , char val[25], int zone){
    printf("entre dans la fct modifiif \n ////////// \n");
    FILE *fp , *fp_tmp;
    char c ,id_read[5]  ;
    int id_int , id_source , ptvirg = 0 ;
    long ln_seek ;  
    id_source = atoi(id);  
    fp = fopen("./current.vac","r");
    fp_tmp = fopen("./tmp.vac","w");
    fseek(fp, 0, SEEK_SET);
    fseek(fp_tmp, 0, SEEK_SET);
    //lecture du 1 er id 
    fread(id_read, sizeof(id_read), 1, fp); // lecture de
    id_int = atoi(id_read); 
    fprintf(fp_tmp,"%4d",id_int);   
    fputc(';', fp_tmp);  
    
    while ( ( c = getc(fp)) != EOF ) {                   ///  |a;bcdefg
        if(c=='\n'){
           fread(id_read, sizeof(id_read), 1, fp); // lecture de l id 
           id_int = atoi(id_read);  
           fputc('\n', fp_tmp); 
           fprintf(fp_tmp,"%4d",id_int);   
           fputc(';', fp_tmp);   
           ptvirg = 0 ; 
        }
        if(c==';')
            ptvirg++;
        if(id_int == id_source && ptvirg == zone ){ 
            while ( ( c = getc(fp)) != EOF ){
                if(c==';' || c =='\n')
                    break;
            }
            if(zone != 0)    
                fputc(';', fp_tmp);  
            fputs(val,fp_tmp); 
            id_int = -1 ;
            //ptvirg = 0 ;    
                        
        }
        if(c !='\n')
            fputc(c, fp_tmp);     
    }
    fclose(fp);
    fclose(fp_tmp);

    remove("./current.vac");
    rename("./tmp.vac","./current.vac");
}

///////////////////////////////////////////////////////////////////
////////////////////////find salt
///////////////////////////////////////////////////////////////////
void findobj(char ps[MAXSTRING] ){
    printf("enter findobj fct\n ");
    FILE *fp , *fp_tmp;
    fp = fopen("./superuser","r");
    long ln_seek ; 
    char c ;
    int pos = 0 ;
    char pseudo[MAXSTRING];
    fseek(fp, 0, SEEK_SET);
    while ( ( c = getc(fp)) != EOF ) {   


        if(c==';'){
            pseudo[pos] = '\0';
            pos = 0;
            if( strcmp(pseudo , ps) ==0){
                  while ( ( c = getc(fp)) != ';'){
                    salt[pos] = c ;
                    pos++;
                  }
                  pos = 0;
                  while ( ( c = getc(fp)) != '\n'){
                    salthash[pos] = c ;
                    pos++;
                  }
                  return ; 
            }
        }
        
        pseudo[pos] = c ;
        pos ++; 

        if(c=='\n'){
            pos =0;
        }
        if(c=='\r'){
            pos =0;
        }
        
        if(pos >= MAXSTRING )
            pos = 0 ;

        


    }

    strcpy(salt, "ERROR\0");



}
///////////////////////////////////////////////////////////////////