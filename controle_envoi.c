#include "commun.h"


// Fonction de vérification du temps ecoulé
int deadline_atteinte(int duree_max, struct timeval *debut)
{
    if (duree_max <= 0) {
        return 0;
    }

    struct timeval maintenant;
    gettimeofday(&maintenant, NULL);

    double ms_ecoules = temps_ecoule_ms(debut, &maintenant);
    double ms_limite = duree_max * 1000.0;

    if (ms_ecoules >= ms_limite) {
        printf("\nDurée maximale atteinte (%d secondes).\n", duree_max);
        return 1;
    }

    return 0;
}


// Fonction de vérification du nombre de paquet envoyés
int quota_atteint(int nb_max)
{
    if (nb_max <= 0) {
        return 0;
    }

    if (g_nb_envoyes >= nb_max) {
        return 1;
    }
    return 0;
}


// Fonction de temps d'attente
void faire_pause(double secondes)
{
    unsigned int microsec = (unsigned int)(secondes * 1000000.0);
    usleep(microsec);
}
