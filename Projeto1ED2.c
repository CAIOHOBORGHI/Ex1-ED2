/*
    PROJETO 1 - ESTRUTURA DE DADOS 2
    Gustavo Marinho
    Caio Borghi
    Unesp Rio Claro - Demac
*/

#include<stdio.h>
#include<conio.h>
#include<stdlib.h>
#include<string.h>

void inserir();
//void remover();
//void compactar();
void carregarArquivos();
//void dump();


//struct contendo os dados de cada livro
struct dados{
    char ISBN[14], tituloLivro[50], autorLivro[50], anoLivro[5];
};

struct dados *vet;

//struct contendo os arquivos para serem removidos
struct data{
    char ISBN[13];
};

char *retornaString(struct dados data);
int header;

int main(void){

    FILE *fileA; //arquivo contendo os dados finais
    char *buffer, *token;
    char tam;
    int campo, opc, i;

    //Verifica se o arquivo já existe, caso contrario, cria-o.
    if((fileA = fopen("dados.bin", "rb")) == NULL){
        fileA = fopen("dados.bin", "wb");
        if(fileA == NULL)
            printf("Erro ao abrir o arquivo");
    }


    //MENU DE ESCOLHAS
    do{
        system("cls");
        printf("MENU, TECLE\n");
        printf("1 - INSERIR\n");
        printf("2 - REMOVER\n");
        printf("3 - COMPACTAR\n");
        printf("4 - CARREGAR ARQUIVOS\n");
        printf("5 - DUMP DO ARQUIVO\n");
        printf("6 - SAIR DO PROGRAMA\n");
        scanf("%d", &opc);

        switch(opc){
	        case 1: 
				inserir(fileA);
				break;
	        //case 2: remover(); break;
	        //case 3: compactar(); break;
	        case 4: 
				carregarArquivos();
				char *teste = retornaString(vet[1]);
				printf("\n%s", teste);
				_getch();				
				break;
	        //case 5: dump(); break;
	        //case 6: exit(0); break;
	        //default: printf("Opcao invalida!"); system("pause");
        }

    }while(opc != 6);

    fclose(fileA);
}

void inserir(FILE *fileA){
	//TODO -> Verificar se existe arquivo .info 
	
	//char *stringInsercao = retornaString();
	
}

void formataString(char cToStr[2], char atual, char *stringInsercao, int *j, int size, int lastProp){
	cToStr[0] = atual;
	if(cToStr[0] == '\0')
	{
		*j = size;
		if(!lastProp)
			strcat(stringInsercao, "#");
	}else
		strcat(stringInsercao, cToStr);
}

char *retornaString(struct dados data){
	char *stringInsercao;
	stringInsercao = malloc(5 + (sizeof(char) * sizeof(data)));
	strcpy(stringInsercao, "\0");
	char cToStr[2];
	cToStr[1] = '\0';
	int size  = 0;
	//4 pois são as propriedades
	for(int property = 0; property < 4; property++){
		switch(property){
			//
			case 0:
				size = 14;
				for(int j = 0; j < size; j++)
					formataString(cToStr,  data.ISBN[j], stringInsercao, &j, size, 0);
				break;
			case 1:
			case 2:
				size = 50;
				for(int j = 0; j < size; j++)
					if(property == 1)
						formataString(cToStr,  data.tituloLivro[j], stringInsercao, &j, size, 0);
					else
						formataString(cToStr,  data.autorLivro[j], stringInsercao, &j, size, 0);
				
				break;
			case 3:
				size = 5;
				for(int j = 0; j < size; j++)
					formataString(cToStr,  data.anoLivro[j], stringInsercao, &j, size, 1);	
				break;
		}		
	}
	int sizeOfReg = strlen(stringInsercao);
	char *finalString = malloc(sizeOfReg + 4);
	snprintf(finalString, sizeOfReg + 4, "%d%s", sizeOfReg, stringInsercao);
	return finalString;
}
void carregarArquivos(){

    FILE *fileB, *fileA;
    int numReg, i=0, tam = 3;
    //struct dados *vet;

    //carrega o conteudo de biblioteca.bin
    fileB = fopen("biblioteca.bin", "rb");
    if(!fileB)
        printf("Não foi possivel abrir o arquivo");
    

    /*carrega o arquivo info.dat, deverá conter 2 inteiros, o primeiro indicará quantos registros foram incluídos,
    e o segundo quantos foram removidos
    */


    if((fileA = fopen("info.dat", "r+b")) == NULL){
        fileA = fopen("info.dat", "w+b");
        if(fileA == NULL)
            printf("Erro ao abrir o arquivo");
    }

    fseek(fileB, 0, SEEK_END);
    long tamanhoArq = ftell(fileB); //calculo o numero de bytes do arquivo
    int numRegs = tamanhoArq / 119;  //calculo o numero de registros no meu vetor
	int tamanhoVet = numRegs * sizeof(struct dados); //Calcula tamanho do meu ponteiro
	
    vet = (struct dados*)malloc(tamanhoVet);
    fseek(fileB, 0, 0);
    fread(vet, tamanhoVet ,1,fileB);

    while(i < numRegs){
            printf("ISBN: %s\n", vet[i].ISBN);
            printf("Titulo: %s\n", vet[i].tituloLivro);
            printf("Autor: %s\n", vet[i].autorLivro);
            printf("Ano: %s\n\n", vet[i].anoLivro);
            i++;
	}
}
