#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX 30
#define CAR 64
char NUOVA_PARTITA[15]="+nuova_partita";
char INSERISCI_INIZIO[18]="+inserisci_inizio";
char INSERISCI_FINE[16]="+inserisci_fine";
char STAMPA_FILTRATE[17]="+stampa_filtrate";

struct nodo{
    char *word;
    struct nodo *next;
};

struct rb_node{
    char *word;
    struct rb_node *left;
    struct rb_node *right;
};

typedef struct nodo *lista;

typedef struct{
    int esatto[CAR];
    int minimo[CAR];
    char **matrice;
}matrix;

/* il mio rb tree */
struct rb_node *rb_tree=NULL;
struct rb_node *albero_filtrate=NULL;
void aggiorna_lista(lista *l, char *f, matrix mat);
void inserisci_in_testa(lista *l, struct rb_node *root)
{
    struct rb_node *tmp=root;
    if(tmp!=NULL){
        inserisci_in_testa(l,tmp->right);
        lista temp = malloc(sizeof(struct nodo));
        temp->word=calloc(MAX,sizeof(char));
        strcpy(temp->word,tmp->word);
        temp->next = *l;
        *l = temp;
        inserisci_in_testa(l,tmp->left);
    }
}

void rimuovi (lista *l, char *w)
{
    if (*l!=NULL)
    {
        if (strcmp((*l)->word, w)==0)
        {
            lista tmp = *l;
            *l = (*l)->next;
            free(tmp->word);
            free(tmp);
        }
        else
            rimuovi(&((*l)->next),w);
    }
}

int lunghezza_lista(lista l)
{
    if (l==NULL)
        return 0;
    else
        return 1 + lunghezza_lista(l->next);
}

/*Inserimento nell'albero*/
void rb_insert(struct rb_node **root, char *w){
    struct rb_node *z=malloc(sizeof(struct rb_node));
    z->word=calloc(MAX,sizeof(char));
    strcpy(z->word,w);
    z->left=NULL;
    z->right=NULL;;
    struct rb_node *x=*root;
    struct rb_node *y=NULL;
    while(x!=NULL){
        y=x;
        if(strcmp(z->word,x->word)<0){
            x=x->left;
        } else {
            x=x->right;
        }
    }
    if(y==NULL){
        *root=z;
    } else if(strcmp(z->word,y->word)<0){
        y->left=z;
    } else {
        y->right = z;
    }
}

/*normalizza i caratteri in modo da associarli a 64 numeri*/
int indice(char c){
    if(c==45)
        return 0;
    else if(c>=48 && c<=57)
        return (c-47);
    else if(c>=65 && c<=90)
        return (c-54);
    else if(c==95)
        return 37;
    else if(c>=97 && c<=122)
        return (c-59);
    else
        return -1;
}

char inverso_indice(int n){
    char c;
    if(n==0)
        c='-';
    else if(n>=1 && n<=10)
        c=n+47;
    else if(n>=11 && n<=36)
        c=n+54;
    else if(n==37)
        c='_';
    else if(n>=38 && n<=64)
        c=n+59;
    else
        c='#';
    return c;
}
void svuota_lista(lista *l){
    while(*l!=NULL){
        lista tmp=*l;
        *l=(*l)->next;
        free(tmp->word);
        free(tmp);
    }
}


/*Procedura dopo inserimento comando +inserisci_inizio*/
void inserisci_nuove_parole(lista *filtrate, char *f, matrix mat){
  struct rb_node *albero_matricole=NULL;
  lista tmp=*filtrate;
    while(tmp!=NULL){
      rb_insert(&albero_filtrate, tmp->word);
      tmp=tmp->next;
    }
    svuota_lista(&tmp);
    int n;
    char *s=calloc(MAX,sizeof(char));
    do{
        n=scanf("%s", s);
        if(strcmp(s,INSERISCI_FINE)==0) {
            break;
        }
        else {
            rb_insert(&albero_matricole, s);
        }
    }while(strcmp(s,INSERISCI_FINE)!=0 && n>0);
    inserisci_in_testa(&tmp,albero_matricole);
    aggiorna_lista(&tmp,f,mat);
    while(tmp!=NULL){
      rb_insert(&albero_filtrate, tmp->word);
      tmp=tmp->next;
    }
    svuota_lista(&tmp);
    inserisci_in_testa(&tmp, albero_filtrate);
}

/*Prima di fare il confronto controllo che la parola appartenga all'insieme delle parole ammissibili*/
int cerca_tra_parole_ammissibili(struct rb_node *root, char *s){
    struct rb_node *tmp=root;
    if(root==NULL)
        return 0;
    else if(strcmp(s,tmp->word)==0)
        return 1;
    else if(strcmp(s,tmp->word)<0)
        return cerca_tra_parole_ammissibili(root->left,s);
    else
        return cerca_tra_parole_ammissibili(root->right,s);

}


/*Produco esito confronto tra due parole, r riferimento s tentativo*/
char *compare(char *r, char *s){
    char *res = calloc(MAX , sizeof(char));
    int i,j,l,k;
    int a[CAR]={0};
    k=strlen(r);
    for(i=0;i<k;i++){/*conto occorrenze ogni carattere*/
        j=indice(r[i]);
        a[j]++;
    }
    for(i=0;i<k;i++){ /*Prima controllo simboli uguali, poi il resto*/
        j=indice(r[i]);
        if(r[i]==s[i]){
            res[i]='+';
            a[j]--;
        }
        else
            res[i]='*'; /*Simbolo ausiliario*/
    }
    for(i=0;i<k;i++){
        l=indice(s[i]);
        if(r[i]!=s[i] && a[l]>0 && res[i]=='*') {
            res[i] = '|';
            a[l]--;
        }
        else if(r[i]!=s[i] && res[i]=='*') {
            res[i]='/';
        }
    }
    res[k]='\0';
    return res;
}

/*Restituisce se possibile il numero esatto di occorrenze del simbolo s*/
int numero_esatto_occorrenze(char *tentativo, char *esito_compare, char s){
    int count=0;
    int flag=0;
    int i,k;
    k=strlen(tentativo);
    for(i=0;i<k;i++){ /*Ho numero esatto se s appare almeno una volta con / in conf*/
        if(tentativo[i]==s && esito_compare[i]=='/'){
            flag=1;
        }
    }
    if(flag==1){
        for(i=0;i<k;i++){
            if(tentativo[i]==s && esito_compare[i]!='/')
                count++;
        }
    }
    else{
        return -1;
    }
    return count; /*Se ho numero esatto occorrenze lo ritorno altrimenti ritorna -1*/
}

/*Restituisce se possibile il numero minimo di occorrenze di s*/
int numero_minimo_occorrenze(char *tentativo, char *esito_compare, char s){ /*Se non ho numero esatto ma ho | conosco numero minimo di occorrenze*/
    int i;
    int count=-1; /*Non ho numero minimo se count = -1*/
    if(numero_esatto_occorrenze(tentativo,esito_compare,s)==-1){  /*Se non ho numero esatto di quel carattere sicuramente ho numero minimo*/
        count=0;
        for(i=0;i<strlen(esito_compare);i++){
            if(esito_compare[i]!='/' && tentativo[i]==s){
                count++;
            }
        }
    }
    return count;
}


/*Devo controllare che quel simbolo ci sia almeno n volte, prendo in ingresso la parola di cui devo controllare se c ci sta almeno n volte
 * restituisco uno se quel carattere non ci sta abbastanza volte, altrimenti zero*/
int non_contiene_abbastanza_volte_simbolo(char *parola, char c, int n){
    int i,count;
    count=0;
    for(i=0;i<strlen(parola);i++){
        if(parola[i]==c)
            count++;
    }
    if(count<n)
        return 1;
    else
        return 0;
}

/*Prende in ingresso parola, il carattere di cui so numero esatto e il numero di volte in cui deve esserci e ritorna uno se
 * effettivamente contiene un numero sbagliato di volte quel carattere */
int contiene_un_numero_sbagliato_di_volte_quel_simbolo(char *parola, char c, int n){
    int i,count;
    count=0;
    for(i=0;i<strlen(parola);i++){
        if(parola[i]==c)
            count++;
    }
    if(count!=n)
        return 1;
    else
        return 0;
}


/*Se esito_compare produce un + lo metto in stringa uscita */
void aggiorno_filtro_simboli_che_conosco(char *tentativo, char* esito_compare, char *filtro){
    int i;
    for(i=0;i<strlen(esito_compare);i++){
        if(esito_compare[i]=='+')
            filtro[i]=tentativo[i];
    }
}

/*Controllo che la parola rispetti i vincoli imposti dai +*/
int check(char *parola, char *filtro){
    int i,flag;
    flag=0;
    i=0;
    while(i<strlen(filtro)){
        if(filtro[i]=='*')
            i++;
        else if(filtro[i]!=parola[i]){ /*Se ho carattere e la parola considerata non lo ha la scarto altrimenti passo al carattere successivo*/
            flag=1;
            break;
        }
        else{
            i++;
        }
    }
    return flag;
}

/*int check_minime(char *parola, matrix mat){
    int i,flag;
    flag=0;
    for(i=0;i<CAR;i++){
        if(mat.minimo[i]>0 && non_contiene_abbastanza_volte_simbolo(parola, inverso_indice(i), mat.minimo[i])==1){
            flag=1;
            break;
        }
    }
    return flag;
}*/

int check_esatte_e_minime(char *parola, matrix mat){
    int i,flag;
    flag=0;
    for(i=0;i<CAR;i++){
        if(mat.esatto[i]>0 && contiene_un_numero_sbagliato_di_volte_quel_simbolo(parola, inverso_indice(i), mat.esatto[i])==1){
            flag=1;
            break;
        }
        if(mat.minimo[i]>0 && non_contiene_abbastanza_volte_simbolo(parola, inverso_indice(i), mat.minimo[i])==1){
            flag=1;
            break;
          }
    }
    return flag;
}

/*Se e_filtrata è uguale a zero la parola va bene: per ogni parola controllo la rispettiva cella nella matrice e decido se scartare o meno*/
int e_filtrata(char *parola, matrix mat, char *f) {
    char j;
    int i,k,flag;
    int l,m;
    flag=0;
    i=0;
    k=strlen(parola);
    if(check(parola,f)==0 && check_esatte_e_minime(parola,mat)==0){  /*Controllo che la parola passi il filtro dei + e i caratteri minime sennò la scarto subito*/
        while(i<k){
            j=mat.matrice[indice(parola[i])][i];
            l=mat.esatto[indice(parola[i])];  /*Numero esatto per quel carattere se c'è*/
            m=mat.minimo[indice(parola[i])];  /*Numero minimo per quel carattere se c'è*/
            if(j=='+'){
                i++;
            }
            else if(j=='/'){
                flag=1;
                break;
            }
           else if(l!=-1){
                if(contiene_un_numero_sbagliato_di_volte_quel_simbolo(parola, parola[i], l)==1){
                   flag=1;
                    break;
                }
                else{
                    i++;
                }
            }
            else if(m>0){
                if(non_contiene_abbastanza_volte_simbolo(parola,parola[i], m)==1){
                    flag=1;
                    break;
                }
                else{
                    i++;
                }
            }
            else{
                i++;
           }
        }
    }
    else{
        flag=1;
    }
    return flag;
}

/*Modifica lista ogni volta che faccio un tentativo*/
void aggiorna_lista(lista *l, char *f, matrix mat){
    lista tmp = *l;
    while((tmp)!=NULL){
        char *word = (tmp)->word;
        tmp = (tmp)->next;
        if(e_filtrata(word,mat,f)!=0){
            rimuovi(l,word);
        }
    }
}


/*Procedura dopo comando +stampa_filtrate*/
void filtra_parole_ammissibili(lista l) {
    if (l==NULL)
        return;
    else
    {
        printf("%s\n",l->word);
        filtra_parole_ammissibili(l->next);
    }
}

/*Completo la matrice a ogni tentativo*/
matrix aggiungi_vincolo(char *tentativo, char *esito_compare, matrix mat){
    int i,j,l,k,m,x;
    k=strlen(tentativo);
    for (i = 0; i < k; i++) {
        j = numero_esatto_occorrenze(tentativo, esito_compare, tentativo[i]);
        l = numero_minimo_occorrenze(tentativo, esito_compare, tentativo[i]);
        x=indice(tentativo[i]);
        m = mat.minimo[x];
        if (j != -1) {
            mat.esatto[x]=j;  /*Se ho numero esatto metto intero trasformato in carattere che poi riconverto*/
        } else if (l != -1 && l > m) {         /*Modifico numero minimo solo se non lo avevo o quello che ho ora è maggiore del precedente*/
            mat.minimo[x]=l;  /*Se ho numero minimo metto intero trasformato in carattere e poi riconverto*/
        }
        if (esito_compare[i] == '+') {
            mat.matrice[x][i] = '+';
        } else if (esito_compare[i] == '/'|| esito_compare[i] == '|' ) {
            mat.matrice[x][i] = '/';
        }
    }
    /*for (i = 0; i <k; i++) {
        if (esito_compare[i] == '+') {
            mat.matrice[indice(tentativo[i])][i] = '+';
        } else if (esito_compare[i] == '/') {
            mat.matrice[indice(tentativo[i])][i] = '/';
        } else if (esito_compare[i] == '|') {
            mat.matrice[indice(tentativo[i])][i] = '|';
        }
    }*/

    return mat;
}


/*Procedura dopo che inizio partita e switch tra vari comandi*/
void inizia_nuova_partita(matrix mat){
    int n,i,j,k;
    int mosse_rimanenti;
    char *riferimento=calloc(MAX,sizeof(char));
    char *s =calloc(MAX,sizeof(char));
    char *t,*f;
    lista filtrate=NULL;
    f=NULL;
    if(scanf("%s", riferimento)>0) {   /*Leggo parola da indovinare*/
        k=strlen(riferimento);
        f=calloc(k,sizeof(char));
        for(i=0;i<k;i++){
            f[i]='*';
        }
        f[k]='\0';
        if (scanf("%d", &mosse_rimanenti) > 0) {  /*Leggo numero di tentativi possibili*/
            for(i=0;i<CAR;i++){   /*Azzero matrice*/
                for(j=0;j<k+1;j++){
                    mat.matrice[i][j]='*';
                    mat.esatto[i]=-1;
                    mat.minimo[i]=0;
                }
            }

            inserisci_in_testa(&filtrate, rb_tree); /*Inserisco tutte le parole dell'albero nella lista delle parole filtrate*/
            do {
                n = scanf("%s", s);
                if (strcmp(riferimento, s) == 0) {
                    printf("ok\n");
                    break;
                }
                else if (strcmp(s, INSERISCI_INIZIO) == 0) {
                    inserisci_nuove_parole(&filtrate,f,mat);
                    aggiorna_lista(&filtrate,f,mat);
                }
                else if (strcmp(s, STAMPA_FILTRATE) == 0)
                    filtra_parole_ammissibili(filtrate);
                else if (strcmp(s, NUOVA_PARTITA) == 0)
                    inizia_nuova_partita(mat);
                else if (cerca_tra_parole_ammissibili(rb_tree,s) == 0)
                    printf("not_exists\n");
                else {
                    t=compare(riferimento,s);
                    printf("%s\n", t);
                    mat=aggiungi_vincolo(s,t,mat);
                    if(strchr(t,'+')!=NULL){
                    aggiorno_filtro_simboli_che_conosco(s,t,f);    /*A ogni tentativo aggiorno il filtro con i simboli +*/
                   } aggiorna_lista(&filtrate, f, mat);                 /*Aggiorno la lista delle parole filtrate*/
                    printf("%d\n", lunghezza_lista(filtrate));          /*Per sapere quante parole vanno ancora bene conto lunghezza lista*/
                    mosse_rimanenti--;
                }
            } while (mosse_rimanenti > 0 && n > 0);
            if(mosse_rimanenti==0)
                printf("ko\n");
        }
    }
}

int main() {
    int k,i,n;
    lista filtrate=NULL;
    char *f=NULL;
    matrix mat;
    mat.matrice=(char**)malloc(CAR*sizeof(char*));  /*Alloco le righe*/
    char *s= calloc(MAX, sizeof(char));
    if(scanf("%d", &k)>0) {
        f=calloc(MAX,sizeof(char));
        for(i=0;i<k;i++){
            f[i]='*';
        }
        f[k]='\0';
        for(i=0;i<CAR;i++){
            mat.matrice[i]=malloc(sizeof(char)*(k+1));   /*Alloco le colonne k=lunghezza parola e due array per numero esatto e numero minimo*/
        }
        do {                                        /*Main loop*/
            n = scanf("%s", s);
            if (strcmp(s, NUOVA_PARTITA) == 0) {
                inizia_nuova_partita(mat);
            } else if (strcmp(s, INSERISCI_INIZIO) == 0) {
                inserisci_nuove_parole(&filtrate,f,mat);
                aggiorna_lista(&filtrate,f,mat);
            }
            else {
                rb_insert(&rb_tree,s);
            }
        } while (n > 0);
    }
    return 0;
}
