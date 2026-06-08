#include "commun.h"


int main(int argc, char *argv[])
{
// LECTURE DES OPTIONS

    int    nb_paquets_max = 0;
    double intervalle_sec = 1.0;
    int    taille_donnees = TAILLE_DONNEES_DEFAUT;
    int    ttl            = -1;
    int    duree_max_sec  = 0;
    int    mode_silencieux = 0;
    int    sans_dns       = 0;

    int option;
    while ((option = getopt(argc, argv, "c:i:s:t:w:qnh")) != -1) {
        switch (option) {
        case 'c':
            nb_paquets_max = atoi(optarg);
            break;
        case 'i':
            intervalle_sec = atof(optarg);
            break;
        case 's':
            taille_donnees = atoi(optarg);
            break;
        case 't':
            ttl = atoi(optarg);
            break;
        case 'w':
            duree_max_sec = atoi(optarg);
            break;
        case 'q':
            mode_silencieux = 1;
            break;
        case 'n':
            sans_dns = 1;
            break;
        case 'h':
            afficher_aide(argv[0]);
            return 0;
        default:
            afficher_aide(argv[0]);
            return 2;
        }
    }

   // Verification de la saisie d'une destination
    if (optind >= argc) {
        fprintf(stderr, "Erreur : vous devez indiquer une destination.\n\n");
        afficher_aide(argv[0]);
        return 2;
    }

    const char *destination = argv[optind];
    strncpy(g_nom_destination, destination, sizeof(g_nom_destination) - 1);

// RECHERCHE DE L'ADRESSE IP
    char               ip_texte[64];
    struct sockaddr_in adresse_dest;

    if (obtenir_ip(destination, ip_texte, &adresse_dest) < 0) {
        return 1;
    }


// OUVERTURE D'UN CANAL
    int socket_fd = ouvrir_socket(ttl);
    if (socket_fd < 0) {
        return 1;
    }
    signal(SIGINT, arreter_le_ping); // Appel la fonction "arreter_le_ping" si l'utilisateur tape "CTRL+C"'


// AFFICHAGE
    afficher_debut(destination, ip_texte, taille_donnees);

    int mon_pid = getpid() & 0xFFFF;
    int numero_sequence = 0;

    struct timeval heure_debut;
    gettimeofday(&heure_debut, NULL);


    /* ========================================================
     *  ÉTAPE 5 : BOUCLE PRINCIPALE
     *
     *  Cette boucle tourne tant que g_continuer vaut 1.
     *  g_continuer passe à 0 quand :
     *    - l'utilisateur fait Ctrl+C
     *    - on a envoyé assez de paquets (-c)
     *    - la durée max est atteinte (-w)
     * ======================================================== */
    while (g_continuer) {
        if (deadline_atteinte(duree_max_sec, &heure_debut)) {
            break; /* break = sortir de la boucle while */
        }
        if (quota_atteint(nb_paquets_max)) {
            break;
        }

        // Construction du paquet ICMP
        int taille_paquet = 0;
        unsigned char *paquet = creer_paquet_icmp(
            taille_donnees,
            mon_pid,
            numero_sequence,
            &taille_paquet
        );
        if (paquet == NULL) {
            break;
        }

        int resultat_envoi = envoyer(socket_fd, paquet, taille_paquet,
                                     &adresse_dest);
        free(paquet);
        if (resultat_envoi < 0) {
            numero_sequence++;
            continue;
        }
        recevoir(socket_fd, mon_pid, numero_sequence,
                 mode_silencieux, sans_dns);
        numero_sequence++;
        faire_pause(intervalle_sec);
    }


// FERMETURE DU SOCKET ET AFFICHAGE DE RESULTAT
    close(socket_fd);
    afficher_resultats(&heure_debut);
    if (g_nb_recus > 0) {
        return 0;
    } else {
        return 1;
    }
}
