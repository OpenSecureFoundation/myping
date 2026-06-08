
#include "commun.h"

// Fonction pour afficher l'Aide
void afficher_aide(const char *nom_programme)
{
    printf("Usage: sudo %s [options] <destination>\n\n", nom_programme);
    printf("  <destination> : adresse IP (ex: 192.168.1.1) ou nom (ex: google.com)\n\n");

    printf("Options disponibles:\n");
    printf("  -c <nombre>  Envoyer un nombre précis de paquets (ex: -c 4)\n");
    printf("  -i <secondes> Attente entre chaque paquet (ex: -i 0.5)\n");
    printf("  -s <octets>  Taille des données envoyées (ex: -s 128)\n");
    printf("  -t <valeur>  Time To Live : nombre max de routeurs traversés\n");
    printf("  -w <secondes> Durée maximale totale (ex: -w 10)\n");
    printf("  -q           Mode silencieux : affiche seulement le résumé final\n");
    printf("  -n           Ne pas résoudre les noms DNS\n");
    printf("  -h           Afficher cette aide\n\n");

    printf("Exemples:\n");
    printf("  sudo %s google.com\n", nom_programme);
    printf("  sudo %s 192.168.1.1\n", nom_programme);
    printf("  sudo %s -c 4 google.com\n", nom_programme);
    printf("  sudo %s -c 10 -i 0.5 192.168.1.1\n", nom_programme);
    printf("  sudo %s -s 128 -t 64 example.com\n", nom_programme);
}


// Fonction d'affichage

// 1. Affichage du ping a effectuer
void afficher_debut(const char *destination, const char *ip, int taille)
{

    if (strcmp(destination, ip) != 0) {
        printf("PING %s (%s) : %d octets de donnees.\n",
               destination, ip, taille);
    } else {
        printf("PING %s : %d octets de donnees.\n", ip, taille);
    }
}

// 2. Affichage du résultats
void afficher_resultats(struct timeval *debut)
{
    // Calculer la durée totale
    struct timeval fin;
    gettimeofday(&fin, NULL);
    double duree_totale_ms = temps_ecoule_ms(debut, &fin);

    printf("\n--- %s ping statistiques ---\n", g_nom_destination);

    // Calcul du nombre de paquets perdus
    long perdus = g_nb_envoyes - g_nb_recus;

    // Calcul du pourcentage de perte
    double pourcent_perte = 0.0;
    if (g_nb_envoyes > 0) {
        pourcent_perte = (double)perdus / (double)g_nb_envoyes * 100.0;
    }

    printf("%ld paquets transmis, %ld recus, %.0f%% perte, duree %.0f ms\n",
           g_nb_envoyes, g_nb_recus, pourcent_perte, duree_totale_ms);

    // Affichage de la réponse si paquet réçu
    if (g_nb_recus > 0) {
        double moyenne = g_somme_rtt / g_nb_recus;
        double variance = (g_somme_rtt_carre / g_nb_recus) - (moyenne * moyenne);

        double ecart_type = 0.0;
        if (variance > 0) {
            ecart_type = sqrt(variance);
        }

        printf("rtt min/moy/max/mdev = %.3f/%.3f/%.3f/%.3f ms\n",
               g_rtt_min, moyenne, g_rtt_max, ecart_type);
    }
}
