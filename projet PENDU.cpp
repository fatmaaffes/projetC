#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#define CURSOR_HOME "\033[H"
#define CLEAR_SCREEN "\033[2J"
#define BG_WHITE "\033[48;2;255;255;255m"
#define BOLD       "\x1b[1m"
#define UNDERLINE  "\x1b[4m"
#define BLINK      "\x1b[5m"
#define BG_WHITE   "\x1b[47m"
#define BG_BRIGHT_WHITE   "\x1b[107m"
#define RED        "\x1b[31m"
#define RESET      "\x1b[0m"
#define GREEN      "\x1b[32m"
#define YELLOW     "\x1b[33m"
#define BLUE       "\x1b[34m"
#define MAGENTA    "\x1b[35m"
#define CYAN      "\x1b[36m"
#define WHITE     "\x1b[37m"
#define BRIGHT_MAGENTA "\x1b[95m"
#define BRIGHT_YELLOW "\x1b[95m"
#define BRIGHT_BLUE  "\x1b[94m"
#define BRIGHT_GREEN "\x1b[94m"
#define BRIGHT_RED   "\x1b[94m"
#define BRIGHT_CYAN  "\x1b[94m"
#define BRIGHT_WHITE "\x1b[94m"
#define ERROR_COLOR "\x1b[31m"
#define INFO_COLOR "\x1b[32m"
#define DEBUG_COLOR "\x1b[34m"
#define TRACE_COLOR "\x1b[36m"

// Structures
typedef struct Lettre {
    char lettre;
    struct Lettre *suivant;
} Lettre;

typedef struct Joueur {
    char nom[50];
    int score;
    int tempsTotal; // Ajout de tempsTotal pour stocker le temps total pris par le joueur
    struct Joueur *suivant;
} Joueur;

// Prototypes des fonctions
void ajouterLettre(Lettre **liste, char c);
int rechercherLettre(Lettre *liste, char c);
void afficherMotMasque(Lettre *mot, Lettre *devinees);
void afficherMotComplet(Lettre *mot);
int verifierLettre(Lettre *mot, char c);
int motDevine(Lettre *mot, Lettre *devinees);
void lireMot(Lettre **mot);
int longueurMot(Lettre *mot);
void jouerSolo(int maxTentatives);
void jouerGroupe(int maxTentatives);
void jouer(Lettre *mot, Joueur *joueurs, int mode, int maxTentatives, Joueur *currentJoueur, Joueur *arbitre);
void afficherScores(Joueur *joueurs, Joueur *arbitre);
Joueur* determinerGagnant(Joueur *joueurs, Joueur *arbitre);

// Ajoute une lettre à une liste chaînée
void ajouterLettre(Lettre **liste, char c) {
    Lettre *nouvelle = (Lettre *)malloc(sizeof(Lettre));
    nouvelle->lettre = c;
    nouvelle->suivant = *liste;
    *liste = nouvelle;
}

// Recherche une lettre dans une liste chaînée
int rechercherLettre(Lettre *liste, char c) {
    while (liste != NULL) {
        if (liste->lettre == c)
            return 1;
        liste = liste->suivant;
    }
    return 0;
}

// Affiche le mot masqué
void afficherMotMasque(Lettre *mot, Lettre *devinees) {
    while (mot != NULL)
 {
        if (rechercherLettre(devinees, mot->lettre)) {
            printf("%c ", mot->lettre);
        } else {
            printf("_ ");
        }
        mot = mot->suivant;
    }
    printf("\n");
}
// affiche le modele du hangman
void afficherHangman(int tentatives,int maxTentatives) {
    const char *hangman[] = {
        "   _____\n"
        "  |     |\n"
        "        |\n"
        "        |\n"
        "        |\n"
        "        |\n"
        "  ______|\n",

        "   _____\n"
        "  |     |\n"
        "  O     |\n"
        "        |\n"
        "        |\n"
        "        |\n"
        "  ______|\n",

        "   _____\n"
        "  |     |\n"
        "  O     |\n"
        "  |     |\n"
        "        |\n"
        "        |\n"
        "  ______|\n",

        "   _____\n"
        "  |     |\n"
        "  O     |\n"
        " /|     |\n"
        "        |\n"
        "        |\n"
        "  ______|\n",

        "   _____\n"
        "  |     |\n"
        "  O     |\n"
        " /|\\    |\n"
        "        |\n"
        "        |\n"
        "  ______|\n",

        "   _____\n"
        "  |     |\n"
        "  O     |\n"
        " /|\\    |\n"
        " /      |\n"
        "        |\n"
        "  ______|\n",

        "   _____\n"
        "  |     |\n"
        "  O     |\n"
        " /|\\    |\n"
        " / \\    |\n"
        "        |\n"
        "  ______|\n",
        "   _____\n"
        "   |     |\n"
        "   O     |\n"
        " //|\\    |\n"
        "  / \\    |\n"
        "        |\n"
        "  ______|\n",
        "   _____\n"
        "   |     |\n"
        "   O     |\n"
        " //|\\    |\n"
        " // \\    |\n"
        "        |\n"
        "  ______|\n",
        "   _____\n"
        "   |     |\n"
        "   O     |\n"
        " //|\\    |\n"
        " //| \\    |\n"
        "        |\n"
        "  ______|\n"

    };
    if (tentatives < maxTentatives) {
        printf(BRIGHT_RED"\n%s\n"RESET, hangman[tentatives]);
    } else {
        printf(BRIGHT_RED"\n%s\n"RESET, hangman[6]);
    }
}


// Affiche le mot complet.
void afficherMotComplet(Lettre *mot) {
    printf(TRACE_COLOR"Le mot a deviner etait : "RESET);
    while (mot != NULL) {
        printf("%c", mot->lettre);
        mot = mot->suivant;
    }
    printf("\n");
}

// Vérifie si une lettre est présente dans le mot
int verifierLettre(Lettre *mot, char c) {
    while (mot != NULL) {
        if (mot->lettre == c)
            return 1;
        mot = mot->suivant;
    }
    return 0;
}

// Vérifie si le mot est entièrement deviné
int motDevine(Lettre *mot, Lettre *devinees) {
    while (mot != NULL) {
        if (!rechercherLettre(devinees, mot->lettre))
            return 0;
        mot = mot->suivant;
    }
    return 1;
}

//demande a l'arbitre d'entrer un mot et le lire avec contrôle de saisie
void lireMot(Lettre **mot) {
    char texte[50];
    int valide = 0;

    while (!valide) {
        printf(INFO_COLOR"Arbitre!. Veuillez entrer le mot à deviner (attention! Compose uniquement des lettres) : "RESET);
        fgets(texte, 50, stdin);
        texte[strcspn(texte, "\n")] = '\0';

        valide = 1;
        for (int i = 0; i < strlen(texte); i++) {
            if (!isalpha(texte[i])) {
                valide = 0;
                printf(ERROR_COLOR"Erreur ! le mot doit contenir uniquement des lettres.\n"RESET);
                break;
            }
        }
    }

    for (int i = strlen(texte) - 1; i >= 0; i--) {
        ajouterLettre(mot, tolower(texte[i]));
    }
}

// determiner le Gagnant
Joueur* determinerGagnant(Joueur *joueurs, Joueur *arbitre) {
    Joueur *gagnant = NULL;
    int scoreMax = -1;

    Joueur *current = joueurs;
    do {
        if (current != arbitre && current->score > scoreMax) {
            scoreMax = current->score;
            gagnant = current;
        }
        current = current->suivant;
    } while (current != joueurs); // Boucle circulaire

    return gagnant;
}


// Mode solo : Le programme choisit des mots aléatoires
//ajout du fichier
void jouerSolo(int maxTentatives) {
    FILE *fichier = fopen("C:/Users/AFFES/CLionProjects/untitled1/bddsolo.txt", "r"); // Ouvre le fichier en mode lecture
    if (fichier == NULL) {
        printf(ERROR_COLOR"Erreur : impossible d'ouvrir le fichier 'basededonne'.\n"RESET);
        return;
    }

    // Lire les mots du fichier et les stocker dans un tableau dynamique
    char mots[200][20]; // maximum 200 mots de 20 caractères
    int nombreMots = 0;

    while (fgets(mots[nombreMots], 20, fichier) != NULL) {
        mots[nombreMots][strcspn(mots[nombreMots], "\n")] = '\0';
        nombreMots++;
        if (nombreMots >= 200) {
            break;
        }
    }

    fclose(fichier); // Fermer le fichier

    if (nombreMots == 0) {
        printf(YELLOW"Le fichier est vide ou ne contient pas de mots valides.\n"RESET);
        return;
    }

    // Choisir un mot aléatoire parmi ceux lus
    srand(time(NULL)); // Initialiser le générateur de nombres aléatoires
    int indiceAleatoire = rand() % nombreMots;
    const char *motAleatoire = mots[indiceAleatoire];

    // Construire la liste chaînée pour le mot choisi
    Lettre *mot = NULL;
    for (int i = strlen(motAleatoire) - 1; i >= 0; i--) {
        ajouterLettre(&mot, motAleatoire[i]);
    }

    printf(YELLOW"\nVous avez choisi  mode solo  !\n"RESET);
    jouer(mot, NULL, 1, maxTentatives, NULL, NULL);
}


void jouerGroupe(int maxTentatives) {
    int modeGroupe;
    int nbJoueurs;

    // Choisir le mode de jeu
    printf(INFO_COLOR"\n Veuillez entrer votre choix pour le mode groupe :\n"RESET);
    printf(TRACE_COLOR"1. Jouer avec arbitre\n"RESET);
    printf(TRACE_COLOR"2. Jouer avec des mots aleatoires\n"RESET);
    printf(CYAN" votre choix est : "RESET);
    scanf("%d", &modeGroupe);
    getchar();

    // Demander le nombre de joueurs en effectuant le controle de saisie en fonction du mode choisi
    while (1) {
        if (modeGroupe == 1) {
            // Si le mode est avec arbitre, le nombre de joueurs doit être entre 3 et 10
            printf(TRACE_COLOR" Veuillez choisir le nombre de joueurs (le nombre doit etre entre 3 et 10) : "RESET);
            scanf("%d", &nbJoueurs);
            getchar();

            if (nbJoueurs >= 3 && nbJoueurs <= 10) {
                break; // Sortir de la boucle si le nombre de joueurs est valide
            } else {
                printf(ERROR_COLOR"Erreur : Le nombre de joueurs doit être superieur a 3 et inferieur a 10.\n"RESET);
            }
        } else if (modeGroupe == 2) {
            // Si le mode est avec mots aléatoires, le nombre de joueurs doit être entre 2 et 10
            printf(TRACE_COLOR" Veuillez choisir le nombre de joueurs (le nombre doit etre entre 2 et 10) : "RESET);
            scanf("%d", &nbJoueurs);
            getchar();

            if (nbJoueurs >= 2 && nbJoueurs <= 10) {
                break; // Sortir de la boucle si le nombre de joueurs est valide
            } else {
                printf(ERROR_COLOR"Erreur : Le nombre de joueurs doit être superieur a 2 et inferieur 10.\n"RESET);
            }
        } else {
            printf(YELLOW"Choix invalide, veuillez entrer 1 pour arbitre ou 2 pour mots aléatoires.\n"RESET);
            printf(CYAN"Votre choix est  : "RESET);
            scanf("%d", &modeGroupe);
            getchar();
        }
    }

    // Création des joueurs
    Joueur *joueurs = NULL, *dernier = NULL;
    for (int i = 0; i < nbJoueurs; i++) {
        Joueur *nouveau = (Joueur *)malloc(sizeof(Joueur));
        printf(TRACE_COLOR"Entrez le nom du joueur %d : "RESET, i + 1);
        fgets(nouveau->nom, 50, stdin);
        nouveau->nom[strcspn(nouveau->nom, "\n")] = '\0';
        nouveau->score = 0;
        nouveau->suivant = NULL;

        if (joueurs == NULL) {
            joueurs = nouveau;
        } else {
            dernier->suivant = nouveau;
        }
        dernier = nouveau;
    }
    dernier->suivant = joueurs; // Rendre la liste circulaire

    Joueur *arbitre = NULL; // Désigner l'arbitre de la partie
    if (modeGroupe == 1) {
        int choixArbitre;
        printf(TRACE_COLOR"Entrez le numéro de l'arbitre (entre 1 et %d) : "RESET, nbJoueurs);
        while (1) {
            scanf("%d", &choixArbitre);
            getchar();

            if (choixArbitre >= 1 && choixArbitre <= nbJoueurs) {
                break; // Si le numéro est valide, sortir de la boucle
            } else {
                printf(ERROR_COLOR"Erreur ! Le numero de l'arbitre doit être entre 1 et %d. Veuillez réessayer : "RESET, nbJoueurs);
            }
        }

        arbitre = joueurs;
        for (int i = 1; i < choixArbitre; i++) {
            arbitre = arbitre->suivant;
        }
    }

    Joueur *currentJoueur = joueurs;
    while (1) {
        if (currentJoueur == arbitre) {
            currentJoueur = currentJoueur->suivant;
            continue;
        }

        Lettre *mot = NULL;
        if (modeGroupe == 2) {
            // Jouer avec des mots à partir d'un fichier
            FILE *fichier = fopen("C:/Users/AFFES/CLionProjects/untitled1/bddgroupe.txt", "r");
            if (fichier == NULL) {
                printf(ERROR_COLOR"Erreur : impossible d'ouvrir le fichier des mots.\n"RESET);
                return;
            }

            // Compter le nombre de mots dans le fichier
            int nbMots = 0;
            char ligne[100];
            while (fgets(ligne, sizeof(ligne), fichier)) {
                nbMots++;
            }
            rewind(fichier);

            // Choisir un mot au hasard
            srand(time(NULL));
            int indexAleatoire = rand() % nbMots;

            // Lire le mot aléatoire
            for (int i = 0; i <= indexAleatoire; i++) {
                fgets(ligne, sizeof(ligne), fichier);
            }


            ligne[strcspn(ligne, "\n")] = '\0';

            // Ajouter les lettres du mot dans la liste chaînée
            for (int i = strlen(ligne) - 1; i >= 0; i--) {
                ajouterLettre(&mot, ligne[i]);
            }
            fclose(fichier); // Fermer le fichier
        } else {

            printf(CYAN"\nL'arbitre %s va choisir  un mot pour %s.\n"RESET, arbitre->nom, currentJoueur->nom);
            lireMot(&mot);
        }

        jouer(mot, joueurs, 2, maxTentatives, currentJoueur, arbitre);

        // Exclure l'arbitre de la notion du score
        if (currentJoueur != arbitre) {
            //  mettre les scores de tous les joueurs participant sauf l'arbitre
        }

        currentJoueur = currentJoueur->suivant; // Passer au joueur suivant

            char continuer;
            printf(INFO_COLOR" si vous voulez quitter le jeu choisisez non 'n' ou 'N' sinon tapez sur n'importe quelle lettre: "RESET);
            scanf(" %c", &continuer);
            getchar();
            if (continuer == 'n' || continuer == 'N') break;


    }

    afficherScores(joueurs, arbitre);
    Joueur *gagnant = determinerGagnant(joueurs, arbitre);
    printf(BOLD BRIGHT_MAGENTA"\n Bravo ! le gagnant  est %s avec %d points!\n"RESET, gagnant->nom, gagnant->score);
}



void jouer(Lettre *mot, Joueur *joueurs, int mode, int maxTentatives, Joueur *currentJoueur, Joueur *arbitre) {
    Lettre *devinees = NULL;
    Lettre *proposees = NULL;
    int tentatives = 0;

    // Début du chronométrage
    clock_t debut = clock();

    while (tentatives < maxTentatives) {
        printf("\nMot a deviner : ");
        afficherMotMasque(mot, devinees);

        // Affiche l'état du pendu
        afficherHangman(tentatives,maxTentatives);

        char lettre;
        printf(TRACE_COLOR"Proposez une lettre : "RESET);
        scanf(" %c", &lettre);
        getchar();

        lettre = tolower(lettre);
        if (rechercherLettre(proposees, lettre)) {
            printf(YELLOW"Vous avez deja propose cette lettre.\n"RESET);
            continue;
        }

        ajouterLettre(&proposees, lettre);

        if (verifierLettre(mot, lettre)) {
            printf(GREEN"Continuez!La lettre %c est correcte !\n"RESET, lettre);
            ajouterLettre(&devinees, lettre);
            if (motDevine(mot, devinees)) {
                printf(BRIGHT_GREEN"FELICITATIONS! %s, vous avez devine le mot !\n"RESET, currentJoueur->nom);

                // Calcul du temps écoulé
                clock_t fin = clock();
                double tempsPris = (double)(fin - debut) / CLOCKS_PER_SEC;

                // Ajustement du score et du temps total
                if (currentJoueur != arbitre) {
                    currentJoueur->tempsTotal += (int)tempsPris;
                    int bonus = (int)(10 - tempsPris);
                    if (bonus < 1) bonus = 1;
                    currentJoueur->score += bonus;
                }
                break;
            }
        } else {
            printf(RED"Malheureusement ,la lettre %c est incorrecte.\n"RESET, lettre);
            if (currentJoueur != arbitre) {
                currentJoueur->score -= 1;
            }
            tentatives++;
        }

        if (tentatives == maxTentatives) {
            afficherMotComplet(mot);
        }
    }

    if (tentatives >= maxTentatives) {
        printf(BRIGHT_RED"Desole %s, vous avez epuise toutes vos tentatives.\n"RESET, currentJoueur->nom);
        afficherHangman(tentatives,maxTentatives);
    }
}





void afficherScores(Joueur *joueurs, Joueur *arbitre) {
    Joueur *joueursc = joueurs;
    printf(BRIGHT_BLUE"\n--- Scores des joueurs ---\n"RESET);
    printf(BRIGHT_BLUE"Formule de calcul du score :\n"RESET);
    printf(BRIGHT_BLUE"  - Score de base = nombre de mots devines correctement.\n"RESET);
    printf(BRIGHT_BLUE"  - Bonus de rapidite : si un mot est devine en moins de 10 secondes, un bonus de 5 points est ajoute.\n"RESET);


    do {
        if (joueursc != arbitre) { // scores des joueurs privé de l'arbitre
            printf(BRIGHT_YELLOW"Nom : %s\n"RESET, joueursc->nom);
            printf(BRIGHT_YELLOW"  - Score de base : %d points\n"RESET, joueursc->score);
            printf(BRIGHT_YELLOW"  - Temps total pris : %d msecondes\n"RESET, joueursc->tempsTotal);
            printf(BRIGHT_YELLOW"  - Détails du calcul du score :\n"RESET);
            printf(BRIGHT_YELLOW"    * Score = %d points (base sur les mots devines)\n"RESET, joueursc->score);
            if (joueursc->tempsTotal < 100) {
                printf(BRIGHT_YELLOW"    * Bonus de rapidite ajoute : 5 points\n"RESET);
                printf(BRIGHT_YELLOW"    * Score total avec bonus : %d points\n"RESET, joueursc->score + 5);
            }
            printf("\n");
        }
        joueursc = joueursc->suivant;
    } while (joueursc != joueurs);
}




int main() {
    int choix, maxTentatives;
    do {
        printf(BOLD BRIGHT_BLUE"\n--- Bienvenue au jeu du Pendu ---\n"RESET);
        printf(UNDERLINE BRIGHT_CYAN"*** c'est le temps pour commencer  ***\n"RESET);
        printf(TRACE_COLOR"Saisir le nombre maximale d'echec (Attention! le nombre ne doit pas depasser  10) : "RESET);
        scanf("%d", &maxTentatives);
        getchar();

        if (maxTentatives < 1 || maxTentatives > 10) {
            printf(ERROR_COLOR"Erreur : Le nombre d'echec doit etre entre 1 et 10.\n"RESET);
            continue;
        }

        printf(TRACE_COLOR"\nChoix1: Mode Solo\n"RESET);
        printf(TRACE_COLOR"Choix2: Mode Groupe\n"RESET);
        printf(TRACE_COLOR"Choix3: Quitter le jeu pendu.BYE!\n"RESET);
        printf(INFO_COLOR"Votre choix est: "RESET);
        scanf("%d", &choix);
        getchar();

        switch (choix) {
            case 1:
                jouerSolo(maxTentatives);
                break;
            case 2:
                jouerGroupe(maxTentatives);
                break;
            case 3:
                printf(YELLOW"Merci d'avoir joue ! A bientot.\n"RESET);
                break;
            default:
                printf(INFO_COLOR"Aucun choix n'etait saisi. Veuillez ressayer.\n"RESET);
        }
    } while (choix != 3);

    return 0;
}
