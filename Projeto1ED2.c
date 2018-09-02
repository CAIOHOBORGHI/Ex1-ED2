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
void compactar();
void carregarArquivos();
FILE *criaArquivo();
void atualizaHeader();
void atualizaInfos();
void getInfos();
int getHeader();
int getRegTam(FILE *fp);
int excluiRegistro(char ISBN[14], FILE *fp);
void remover(FILE *fp);
//void dump();


//struct contendo os dados de cada livro
typedef struct{
    char ISBN[14], tituloLivro[50], autorLivro[50], anoLivro[5];
}dados;

typedef struct{
	char *ISBN, *tituloLivro, *autorLivro, *anoLivro;
	int offset;
}livroD;

typedef struct{
	int incluidos, excluidos;
}info;

info infos;

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

    //Verifica se o arquivo j� existe, caso contrario, cria-o.
    if((fileA = fopen("dados.bin", "r+b")) == NULL)
        fileA = criaArquivo("dados.bin");
	
	header = getHeader(fileA);
	
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
				break;
	        case 2: 
				remover(fileA); 
				break;
	        case 3: compactar(fileA); break;
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

void remover(FILE *fp){
	getInfos();
	if(infos.incluidos == 0){
		printf("Insira algum elemento para poder excluir.");
		_getch();
		return;
	}
	int excluidos = infos.excluidos;
	FILE *rem = fopen("remove.bin", "rb");
	if(rem == NULL)
	{
		printf("Erro ao abrir arquivo");
		exit(0);
	}
	char ISBN[13];
	fseek(rem, 14 * excluidos, SEEK_SET);
	fread(&ISBN, 13, 1, rem);
	if(excluiRegistro(ISBN, fp)){
		infos.excluidos++;
		atualizaInfos();
	}
	_getch();
	fclose(rem);
}

char *substring(char *str, int beg){
	char *novo = malloc((sizeof(char) * strlen(str)) + 2);
	int i = beg;
	for(; i < strlen(str); i++){
		novo[i - beg] = str[i];
	}
	return novo;
}

void inserir(FILE *fp){
	header = getHeader(fp);
	
	if(lista.registros == NULL || lista.count == 0)
		carregarArquivos();

	getInfos();

	int pos = infos.incluidos;
	if(pos == lista.count)
		printf("\nTodos os registros j� foram adicionados");
	else{
		char *stringInsercao = retornaString(lista.registros[pos]);
		
		//Header � o offset para primeiro arquivo exclu�do
		int offset = header;
		int anterior = -1;
		char *novoReg = strcat(substring(stringInsercao, 2), "$");
		int tamNovo = strlen(novoReg);
		while(offset != -1){
			fseek(fp, offset, SEEK_SET);
			
			//Le o tamanho do registro exclu�do
			char *buffer = malloc(sizeof(char) * 2);
			fread(buffer, 2, 1, fp);
			int tamExcluido = atoi(buffer);
			fseek(fp, 1, SEEK_CUR);//PULA ASTERISCO
			
			//Verifica se novoRegistro cabe no registro exclu�do
			if(tamExcluido >= tamNovo){
				int prox;
				fread(&prox, sizeof(int), 1, fp);
				if(anterior != -1){
					fseek(fp, anterior + 3, SEEK_SET);
					fwrite(&prox, sizeof(int), 1, fp);
				}else
					atualizaHeader(fp, prox);
				fseek(fp, offset + 2, SEEK_SET);
				fprintf(fp, "%s", novoReg);
				break;
			}
			else{
				anterior = offset;
				fread(&offset, sizeof(int), 1, fp);
			}
		}
		
		if(offset == -1){
			//write at end of file
			fseek(fp, 0, SEEK_END);
			fprintf(fp, stringInsercao);
		}
	
		infos.incluidos++;
		atualizaInfos();
		printf("\nRegistro inclu�do com sucesso!");
		_getch();
	}

}

void atualizaInfos(){
	FILE *fp = fopen("info.dat", "rb+");
	if(fp == NULL)
	{
		printf("Erro ao atualizar arquivo de informa��es");
		exit(0);
	}
	
	fprintf(fp, "%d %d", infos.incluidos, infos.excluidos);
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

void getInfos(){
	int incluidos, excluidos;
	FILE *fp = fopen("info.dat", "rb+");
	if(fp == NULL){
		fp = criaInfo();
	}else{
		fscanf(fp, "%d%d",&incluidos, &excluidos);
		infos.incluidos = incluidos;
		infos.excluidos = excluidos;
	}
	fclose(fp);
}

int getHeader(FILE *fp){
	if(fp == NULL){
		printf("Erro ao buscar header do arquivo");
		_getch();
		exit(0);
	}
	int curr = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	int header;
	fread(&header, sizeof(int), 1,fp);
	rewind(fp);
	fseek(fp, curr, SEEK_SET);
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
	char *stringInsercao = malloc(sizeof(char) * sizeof(data)); //Atribui tamanho m�ximo que a string pode ter
	strcpy(stringInsercao, "\0");
	char cToStr[2];
	cToStr[1] = '\0'; //Usa-se string para poder user fun��o concat
	int size  = 0; //Tamanho das strings dentro do objeto Data
	
	//4 pois s�o as propriedades
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
    
    //Adiciona cabe�alho
    atualizaHeader(fp, -1);
    return fp;
}

int arquivoVazio(FILE *fp){
	if(fp == NULL)
		return 1;
		
	int size = 0;
	int curr = ftell(fp); //armazena posi��o atual;
	fseek(fp, 0, SEEK_END);
	if(ftell(fp) == size)
		return 1;
	
	fseek(fp, 0, SEEK_SET);
	fseek(fp, curr, SEEK_SET);
	return 0;
}

void atualizaHeader(FILE *fp, int novoHeader){
	int empty = arquivoVazio(fp);
    fseek(fp, 0, SEEK_SET);//Volta ao in�cio do arquivo
    int header = 0;
	if(!empty){
		fread(&header, sizeof(int), 1, fp);
		fseek(fp, 0, SEEK_SET);
		fwrite(&novoHeader, sizeof(int),1,fp);		
	}else
		fwrite(&novoHeader, sizeof(int),1,fp);

}

int getRegTam(FILE *fp){
	char aux[3];
	aux[2] = '\0';
	aux[0] = fgetc(fp);
	aux[1] = fgetc(fp);
	return atoi(aux);
}

livroD* getLivroD(FILE *fp){
	if(ftell(fp) == 0)
		//pula header
		fseek(fp, sizeof(int), SEEK_SET);
	
	livroD *livro = (livroD *)malloc(sizeof(livroD));
	
	int tam = getRegTam(fp);
	livro->offset = tam;
	char cToStr[2];
	cToStr[0] = '\0';
	cToStr[1] = '\0';
	char *wordBuffer = malloc(sizeof(tam) + 1);
	int prop = 0;
	int i = 0;
	for(; i < tam && cToStr[0] != '$'; i++){
		cToStr[0] = fgetc(fp);
		if(i == 0 && cToStr[0] =='*'){
			fseek(fp, tam - 1, SEEK_CUR);
			free(livro);
			free(wordBuffer);
			return NULL;
		}
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
	
	//Ignora caracteres dentro do registro ap�s sinal de exclus�o
	if(cToStr[0] == '$')
		while(i < tam){
			fgetc(fp);
			i++;
		}
	free(wordBuffer);
	return livro;
}

int excluiRegistro(char ISBN[14], FILE *fp){
	if(fp == NULL){
		printf("\nArquivo est� vazio!");
		return 0;
	}
	int size = 0;
	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	rewind(fp);
	
	while(1){
		livroD *aux = getLivroD(fp);
		if(aux == NULL)
		{
			if(ftell(fp) == size || feof(fp)){
				printf("\nRegistro n�o encontrado!");
				return 0;
			}
				
			continue;
		}
		
		if(strcmp(ISBN, aux->ISBN) == 0)
		{
			printf("\nExcluindo registro...");
			fseek(fp, -(aux->offset), SEEK_CUR);
			int novoHeader = ftell(fp) - 2;
			header = getHeader(fp);
			fprintf(fp, "*");
			fwrite(&header, sizeof(int),1, fp);
			atualizaHeader(fp, novoHeader);
			printf("\nRegistro exclu�do com sucesso!");
			return 1;
		}
		
		if(ftell(fp) == size || feof(fp)){
			printf("\nRegistro n�o encontrado!");
			rewind(fp);
			return 0;
		}
	};
}

void carregarArquivos(){
    FILE *fileB;
    int numReg, i=0, tam = 3;
    //struct dados *listaRegistros;

    //carrega o conteudo de biblioteca.bin
    fileB = fopen("biblioteca.bin", "rb");
    if(!fileB)
        printf("N�o foi possivel abrir o arquivo");

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

void compactar(FILE *fp){
		
	livroD *livro;  
	FILE *temp;
	char *old_filename ="dados.bin" , *new_filename = "temporario.bin";
	
	
	temp = criaArquivo("temporario.bin");
	
    livro = getLivroD(fp);

    
    
	fclose(fp);
	fclose(temp);    
	remove(old_filename);
	rename(new_filename, old_filename);
    
    system("pause");
}
