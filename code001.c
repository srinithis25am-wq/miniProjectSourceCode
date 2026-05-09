#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 100

typedef struct {
    unsigned int acctNum;
    char lastName[15];
    char firstName[10];
    double balance;
} Client;

// Helper functions
void clearBuffer() { int c; while((c=getchar())!='\n' && c!=EOF); }
void readAccount(FILE *f, unsigned int n, Client *c) { fseek(f,(n-1)*sizeof(Client),SEEK_SET); fread(c,sizeof(Client),1,f); }
void writeAccount(FILE *f, unsigned int n, Client *c) { fseek(f,(n-1)*sizeof(Client),SEEK_SET); fwrite(c,sizeof(Client),1,f); }

void initializeFile(FILE *f) {
    Client blank={0,"","",0};
    for(int i=0;i<MAX;i++) fwrite(&blank,sizeof(Client),1,f);
}

unsigned int menu() {
    unsigned int ch;
    printf("\n1-TextFile 2-Update 3-New 4-Delete 5-Display\n6-SearchNum 7-SearchName 8-High 9-Low 10-Count 11-Transfer 12-Exit\nChoice: ");
    scanf("%u",&ch); clearBuffer(); return ch;
}

// Operations
void newRecord(FILE *f) {
    unsigned int n; Client c;
    printf("New account (1-%d): ", MAX); scanf("%u",&n); clearBuffer();
    if(n<1||n>MAX) { printf("Invalid\n"); return; }
    readAccount(f,n,&c);
    if(c.acctNum!=0){ printf("Exists\n"); return; }
    printf("Last First Balance: "); scanf("%14s %9s %lf",c.lastName,c.firstName,&c.balance); clearBuffer();
    c.acctNum=n; writeAccount(f,n,&c); printf("Created\n");
}

void updateRecord(FILE *f) {
    unsigned int n; double amt; Client c;
    printf("Account: "); scanf("%u",&n); clearBuffer();
    if(n<1||n>MAX){ printf("Invalid\n"); return; }
    readAccount(f,n,&c);
    if(c.acctNum==0){ printf("Not found\n"); return; }
    printf("Balance: %.2f. Enter +deposit/-withdraw: ",c.balance); scanf("%lf",&amt); clearBuffer();
    if(c.balance+amt<0){ printf("Insufficient\n"); return; }
    c.balance+=amt; writeAccount(f,n,&c); printf("Updated %.2f\n",c.balance);
}

void deleteRecord(FILE *f) {
    unsigned int n; Client blank={0,"","",0}, c;
    printf("Delete account: "); scanf("%u",&n); clearBuffer();
    if(n<1||n>MAX){ printf("Invalid\n"); return; }
    readAccount(f,n,&c);
    if(c.acctNum==0){ printf("Not found\n"); return; }
    writeAccount(f,n,&blank); printf("Deleted\n");
}

void displayAll(FILE *f) {
    Client c; rewind(f);
    printf("\n%-6s%-15s%-10s%10s\n","Acct","Last","First","Balance");
    for(int i=0;i<MAX;i++){
        fread(&c,sizeof(Client),1,f);
        if(c.acctNum!=0) printf("%-6d%-15s%-10s%10.2f\n",c.acctNum,c.lastName,c.firstName,c.balance);
    }
}

void searchByNumber(FILE *f) {
    unsigned int n; Client c;
    printf("Account number: "); scanf("%u",&n); clearBuffer();
    if(n<1||n>MAX){ printf("Invalid\n"); return; }
    readAccount(f,n,&c);
    if(c.acctNum!=0) printf("Account %d %s %s Balance %.2f\n",c.acctNum,c.lastName,c.firstName,c.balance);
    else printf("Not found\n");
}

void searchByName(FILE *f) {
    char name[15]; Client c; int found=0;
    printf("Last Name: "); scanf("%14s",name); clearBuffer();
    rewind(f);
    for(int i=0;i<MAX;i++){
        fread(&c,sizeof(Client),1,f);
        if(c.acctNum!=0 && strcmp(c.lastName,name)==0){ printf("%d %s %s %.2f\n",c.acctNum,c.lastName,c.firstName,c.balance); found=1; }
    }
    if(!found) printf("No match\n");
}

void highestBalance(FILE *f) {
    Client c,max; int found=0; rewind(f);
    for(int i=0;i<MAX;i++){ fread(&c,sizeof(Client),1,f); if(c.acctNum!=0 && (!found||c.balance>max.balance)){ max=c; found=1; } }
    if(found) printf("Highest: %d %s %s %.2f\n",max.acctNum,max.lastName,max.firstName,max.balance);
    else printf("No accounts\n");
}

void lowestBalance(FILE *f) {
    Client c,min; int found=0; rewind(f);
    for(int i=0;i<MAX;i++){ fread(&c,sizeof(Client),1,f); if(c.acctNum!=0 && (!found||c.balance<min.balance)){ min=c; found=1; } }
    if(found) printf("Lowest: %d %s %s %.2f\n",min.acctNum,min.lastName,min.firstName,min.balance);
    else printf("No accounts\n");
}

void countAccounts(FILE *f) {
    Client c; int cnt=0; rewind(f);
    for(int i=0;i<MAX;i++){ fread(&c,sizeof(Client),1,f); if(c.acctNum!=0) cnt++; }
    printf("Active accounts: %d\n",cnt);
}

void transferMoney(FILE *f) {
    unsigned int from,to; double amt; Client c1,c2;
    printf("FROM: "); scanf("%u",&from);
    printf("TO: "); scanf("%u",&to);
    printf("Amount: "); scanf("%lf",&amt); clearBuffer();
    readAccount(f,from,&c1); readAccount(f,to,&c2);
    if(c1.acctNum==0||c2.acctNum==0){ printf("Invalid\n"); return; }
    if(c1.balance<amt){ printf("Insufficient\n"); return; }
    c1.balance-=amt; c2.balance+=amt; writeAccount(f,from,&c1); writeAccount(f,to,&c2);
    printf("Transferred\n");
}

void createTextFile(FILE *f){
    FILE *txt=fopen("accounts.txt","w"); Client c;
    if(!txt){ printf("Cannot create file\n"); return; }
    rewind(f); fprintf(txt,"%-6s%-15s%-10s%10s\n","Acct","Last","First","Balance");
    for(int i=0;i<MAX;i++){ fread(&c,sizeof(Client),1,f); if(c.acctNum!=0) fprintf(txt,"%d %-15s %-10s %.2f\n",c.acctNum,c.lastName,c.firstName,c.balance); }
    fclose(txt); printf("Text file created\n");
}

// Main
int main() {
    FILE *f = fopen("credit.dat","rb+");
    if(!f){ f=fopen("credit.dat","wb+"); if(!f){ printf("Cannot create file\n"); exit(1); } initializeFile(f); }

    while(1){
        switch(menu()){
            case 1: createTextFile(f); break;
            case 2: updateRecord(f); break;
            case 3: newRecord(f); break;
            case 4: deleteRecord(f); break;
            case 5: displayAll(f); break;
            case 6: searchByNumber(f); break;
            case 7: searchByName(f); break;
            case 8: highestBalance(f); break;
            case 9: lowestBalance(f); break;
            case 10: countAccounts(f); break;
            case 11: transferMoney(f); break;
            case 12: fclose(f); printf("Exiting\n"); exit(0);
            default: printf("Invalid\n");
        }
    }
}