#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <cmath>
#include <algorithm>

using namespace std;

using MatrizAdj = vector<vector<int>>;
using Comunidades = vector<int>;
using Vertices = vector<int>;
using Names = vector<string>;

Names nomes;

bool draw = false;
/*
	Esta função divide uma string 's' utilizando o caracter 'c' como separador
	
	Args:
	s -> A String a fer dividada
	c -> O caracter de separação

	Return:
	um vetor de substrings de 's'
 */
const vector<string> splitString(const string& s, const char& c)
{
	string buff{ "" };
	vector<string> v;

	for (auto n : s)
	{
		if (n != c) buff += n; else
			if (n == c && buff != "") { v.push_back(buff); buff = ""; }
	}
	if (buff != "") v.push_back(buff);

	return v;
}

/*
	Esta função Lê um arquivo no estilo .net e gera um grafo para o algoritmo
	
	Args:
	instancia -> O nome do arquivo a ser lido

	Return:
	a matriz de adjacência do grafo lido
 */
MatrizAdj obterInstancia(string instancia) {
	MatrizAdj matriz;
	ifstream arquivo(instancia);
	string linha;

	getline(arquivo, linha);
	vector<string> separado{ splitString(linha, ' ') };

	int N = stoi(separado[1]);
	nomes = Names(N);
	for (int i = 0; i < N; i++) {
		getline(arquivo, linha);
		// Aqui são lidos os labels do grafo, eles não são utilizados no algorítmo, mas são úteis na exibição das resostas
		vector<string> nodo{ splitString(linha, ' ') };
		nomes[i] = nodo[1];
		// cout<<"{id: "<<i<<", label: "<<nodo[1]<<", group: 0},";
		matriz.push_back(vector<int>());
		for (int j = 0; j < N; j++)
			matriz[i].push_back(0);
	}

	getline(arquivo, linha);
	if(draw)
		cout<<"var edges = [";

	while (getline(arquivo, linha)) {
		separado = { splitString(linha, ' ') };
		int i = stoi(separado[0]) - 1;
		int j = stoi(separado[1]) - 1;
		
		if(draw)
			cout << "{from: "<<i<<", to: "<<j<<"},";
		matriz[i][j] = 1;
		matriz[j][i] = 1;
	}
	if(draw)
		cout<<"];\n";

	return matriz;
}

/*
	Este procedimento renderiza uma matriz  no console
	
	Args:
	matriz -> A matriz a ser exibida
	
 */
void exibirMatriz(MatrizAdj matriz) {
	int N = matriz.size();
	for (int i = 0; i < N; i++) {
		printf("\n|");
		for (int j = 0; j < N; j++) {
			printf("%d|", matriz[i][j]);
		}
	}
}

// Esta estrutura é referente ao algoritmo, e representa seus atributos
struct Louvain {
	MatrizAdj matriz;
	Vertices vertices;
	Comunidades comunidades;
	double modularidade;
	
	// O construtor do algortmo recebe como parâmetro o nome do arquivo onde está descrito o grafo
	Louvain(string instancia) {
		matriz = obterInstancia(instancia);

		const int N = matriz.size();
		comunidades = Comunidades(N);
		vertices = Vertices(N);
		for (int i = 0; i < N; i++) {
			comunidades[i] = i;
			vertices[i] = i;
		}
	}

	Louvain(MatrizAdj matriz, Vertices vertices, Comunidades comunidades, double modularidade) :
		matriz(matriz), vertices(vertices), comunidades(comunidades), modularidade(modularidade) {};
};

/*
	Esta função obtem os vertices de uma comunidade
	
	Args:
	louvain -> A instancia do algoritmo
	comunidade -> O id referenta à comunidade

	Return:
	uma lista com todos os vertices da 'comunidade'
 */
Vertices obterVerticesDaComunidade(Louvain louvain, int comunidade) {
	Vertices vertices = Vertices();
	for (int i = 0; i < louvain.vertices.size(); i++)
		if (louvain.vertices[i] == comunidade)
			vertices.push_back(i);
	return vertices;
}

/*
	Esta função obtem o id de uma comunidade
	
	Args:
	louvain -> A instancia do algoritmo
	comunidade -> O id referenta à comunidade

	Return:
	o id da 'comunidade'
 */
int obterIndiceComunidade(Louvain louvain, int comunidade) {	
	for (int c = 0; c < louvain.comunidades.size(); c++)
		if (louvain.comunidades[c] == comunidade)
			return c;
	return -1;
}

/*
	Esta função obtem o grau de um vertice
	
	Args:
	louvain -> A instancia do algoritmo
	vertice -> O id do vertice

	Return:
	O grau do vertice 'id'
 */
int obterGrau(Louvain louvain, int vertice) {
	int grau = 0;
	for (int i = 0; i < louvain.vertices.size(); i++)
		grau += louvain.matriz[vertice][i];
	return grau;
}

/*
	Esta função obtem a quantidade total de arestas em um grafo
	
	Args:
	matriz -> a matriz de adjacência de um grafo

	Return:
	O número de arestas no grafo
 */
int obterQuantidadeTotalArestas(MatrizAdj matriz) {
	int total = 0;
	for (int i = 0; i < matriz.size(); i++)
		for (int j = 0; j < matriz.size(); j++)
			total += matriz[i][j];
	return total/2;
}

/*
	Esta função obtem as comunidades vizinhas à  uma 'comunidade'
	
	Args:
	louvain -> A instancia do algoritmo
	comunidade -> O id referenta à comunidade

	Return:
	Uma lista com as comunidades vizinhas à 'comunidade'
 */
Comunidades obterComunidadesVizinhas(Louvain louvain, int comunidade) {	
	Comunidades comunidades = Comunidades();
	Vertices vertices_comunidade = obterVerticesDaComunidade(louvain, comunidade);

	for (int i = 0; i < vertices_comunidade.size(); i++)
	{
		for (int j = 0; j < louvain.matriz[i].size(); j++)
		{
			if (louvain.matriz[i][j] == 1) {
				if (louvain.vertices[i] != louvain.vertices[j]) {
					comunidades.push_back(louvain.vertices[j]);
				}
			}
		}
	}

	for (int i = 0; i < comunidades.size(); i++)
	{
		for (int j = 0; j < comunidades.size(); j++)
		{
			if (comunidades.at(i) == comunidades.at(j)) {
				comunidades.erase(comunidades.begin() + j);
			}
		}
	}
	
	return Comunidades();
}

/*
	Esta função obtem a modularidadeQ de do grafo com suas atuais modularidades
	Esta é a função objetivo do algoritmo
	
	Args:
	louvain -> A instancia do algoritmo
	
	Return:
	Um valor que indica a modularidade
 */
double obterModularidadeQ(Louvain louvain) {
	double soma = 0;
	double somaCom = 0;
	double somaPA = 0;
	for (int i = 0; i < louvain.comunidades.size(); i++)
	{
		for (int x = 0; x < obterVerticesDaComunidade(louvain, i).size(); x++)
		{
			for (int j = 0; j < obterVerticesDaComunidade(louvain, i).size(); j++)
			{
				soma += (louvain.matriz[x][j] - ((obterGrau(louvain, x) * obterGrau(louvain, j)) / (2 * fabs(obterQuantidadeTotalArestas(louvain.matriz)))));
			}
		}
	}
	return (1 / (2 * fabs(obterQuantidadeTotalArestas(louvain.matriz)))) * soma;
}

/*
	Esta função tenta unir dua comunidades e ver se isto aumenta sua modulariade
	
	Args:
	louvain -> A instancia do algoritmo
	comunidadeOrigem -> id da comunidade base
	comunidadeDestino -> id da comunidade a ser unificada
	
	Return:
	Uma nova instância do algoritmo
 */
Louvain migrarVertices(Louvain louvain, int comunidadeOrigem, int comunidadeDestino) {
	for (int v = 0; v < louvain.vertices.size(); v++)
		if (louvain.vertices[v] == comunidadeOrigem)
			louvain.vertices[v] = comunidadeDestino;

	louvain.modularidade = obterModularidadeQ(louvain);
	
	int verticesNaAntigaComunidade = obterVerticesDaComunidade(louvain, comunidadeOrigem).size();

	int indiceComunidadeOrigem = obterIndiceComunidade(louvain, comunidadeOrigem);
	
	if (indiceComunidadeOrigem == -1 && verticesNaAntigaComunidade == 0)
		louvain.comunidades.erase(louvain.comunidades.begin(), louvain.comunidades.begin() + indiceComunidadeOrigem);
	return louvain;
}

/*	
	Esta é a função principal
	
	Args:
	Ela deve receber o arquivo .net com a descrição do grafo como parametro em sua execução
 */
int main(int argc, char** argv)
{
	
	// Caso um valor menor que dois, entõa o arquivo.net não foi passado como parâmetro para o programa
	//if (argc < 2)
 //       cerr << "no input file's name\n"<< endl;
	//if (argc >= 3){
	//	string d = "-draw";
	//	if(d.compare(argv[2]) == 0){
	//		draw = true;
	//	}
	//		
	//}

	draw = true;
	Louvain louvain = Louvain("instancias/civilwar2.net");	
	
	louvain.modularidade = obterModularidadeQ(louvain);
	bool houveMelhora;
	
	do {
		houveMelhora = false;
		for (int comunidade : louvain.comunidades) {
			Louvain melhorAlteracao = louvain;			
			Comunidades vizinhas = obterComunidadesVizinhas(louvain, comunidade);			

			for (int vizinha : vizinhas) {
				Louvain alteracao = migrarVertices(louvain, comunidade, vizinha);
				if (alteracao.modularidade > melhorAlteracao.modularidade) {
					melhorAlteracao = alteracao;
					houveMelhora = true;
				}
			}

			louvain = melhorAlteracao;
		}
	} while (houveMelhora);
	
	if(draw){
		cout<<"var nodes = [";
		for (int v = 0; v < louvain.vertices.size(); v++)
			cout<<"{id: "<<v<<", label: "<<nomes[v]<<", group: "<<louvain.vertices[v]<<"},";
		cout<<"];\n";
		
	}else{
		for (int c = 0; c < louvain.comunidades.size(); c++){
			Vertices vertices = obterVerticesDaComunidade(louvain, c);
			if(vertices.size() > 0){
				cout<< "\nComunidade "<< c <<'\n';
				for(int v : vertices)
					cout << nomes[v];
			}
		}
	}
}