
// FONCTION DE PROTECTION DE LECTURE
#ifndef COMMUN_H
#define COMMUN_H


// LISTE DES BIBLIOTEQUES UTILISEES
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <math.h>
#include <errno.h>

#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <netdb.h>

// LISTE DES CONSTANTE UTILISEES
#define TAILLE_DONNEES_DEFAUT   56
#define TAILLE_MAX_PAQUET       1024
#define TIMEOUT_SECONDES        1


// LISTE DES VARIABLES GLOBALES
extern int    g_continuer;
extern long   g_nb_envoyes;
extern long   g_nb_recus;
extern double g_rtt_min;
extern double g_rtt_max;
extern double g_somme_rtt;
extern double g_somme_rtt_carre;
extern char   g_nom_destination[256];


// LISTE DES FONCTION APPELLER PAR FICHIER
// 1. controle_envoi.c
int  deadline_atteinte(int duree_max, struct timeval *debut);
int  quota_atteint(int nb_max);
void faire_pause(double secondes);

// 2. taille_contenu.c
unsigned char *creer_paquet_icmp(int taille_donnees, int mon_pid,
                                 int numero_sequence, int *taille_totale);

// 3. reseau.c
int  ouvrir_socket(int ttl);
int  envoyer(int socket, unsigned char *paquet, int taille,
             struct sockaddr_in *destination);
int  recevoir(int socket, int mon_pid, int numero_seq,
              int mode_silencieux, int sans_dns);

// 4. format_ip.c
int  obtenir_ip(const char *nom, char *ip_texte,
                struct sockaddr_in *adresse);

// 5. affichage.c
void afficher_aide(const char *nom_programme);
void afficher_debut(const char *destination, const char *ip, int taille);
void afficher_resultats(struct timeval *debut);

// 6. calcul.c
void   arreter_le_ping(int signal_recu);
double temps_ecoule_ms(struct timeval *debut, struct timeval *fin);
unsigned short calculer_checksum(void *donnees, int longueur);

#endif
