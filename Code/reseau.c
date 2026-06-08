#include "commun.h"

// FONCTION D'OUVERTURE D'UN SOCKET
int ouvrir_socket(int ttl)
{
    /* Créer le socket :
     *   AF_INET      = pour les adresses IPv4
     *   SOCK_RAW     = socket brut (on gère nous-mêmes les paquets)
     *   IPPROTO_ICMP = protocole ICMP (celui du ping)*/
    int socket_fd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);

    /* Si socket_fd est négatif, la création a échoué */
    if (socket_fd < 0) {
        perror("Erreur creation socket");
        if (errno == EPERM) {
            fprintf(stderr, "→ Ce programme necessite sudo\n");
        }
        return -1;
    }

    //Si l'utilisateur a demandé un TTL particulier avec -t, on l'applique au socket
    if (ttl > 0) {
        setsockopt(socket_fd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl));
    }

    return socket_fd;
}


// FONCTION D'ENVOIE
int envoyer(int socket, unsigned char *paquet, int taille,
            struct sockaddr_in *destination)
{
    ssize_t octets_envoyes = sendto(
        socket,
        paquet,
        taille,
        0,
        (struct sockaddr *)destination,
        sizeof(struct sockaddr_in)
    );

    g_nb_envoyes++;

    if (octets_envoyes < 0) {
        perror("Erreur envoi paquet");
        return -1;
    }

    return 0;
}


// FONCTION DE RECEPTION
int recevoir(int socket, int mon_pid, int numero_seq,
             int mode_silencieux, int sans_dns)
{
    struct timeval timeout;
    timeout.tv_sec  = TIMEOUT_SECONDES;
    timeout.tv_usec = 0;

    fd_set ensemble;
    FD_ZERO(&ensemble);
    FD_SET(socket, &ensemble);

    int donnees_disponibles = select(socket + 1, &ensemble, NULL, NULL, &timeout);

    if (donnees_disponibles <= 0) {
        if (!mode_silencieux) {
            printf("Pas de reponse pour icmp_seq=%d\n", numero_seq);
        }
        return 0;
    }

    unsigned char buffer[TAILLE_MAX_PAQUET];
    struct sockaddr_in expediteur;
    socklen_t taille_expediteur = sizeof(expediteur);
    struct timeval heure_reception;
    ssize_t nb_octets_recus = recvfrom(
        socket,
        buffer,
        sizeof(buffer),
        0,
        (struct sockaddr *)&expediteur,
        &taille_expediteur
    );

    gettimeofday(&heure_reception, NULL);
    if (nb_octets_recus <= 0) {
        return 0;
    }

    // Décodage du paquet
    struct ip *entete_ip = (struct ip *)buffer;
    int longueur_entete_ip = entete_ip->ip_hl * 4;

    // L'en-tête ICMP commence juste après l'en-tête IP
    struct icmphdr *entete_icmp = (struct icmphdr *)(buffer + longueur_entete_ip);

    // Vérification de la réponse
    if (entete_icmp->type != ICMP_ECHOREPLY) {
        return 0;
    }
    if (ntohs(entete_icmp->un.echo.id) != (unsigned short)mon_pid) {
        return 0;
    }

    // Calcul du RTT
    struct timeval *heure_envoi = (struct timeval *)
        (buffer + longueur_entete_ip + sizeof(struct icmphdr));
    double rtt_ms = temps_ecoule_ms(heure_envoi, &heure_reception);

    // Mise a jour des statisques
    g_nb_recus++;
    if (g_rtt_min < 0 || rtt_ms < g_rtt_min) {
        g_rtt_min = rtt_ms;
    }
    if (rtt_ms > g_rtt_max) {
        g_rtt_max = rtt_ms;
    }
    g_somme_rtt       += rtt_ms;
    g_somme_rtt_carre += rtt_ms * rtt_ms;

    // Affichage des résultats
    if (!mode_silencieux) {
        char ip_expediteur[64];
        inet_ntop(AF_INET, &expediteur.sin_addr,ip_expediteur, sizeof(ip_expediteur)); //Convertir l'IP de l'expéditeur en texte lisible

        char nom_expediteur[256] = "";
        if (!sans_dns) {
            struct hostent *info_dns = gethostbyaddr(
                &expediteur.sin_addr,
                sizeof(expediteur.sin_addr),
                AF_INET
            );
            if (info_dns != NULL) {
                strncpy(nom_expediteur, info_dns->h_name,
                        sizeof(nom_expediteur) - 1);
            }
        }

        int nb_octets = (int)nb_octets_recus - longueur_entete_ip;
        int ttl_recu  = entete_ip->ip_ttl;
        int seq_recu  = ntohs(entete_icmp->un.echo.sequence);

        if (nom_expediteur[0] != '\0') {
            printf("%d octets de %s (%s): icmp_seq=%d ttl=%d temps=%.3f ms\n",
                   nb_octets, nom_expediteur, ip_expediteur,
                   seq_recu, ttl_recu, rtt_ms);
        } else {
            printf("%d octets de %s: icmp_seq=%d ttl=%d temps=%.3f ms\n",
                   nb_octets, ip_expediteur,
                   seq_recu, ttl_recu, rtt_ms);
        }
    }
    return 1;
}
