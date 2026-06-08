#include "commun.h"

// FONCTION DE CONVERSION Nom_Domaine --> Adresse_IP
int obtenir_ip(const char *nom, char *ip_texte, struct sockaddr_in *adresse)
{
    // Étape 1 : Recherche DNS
    struct addrinfo criteres;
    memset(&criteres, 0, sizeof(criteres));

    criteres.ai_family   = AF_INET;
    criteres.ai_socktype = SOCK_RAW;

    struct addrinfo *resultats; //resultats = la liste des adresses trouvées pour ce nom

    int code_erreur = getaddrinfo(nom, NULL, &criteres, &resultats);
    if (code_erreur != 0) {
        fprintf(stderr, "Impossible de trouver '%s' : %s\n",
                nom, gai_strerror(code_erreur));
        return -1;
    }


    // Étape 2 : Récupération de l'adresse trouvée

    //On cast le résultat en sockaddr_in (spécifique IPv4)
    struct sockaddr_in *adresse_ipv4 = (struct sockaddr_in *)resultats->ai_addr;

    // On copie toute la structure dans notre variable de sortie
    memcpy(adresse, adresse_ipv4, sizeof(struct sockaddr_in));


    // Étape 4 : Convertir l'IP binaire en texte lisible
    inet_ntop(AF_INET, &adresse_ipv4->sin_addr, ip_texte, 64);


    // Étape 5 : Libérer la mémoire des résultats DNS
    freeaddrinfo(resultats);
    return 0;
}
