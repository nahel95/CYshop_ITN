#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>




void mainMenu() {
    printf("\n");
    printf("\t\t\t  ===============================\n");
    printf("\t\t\t       BIENVENUE SUR CY SHOP\n");
    printf("\t\t\t  ===============================");

    printf("\n\nTapez :");
    printf("\n 1.) MODE GESTION");
    printf("\n 2.) MODE ACHAT");
    printf("\n 3.) MON COMPTE");
    printf("\n 0.) QUITTER LE MAGASIN");
    printf("\n--> ");
}



typedef struct{
    char name[50];
    int reference;
    int size;
    int quantity;
    float price;
}drink;

typedef struct{
    char last_name[20];
    char first_name[20];
    char password[20];
    drink purchase_history[1000];
    int purchase_count;
}customer;


void savePurchaseToFile(const char* last_name, const char* first_name, drink* purchase) {
    char fileName[50];
    sprintf(fileName, "%s_%s.txt", last_name, first_name);

    FILE* file = fopen(fileName, "a");
    if (file == NULL) {
        printf("Erreur lors de l'ouverture du fichier.\n");
        return;
    }

    fprintf(file, "Nom du produit : %s\n", purchase->name);
    fprintf(file, "Reference : %d\n", purchase->reference);
    fprintf(file, "Quantite : %d\n", purchase->quantity);
    fprintf(file, "Prix unitaire : %f\n", purchase->price);
    fprintf(file, "-------------------------\n");

    fclose(file);
}


void addPurchasesToCustomer(customer* cust, drink purchases[], int purchaseCount) {
    for (int i = 0; i < purchaseCount && cust->purchase_count <= 3; i++) {
        // Ajouter chaque achat à la liste des achats du client
        cust->purchase_history[cust->purchase_count] = purchases[i];
        cust->purchase_count++;

        // Enregistrer l'achat dans le fichier correspondant
        savePurchaseToFile(cust->last_name, cust->first_name, &purchases[i]);
    }
}





//Fonction permettant d'augmenter la quantité d'un article déjà présent en stock
void increaseQuantity(int reference, int quantityToAdd) {
    FILE* fichier = fopen("stock.txt", "r");
    FILE* fichierTemp = fopen("temp.txt", "w");

    if (fichier == NULL || fichierTemp == NULL) {
        printf("Erreur lors de l'ouverture des fichiers.\n");
        return;
    }

    drink temp;
    char line[100];

    while (fgets(line, sizeof(line), fichier) != NULL) {
        if (strstr(line, "Ref :") != NULL) {
            sscanf(line, "Nom du produit : %[^:] : %d  Taille : %d  Qte : %d  Prix : %f",
                temp.name, &temp.reference, &temp.size, &temp.quantity, &temp.price);
            if (temp.reference == reference) {
                temp.quantity += quantityToAdd;
            }
            fprintf(fichierTemp, "Nom du produit : %s: %d   Taille : %d   Qte : %d   Prix : %.2f\n",
                temp.name, temp.reference, temp.size, temp.quantity, temp.price);
        } else {
            fputs(line, fichierTemp);
        }
    }
    fclose(fichier);
    fclose(fichierTemp);

    remove("stock.txt");
    rename("temp.txt", "stock.txt");

    printf("Quantite mise a jour avec succes.\n");
}





//Procédure pour ajouter des articles au stock
#define SIZE_MAX_STOCK 1000

void addProduct() {
    int i;
    int nbrDrink;
    int availableSpace;
    int choice;

    printf("Tapez :\n");
    printf("1.) Ajoutez un nouvel article\n");
    printf("2.) Ajouter un article deja present\n");
    scanf("%d", &choice);

    if (choice == 2) {
        int reference, quantityToAdd;
        printf("Entrez la reference du produit existant : ");
        scanf("%d", &reference);
        printf("Entrez la quantite a ajouter : ");
        scanf("%d", &quantityToAdd);

        increaseQuantity(reference, quantityToAdd);
    }

    else if (choice == 1){

    printf("Combien d'articles voulez-vous ajouter au stock ? ");

    // Vérification de la saisie correcte du nombre de boissons
    if (scanf("%d", &nbrDrink) <= 0 || nbrDrink >= SIZE_MAX_STOCK) {
        printf("Nombre invalide.\n");
        return;
    }

    FILE* fichier = fopen("stock.txt", "a");
    if (fichier == NULL) {
        printf("Erreur lors de l'ouverture du fichier stock.txt.\n");
        return;
    }

    // Vérification du dépassement de la taille maximale du stock
    fseek(fichier, 0, SEEK_END);
    int currentSize = ftell(fichier) / sizeof(drink);
    availableSpace = SIZE_MAX_STOCK - currentSize;

    if (nbrDrink > availableSpace) {
        printf("La place restante en magasin (%d) est insuffisante pour ajouter %d articles.\n", availableSpace, nbrDrink);
        fclose(fichier);
        return;
    }

    printf("Place disponible en magasin : %d\n", availableSpace);

    for (i = 0; i < nbrDrink; i++) {
        printf("\nBoisson %d :\n", i + 1);

        drink drink;

        printf("Nom : ");
        fflush(stdin); // Vider le tampon d'entrée avant d'utiliser fgets
        fgets(drink.name, sizeof(drink.name), stdin);
        drink.name[strcspn(drink.name, "\n")] = '\0'; // Supprimer le caractère de saut de ligne '\n'

        printf("Numero de reference : ");
        scanf("%d", &drink.reference);


        printf("Taille (1: Petit, 2: Moyen, 3: Grand) : ");
        scanf("%d", &drink.size);

        // Vérification de la taille de l'article pour définir la quantité maximale
        int maxQuantity;
        if (drink.size == 3) {
            maxQuantity = 15;
        } else if (drink.size == 2) {
            maxQuantity = 20;
        } else if (drink.size == 1) {
            maxQuantity = 30;
        } else {
            printf("Taille invalide.\n");
            fclose(fichier);
            return;
        }

        printf("Quantite (max %d) : ", maxQuantity);
        scanf("%d", &drink.quantity);

        if (drink.quantity > maxQuantity) {
            printf("La quantite saisie depasse la quantite maximale pour cette taille d'article.\n");
            fclose(fichier);
            return;
        }

        printf("Prix : ");
        scanf("%f", &drink.price);

        fprintf(fichier, "Nom du produit : %s   Ref : %d   Taille : %d   Qte : %d   Prix : %.2f\n", drink.name, drink.reference, drink.size, drink.quantity, drink.price);
    }

    fclose(fichier);
}
}









//afficher les stocks les plus bas


void displayLowStockItems() {
    FILE* fichier = fopen("stock.txt", "r");
    if (fichier == NULL) {
        printf("Erreur lors de l'ouverture du fichier stock.txt.\n");
        return;
    }

    drink temp;
    int numDrinks = 0;
    int emptyStockCount = 0;
    drink lowStockItems[5];
    drink allDrinks[100];  // Tableau pour stocker tous les articles

    char line[100];
    printf("Article avec un stock vide :\n");
    while (fgets(line, sizeof(line), fichier) != NULL) {
        if (strstr(line, "Qte : 0") != NULL) {
            sscanf(line, "Nom du produit : %[^\n]s", temp.name);
            printf("\nNom : %s\n", temp.name);
            printf("-------------------------\n");

            emptyStockCount++;
        } else if (strstr(line, "Qte :") != NULL) {
            sscanf(line, "Nom du produit : %[^\n]s",
                   allDrinks[numDrinks].name);

            numDrinks++;
        }
    }

    fclose(fichier);

    if (emptyStockCount == 0) {
        printf("Aucun article avec stock vide.\n");
    }

    if (numDrinks == 0) {
        printf("Aucun article avec stock disponible.\n");
        return;
    }

    // Trier tous les articles par quantité de stock (tri à bulles)
    for (int i = 0; i < numDrinks - 1; i++) {
        for (int j = 0; j < numDrinks - i - 1; j++) {
            if (allDrinks[j].quantity > allDrinks[j + 1].quantity) {
                drink temp = allDrinks[j];
                allDrinks[j] = allDrinks[j + 1];
                allDrinks[j + 1] = temp;
            }
        }
    }

    printf("\n\nStocks les plus faibles :\n");
    int lowStockCount = 0;
    for (int i = 0; i < numDrinks && lowStockCount < 5; i++) {
        if (allDrinks[i].quantity > 0) {
            printf("- %s\n", allDrinks[i].name);
            printf("-------------------------\n");
            lowStockCount++;
        }
    }
    if (lowStockCount == 0){
        printf("Aucun produit avec un stock faible.\n");
    }
}




















//procédure pour créer un compte client 

void createAccountCustomer() {
    customer newCustomer;
    printf("=== Creation d'un compte client ===\n");
    printf("Nom : ");
    scanf("%s", newCustomer.last_name);
    printf("Prenom : ");
    scanf("%s", newCustomer.first_name);
    printf("Mot de passe : ");
    scanf("%s", newCustomer.password);

    char fileName[50];
    sprintf(fileName, "%s_%s.txt", newCustomer.last_name, newCustomer.first_name);

    FILE* fichier = fopen(fileName, "w");
    if (fichier == NULL) {
        printf("Erreur lors de l'ouverture du fichier de compte client.\n");
        return;
    }

    fprintf(fichier, "Nom : %s\n", newCustomer.last_name);
    fprintf(fichier, "Prenom : %s\n", newCustomer.first_name);
    fprintf(fichier, "Mot de Passe : %s\n", newCustomer.password);
    fprintf(fichier, "-------------------------\n");

    fclose(fichier);

    printf("Le compte client a ete cree avec succes.\n");
}


//procédure pour supprimer un compte client

void deleteCustomerAccount() {
    char last_name[20];
    char first_name[20];
    char password[20];

    printf("=== Suppression du compte ===\n");
    printf("Nom : ");
    scanf("%s", last_name);
    printf("Prenom : ");
    scanf("%s", first_name);
    printf("Mot de passe : ");
    scanf("%s", password);

// Générer le nom du fichier pour le client
    char fileName[50];
    sprintf(fileName, "%s_%s.txt", last_name, first_name);
    

    FILE* fichier = fopen(fileName, "r");
    if (fichier == NULL) {
        printf("Erreur : compte client introuvable.\n");
        return;
    }

    fclose(fichier);

    if (remove(fileName) == 0) {
        printf("Le compte client a ete supprime avec succes.\n");
    } else {
        printf("Erreur lors de la suppression du compte client.\n");
    }
}



//procédure pour s'identifier au mode achat
bool identifyCustomer() {
    char last_name[20];
    char first_name[20];
    char password[20];

    printf("=== Identification du client ===\n");
    printf("Nom : ");
    scanf("%s", last_name);
    printf("Prenom : ");
    scanf("%s", first_name);
    printf("Mot de passe : ");
    scanf("%s", password);

    char fileName[50];
    sprintf(fileName, "%s_%s.txt", last_name, first_name);

    FILE* fichier = fopen(fileName, "r");
    if (fichier == NULL) {
        printf("Erreur lors de l'ouverture du fichier.\n");
        return false;
    }

    bool accountFind = false;
    char ligne[100];
    while (fgets(ligne, sizeof(ligne), fichier)) {
        if (strstr(ligne, "Nom :") != NULL && strstr(ligne, last_name) != NULL) {
            fgets(ligne, sizeof(ligne), fichier);  // Lire la ligne Prenom
            fgets(ligne, sizeof(ligne), fichier);  // Lire la ligne Mot de Passe
            char stored_password[20];
            sscanf(ligne, "Mot de Passe : %s\n", stored_password);

            if (strcmp(stored_password, password) == 0) {
                accountFind = true;
                break;
            }
        }
    }

    fclose(fichier);

    if (accountFind) {
        printf("\nIdentification reussie.\n");
        return true;
    } else {
        printf("\nIdentifiant ou mot de passe incorrect. Veuillez reessayer.\n");
        return false;
    }
}


//procédure pour afficher le compte d'un client depuis la menu principal
void printAccountCustomer() {
    char last_name[20];
    char first_name[20];
    char password[20];
    int retour;

    printf("=== Affichage du compte ===\n");
    printf("\nNom : ");
    scanf("%s", last_name);
    printf("\nPrenom : ");
    scanf("%s", first_name);
    printf("\nMot de passe : ");
    scanf("%s", password);

    char fileName[50];
    sprintf(fileName, "%s_%s.txt", last_name, first_name);

    FILE* fichier = fopen(fileName, "r");
    if (fichier == NULL) {
        printf("Le compte client n'existe pas.\n");
        return;
    }

    bool accountExists = false;
    char ligne[100];
    while (fgets(ligne, sizeof(ligne), fichier)) {
        if (strstr(ligne, "Nom :") != NULL && strstr(ligne, last_name) != NULL) {
            fgets(ligne, sizeof(ligne), fichier);  // Lire la ligne Prenom
            fgets(ligne, sizeof(ligne), fichier);  // Lire la ligne Mot de Passe
            char stored_password[20];
            sscanf(ligne, "Mot de Passe : %s\n", stored_password);

            if (strcmp(stored_password, password) == 0) {
                accountExists = true;
                break;
            }
        }
    }

    fclose(fichier);

    if (accountExists) {
        fichier = fopen(fileName, "r");
        if (fichier == NULL) {
            printf("Erreur lors de l'ouverture du fichier.\n");
            return;
        }

        printf("\nInformations concernant votre compte :\n");
        while (fgets(ligne, sizeof(ligne), fichier)) {
            printf("%s", ligne);
        }

        printf("\nAchats effectues :\n");

char fileName[50];
sprintf(fileName, "%s_%s.txt", last_name, first_name);
FILE* purchaseFile = fopen(fileName, "r");
if (purchaseFile == NULL) {
    printf("Aucun achat enregistre pour ce compte.\n");
} else {
    while (fgets(ligne, sizeof(ligne), purchaseFile)) {
        printf("%s", ligne);
    }
    fclose(purchaseFile);
}

        printf("\nTapez :\n");
        printf("1) Supprimer definitivement mon compte\n");
        printf("0) Retour au menu principal\n");
        scanf("%d", &retour);
        if (retour == 1) {
            printf("Etes-vous sur(e) de vouloir supprimer definitivement votre compte ?\n");
            printf("1) Oui\n");
            printf("0) Non\n");
            scanf("%d", &retour);
            if (retour == 1) {
                fclose(fichier);
                deleteCustomerAccount(last_name, first_name);
            } else {
                printf("\nSuppression annulee");
            }
        } else {
            fclose(fichier);
        }
    } else {
        printf("Mot de passe incorrect ou compte introuvable.\n");
    }
}















//procédure pour le mode gestion

void gestionMode() {
    int retour;
    char login[25];
    char password[25];
    bool identifie = false;
    bool isConnected = false;

    while (true) {
        printf("\nBienvenue dans le mode Gestion. Tapez :\n");
        printf("1.) S'identifier\n");
        printf("0.) Retourner au menu principal\n");
        printf("--> ");
        scanf("%d", &retour);

        if (retour == 0) {
            break;
        }
        else if (retour == 1) {
            if (identifie) {
                printf("\nVous etes deja identifie.\n");
                break;
            }
            else {
                printf("\nVeuillez vous identifier");
                printf("\nIdentifiant : ");
                scanf("%s", login);
                printf("\nMot de passe : ");
                scanf("%s", password);

                if (strcmp(login, "CY_admin") == 0 && strcmp(password, "admin123") == 0) {
                    identifie = true;
                    isConnected = true;
                    printf("\nIdentification reussie.\n");
                    while (isConnected) {
                        printf("Mode Gestion :\n");
                        displayLowStockItems();
                        printf("\n1.) Ajouter du stock\n");
                        printf("0.) Retour\n");
                        printf("--> ");
                        scanf("%d", &retour);

                        if (retour == 1) {
                            addProduct();
                        }
                        else if (retour == 0) {
                            break;  // Retour
                        }
                        else {
                            printf("Choix invalide. Veuillez reessayer.\n");
                        }
                    }
                }
                else {
                    printf("\nIdentifiant ou mot de passe incorrect. Veuillez reessayer ou changer de mode.\n");
                }
            }
        }
        else {
            printf("Choix invalide. Veuillez reessayer.\n");
        }
    }
}

// Déclaration et initialisation du tableau de produits (panier)
#define MAX_ITEMS 1000  // Définir la taille maximale du panier à 100 (à ajuster selon vos besoins)
drink cart[MAX_ITEMS];
int itemCount = 0;



//procédure pour afficher le panier du client
void displayCart(drink cart[], int itemCount) {
    printf("\nPanier :\n");
    float totalPrice = 0.0;
    for (int i = 0; i < itemCount; i++) {
        printf("Nom du produit : %s\n", cart[i].name);
        printf("Reference : %d\n", cart[i].reference);
        printf("Quantite : %d\n", cart[i].quantity);
        printf("Taille : %d\n", cart[i].size);
        printf("Prix unitaire : %f\n", cart[i].price);
        printf("-------------------------\n");

        totalPrice += cart[i].quantity * cart[i].price;
    }

    printf("Prix total : %.2f\n", totalPrice);
}



//procédure pour finaliser l'achat

void finalizePurchase(drink cart[], int itemCount, customer* cust) {
    // Afficher le contenu du panier avec le prix total
    displayCart(cart, itemCount);

    // Demander à l'utilisateur de confirmer l'achat
    char confirmation;
    printf("\nVoulez-vous confirmer l'achat ? (O/N) ");
    scanf(" %c", &confirmation);

    if (confirmation == 'O' || confirmation == 'o') {
        FILE* inputFile = fopen("stock.txt", "r");
        FILE* tempFile = fopen("temp_stock.txt", "w");

        if (inputFile == NULL || tempFile == NULL) {
            printf("Erreur lors de l'ouverture des fichiers.\n");
            return;
        }

        // Lire le contenu du fichier stock.txt dans une structure de données
        drink stock[MAX_ITEMS];
        int stockItemCount = 0;

        char line[100];
        while (fgets(line, sizeof(line), inputFile)) {
            if (strstr(line, "Nom du produit :") != NULL) {
                drink item;
                sscanf(line, "Nom du produit : %[^Ref :] Ref : %d Taille : %d Qte : %d Prix : %f",
                       item.name, &item.reference, &item.size, &item.quantity, &item.price);

                stock[stockItemCount] = item;
                stockItemCount++;
            }
        }

        // Mettre à jour les quantités dans la structure de données en mémoire
        for (int i = 0; i < itemCount; i++) {
            for (int j = 0; j < stockItemCount; j++) {
                if (cart[i].reference == stock[j].reference) {
                    stock[j].quantity -= cart[i].quantity;
                    break;
                }
            }
        }

        // Écrire le contenu mis à jour dans le fichier temporaire
        for (int i = 0; i < stockItemCount; i++) {
            fprintf(tempFile, "Nom du produit : %s   Ref : %d   Taille : %d   Qte : %d   Prix : %f\n",
                    stock[i].name, stock[i].reference, stock[i].size, stock[i].quantity, stock[i].price);
        }

        fclose(inputFile);
        fclose(tempFile);

        remove("stock.txt");  // Supprimer l'ancien fichier stock
        rename("temp_stock.txt", "stock.txt");  // Renommer le fichier temporaire

        printf("Achat effectue avec succes ! Panier vide.\n");
        itemCount = 0;
        memset(cart, 0, sizeof(drink) * MAX_ITEMS);
        addPurchasesToCustomer(cust, cart, itemCount);
    } else {
        itemCount = 0;
        memset(cart, 0, sizeof(drink) * MAX_ITEMS);
        printf("Achat annule. Panier vide. Deconnexion du Mode Achat.\n");
    }
}









void displayCartAndFinalize(drink cart[], int itemCount) {
    // Afficher le contenu du panier

    customer cust;
    strcpy(cust.last_name, "Doe");
    strcpy(cust.first_name, "John");
// Initialiser d'autres champs de la structure customer si nécessaire

    displayCart(cart, itemCount);

    // Demander à l'utilisateur de finaliser l'achat
    int choice;
    printf("\nQue souhaitez-vous faire ?\n");
    printf("1.) Finaliser l'achat\n");
    printf("2.) Retour\n");
    printf("--> ");
    scanf("%d", &choice);

    if (choice == 1) {
        finalizePurchase(cart, itemCount, &cust);
    } else if (choice == 2) {
        return;
    } else {
        printf("Choix invalide. Veuillez reessayer.\n");
        displayCartAndFinalize(cart, itemCount); // Demander à nouveau le choix
    }
}








//procédure pour ajouter un article au panier
void addToCart(drink cart[], int *itemCount) {
    FILE* fichier = fopen("stock.txt", "r");
    if (fichier == NULL) {
        printf("Erreur lors de l'ouverture du fichier stock.txt.\n");
        return;
    }

    int reference;
    printf("Entrez le numero de référence du produit a ajouter au panier : ");
    scanf("%d", &reference);

    int quantity;
    printf("Entrez la quantite du produit a ajouter au panier : ");
    scanf("%d", &quantity);

    drink temp;
    bool found = false;
    char line[100];

    while (fgets(line, sizeof(line), fichier) != NULL) {
        if (strstr(line, "Ref :") != NULL) {
            sscanf(line, "Nom du produit : %[^\n] Ref : %d Taille : %d Qte : %d Prix : %f",
                temp.name, &temp.reference, &temp.size, &temp.quantity, &temp.price);

            if (temp.reference == reference) {
                found = true;
                break;
            }
        }
    }

    fclose(fichier);

    if (found) {
        if (quantity <= temp.quantity) {
            // Ajouter le produit au panier
            strcpy(cart[*itemCount].name, temp.name);
            cart[*itemCount].reference = temp.reference;
            cart[*itemCount].quantity = quantity;
            cart[*itemCount].price = temp.price;
            (*itemCount)++; // Incrémenter le compteur d'articles

            printf("Produit ajoute au panier avec succes.\n");
            displayCartAndFinalize(cart, *itemCount);
        } else {
            printf("La quantite demandee n'est pas disponible dans le stock.\n");
        }
    } else {
        printf("Aucun produit correspondant trouve avec le numero de reference fourni.\n");
    }
}
















//procédure pour afficher le catalogue entier
void displayAllStock(bool addToCart) {
    drink temp;
    temp.reference = 0;  // Initialisation de la référence à une valeur par défaut

    // Ouvrir le fichier stock.txt
    FILE* fichier = fopen("stock.txt", "r");
    if (fichier == NULL) {
        printf("Erreur lors de l'ouverture du fichier stock.txt.\n");
        return;
    }

    printf("\nStock:\n");
    printf("------------------------------\n");

    char line[100];

    while (fgets(line, sizeof(line), fichier) != NULL) {
        printf("%s", line);
        printf("------------------------------\n");
    }

    fclose(fichier);

    int reference;
    printf("Entrez le numero de reference du produit a ajouter au panier : ");
    scanf("%d", &reference);

    FILE* stockFile = fopen("stock.txt", "r");
    if (stockFile == NULL) {
        printf("Erreur lors de l'ouverture du fichier stock.txt.\n");
        return;
    }

    bool found = false;
    char stockLine[100];
    char keyword[50];

    while (fgets(stockLine, sizeof(stockLine), stockFile) != NULL) {
        if (strstr(stockLine, "Ref :") != NULL) {
            sscanf(stockLine, "Nom du produit : %s Ref  : %d Taille : %d Qte : %d Prix : %f",
                   temp.name, &temp.reference, &temp.size, &temp.quantity, &temp.price);

            if (temp.reference == reference) {
                found = true;
                break;
            }
        }
    }

    fclose(stockFile);

    if (found) {
        printf("\nProduit trouve :\n");
        printf("Nom : %s\n", temp.name);
        printf("Numero de Reference : %d\n", temp.reference);
        printf("Taille : %d\n", temp.size);
        printf("Quantite : %d\n", temp.quantity);
        printf("Prix : %f\n", temp.price);

        if (addToCart) {
            int quantity;
            printf("Entrez la quantite du produit a ajouter au panier : ");
            scanf("%d", &quantity);

            if (quantity <= temp.quantity) {
                // Ajouter le produit au panier
                cart[itemCount] = temp;
                cart[itemCount].quantity = quantity;
                itemCount++; // Incrémenter le compteur d'articles

                printf("Produit ajoute au panier avec succes.\n");
            } else {
                printf("La quantite demandee est superieure a la quantite disponible en stock.\n");
            }
        }
    } else {
        printf("Aucun produit correspondant trouve.\n");
    }
}









//procédure pour rechercher un produit par nom ou numéro de référence

void strToLower(char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        str[i] = tolower(str[i]);
    }
}
void searchProduct(bool addToCart){
    char keyword[50];
    printf("\nRechercher un produit par nom ou reference :\n");
    printf("--> ");
    scanf("%s", keyword);

    FILE* fichier = fopen("stock.txt", "r");
    if (fichier == NULL) {
        printf("Erreur lors de l'ouverture du fichier stock.txt.\n");
        return;
    }

    drink temp;
    bool found = false;
    char line[100];
    // Réinitialiser le curseur du fichier au début
    rewind(fichier);


    while (fgets(line, sizeof(line), fichier)) {
        if (strstr(line, "Nom du produit :") != NULL) {
            sscanf(line, "Nom du produit : %s Ref  : %d Taille : %d Qte : %d Prix : %f",
                   temp.name, &temp.reference, &temp.size, &temp.quantity, &temp.price);

            char lowerKeyword[50];
            char lowerName[50];
            strcpy(lowerKeyword, keyword);
            strcpy(lowerName, temp.name);
            strToLower(lowerKeyword);
            strToLower(lowerName);

            if (strcmp(lowerName, lowerKeyword) == 0 || temp.reference == atoi(keyword)) {
                found = true;
                break;
            }
        }
    }

    fclose(fichier);

    if (found){
        printf("\nProduit trouve :\n");
        printf("Nom : %s\n", temp.name);
        printf("Numero de Reference : %d\n", temp.reference);
        printf("Taille : %d\n", temp.size);
        printf("Quantite : %d\n", temp.quantity);
        printf("Prix : %f\n", temp.price);

        if (addToCart) {
            int quantity, choice;
            printf("\nEntrez la quantite du produit a ajouter au panier : ");
            scanf("%d", &quantity);

           if (quantity <= temp.quantity) {
                // Ajouter le produit au panier
                cart[itemCount] = temp;
                cart[itemCount].quantity = quantity;
                itemCount++; // Incrémenter le compteur d'articles

                printf("Produit ajoute au panier avec succes.\n");
                displayCartAndFinalize(cart, itemCount);
        }
    } else {
        printf("Aucun produit correspondant trouve.\n");
    }
}
}









//procédure pour le mode achat


void achatMode(){
    int retour;
    bool estConnecte = false;  // Variable pour vérifier si le client est connecté ou non

    while (true) {
        printf("\nMode Achat. Tapez :\n");
        printf("1.) Se connecter\n");
        printf("2.) Creer un compte\n");
        printf("0.) Retour\n");
        printf("--> ");
        scanf("%d", &retour);

        if (retour == 1) {
            estConnecte = identifyCustomer();  // Appel de la fonction identifyCustomer pour vérifier l'identification
        }
        else if (retour == 2) {
            createAccountCustomer();
        }
        else if (retour == 0) {
            break;
        }
        else {
            printf("Choix invalide. Veuillez reessayer.\n");
            continue;
        }

        if (estConnecte) {  // Vérifier si le client est connecté
            while (true) {
                printf("\nBienvenue dans le Mode Achat. Tapez :\n");
                printf("1.) Acceder au catalogue\n");
                printf("2.) Rechercher un article par nom/numero de reference\n");
                printf("3.) Afficher mon panier et finaliser l'achat\n");
                printf("0.) Retour\n");
                printf("--> ");
                scanf("%d", &retour);

                if (retour == 1) {
                    displayAllStock(addToCart);
                }
                else if (retour == 2) {
                    searchProduct(true);  // Ajouter le paramètre true pour ajouter au panier
                    break;
                }
                else if (retour == 3){
                    displayCartAndFinalize(cart, itemCount);
                }
                else if (retour == 0) {
                    break;
                }
                else {
                    printf("Choix invalide. Veuillez reessayer.\n");
                }
            }
        }
        else {
            printf("Identification invalide. Acces au catalogue interdit.\n");
        }
    }
}






//fonction principale
int main() {
    int choice;
    bool gestion = false;
    bool achat = false;

    while (true) {
        mainMenu();
        scanf("%d", &choice);

        if (choice == 1) {
            gestion = true;
            achat = false;
            gestionMode();
        }
        else if (choice == 2) {
            achat = true;
            gestion = false;
            achatMode();
        }
        else if (choice == 3) {
            printAccountCustomer();
            //afficher mon compte
        }
        else if (choice == 0) {
            printf("Au revoir et a bientot chez CY SHOP !");
            break;            
        }
        else {
            printf("Choix invalide. Veuillez reessayer.\n");
        }

        // Vider le tampon d'entrée pour éviter les problèmes de lecture ultérieure
        while (getchar() != '\n');
    }

    return 0; 
}
