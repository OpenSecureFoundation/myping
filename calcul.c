#include "commun.h"



// Déclaration des variables utilisé dans le système

int    g_continuer       = 1;
long   g_nb_envoyes      = 0;
long   g_nb_recus        = 0;
double g_rtt_min         = -1;
double g_rtt_max         = 0;
double g_somme_rtt       = 0;
double g_somme_rtt_carre = 0;
char   g_nom_destination[256];


// Fonction de sortie
void arreter_le_ping(int signal_recu)
{
    (void)signal_recu;
    g_continuer = 0;
}


// Fonction de calcul du temps écoulé
double temps_ecoule_ms(struct timeval *debut, struct timeval *fin)
{
    double diff_secondes = (double)(fin->tv_sec - debut->tv_sec) * 1000.0;

    double diff_microsec = (double)(fin->tv_usec - debut->tv_usec) / 1000.0;

    return diff_secondes + diff_microsec;
}


// Fonction de calcul du checksum
unsigned short calculer_checksum(void *donnees, int longueur)
{
    unsigned short *pointeur = (unsigned short *)donnees;

    unsigned int total = 0;
    while (longueur > 1) {
        total    += *pointeur;
        pointeur++;
        longueur -= 2;
    }
    if (longueur == 1) {
        total += *(unsigned char *)pointeur;
    }

    total = (total >> 16) + (total & 0xFFFF);
    total = total + (total >> 16);

    return (unsigned short)(~total);
}
