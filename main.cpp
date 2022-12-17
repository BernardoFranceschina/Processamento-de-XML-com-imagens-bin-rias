// Copyright [2022] <ALISSON RAFAEL BARBOSA..>
// Copyright [2022] <Bernardo C. Franceschina>
#include <iostream>
#include <fstream>
#include "./array_stack.h"
#include "./array_queue.h"
#include <string>
#include <vector>

using namespace std;

struct Imagem {
    string name;
    vector<char> data;
    string height;
    string width;
    vector<vector<char>> zero;
    int conexos;
};

size_t numImgs = 0;
Imagem listaImagens[150];
structures::ArrayQueue<char> fila(10000000);
int rotulo = 0;

bool verificaXML (std::string trecho_programa) {
    // Variáveis para fazer a validação
    structures::ArrayStack<string> pilha(10000000);
    int tamanho  = trecho_programa.length();
    bool resposta = true;
    bool mountTag = false;
    bool fechaTag = false;
    string tag = "";

    // Variáveis para extrair as informações
    bool coletarInfo = false;
    string imgName = "";
    vector<char> imgData = {};
    string imgHeight = "";
    string imgWidth = "";
    vector<vector<char>> zero;
    vector<char> zero2;

    for (int i = 0; i < tamanho; i++) {
        // Verifica as aberturas de tag
        if ((trecho_programa[i] == '<' && trecho_programa[i+1] != '/') || mountTag) {
            tag += trecho_programa[i];
            mountTag = true;
        }
        // Fecha a tag e adiciona ela na pilha
        if (trecho_programa[i] == '>' && mountTag) {
            pilha.push(tag);
            coletarInfo = true;
            mountTag = false;
            tag = "";
        }
        // Verifica o fechamento das tags
        if ((trecho_programa[i] == '<' && trecho_programa[i+1] == '/') || fechaTag) {
            tag += trecho_programa[i];
            fechaTag = true;
            coletarInfo = false;
        }
        // Valida fechamento da tag e remove ela da pilha
        if (trecho_programa[i] == '>' && fechaTag) {
            tag.erase(tag.begin() + 1); // tira a barra "/" do fechamento da tag para comparar com a tag topo da pilha
            if (pilha.size() > 0) {
                string pop = pilha.pop();
                if (pop != tag) return false;
                if (pop == "<img>") {
                    int k = 0;
                    for (int j = 0; j < imgData.size(); j++) {
                        zero2.push_back('0');
                        k++;
                        if (k % stoi(imgWidth) == 0) {
                            zero.push_back(zero2);
                            zero2 = {};
                        }
                    }
                    // Caso tenha um a tag img no pop quer dizer que os dados da imagem acabaram
                    struct Imagem img = {imgName, imgData, imgHeight, imgWidth, zero};
                    listaImagens[numImgs] = img;
                    numImgs++;
                    imgName = "";
                    zero = {};
                    zero2 = {};
                    imgData = {};
                    imgHeight = "";
                    imgWidth  = "";
                }
            }
            coletarInfo = false;
            fechaTag = false;
            tag = "";
        }

        if (pilha.size() > 0) {
            if (trecho_programa[i] != '>' && coletarInfo) {
                if (pilha.top() == "<name>") imgName += trecho_programa[i];
                if (pilha.top() == "<height>") imgHeight += trecho_programa[i];
                if (pilha.top() == "<width>") imgWidth += trecho_programa[i];
                if (pilha.top() == "<data>") imgData.push_back(trecho_programa[i]);
                coletarInfo = true;
            }
        }
    }
    
    if (pilha.size() > 0) resposta = false;
    return resposta;
}

vector<vector<char>> validaPosicoes(vector<vector<char>> s, vector<vector<char>> zero, int height, int width, int i, int j) {
    if (s[i][j] == '1' && zero[i][j] == '0') {
        fila.enqueue('1');
        zero[i][j] = '1';
        if ((i-1 >= 0) && s[i-1][j] == '1') {
            zero = validaPosicoes(s, zero, height, width, i-1, j);
        }
        if ((i+1 < height) && s[i+1][j] == '1') {
            zero = validaPosicoes(s, zero, height, width, i+1, j);
        }
        if ((j-1 >= 0) && s[i][j-1] == '1') {
            zero = validaPosicoes(s, zero, height, width, i, j-1);
        }
        if ((j+1 < width) && s[i][j+1] == '1') {
            zero = validaPosicoes(s, zero, height, width, i, j+1);
        }
        fila.dequeue();
        if (fila.empty()) {
            rotulo++;
        }
    }
    return zero;
}

int main() {
    
    char xmlfilename[100];
    cin >> xmlfilename;

    ifstream dataset;
    dataset.open(xmlfilename);

    string line;
    string fullText;
    bool validaXML;
    
    if (dataset.is_open()) {
        while (getline(dataset, line)) fullText.append(line); // Monta a string contendo todo o arquivo XML
        validaXML = verificaXML(fullText);
        dataset.close();
    }
    if (!validaXML) cout << "error\n";
    else {
        for (size_t l = 0; l < numImgs; l++) {
            int height = stoi(listaImagens[l].height);
            int width = stoi(listaImagens[l].width);

            vector<vector<char>> s;
            vector<char> s2;
            int k = 0;
            for (int j = 0; j < listaImagens[l].data.size(); j++) {
                s2.push_back(listaImagens[l].data[j]);
                k++;
                if (k % width == 0) {
                    s.push_back(s2);
                    s2 = {};
                }                
            }
            rotulo = 0;
            for (int i = 0; i < s.size(); i++) {
                for (int j = 0; j < s[i].size(); j++) {
                    if (s[i][j] == '1') {
                        listaImagens[l].zero = validaPosicoes(s, listaImagens[l].zero, height, width, i, j);
                    }                   
                }
            }
            cout << listaImagens[l].name << " " << rotulo << endl;
        }
    }

    return 0;
}