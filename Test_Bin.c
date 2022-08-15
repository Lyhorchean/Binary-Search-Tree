#include<stdio.h>
#include<stdlib.h>
#include<string.h>
struct node{
    int info;
    struct node* left;
    struct node* right;
};
struct node* getNode(){
    struct node* p;
    p = (struct node* )malloc(sizeof(struct node));
    return p;
}
struct node* createNode(int item){
    struct node* p = getNode();
    p->info = item;
    p->left = NULL;
    p->right = NULL;
    return p;
}
struct node* insert(struct node* proot, int item){
    if(proot == NULL)
        proot = createNode(item);
    else{
        if(proot->info == item)
            printf("p node has in binary search tree");

        else if(proot->info > item)
            proot->left = insert(proot->left, item);

        else if(proot->info < item)
            proot->right = insert(proot->right, item);
    }
    return proot;
}
struct node* input(struct node* proot, int n){
    if(n == 0)
        return NULL;
    int num;
    for(int i=0; i<n; i++){
        printf("Enter info of node : "); scanf("%d",&num);
        proot = insert(proot, num);
    }
    return proot;
}
void inOrder(struct node* proot){
    if(proot != NULL){
        inOrder(proot->left);
        printf("%d   ",proot->info);
        inOrder(proot->right);
    }
}
// void printBinaryTree(struct node* proot, int n ){
//     if(proot != NULL){
//         printBinaryTree(proot->right, n+1);
//         printf("\n");
//         for(int i=0; i<n; i++)
//           printf("\t");
//           printf("%d",proot->info);
//           printBinaryTree(proot->left, n+1);
//     }
// }
typedef struct Tree Tree;
struct Tree{
    int info;
    Tree* left;
    Tree* right;
};

typedef struct asciinode_struct asciinode;

struct asciinode_struct {
    asciinode *left, *right;

    //length of the edge from this node to its children
    int edge_length;

    int height;

    int lablen;

    //-1=I am left, 0=I am root, 1=right
    int parent;

    //max supported unit32 in dec, 10 digits max
    char label[11];
};


#define MAX_HEIGHT 100
int leftTree[MAX_HEIGHT];
int rightTree[MAX_HEIGHT];
#define INFINITY (1<<20)

//adjust gap between left and right nodes
int gap = 3;

//used for printing next node in the same level,
//this is the x coordinate of the next char printed
int print_next;

int MIN(int a, int b) {
    return ((a) < (b)) ? (a) : (b);
}

int MAX(int a, int b) {
    return ((a) > (b)) ? (a) : (b);
}

asciinode *build_ascii_tree_recursive(Tree *t) {
    asciinode *node;

    if (t == NULL) return NULL;

    node = (asciinode*)malloc(sizeof(asciinode));
    node->left = build_ascii_tree_recursive(t->left);
    node->right = build_ascii_tree_recursive(t->right);

    if (node->left != NULL) {
        node->left->parent = -1;
    }

    if (node->right != NULL) {
        node->right->parent = 1;
    }

    sprintf(node->label, "%d", t->info);
    node->lablen = (int) strlen(node->label);

    return node;
}


//Copy the tree into the ascii node structre
asciinode *build_ascii_tree(Tree *t) {
    asciinode *node;
    if (t == NULL) return NULL;
    node = build_ascii_tree_recursive(t);
    node->parent = 0;
    return node;
}


//Free all the nodes of the given tree
void free_ascii_tree(asciinode *node) {
    if (node == NULL) return;
    free_ascii_tree(node->left);
    free_ascii_tree(node->right);
    free(node);
}

//The following function fills in the lprofile array for the given tree.
//It assumes that the center of the label of the root of this tree
//is located at a position (x,y).  It assumes that the edge_length
//fields have been computed for this tree.
void compute_leftTree(asciinode *node, int x, int y) {
    int i, isleft;
    if (node == NULL) return;
    isleft = (node->parent == -1);
    leftTree[y] = MIN(leftTree[y], x - ((node->lablen - isleft) / 2));
    if (node->left != NULL) {
        for (i = 1; i <= node->edge_length && y + i < MAX_HEIGHT; i++) {
            leftTree[y + i] = MIN(leftTree[y + i], x - i);
        }
    }
    compute_leftTree(node->left, x - node->edge_length - 1, y + node->edge_length + 1);
    compute_leftTree(node->right, x + node->edge_length + 1, y + node->edge_length + 1);
}

void compute_rightTree(asciinode *node, int x, int y) {
    int i, notleft;
    if (node == NULL) return;
    notleft = (node->parent != -1);
    rightTree[y] = MAX(rightTree[y], x + ((node->lablen - notleft) / 2));
    if (node->right != NULL) {
        for (i = 1; i <= node->edge_length && y + i < MAX_HEIGHT; i++) {
            rightTree[y + i] = MAX(rightTree[y + i], x + i);
        }
    }
    compute_rightTree(node->left, x - node->edge_length - 1, y + node->edge_length + 1);
    compute_rightTree(node->right, x + node->edge_length + 1, y + node->edge_length + 1);
}

//This function fills in the edge_length and
//height fields of the specified tree
void compute_edge_lengths(asciinode *node) {
    int h, hmin, i, delta;
    if (node == NULL) return;
    compute_edge_lengths(node->left);
    compute_edge_lengths(node->right);

    /* first fill in the edge_length of node */
    if (node->right == NULL && node->left == NULL) {
        node->edge_length = 0;
    } else {
        if (node->left != NULL) {
            for (i = 0; i < node->left->height && i < MAX_HEIGHT; i++) {
                rightTree[i] = -INFINITY;
            }
            compute_rightTree(node->left, 0, 0);
            hmin = node->left->height;
        } else {
            hmin = 0;
        }
        if (node->right != NULL) {
            for (i = 0; i < node->right->height && i < MAX_HEIGHT; i++) {
                leftTree[i] = INFINITY;
            }
            compute_leftTree(node->right, 0, 0);
            hmin = MIN(node->right->height, hmin);
        } else {
            hmin = 0;
        }
        delta = 4;
        for (i = 0; i < hmin; i++) {
            delta = MAX(delta, gap + 1 + rightTree[i] - leftTree[i]);
        }

        //If the node has two children of height 1, then we allow the
        //two leaves to be within 1, instead of 2
        if (((node->left != NULL && node->left->height == 1) ||
             (node->right != NULL && node->right->height == 1)) && delta > 4) {
            delta--;
        }

        node->edge_length = ((delta + 1) / 2) - 1;
    }

    //now fill in the height of node
    h = 1;
    if (node->left != NULL) {
        h = MAX(node->left->height + node->edge_length + 1, h);
    }
    if (node->right != NULL) {
        h = MAX(node->right->height + node->edge_length + 1, h);
    }
    node->height = h;
}
//This function prints the given level of the given tree, assuming
//that the node has the given x cordinate.
void print_level(asciinode *node, int x, int level) {
    int i, isleft;
    if (node == NULL) return;
    isleft = (node->parent == -1);
    if (level == 0) {
        for (i = 0; i < (x - print_next - ((node->lablen - isleft) / 2)); i++) {
            printf(" ");
        }
        print_next += i;
        printf("%s", node->label);
        print_next += node->lablen;
    } else if (node->edge_length >= level) {
        if (node->left != NULL) {
            for (i = 0; i < (x - print_next - (level)); i++) {
                printf(" ");
            }
            print_next += i;
            printf("/");
            print_next++;
        }
        if (node->right != NULL) {
            for (i = 0; i < (x - print_next + (level)); i++) {
                printf(" ");
            }
            print_next += i;
            printf("\\");
            print_next++;
        }
    } else {
        print_level(node->left,
                    x - node->edge_length - 1,
                    level - node->edge_length - 1);
        print_level(node->right,
                    x + node->edge_length + 1,
                    level - node->edge_length - 1);
    }
}

//prints ascii tree for given Tree structure
void print_binary_tree(Tree *t) {
    asciinode *proot;
    int xmin, i;
    if (t == NULL) return;
    proot = build_ascii_tree(t);
    compute_edge_lengths(proot);
    for (i = 0; i < proot->height && i < MAX_HEIGHT; i++) {
        leftTree[i] = INFINITY;
    }
    compute_leftTree(proot, 0, 0);
    xmin = 0;
    for (i = 0; i < proot->height && i < MAX_HEIGHT; i++) {
        xmin = MIN(xmin, leftTree[i]);
    }
    for (i = 0; i < proot->height; i++) {
        print_next = 0;
        print_level(proot, -xmin, i);
        printf("\n");
    }
    if (proot->height >= MAX_HEIGHT) {
        printf("(This tree is taller than %d, and may be drawn incorrectly.)\n", MAX_HEIGHT);
    }
    free_ascii_tree(proot);
}
struct node* MinNode(struct node* proot){
    while(proot->left != NULL)
        proot = proot->left;

    return proot;
}
struct node* MaxNode(struct node* proot){
    while(proot->right != NULL)
        proot = proot->right;

    return proot;
}
struct node* search(struct node* proot, int item){
    struct node* p = NULL;
    if(proot != NULL){
        if(proot->info > item)
            p = search(proot->left, item);
        else if(proot->info < item)
            p = search(proot->right, item);
        else
            return proot;

    }
    return p;
}

struct node* delete(struct node *proot, int item)
{
    if(proot==NULL)
        return NULL;
    if (item >  proot->info)
        proot->right = delete(proot->right, item);
    else if(item < proot->info)
        proot->left = delete(proot->left, item);
    else
    {

        //No Children
        if(proot->left==NULL && proot->right==NULL)
        {
            printf("deleted %d", proot->info);
            free(proot);
            return NULL;
        }

        //One Child
        else if(proot->left==NULL || proot->right==NULL)
        {
            struct node *temp;
            if(proot->left ==NULL)
                temp = proot->right;
            else
                temp = proot->left;

            printf("deleted %d", proot->info);
            free(proot);
            return temp;
        }

        //Two Children
        else
        {
            char ch;
            int num;
            struct node *temp;
            printf("right(r) ,left(l) : "); fflush(stdin); scanf("%c",&ch);
            if(ch == 'r'){
                temp = MinNode(proot->right);
                num = proot->info;
                proot->info = temp->info;
                temp->info = num;
                proot->right = delete(proot->right, temp->info);
            }
            else{
                temp = MaxNode(proot->left);
                num = proot->info;
                proot->info = temp->info;
                temp->info = num;
                proot->left= delete(proot->left, temp->info);
            }
        }
    }

    return proot;
}
void clearTree(struct node* proot){
    if(proot == NULL)
        return ;

        clearTree(proot->left);
        clearTree(proot->right);
        printf("\nDeleted : %d",proot->info);
        free(proot);

}

void showMenu(){
    //printf("\n\n");
    printf("\n [1]. Input n node");
    printf("\n [2]. Print");
    printf("\n [3]. Search");
    printf("\n [4]. Delete");
    printf("\n [5]. Clear Tree");
    printf("\n [0]. Exit");
    printf("\n Enter option : ");
}
int main(){
    struct node* root = NULL;

    int ch,n;
    do{
        showMenu();
        scanf("%d",&ch);
        switch (ch)
        {
        case 1 :
            printf("Enter n node :"); scanf("%d",&n);
            root = input(root, n);
            break;
        case 2 :
            // if(root == NULL)
            //     printf("binary tree is empty ");
            // else{
            //     printf("Print in InOrder   : ");
                 inOrder(root);
                 printf("\n\n");
                 printf("\t\t\tPrint in Binary Tree\n\n\n");
            //     printBinaryTree(root,1);
            // }
            print_binary_tree(root);
            break;
        case 3 :
            printf("Enter number to search : "); scanf("%d",&n);
            struct node* temp = search(root, n);
            if(temp == NULL)
                printf("Search not found !!");
            else
                printf("Found [%d] in binary tree", temp->info);
            break;
        case 4 :
            printf("Enter number to delete : "); scanf("%d",&n);
            root = delete(root, n);
            break;
        case 5 :
            clearTree(root);
            break;

        default:
            break;
        }
    }while(ch != 0);
    return 0;
}
