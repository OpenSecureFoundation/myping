#include "commun.h"

unsigned char *creer_paquet_icmp(int taille, int pid, int seq,
                                 int *taille_paquet_out)
{
    int taille_totale = (int)sizeof(struct icmphdr) + taille;
    *taille_paquet_out = taille_totale;

    // Allocattion du buffer
    unsigned char *paquet = (unsigned char *)malloc(taille_totale);
    if (!paquet) {
        perror("malloc");
        return NULL;
    }
    memset(paquet, 0, taille_totale);

    // Remplissage l'en-tête ICMP
    struct icmphdr *icmp = (struct icmphdr *)paquet;

    icmp->type             = ICMP_ECHO;
    icmp->code             = 0;
    icmp->un.echo.id       = htons((unsigned short)pid);
    icmp->un.echo.sequence = htons((unsigned short)seq);

   // Insertion du timestamp
    struct timeval *ts = (struct timeval *)(paquet + sizeof(struct icmphdr));
    gettimeofday(ts, NULL);

    int debut_remplissage = (int)sizeof(struct timeval);
    for (int i = debut_remplissage; i < taille; i++)
        paquet[sizeof(struct icmphdr) + i] = (unsigned char)i;

    icmp->checksum = calculer_checksum(paquet, taille_totale); // Insertion du checksum

    return paquet;
}
