#include<stdio.h>

int main() {
    FILE *fd;

    //////////////////////////////
    struct livro {
        char isbn[14];
        char titulo[50];
        char autor[50];
        char ano[5];
    } vet[3] = {{"1111111111111", "Dom Quixote", "Miguel de Cervantes", "1600"},
                {"2222222222222", "A Parte Que Falta", "Silverstein", "2016"},
                {"3333333333333", "O Poder do Hábito", "C. Duhigg", "2015"}};

    fd = fopen("biblioteca.bin", "w+b");
    fwrite(vet, sizeof(vet), 1, fd);
    fclose(fd);

    //////////////////////////////
	struct remove {
        char isbn[14];
    } vet_r[3] = {{"1111111111111"},
                {"7777777777777"},
                {"3333333333333"}};

    fd = fopen("remove.bin", "w+b");
    fwrite(vet_r, sizeof(vet_r), 1, fd);
    fclose(fd);
}

