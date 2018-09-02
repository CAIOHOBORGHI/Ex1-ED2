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
#include<locale.h>

void inserir();
//void remover();
//void compactar();
void carregarArquivos();
FILE *criaArquivo();
void atualizaHeader();
void atualizaIncluidos();
int getHeader();
int getIncluidos();
void achaRegistro();
//void dump();


//struct contendo os dados de cada livro
typedef struct{
    char ISBN[14], tituloLivro[50], autorLivro[50], anoLivro[5];
}dados;

typedef struct{
	char *ISBN, *tituloLivro, *autorLivro, *anoLivro;
}livroD;

typedef struct{
	dados *registros;
	int count;
}listaRegistros;
listaRegistros lista;

//struct contendo os arquivos para serem removidos
struct data{ char ISBN[13]; };

char *retornaString(dados data);
int header = 1;

int main(void){
	setlocale(LC_ALL, "");
    FILE *fileA; //arquivo contendo os dados finais
    char *buffer, *token;
    char tam;
    int campo, opc, i;
    lista.count = 0;

    //Verifica se o arquivo já existe, caso contrario, cria-o.
    if((fileA = fopen("dados.bin", "rb+")) == NULL)
        fileA = criaArquivo("dados.bin");

	achaRegistro("", fileA);
    //MENU DE ESCOLHAS
    do{
        system("cls");
        printf("MENU, TECLE\n");
        printf("1 - INSERIR\n");
        printf("2 - REMOVER\n");
        printf("3 - COMPACTAR\n");
        printf("4 - CARREGAR ARQUIVOS\n");
        printf("5 - DUMP DO ARQUIVO\n");
        printf("0 - SAIR DO PROGRAMA\n");
        scanf("%d", &opc);

        switch(opc){
	        case 1: 
				inserir(fileA);
				_getch();
				break;
	        //case 2: remover(); break;
	        //case 3: compactar(); break;
	        case 4: 
				carregarArquivos();	
				break;
	        //case 5: dump(); break;
	        //case 6: exit(0); break;
	        //default: printf("Opcao invalida!"); system("pause");
        }

    }while(opc != 0);

    fclose(fileA);
}

void inserir(FILE *fp){
	header = getHeader(fp);
	if(header == -1){
		fseek(fp, 0, SEEK_END);
		if(lista.registros == NULL || lista.count == 0)
			carregarArquivos();
		
		//Posição da lista deve ser buscada no arquivo info.dat
		int pos = getIncluidos();
		if(pos == lista.count)
			printf("\nTodos os registros já foram adicionados");
		else{
			printf("\nInserindo registro...");
			char *stringInsercao = retornaString(lista.registros[pos]);
			fprintf(fp, stringInsercao);
			atualizaIncluidos(++pos);
			printf("\nRegistro incluído com sucesso!");
		}
	}else{
		//Header é o offset para primeiro arquivo excluído
		fseek(fp, 0, SEEK_SET);
		fseek(fp, header, SEEK_SET);
	}
}

void atualizaIncluidos(int novoIncluidos){
	FILE *fp = fopen("info.dat", "rb+");
	if(fp == NULL)
	{
		printf("Erro ao atualizar arquivo de informações");
		exit(0);
	}
	fprintf(fp, "%d", novoIncluidos);
	fclose(fp);
}

FILE *criaInfo(){
	FILE *info = fopen("info.dat", "wb");
	if(fopen == NULL){
		printf("Erro ao abrir arquivo");
		exit(0);
	}
	
	fprintf(info, "0 0");
	return info;
}

int getIncluidos(){
	int incluidos = 0;
	FILE *fp = fopen("info.dat", "rb+");
	//Arquivo info.dat ainda não existe
	//Info.dat -> 2 ints, o primeiro para incluídos e o segundo para excluídos
	if(fp == NULL){
		fp = criaInfo();
	}else{
		fseek(fp, 0, SEEK_SET);
		fscanf(fp, "%d", &incluidos);
	}
	fclose(fp);
	return incluidos;
}

int getHeader(FILE *fp){
	if(fp == NULL){
		printf("Erro ao buscar header do arquivo");
		_getch();
		exit(0);
	}
	fseek(fp, 0, SEEK_SET);
	int header;
	fread(&header, sizeof(int), 1,fp);
	return header;
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

char *retornaString(dados data){
	char *stringInsercao = malloc(sizeof(char) * sizeof(data)); //Atribui tamanho máximo que a string pode ter
	strcpy(stringInsercao, "\0");
	char cToStr[2];
	cToStr[1] = '\0'; //Usa-se string para poder user função concat
	int size  = 0; //Tamanho das strings dentro do objeto Data
	
	//4 pois são as propriedades
	for(int property = 0; property < 4; property++){
		switch(property){
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

FILE *criaArquivo(char *name){
	FILE *fp = fopen(name, "wb+");
	if(fp == NULL){
    	printf("Erro ao abrir o arquivo");
    	exit(0);
    }
    
    //Adiciona cabeçalho
    atualizaHeader(fp, -1);
    return fp;
}

int arquivoVazio(FILE *fp){
	if(fp == NULL)
		return 1;
		
	int size = 0;
	int curr = ftell(fp); //armazena posição atual;
	fseek(fp, 0, SEEK_END);
	if(ftell(fp) == size)
		return 1;
	
	fseek(fp, 0, SEEK_SET);
	fseek(fp, curr, SEEK_SET);
	return 0;
}

void atualizaHeader(FILE *fp, int novoHeader){
	int empty = arquivoVazio(fp);
    fseek(fp, 0, SEEK_SET);//Volta ao início do arquivo
    int header = 0;
	if(!empty){
		printf("\nArquivo não vazio...");
		fread(&header, sizeof(int), 1, fp);
		fseek(fp, 0, SEEK_SET);
		fwrite(&novoHeader, sizeof(int),1,fp);		
	}else{
		printf("\nDefinindo header....");
		fwrite(&novoHeader, sizeof(int),1,fp);
	}
}

int getRegTam(FILE *fp){
	char aux[3];
	aux[2] = '\0';
	aux[0] = fgetc(fp);
	aux[1] = fgetc(fp);
	return atoi(aux);
}

livroD* getLivroD(FILE *fp){
	if(ftell(fp) == 0){
		//pula header
		fseek(fp, sizeof(int), SEEK_CURR);
	}
	livroD *livro = (livroD *)malloc(sizeof(livroD));
	int tam = getRegTam(fp);
	char cToStr[2];
	cToStr[0] = '\0';
	cToStr[1] = '\0';
	char *wordBuffer = malloc(sizeof(tam) + 1);
	int prop = 0;
	
	for(int i = 0; i < tam && cToStr[0] != '$'; i++){
		cToStr[0] = fgetc(fp);
		if(cToStr[0] == '#' || i == tam-1){
			switch(prop){
				case 0:
					livro->ISBN = malloc(strlen(wordBuffer) + 1);
					strcpy(livro->ISBN, wordBuffer);
					break;		
				case 1:
					livro->tituloLivro = malloc(strlen(wordBuffer) + 1);
					strcpy(livro->tituloLivro, wordBuffer);
					break;
				case 2:
					livro->autorLivro = malloc(strlen(wordBuffer) + 1);
					strcpy(livro->autorLivro, wordBuffer);
					break;
				case 3:
					strcat(wordBuffer, cToStr);
					livro->anoLivro = malloc(strlen(wordBuffer) + 1);
					strcpy(livro->anoLivro, wordBuffer);
					break;
			}
			prop++;
			strcpy(wordBuffer,"");
		}
		else
			strcat(wordBuffer, cToStr);
	}
	return livro;
}

void achaRegistro(char ISBN[14], FILE *fp){
	if(fp == NULL){
		printf("\nArquivo está vazio!");
		return;
	}
	int size = 0;
	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	rewind(fp);
	
	while(1){
		if(ftell(fp) == size)//Fim de arquivo
			break;
		livroD *aux = getLivroD(fp);

	};
	_getch();
}


void carregarArquivos(){
    FILE *fileB;
    int numReg, i=0, tam = 3;
    //struct dados *listaRegistros;

    //carrega o conteudo de biblioteca.bin
    fileB = fopen("biblioteca.bin", "rb");
    if(!fileB)
        printf("Não foi possivel abrir o arquivo");

	printf("\nCarregando registros da biblioteca...");
    fseek(fileB, 0, SEEK_END);
    long tamanhoArq = ftell(fileB); //calculo o numero de bytes do arquivo
    int numRegs = tamanhoArq / 119;  //calculo o numero de registros no meu listaRegistrosor
	int tamanholistaRegistros = numRegs * sizeof(dados); //Calcula tamanho do meu ponteiro
	
    lista.registros = (dados*)malloc(tamanholistaRegistros);
    lista.count = numRegs;
    fseek(fileB, 0, 0);
    fread(lista.registros, tamanholistaRegistros, 1,fileB);
    printf("\nRegistros carregados...");
    fclose(fileB);
}
