#include <stdio.h>
#include <stdlib.h>

typedef struct node{
    int freq;
    char ch;
    struct node* left;
    struct node* right;
} Node;

typedef struct queue{
    int MAX;
    int current_size;
    Node** array;
} Queue;

int read_file(Queue* Queue_Data);
int read_char(Queue* Queue_Data, char data[], int i);
int create(Queue* Queue_Data, int n);
int enqueue(Queue* Queue_Data, Node* ptr);
Node* dequeue(Queue* Queue_Data);
Node* top(Queue* Queue_Date);
int heapify_up(Queue* Queue_Data, int i);
int heapify_down(Queue* Queue_Data, int i, int n);
int swap(Node** a, Node** b);
int create_Huffman(Queue* Queue_Data);
int encode(Node* root, int data[], int top, FILE* fp, char ch);
int code_disp(int data[], int n, FILE* fp);
int decode(Node* root);
int huffman_bits_count(Node* ptr, int level);

int main(){
    Queue Queue_Data;
    create(&Queue_Data, 256);

    int ascii_bits, huffman_bits;
    ascii_bits = read_file(&Queue_Data); /*read_file() returns total ascii bits required*/
    create_Huffman(&Queue_Data);
    huffman_bits = huffman_bits_count(Queue_Data.array[1], 1);

    int data[1000];

    FILE* fp = fopen("code.txt", "w");
    FILE* fp2 = fopen("file.txt", "r");
    char ch;
    while((ch=fgetc(fp2))!=EOF){
        encode(top(&Queue_Data), data, 0, fp, ch);
    }
    fclose(fp);
    fclose(fp2);

    decode(top(&Queue_Data));

    printf("huffmann bits : %d\nascii bits : %d\n", huffman_bits, ascii_bits);
    printf("Efficiency : %f\n", (float)huffman_bits/ascii_bits*100);


    return 1;
}

int read_file(Queue* Queue_Data){
    FILE* fp;                   /* Reads string from file.txt */
    char name[] = "file.txt";
    fp = fopen(name, "r");
    if(fp!=NULL){
        char ch, data[1000];
        int i=0;
        while((ch=fgetc(fp))!=EOF){
            data[i++] = ch;
        }
        fclose(fp);
        return read_char(Queue_Data, data, i);  /*read_char() return total ascii bits which is assigned to main*/
    }
    else{
        printf("Error in opening file\n");
        return -1;
    }
}

int read_char(Queue* Queue_Data, char data[], int i){
    int j, k, ascii_bits=0;        /* Reads entire string{data[]} && counts frequency && total ascii bits*/
    Node* ptr;
    for(j=0; j<i-1; j++){
        if(data[j]==-1){
            continue;
        }
        int freq = 1;
        char ch = data[j];
        for(k=j+1; k<i; k++){
            if(data[k]==ch){
                freq++;
                data[k] = -1;
            }
        }
        printf(" '%c' = %d\n", ch, freq);  /*unique character and frequency*/
        ascii_bits += 8; /* Since 1 a.s.c.i.i char takes 8 bits* */
        ptr = (Node*)malloc(sizeof(Node));  /*Node creation for Priority Queue*/
        ptr->ch = ch;
        ptr->freq = freq;
        ptr->left = NULL;
        ptr->right = NULL;
        enqueue(Queue_Data, ptr);
    }
    return ascii_bits;
}

int create(Queue* Queue_Data, int n){
    Queue_Data->MAX = n;
    Queue_Data->current_size = 1;
    Queue_Data->array = (Node**)calloc(n+1, sizeof(Node*)); /*array of pointers for priority queue*/
    return 1;
}

int enqueue(Queue* Queue_Data, Node* ptr){
    if(Queue_Data->current_size==Queue_Data->MAX+1){    /*overflow condition*/
        printf("The Queue is full\n");
        return 0;
    }
    else{
        Queue_Data->array[Queue_Data->current_size++] = ptr;
        heapify_up(Queue_Data, Queue_Data->current_size-1);
    }
    return 1;
}

Node* dequeue(Queue* Queue_Data){
    if(Queue_Data->current_size>1){
        Node* ptr = Queue_Data->array[1];
        swap(&Queue_Data->array[1], &Queue_Data->array[--Queue_Data->current_size]);
        heapify_down(Queue_Data, 1, Queue_Data->current_size-1);
        return ptr;
    }
    else{   /*Underflow condition*/
        printf("There are no elements in the queue\n");
        return NULL;
    }
}

Node* top(Queue* Queue_Data){
    /*return pointer to first node in priority queue no dequeue involved*/
    if(Queue_Data->current_size > 1  &&  Queue_Data->current_size < Queue_Data->MAX+1){
        return Queue_Data->array[1];
    }
    return NULL;
}

int heapify_up(Queue* Queue_Data, int i){
    /* Since Queue enqueue starts from index : 1*/
    if(i>1){
        int parent = i/2;
        if(Queue_Data->array[i]->freq < Queue_Data->array[parent]->freq){
            swap(&Queue_Data->array[i], &Queue_Data->array[parent]);
            heapify_up(Queue_Data, i/2);
        }
    }
    return 1;
}

int heapify_down(Queue* Queue_Data, int i, int n){
    int left = 2*i;
    int right = 2*i+1;
    /*If more than 2 elements are present in Priority Queue*/
    if(left<n){
        if(Queue_Data->array[right]->freq < Queue_Data->array[i]->freq){
            swap(&Queue_Data->array[right], &Queue_Data->array[i]);
            heapify_down(Queue_Data, right, n);
        }
        if(Queue_Data->array[left]->freq < Queue_Data->array[i]->freq){
            swap(&Queue_Data->array[left], &Queue_Data->array[i]);
            heapify_down(Queue_Data, left, n);
        }

    }
        /*If only 2 elements are present in Priority Queue*/
    else if(left==n){
        if(Queue_Data->array[left]->freq<Queue_Data->array[i]->freq){
            swap(&Queue_Data->array[left], &Queue_Data->array[i]);
        }
    }
    return 1;
}

int swap(Node** a, Node** b){
    /*Swapping of values between 2 elements of Priority Queue*/
    Node* temp = *a;
    *a = *b;
    *b = temp;
    return 1;
}

int create_Huffman(Queue* Queue_Data){
    int i=1, n = Queue_Data->current_size;
    Node* ptr;
    /*Special case if only 1 unique character
    of any frequency is present in file.txt*/
    if(i==n-1){
        ptr = (Node*)malloc(sizeof(Node));
        ptr->ch = -1;
        ptr->left = dequeue(Queue_Data);
        ptr->right = NULL;
        ptr->freq = ptr->left->freq;
        enqueue(Queue_Data, ptr);
    }
        /*When more than 1 unique characters are present*/
    else{
        for(i=1; i<n-1; i++){
            ptr = (Node*)malloc(sizeof(Node));
            ptr->ch = -1;
            ptr->left = dequeue(Queue_Data);
            ptr->right = dequeue(Queue_Data);
            ptr->freq = ptr->left->freq + ptr->right->freq;
            enqueue(Queue_Data, ptr);
        }
    }
    return 1;
}

int encode(Node* root, int data[], int top, FILE* fp, char ch){
    /*Traversal through the binary tree
    along with steps(code) written on int data[]
    for every left or right*/
    if(root->left){
        data[top] = 0;
        encode(root->left, data, top+1, fp, ch);
    }
    if(root->right){
        data[top] = 1;
        encode(root->right, data, top+1, fp, ch);
    }
    if(root->ch==ch){
        /*Special case for only 1 unique character
        of any frequency in file.txt*/
        if(top==0){
            data[top] = 0;
            code_disp(data, top+1, fp);
        }
            /*When more than 1 unique characters
            are present*/
        else{
            code_disp(data, top, fp);
        }
    }
    return 1;
}

int code_disp(int data[], int n, FILE* fp){
    /*Whenever a character is detected
    the steps(huffman-code) is written on code.txt*/
    int i=0;
    for(i=0; i<n; i++){
        fprintf(fp, "%d", data[i]);
    }
    return 1;
}

int decode(Node* root){
    int i, n=0;
    char data[1000];
    Node* ptr = root;
    FILE* fp = fopen("code.txt", "r");
    char ch;
    while((ch=fgetc(fp))!=EOF){
        data[n++] = ch;
    }
    fclose(fp);
    printf("\nEncoded String[Saved in code.txt] : ");
    for(i=0; i<n; i++){
        printf("%c", data[i]);
    }
    printf("\n\nThe Decoded String : ");
    /*Tree traversal according
    to the code in code.txt  0->left, 1->right*/
    for(i=0; i<n; i++){
        if(data[i]=='0'){
            ptr = ptr->left;
            if(ptr->ch!=-1){
                printf("%c", ptr->ch);
                ptr = root;
            }
        }
        else if(data[i]=='1'){
            ptr = ptr->right;
            if(ptr->ch!=-1){
                printf("%c", ptr->ch);
                ptr = root;
            }
        }
    }
    printf("\n\n");
    return 1;
}

int huffman_bits_count(Node* ptr, int level){
    /*Recursively travel through every node to count
    the level that in turn is equal to the bits for each character*/
    if(ptr==NULL){
        return 0;
    }
    int counter = 0;
    if(ptr->left){
        if(ptr->left->ch!=-1){
            counter+=level;
        }
    }
    if(ptr->right){
        if(ptr->right->ch!=-1){
            counter+=level;
        }
    }
    return counter+huffman_bits_count(ptr->left, level+1)+huffman_bits_count(ptr->right, level+1);
}