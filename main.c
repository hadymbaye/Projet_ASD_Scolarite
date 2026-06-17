#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h> /* Ajouté pour chronométrer le temps (Benchmark) */

typedef struct {
    int jour;
    int mois;
    int annee;
} Date;

typedef struct {
    int idInscription;
    char filiere[50];
    char niveau[20];
    float frais;
    Date dateInscription;
} Inscription;

typedef struct {
    int matricule;
    char nom[50];
    char *prenom; /* MODIFICATION : Pointeur pour allocation dynamique (exigé par le prof) */
    int age;
    float moyenne;
    Date dateNaissance;
    char adresse[100];
    float notes[5];
    int nbNotes;
    Inscription *inscription;
} Etudiant;

#define MAX 100000 /* Augmenté pour pouvoir tester 50 000 étudiants dans le benchmark */

Etudiant tableau[MAX];
int nbEtudiants = 0;

Etudiant **tabDynamique = NULL;
int nbEtudiantsDyn = 0;
int capacite = 0;

typedef struct Noeud {
    Etudiant data;
    struct Noeud *suivant;
    struct Noeud *precedent;
} Noeud;

Noeud *tete = NULL;
Noeud *queue = NULL;
int nbListe = 0;

/* ===================== STRUCTURE 4 : ARBRE BINAIRE (ABR) ===================== */
typedef struct NoeudABR {
    Etudiant data;
    struct NoeudABR *gauche;
    struct NoeudABR *droit;
} NoeudABR;

NoeudABR *racine = NULL;
int nbArbre = 0;

/* ===================== STRUCTURE 5 : TAS BINAIRE (File de priorité) ===================== */
#define MAX_TAS 100000 /* Augmenté pour les tests */
Etudiant tas[MAX_TAS];
int nbTas = 0;

void afficherEtudiant(Etudiant e);

/* ===================== FONCTION UTILE ===================== */
/* Permet d'allouer la mémoire exacte pour le prénom */
char* allouerChaine(char* buffer) {
    char *nouveau = malloc((strlen(buffer) + 1) * sizeof(char));
    if (nouveau != NULL) {
        strcpy(nouveau, buffer);
    }
    return nouveau;
}

/* ===================== LISTE CHAINEE ===================== */
void insererQueue() {
    Noeud *nouveau = malloc(sizeof(Noeud));
    char buffer[100]; /* Pour lire le prénom avant de l'allouer */

    if (nouveau == NULL) {
        printf("Erreur memoire !\n");
        return;
    }

    printf("\n--- Ajouter un etudiant (Liste chaînée) ---\n");
    printf("Matricule : "); 
    scanf("%d", &nouveau->data.matricule);
    printf("Nom       : "); 
    scanf("%s", nouveau->data.nom);
    
    printf("Prenom    : "); 
    scanf("%s", buffer);
    nouveau->data.prenom = allouerChaine(buffer); /* Allocation dynamique ici */

    printf("Age       : "); 
    scanf("%d", &nouveau->data.age);
    printf("Date de naissance (jj/mm/aaaa) : ");
    scanf("%d/%d/%d", &nouveau->data.dateNaissance.jour,
                      &nouveau->data.dateNaissance.mois,
                      &nouveau->data.dateNaissance.annee);
    printf("Adresse   : ");
    getchar();
    fgets(nouveau->data.adresse, 100, stdin);
    nouveau->data.adresse[strcspn(nouveau->data.adresse, "\n")] = '\0';
    printf("Combien de notes (max 5) : ");
    scanf("%d", &nouveau->data.nbNotes);
    float somme = 0;
    for (int i = 0; i < nouveau->data.nbNotes; i++) {
        printf("  Note %d : ", i + 1);
        scanf("%f", &nouveau->data.notes[i]);
        somme += nouveau->data.notes[i];
    }
    nouveau->data.moyenne = (nouveau->data.nbNotes > 0) ? somme / nouveau->data.nbNotes : 0;
    nouveau->data.inscription = NULL;

    nouveau->suivant = NULL;
    nouveau->precedent = queue;
    if (queue != NULL) {
        queue->suivant = nouveau;
    } else {
        tete = nouveau;
    }
    queue = nouveau;
    nbListe++;
    printf("[OK] Etudiant ajoute a la fin (total : %d)\n", nbListe);
}

void afficherListe() {
    if (tete == NULL) {
        printf("\nListe vide.\n"); return;
    }
    printf("\n--- Liste des %d etudiants (chaînée) ---\n", nbListe);
    Noeud *courant = tete;
    int num = 1;
    while (courant != NULL) {
        printf("\nEtudiant %d :\n", num);
        afficherEtudiant(courant->data);
        courant = courant->suivant;
        num++;
    }
}

void insererTete() {
    Noeud *nouveau = malloc(sizeof(Noeud));
    char buffer[100];

    if (nouveau == NULL) {
        printf("Erreur memoire !\n"); return;
    }
    printf("\n--- Ajouter un etudiant en tete (Liste chainee) ---\n");
    printf("Matricule : "); 
    scanf("%d", &nouveau->data.matricule);
    printf("Nom       : "); 
    scanf("%s", nouveau->data.nom);
    
    printf("Prenom    : "); 
    scanf("%s", buffer);
    nouveau->data.prenom = allouerChaine(buffer);

    printf("Age       : "); 
    scanf("%d", &nouveau->data.age);
    printf("Date de naissance (jj/mm/aaaa) : ");
    scanf("%d/%d/%d", &nouveau->data.dateNaissance.jour,
                      &nouveau->data.dateNaissance.mois,
                      &nouveau->data.dateNaissance.annee);
    printf("Adresse   : ");
    getchar();
    fgets(nouveau->data.adresse, 100, stdin);
    nouveau->data.adresse[strcspn(nouveau->data.adresse, "\n")] = '\0';
    printf("Combien de notes (max 5) : ");
    scanf("%d", &nouveau->data.nbNotes);
    float somme = 0;
    for (int i = 0; i < nouveau->data.nbNotes; i++) {
        printf("  Note %d : ", i + 1);
        scanf("%f", &nouveau->data.notes[i]);
        somme += nouveau->data.notes[i];
    }
    if (nouveau->data.nbNotes > 0) nouveau->data.moyenne = somme / nouveau->data.nbNotes;
    else nouveau->data.moyenne = 0;
    nouveau->data.inscription = NULL;
    
    nouveau->precedent = NULL;
    nouveau->suivant = tete;
    if (tete != NULL) { tete->precedent = nouveau; } 
    else { queue = nouveau; }
    tete = nouveau;
    nbListe++;
    printf("[OK] Etudiant ajoute en tete (total : %d)\n", nbListe);
}

void rechercherListe() {
    int mat;
    printf("\nMatricule a chercher dans la liste : ");
    scanf("%d", &mat);
    Noeud *courant = tete;
    while (courant != NULL) {
        if (courant->data.matricule == mat) {
            printf("\n[TROUVE DANS LA LISTE]\n");
            afficherEtudiant(courant->data);
            return;
        }
        courant = courant->suivant;
    }
    printf("[PAS TROUVE] Aucun etudiant avec matricule %d dans la liste.\n", mat);
}

void supprimerListe() {
    int mat;
    printf("\nMatricule a supprimer dans la liste : ");
    scanf("%d", &mat);
    Noeud *courant = tete;
    while (courant != NULL && courant->data.matricule != mat) { courant = courant->suivant; }
    
    if (courant == NULL) {
        printf("[PAS TROUVE] Aucun etudiant avec matricule %d\n", mat); return;
    }
    if (courant->precedent != NULL) { courant->precedent->suivant = courant->suivant; } 
    else { tete = courant->suivant; }
    if (courant->suivant != NULL) { courant->suivant->precedent = courant->precedent; } 
    else { queue = courant->precedent; }
    
    free(courant->data.prenom); /* Libérer le prénom alloué */
    free(courant);
    nbListe--;
    printf("[OK] Etudiant supprime de la liste (total : %d)\n", nbListe);
}

void afficherEtudiant(Etudiant e) {
    printf("  Matricule : %d\n", e.matricule);
    printf("  Nom       : %s %s\n", e.prenom, e.nom); /* Affiche le prenom dynamique */
    printf("  Age       : %d ans\n", e.age);
    printf("  Moyenne   : %.2f / 20\n", e.moyenne);
    printf("  Naissance : %02d/%02d/%d\n", e.dateNaissance.jour, e.dateNaissance.mois, e.dateNaissance.annee);
    printf("  Adresse   : %s\n", e.adresse);
    printf("  Notes     : ");
    for (int i = 0; i < e.nbNotes; i++) {
        printf("%.2f  ", e.notes[i]);
    }
    printf("\n");
}

/* ===================== TABLEAU STATIQUE ===================== */
void ajouterEtudiant() {
    char buffer[100];
    if (nbEtudiants >= MAX) { printf("Tableau plein !\n"); return; }
    Etudiant e;
    printf("\n--- Ajouter un etudiant ---\n");
    printf("Matricule : "); scanf("%d", &e.matricule);
    printf("Nom       : "); scanf("%s", e.nom);
    
    printf("Prenom    : "); scanf("%s", buffer);
    e.prenom = allouerChaine(buffer);

    printf("Age       : "); scanf("%d", &e.age);
    printf("Date de naissance (jj/mm/aaaa) : ");
    scanf("%d/%d/%d", &e.dateNaissance.jour, &e.dateNaissance.mois, &e.dateNaissance.annee);
    printf("Adresse   : ");
    getchar();  
    fgets(e.adresse, 100, stdin);
    e.adresse[strcspn(e.adresse, "\n")] = '\0';
    printf("Combien de notes (max 5) : ");
    scanf("%d", &e.nbNotes);
    float somme = 0;
    for (int i = 0; i < e.nbNotes; i++) {
        printf("  Note %d : ", i + 1);
        scanf("%f", &e.notes[i]);
        somme += e.notes[i];
    }
    if (e.nbNotes > 0) { e.moyenne = somme / e.nbNotes; } 
    else { e.moyenne = 0; }
    e.inscription = NULL;
    tableau[nbEtudiants] = e;
    nbEtudiants++;
    printf("\n[OK] Etudiant ajoute ! (total : %d)\n", nbEtudiants);
}

void afficherTous() {
    if (nbEtudiants == 0) { printf("\nAucun etudiant.\n"); return; }
    printf("\n--- Liste des %d etudiants ---\n", nbEtudiants);
    for (int i = 0; i < nbEtudiants; i++) {
        printf("\nEtudiant %d :\n", i + 1);
        afficherEtudiant(tableau[i]);
    }
}

void rechercherParMatricule() {
    int mat;
    printf("\nMatricule a chercher : "); scanf("%d", &mat);
    for (int i = 0; i < nbEtudiants; i++) {
        if (tableau[i].matricule == mat) {
            printf("\n[TROUVE] :\n"); afficherEtudiant(tableau[i]); return;
        }
    }
    printf("\n[PAS TROUVE] Aucun etudiant avec matricule %d\n", mat);
}

void supprimerEtudiant() {
    int mat;
    printf("\nMatricule a supprimer : "); scanf("%d", &mat);
    int index = -1;
    for (int i = 0; i < nbEtudiants; i++) {
        if (tableau[i].matricule == mat) { index = i; break; }
    }
    if (index == -1) { printf("[PAS TROUVE]\n"); return; }
    free(tableau[index].prenom); /* Libérer mémoire */
    for (int i = index; i < nbEtudiants - 1; i++) { tableau[i] = tableau[i + 1]; }
    nbEtudiants--;
    printf("[OK] Etudiant supprime ! (total : %d)\n", nbEtudiants);
}

void modifierEtudiant() {
    int mat;
    printf("\nMatricule a modifier : "); scanf("%d", &mat);
    int index = -1;
    for (int i = 0; i < nbEtudiants; i++) {
        if (tableau[i].matricule == mat) { index = i; break; }
    }
    if (index == -1) { printf("[PAS TROUVE]\n"); return; }
    printf("\n--- Modifier l'etudiant ---\n"); afficherEtudiant(tableau[index]);
    printf("\nNouvelle moyenne : "); scanf("%f", &tableau[index].moyenne);
    printf("Nouvel age : "); scanf("%d", &tableau[index].age);
    printf("[OK] Etudiant modifie !\n");
}

void rechercherParIntervalle() {
    float min, max;
    printf("\nMoyenne min : "); scanf("%f", &min);
    printf("Moyenne max : "); scanf("%f", &max);
    int trouve = 0;
    for (int i = 0; i < nbEtudiants; i++) {
        if (tableau[i].moyenne >= min && tableau[i].moyenne <= max) {
            printf("\n--- Etudiant %d ---\n", i+1);
            afficherEtudiant(tableau[i]); trouve++;
        }
    }
    if (trouve == 0) printf("\nAucun etudiant dans cet intervalle.\n");
    else printf("=> %d etudiant(s) trouve(s).\n", trouve);
}

void rechercherParPrefixe() {
    char prefixe[50];
    printf("\nDebut du nom : "); scanf("%s", prefixe);
    int trouve = 0;
    for (int i = 0; i < nbEtudiants; i++) {
        if (strncmp(tableau[i].nom, prefixe, strlen(prefixe)) == 0) {
            printf("\n--- Etudiant %d ---\n", i+1);
            afficherEtudiant(tableau[i]); trouve++;
        }
    }
    if (trouve == 0) printf("\nAucun nom ne commence par '%s'.\n", prefixe);
    else printf("=> %d etudiant(s) trouve(s).\n", trouve);
}

void triInsertion() {
    for (int i = 1; i < nbEtudiants; i++) {
        Etudiant enCours = tableau[i];   
        int j = i - 1;
        while (j >= 0 && tableau[j].moyenne > enCours.moyenne) {
            tableau[j + 1] = tableau[j]; j--;
        }
        tableau[j + 1] = enCours;
    }
    printf("[OK] Etudiants tries par moyenne (croissant).\n");
}

void triRapide(int debut, int fin) {
    if (debut >= fin) return;
    int pivot = tableau[fin].matricule;   
    int i = debut - 1;                    
    for (int j = debut; j < fin; j++) {
        if (tableau[j].matricule < pivot) {
            i++;
            Etudiant tmp = tableau[i];
            tableau[i] = tableau[j];
            tableau[j] = tmp;
        }
    }
    Etudiant tmp = tableau[i + 1];
    tableau[i + 1] = tableau[fin];
    tableau[fin] = tmp;
    triRapide(debut, i);
    triRapide(i + 2, fin);
}

void statistiques() {
    if (nbEtudiants == 0) { printf("Aucun etudiant, pas de stats.\n"); return; }
    float min = tableau[0].moyenne, max = tableau[0].moyenne, somme = 0;
    for (int i = 0; i < nbEtudiants; i++) {
        if (tableau[i].moyenne < min) min = tableau[i].moyenne;
        if (tableau[i].moyenne > max) max = tableau[i].moyenne;
        somme += tableau[i].moyenne;
    }
    float moyenne = somme / nbEtudiants;
    float sommeDiffCarre = 0;
    for (int i = 0; i < nbEtudiants; i++) {
        float difference = tableau[i].moyenne - moyenne;
        sommeDiffCarre += difference * difference;
    }
    float ecartType = sqrt(sommeDiffCarre / nbEtudiants);
    
    /* Copie temporaire pour calculer la médiane sans détruire le tableau */
    Etudiant* temp = malloc(nbEtudiants * sizeof(Etudiant));
    for(int i=0; i<nbEtudiants; i++) temp[i] = tableau[i];
    
    for (int i = 1; i < nbEtudiants; i++) {
        Etudiant enCours = temp[i]; int j = i - 1;
        while (j >= 0 && temp[j].moyenne > enCours.moyenne) { temp[j + 1] = temp[j]; j--; }
        temp[j + 1] = enCours;
    }
    
    float mediane = (nbEtudiants % 2 == 1) ? temp[nbEtudiants / 2].moyenne : (temp[nbEtudiants / 2 - 1].moyenne + temp[nbEtudiants / 2].moyenne) / 2.0;
    free(temp);
    
    printf("\n========== STATISTIQUES ==========\n");
    printf("Nombre d'etudiants  : %d\n", nbEtudiants);
    printf("Minimum             : %.2f\n", min);
    printf("Maximum             : %.2f\n", max);
    printf("Moyenne             : %.2f\n", moyenne);
    printf("Mediane             : %.2f\n", mediane);
    printf("Ecart-type          : %.2f\n", ecartType);
    printf("==================================\n");
}

void sauvegarder() {
    FILE *f = fopen("etudiants.dat", "wb");   
    if (f == NULL) { printf("Erreur fichier.\n"); return; }
    fwrite(&nbEtudiants, sizeof(int), 1, f);
    for(int i=0; i<nbEtudiants; i++) {
        fwrite(&tableau[i].matricule, sizeof(int), 1, f);
        fwrite(tableau[i].nom, sizeof(char), 50, f);
        int len = strlen(tableau[i].prenom);
        fwrite(&len, sizeof(int), 1, f);
        fwrite(tableau[i].prenom, sizeof(char), len+1, f);
        fwrite(&tableau[i].age, sizeof(int), 1, f);
        fwrite(&tableau[i].moyenne, sizeof(float), 1, f);
        fwrite(&tableau[i].dateNaissance, sizeof(Date), 1, f);
        fwrite(tableau[i].adresse, sizeof(char), 100, f);
        fwrite(tableau[i].notes, sizeof(float), 5, f);
        fwrite(&tableau[i].nbNotes, sizeof(int), 1, f);
    }
    fclose(f);
    printf("[OK] Sauvegarde terminee.\n");
}

void charger() {
    FILE *f = fopen("etudiants.dat", "rb");
    if (f == NULL) { printf("Aucun fichier.\n"); return; }
    fread(&nbEtudiants, sizeof(int), 1, f);
    for(int i=0; i<nbEtudiants; i++) {
        fread(&tableau[i].matricule, sizeof(int), 1, f);
        fread(tableau[i].nom, sizeof(char), 50, f);
        int len; fread(&len, sizeof(int), 1, f);
        tableau[i].prenom = malloc((len+1)*sizeof(char));
        fread(tableau[i].prenom, sizeof(char), len+1, f);
        fread(&tableau[i].age, sizeof(int), 1, f);
        fread(&tableau[i].moyenne, sizeof(float), 1, f);
        fread(&tableau[i].dateNaissance, sizeof(Date), 1, f);
        fread(tableau[i].adresse, sizeof(char), 100, f);
        fread(tableau[i].notes, sizeof(float), 5, f);
        fread(&tableau[i].nbNotes, sizeof(int), 1, f);
        tableau[i].inscription = NULL;
    }
    fclose(f);
    printf("[OK] %d etudiant(s) charge(s).\n", nbEtudiants);
}

/* ===================== TABLEAU DYNAMIQUE ===================== */
void ajouterDynamique() {
    char buffer[100];
    if (nbEtudiantsDyn >= capacite) {
        int nouvelleCapacite = (capacite == 0) ? 2 : capacite * 2;  
        Etudiant **temp = realloc(tabDynamique, nouvelleCapacite * sizeof(Etudiant*));
        if (temp == NULL) { printf("Erreur memoire !\n"); return; }
        tabDynamique = temp; capacite = nouvelleCapacite;
    }
    Etudiant *e = malloc(sizeof(Etudiant));  
    if (e == NULL) { printf("Erreur !\n"); return; }
    
    printf("\n--- Ajouter un etudiant (Dynamique) ---\n");
    printf("Matricule : "); scanf("%d", &e->matricule);
    printf("Nom       : "); scanf("%s", e->nom);
    printf("Prenom    : "); scanf("%s", buffer); e->prenom = allouerChaine(buffer);
    printf("Age       : "); scanf("%d", &e->age);
    printf("Date (jj/mm/aaaa) : "); scanf("%d/%d/%d", &e->dateNaissance.jour, &e->dateNaissance.mois, &e->dateNaissance.annee);
    printf("Adresse   : "); getchar(); fgets(e->adresse, 100, stdin); e->adresse[strcspn(e->adresse, "\n")] = '\0';
    printf("Combien de notes (max 5) : "); scanf("%d", &e->nbNotes);
    float somme = 0;
    for (int i = 0; i < e->nbNotes; i++) {
        printf("  Note %d : ", i + 1); scanf("%f", &e->notes[i]); somme += e->notes[i];
    }
    e->moyenne = (e->nbNotes > 0) ? somme / e->nbNotes : 0;
    e->inscription = NULL;
    tabDynamique[nbEtudiantsDyn] = e; nbEtudiantsDyn++;
    printf("[OK] Ajoute.\n");
}

void afficherDynamique() {
    if (nbEtudiantsDyn == 0) { printf("Aucun etudiant.\n"); return; }
    for (int i = 0; i < nbEtudiantsDyn; i++) {
        printf("\nEtudiant %d :\n", i + 1); afficherEtudiant(*tabDynamique[i]);  
    }
}

void rechercherMatriculeDyn() {
    int mat; printf("\nMatricule a chercher : "); scanf("%d", &mat);
    for (int i = 0; i < nbEtudiantsDyn; i++) {
        if (tabDynamique[i]->matricule == mat) { printf("\n[TROUVE] :\n"); afficherEtudiant(*tabDynamique[i]); return; }
    }
    printf("[PAS TROUVE]\n");
}

void supprimerDynamique() {
    int mat; printf("\nMatricule a supprimer : "); scanf("%d", &mat);
    int index = -1;
    for (int i = 0; i < nbEtudiantsDyn; i++) {
        if (tabDynamique[i]->matricule == mat) { index = i; break; }
    }
    if (index == -1) { printf("[PAS TROUVE]\n"); return; }
    free(tabDynamique[index]->prenom); free(tabDynamique[index]);
    for (int i = index; i < nbEtudiantsDyn - 1; i++) { tabDynamique[i] = tabDynamique[i + 1]; }
    nbEtudiantsDyn--;
    printf("[OK] Etudiant supprime\n");
}

void rechercherIntervalleDyn() {
    float min, max; printf("\nMoyenne min : "); scanf("%f", &min); printf("Moyenne max : "); scanf("%f", &max);
    int trouve = 0;
    for (int i = 0; i < nbEtudiantsDyn; i++) {
        if (tabDynamique[i]->moyenne >= min && tabDynamique[i]->moyenne <= max) { afficherEtudiant(*tabDynamique[i]); trouve++; }
    }
    if (trouve == 0) printf("Aucun etudiant.\n");
}

void triInsertionDyn() {
    for (int i = 1; i < nbEtudiantsDyn; i++) {
        Etudiant *enCours = tabDynamique[i]; int j = i - 1;
        while (j >= 0 && tabDynamique[j]->moyenne > enCours->moyenne) { tabDynamique[j + 1] = tabDynamique[j]; j--; }
        tabDynamique[j + 1] = enCours;
    }
    printf("[OK] Trie par moyenne.\n");
}

void triRapideDyn(int debut, int fin) {
    if (debut >= fin) return;
    int pivot = tabDynamique[fin]->matricule; int i = debut - 1;
    for (int j = debut; j < fin; j++) {
        if (tabDynamique[j]->matricule < pivot) {
            i++; Etudiant *tmp = tabDynamique[i]; tabDynamique[i] = tabDynamique[j]; tabDynamique[j] = tmp;
        }
    }
    Etudiant *tmp = tabDynamique[i + 1]; tabDynamique[i + 1] = tabDynamique[fin]; tabDynamique[fin] = tmp;
    triRapideDyn(debut, i); triRapideDyn(i + 2, fin);
}

void statistiquesDyn() {
    printf("Logique des statistiques Dynamique (identique au statique)\n");
}

/* ===================== ABR ===================== */
NoeudABR* insererABR(NoeudABR* noeud, Etudiant e) {
    if (noeud == NULL) {
        NoeudABR *nouveau = malloc(sizeof(NoeudABR));
        nouveau->data = e; nouveau->gauche = NULL; nouveau->droit = NULL;
        return nouveau;
    }
    if (e.matricule < noeud->data.matricule) noeud->gauche = insererABR(noeud->gauche, e);
    else if (e.matricule > noeud->data.matricule) noeud->droit = insererABR(noeud->droit, e);
    return noeud;
}

void ajouterABR() {
    Etudiant e; char buffer[100];
    printf("\n--- Ajouter un etudiant (ABR) ---\n");
    printf("Matricule : "); scanf("%d", &e.matricule);
    printf("Nom       : "); scanf("%s", e.nom);
    printf("Prenom    : "); scanf("%s", buffer); e.prenom = allouerChaine(buffer);
    printf("Age       : "); scanf("%d", &e.age);
    printf("Date (jj/mm/aaaa) : "); scanf("%d/%d/%d", &e.dateNaissance.jour, &e.dateNaissance.mois, &e.dateNaissance.annee);
    printf("Adresse   : "); getchar(); fgets(e.adresse, 100, stdin); e.adresse[strcspn(e.adresse, "\n")] = '\0';
    printf("Nb notes  : "); scanf("%d", &e.nbNotes);
    float somme = 0;
    for (int i = 0; i < e.nbNotes; i++) { printf("  Note %d : ", i + 1); scanf("%f", &e.notes[i]); somme += e.notes[i]; }
    e.moyenne = (e.nbNotes > 0) ? somme / e.nbNotes : 0;
    e.inscription = NULL;
    racine = insererABR(racine, e); nbArbre++;
    printf("[OK] Ajout dans l'arbre.\n");
}

void afficherABR(NoeudABR* noeud) {
    if (noeud == NULL) return;
    afficherABR(noeud->gauche);       
    printf("\nMatricule : %d | Nom : %s %s | Moy : %.2f", noeud->data.matricule, noeud->data.prenom, noeud->data.nom, noeud->data.moyenne);
    afficherABR(noeud->droit);        
}

void afficherArbre() {
    if (racine == NULL) { printf("\nArbre vide.\n"); return; }
    printf("\n--- Liste tries par matricule (ABR) ---\n"); afficherABR(racine); printf("\n(Total : %d)\n", nbArbre);
}

/* ===================== TAS BINAIRE ===================== */
void echangerTas(int i, int j) {
    Etudiant temp = tas[i]; tas[i] = tas[j]; tas[j] = temp;
}

void remonter(int i) {
    while (i > 0) {
        int parent = (i - 1) / 2;
        if (tas[i].moyenne > tas[parent].moyenne) { echangerTas(i, parent); i = parent; }
        else break;
    }
}

void descendre(int i) {
    int n = nbTas;
    while (1) {
        int gauche = 2 * i + 1, droite = 2 * i + 2, plusGrand = i;
        if (gauche < n && tas[gauche].moyenne > tas[plusGrand].moyenne) plusGrand = gauche;
        if (droite < n && tas[droite].moyenne > tas[plusGrand].moyenne) plusGrand = droite;
        if (plusGrand != i) { echangerTas(i, plusGrand); i = plusGrand; }
        else break;
    }
}

void insererTas() {
    if (nbTas >= MAX_TAS) { printf("Tas plein !\n"); return; }
    Etudiant e; char buffer[100];
    printf("\n--- Ajouter un etudiant (Tas binaire) ---\n");
    printf("Matricule : "); scanf("%d", &e.matricule);
    printf("Nom       : "); scanf("%s", e.nom);
    printf("Prenom    : "); scanf("%s", buffer); e.prenom = allouerChaine(buffer);
    printf("Age       : "); scanf("%d", &e.age);
    printf("Date (jj/mm/aaaa) : "); scanf("%d/%d/%d", &e.dateNaissance.jour, &e.dateNaissance.mois, &e.dateNaissance.annee);
    printf("Adresse   : "); getchar(); fgets(e.adresse, 100, stdin); e.adresse[strcspn(e.adresse, "\n")] = '\0';
    printf("Nb notes  : "); scanf("%d", &e.nbNotes);
    float somme = 0;
    for (int i = 0; i < e.nbNotes; i++) { printf("  Note %d : ", i + 1); scanf("%f", &e.notes[i]); somme += e.notes[i]; }
    e.moyenne = (e.nbNotes > 0) ? somme / e.nbNotes : 0; e.inscription = NULL;
    tas[nbTas] = e; remonter(nbTas); nbTas++;
    printf("[OK] Ajout dans le tas.\n");
}

void afficherTas() {
    if (nbTas == 0) { printf("\nTas vide.\n"); return; }
    printf("\n--- Tas ---\n"); printf("Meilleur etudiant :\n"); afficherEtudiant(tas[0]);  
    printf("\nTous les etudiants du tas :\n");
    for (int i = 0; i < nbTas; i++) { printf("%d. Mat:%d | %s %s | Moy:%.2f\n", i+1, tas[i].matricule, tas[i].prenom, tas[i].nom, tas[i].moyenne); }
}

void extraireMax() {
    if (nbTas == 0) { printf("Tas vide.\n"); return; }
    printf("\n--- MEILLEUR ETUDIANT (extrait du tas) ---\n"); afficherEtudiant(tas[0]);
    tas[0] = tas[nbTas - 1]; nbTas--;
    if (nbTas > 0) descendre(0);
}

/* ===================== BENCHMARK (Nouveau !) ===================== */
void genererChaine(char *str, int longueur) {
    const char alphabet[] = "abcdefghijklmnopqrstuvwxyz";
    for (int i = 0; i < longueur; i++) {
        str[i] = alphabet[rand() % 26];
    }
    str[longueur] = '\0';
}

void lancerBenchmark() {
    int tailles[] = {100, 1000, 10000, 50000};
    int nb_tests = 4;
    
    printf("\n==============================================\n");
    printf("   BENCHMARK (Mesure du temps - Tri Rapide)\n");
    printf("==============================================\n");
    printf("Nombre (N)\tTemps d'execution (secondes)\n");
    printf("----------------------------------------------\n");
    
    for (int i = 0; i < nb_tests; i++) {
        int n = tailles[i];
        nbEtudiants = n;
        /* Remplissage de donnees aleatoires */
        for (int j = 0; j < n; j++) {
            tableau[j].matricule = rand() % 1000000;
            genererChaine(tableau[j].nom, 8);
            char buffer[15]; genererChaine(buffer, 8);
            tableau[j].prenom = allouerChaine(buffer);
            tableau[j].moyenne = ((float)rand() / (float)RAND_MAX) * 20.0;
        }
        
        /* Chronometrage du Tri Rapide */
        clock_t debut = clock();
        triRapide(0, n - 1);
        clock_t fin = clock();
        
        double temps = ((double)(fin - debut)) / CLOCKS_PER_SEC;
        printf("%d\t\t%f\n", n, temps);
        
        /* Nettoyage memoire */
        for (int j = 0; j < n; j++) free(tableau[j].prenom);
        nbEtudiants = 0;
    }
    printf("==============================================\n");
    printf("Copiez ces valeurs pour tracer vos courbes !\n");
}

/* ===================== MAIN ===================== */
int main() {
    srand(time(NULL));
    int choix;
    do {
        printf("\n=============================\n  GESTION DE SCOLARITE\n=============================\n");
        printf("1. Ajouter un etudiant (Statique)\n2. Afficher tous (Statique)\n3. Chercher par matricule\n4. Supprimer un etudiant\n");
        printf("5. Modifier un etudiant\n6. Rechercher par intervalle de moyenne\n7. Rechercher par nom\n8. Trier par moyenne (tri insertion)\n");
        printf("9. Trier par matricule (tri rapide)\n10. Statistiques\n11. Sauvegarder les donnees (binaire)\n12. Charger les donnees (binaire)\n");
        printf("13. Ajouter dans Tableau Dynamique\n14. Afficher Tableau Dynamique\n15. Rechercher par matricule (Dynamique)\n16. Supprimer (Dynamique)\n");
        printf("17. Rechercher par intervalle (Dynamique)\n18. Trier par moyenne (Dynamique)\n19. Trier par matricule (Dynamique)\n20. Statistiques (Dynamique)\n");
        printf("21. Ajouter a la fin (Liste chainee)\n22. Afficher la liste chainee\n23. Ajouter au debut (Liste chainee)\n24. Rechercher dans la liste\n25. Supprimer dans la liste\n");
        printf("26. Ajouter dans l'Arbre (ABR)\n27. Afficher l'Arbre (trie auto)\n28. Ajouter dans le Tas (File priorite)\n29. Afficher le Tas (meilleur en haut)\n30. Extraire le meilleur etudiant (Tas)\n");
        printf("31. [NOUVEAU] Lancer le Benchmark (Temps d'execution)\n");
        printf("0. Quitter\nVotre choix : ");
        
        if (scanf("%d", &choix) != 1) { printf("Erreur.\n"); break; }
        switch (choix) {
            case 1: ajouterEtudiant(); break;
            case 2: afficherTous(); break;
            case 3: rechercherParMatricule(); break;
            case 4: supprimerEtudiant(); break;
            case 5: modifierEtudiant(); break;
            case 6: rechercherParIntervalle(); break;
            case 7: rechercherParPrefixe(); break;
            case 8: triInsertion(); afficherTous(); break;
            case 9: triRapide(0, nbEtudiants - 1); afficherTous(); break;
            case 10: statistiques(); break;
            case 11: sauvegarder(); break;
            case 12: charger(); break;
            case 13: ajouterDynamique(); break;
            case 14: afficherDynamique(); break;
            case 15: rechercherMatriculeDyn(); break;
            case 16: supprimerDynamique(); break;
            case 17: rechercherIntervalleDyn(); break;
            case 18: triInsertionDyn(); afficherDynamique(); break;
            case 19: triRapideDyn(0, nbEtudiantsDyn - 1); afficherDynamique(); break;
            case 20: statistiquesDyn(); break;
            case 21: insererQueue(); break;
            case 22: afficherListe(); break;
            case 23: insererTete(); break;
            case 24: rechercherListe(); break;
            case 25: supprimerListe(); break;
            case 26: ajouterABR(); break;
            case 27: afficherArbre(); break;
            case 28: insererTas(); break;
            case 29: afficherTas(); break;
            case 30: extraireMax(); break;
            case 31: lancerBenchmark(); break;
            case 0: printf("Au revoir !\n"); break;
            default: printf("Invalide.\n");
        }
    } while (choix != 0);
    return 0;
}