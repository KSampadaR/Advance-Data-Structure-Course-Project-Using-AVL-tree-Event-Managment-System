#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

struct AVLNode {
    int dd;
    int mm;
    char events[100];
    struct AVLNode* left;
    struct AVLNode* right;
    int height;
};

typedef struct AVLNode AVLNode;

int height(AVLNode* node) {
    if (node == NULL)
        return 0;
    return node->height;
}

int findMax(int a, int b) {
    return (a > b) ? a : b;
}

AVLNode* newNode(int dd, int mm, char* events) {
    AVLNode* node = (AVLNode*)malloc(sizeof(AVLNode));
    node->dd = dd;
    node->mm = mm;
    strcpy(node->events, events);
    node->left = NULL;
    node->right = NULL;
    node->height = 1;
    return node;
}

AVLNode* rightRotate(AVLNode* y) {
    AVLNode* x = y->left;
    AVLNode* T2 = x->right;
    x->right = y;
    y->left = T2;
    y->height = findMax(height(y->left), height(y->right)) + 1;
    x->height = findMax(height(x->left), height(x->right)) + 1;
    return x;
}

AVLNode* leftRotate(AVLNode* x) {
    AVLNode* y = x->right;
    AVLNode* T2 = y->left;
    y->left = x;
    x->right = T2;
    x->height = findMax(height(x->left), height(x->right)) + 1;
    y->height = findMax(height(y->left), height(y->right)) + 1;
    return y;
}

int getBalance(AVLNode* node) {
    if (node == NULL)
        return 0;
    return height(node->left) - height(node->right);
}
                                                                               //1. Add Event
AVLNode* insert(AVLNode* node, int dd, int mm, char* events) {                 //Enter date (DD MM):  
    if (node == NULL)                                                          // 26 04 
        return newNode(dd, mm, events);                                        //Enter event name: B'day
    if (dd < node->dd || (dd == node->dd && mm < node->mm))                    //Event added successfully!
        node->left = insert(node->left, dd, mm, events);
    else if (dd > node->dd || (dd == node->dd && mm > node->mm))
        node->right = insert(node->right, dd, mm, events);
    else
        return node;

    node->height = 1 + findMax(height(node->left), height(node->right));
    int balance = getBalance(node);

    if (balance > 1) {
        if (dd < node->left->dd || (dd == node->left->dd && mm < node->left->mm))
            return rightRotate(node);
        if (dd > node->left->dd || (dd == node->left->dd && mm > node->left->mm)) {
            node->left = leftRotate(node->left);
            return rightRotate(node);
        }
    }

    if (balance < -1) {
        if (dd > node->right->dd || (dd == node->right->dd && mm > node->right->mm))
            return leftRotate(node);
        if (dd < node->right->dd || (dd == node->right->dd && mm < node->right->mm)) {
            node->right = rightRotate(node->right);
            return leftRotate(node);
        }
    }

    return node;
}


AVLNode *readEventsFromCSV(AVLNode *root, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file: %s\n", filename);
        return root;
    }

    int dd, mm;
    char events[100];

    char header[100];
    fgets(header, sizeof(header), file);

    while (fscanf(file, "%d,%d,%99[^\n]", &dd, &mm, events) == 3) {
        root = insert(root, dd, mm, events);
    }

    fclose(file);
    return root;
}

AVLNode* minValueNode(AVLNode* node) {
    AVLNode* current = node;
    while (current->left != NULL)
        current = current->left;
    return current;
}

AVLNode* deleteNode(AVLNode* root, int dd, int mm) {
    if (root == NULL)
        return root;

    if (dd < root->dd || (dd == root->dd && mm < root->mm))
        root->left = deleteNode(root->left, dd, mm);
    else if (dd > root->dd || (dd == root->dd && mm > root->mm))
        root->right = deleteNode(root->right, dd, mm);
    else {
        if ((root->left == NULL) || (root->right == NULL)) {
            AVLNode* temp = root->left ? root->left : root->right;

            if (temp == NULL) {
                temp = root;
                root = NULL;
            } else
                *root = *temp;

            free(temp);
        } else {
            AVLNode* temp = minValueNode(root->right);
            root->dd = temp->dd;
            root->mm = temp->mm;
            strcpy(root->events, temp->events);
            root->right = deleteNode(root->right, temp->dd, temp->mm);
        }
    }

    if (root == NULL)
        return root;

    root->height = 1 + findMax(height(root->left), height(root->right));
    int balance = getBalance(root);

    if (balance > 1) {
        if (getBalance(root->left) >= 0)
            return rightRotate(root);
        if (getBalance(root->left) < 0) {
            root->left = leftRotate(root->left);
            return rightRotate(root);
        }
    }

    if (balance < -1) {
        if (getBalance(root->right) <= 0)
            return leftRotate(root);
        if (getBalance(root->right) > 0) {
            root->right = rightRotate(root->right);
            return leftRotate(root);
        }
    }

    return root;
}

int searchEvent(AVLNode* root, int dd, int mm) {
    if (root == NULL) {
        return 0;
    }

    if (dd < root->dd || (dd == root->dd && mm < root->mm)) {
        return searchEvent(root->left, dd, mm);
    } else if (dd > root->dd || (dd == root->dd && mm > root->mm)) {
        return searchEvent(root->right, dd, mm);
    } else if (dd == root->dd && mm == root->mm) {
        printf("Events: %s\n", root->events);
        return 1;
    }

    return 0;
}

void printAllEvents(AVLNode* root) {                                               //All Events: It displays all events from CSV file.
    if (root != NULL) {
        printAllEvents(root->left);
        printf("Date: %02d/%02d, Events: %s\n", root->dd, root->mm, root->events);
        printAllEvents(root->right);
    }
}

void writeEventsToFileHelper(AVLNode *root, FILE *file) {
    if (root != NULL) {
        writeEventsToFileHelper(root->left, file);
        fprintf(file, "%d,%d,%s\n", root->dd, root->mm, root->events);
        writeEventsToFileHelper(root->right, file);
    }
}

void writeEventsToFile(AVLNode *root, const char *filename) {               //5. Save Events to File
    FILE *file = fopen(filename, "w");                                      //Events saved to file.
    if (file == NULL) {                                                     //Press Enter to continue...
        printf("Error opening file for writing: %s\n", filename);
        return;                                                              //6. Exit
    }

    fprintf(file, "dd,mm,events\n"); 

    writeEventsToFileHelper(root, file);

    fclose(file);
}

void setConsoleColor(int color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

void displayMenu() {
    system("cls"); 
    setConsoleColor(11); // Yellow text color

    printf("\n1. Add Event\n");
    printf("2. Print All Events\n");
    printf("3. Search Events\n");
    printf("4. Delete Event\n");
    printf("5. Save Events to File\n");
    printf("6. Exit\n");
    printf("Enter your choice: ");
    setConsoleColor(15); 
}

void searchEventByDate(AVLNode* root) {                                        
    int dd, mm;                                                                //3.Enter date to search (DD MM): 26 01   
    printf("Enter date to search (DD MM): ");                                  //Events: Republic Day

    scanf("%d %d", &dd, &mm);                                                  //Press Enter to continue...            

    int eventFound = searchEvent(root, dd, mm);

    if (eventFound == 0) {
        printf("No events found for the provided date (%02d/%02d).\n", dd, mm);
    }
}

void deleteEventByDate(AVLNode* root) {                             //4. Delete Event
    int dd, mm;                                                     //Enter date to delete (DD MM): 26 04
    printf("Enter date to delete (DD MM): ");                       //Event on 26/04 deleted.
    scanf("%d %d", &dd, &mm);                                       //Press Enter to continue...
                                     
    root = deleteNode(root, dd, mm);
    printf("Event on %02d/%02d deleted.\n", dd, mm);
}

int main() {
    AVLNode* root = NULL;

    root = readEventsFromCSV(root, "events.csv");

    char ch;
    int dd, mm;
    char buffer[100];

    while (1) {
        displayMenu();
        scanf(" %c", &ch);

        switch (ch) {
            case '1':
                system("cls"); 
                setConsoleColor(14); // Yellow text color
                printf("Enter date (DD MM): ");
                scanf("%d %d", &dd, &mm);
                printf("Enter event name: ");
                getchar(); 
                fgets(buffer, sizeof(buffer), stdin);
                buffer[strlen(buffer) - 1] = '\0'; 
                root = insert(root, dd, mm, buffer);
                setConsoleColor(10); // Green text color
                printf("\nEvent added successfully!\n");
                setConsoleColor(15); 
                break;
            case '2':
                system("cls"); 
                setConsoleColor(10); // Green text color
                printf("All Events:\n");
                setConsoleColor(15);
                printAllEvents(root);
                break;
            case '3':
                system("cls"); 
                setConsoleColor(14); // Yellow text color
                searchEventByDate(root); 
                setConsoleColor(15); 
                break;
            case '4':
                system("cls"); 
                setConsoleColor(12); // Red text color
                deleteEventByDate(root); 
                setConsoleColor(15); 
                break;
            case '5':
                writeEventsToFile(root, "events.csv");
                printf("Events saved to file.\n");
                break;
            case '6':
                writeEventsToFile(root, "events.csv");
                exit(0);
        }

        printf("\nPress Enter to continue...");
        getchar(); 
        getchar(); 
    }

    return 0;
}