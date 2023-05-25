// abb.hpp
// Implementacao de uma ABB generica.
//
// The MIT License (MIT)
//
// Copyright (c) 2023 João Vicente Ferreira Lima, UFSM
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once 

#include <iostream>

template<typename T>
struct Abb {
    T dado;
    Abb<T>* esq;
    Abb<T>* dir;
    int altura;
};

template<typename T>
bool abb_vazio(Abb<T>* no)
{
    return (no == nullptr);
}

template<typename T>
int abb_altura(Abb<T>* no)
{
    if(no == nullptr)
        return 0;
    return no->altura;
}

template<typename T>
int abb_get_fb(Abb<T>* no)
{
    if(no == nullptr)
        return 0;
    return (abb_altura(no->esq) - abb_altura(no->dir));
}

template<typename T>
Abb<T>* abb_esq_rotate(Abb<T>* x)
{
    // TODO
    return x;
}

template<typename T>
Abb<T>* abb_dir_rotate(Abb<T>* x)
{
    // TODO
    return x;
}

// inicia o no da arvore com valores nulos
template<typename T>
Abb<T>* abb_inicia(T v)
{
    Abb<T>* no = new Abb<T>;
    no->dado = v;
    no->altura = 1;
    no->esq = nullptr;
    no->dir = nullptr;

    return no;
}

// recebe uma lista de valores
// precisa converter para uma arvore
template<typename T>
Abb<T>* abb_inicia(std::list<T>& entrada)
{
    Abb<T>* no = nullptr;
    if(entrada.empty() == true)
      return nullptr;

    for(auto it = entrada.begin(); it != entrada.end(); it++){
      no = abb_insere( no, *it );
    }
    return no;
}  

// insere um no pre-alocado na arvore
template<typename T>
Abb<T>* abb_insere( Abb<T>* no, T v )
{
    if( no == nullptr )
        return abb_inicia(v);

    // o novo valor e menor que o meu?
    if(  v < no->dado )
        no->esq = abb_insere( no->esq, v );
    else if( v > no->dado ) // o novo valor e maior ou igual
        no->dir = abb_insere( no->dir, v );
    else 
        return no;

    no->altura = 1 + std::max(
        abb_altura(no->esq),
        abb_altura(no->dir)
    );


    // TODO

    return no;
}

template<typename T>
Abb<T>* abb_no_minimo( Abb<T>* no )
{
     Abb<T>* curr = no;
     while( curr->esq != nullptr )
        curr = curr->esq;
    return curr;
}

// insere um no pre-alocado na arvore
template<typename T>
Abb<T>* abb_remove( Abb<T>* no, T v )
{
    if( no == nullptr )
        return no;

    if( v < no->dado )
        no->esq = abb_remove( no->esq, v );
    else if( v > no->dado )
        no->dir = abb_remove( no->dir, v );
    else {
        if( (no->esq == nullptr) || (no->dir == nullptr) ){
            // temos uma folha
            Abb<T>* temp = no->esq ? no->esq : no->dir;
            if( temp == nullptr ){
                temp = no;
                no = nullptr; // retorna logo abaixo depois dos ifs
            } else 
                *no = *temp; // WARN: cuidado o tipo de dado
            delete temp;
        } else {
            // não temos folha: procura o nó mínimo a direita
            Abb<T>* min = abb_no_minimo(no->dir);
            // WARN: cuidado com o tipo de dado
            no->dado = min->dado;
            no->dir = abb_remove( no->dir, min->dado );
        }
    }

    if( no == nullptr )
        return no;

    // atualiza alturas
    no->altura = 1 + std::max(
        abb_altura(no->esq),
        abb_altura(no->dir)
    );

    // TODO

    return no;
}

// faz o percurso pre-ordem.
// A visita e imprimir o valor de cada no
template<typename T>
void abb_emOrdem(Abb<T>* a)
{
    // primeiro esquerda
    if( a != nullptr ){
        abb_emOrdem(a->esq);
        std::cout << a->dado << "(" << a->altura << ") ";
        abb_emOrdem(a->dir);
    }
}   

template<typename T>
void abb_preOrdem(Abb<T>* a, std::list<T>& saida)
{
    // primeiro esquerda
    if( !abb_vazio(a) ){
        //std::cout << a->dado << "(" << a->altura << ") ";
        saida.push_back( a->dado );
        abb_preOrdem(a->esq, saida);
        abb_preOrdem(a->dir, saida);
    }
}   

template<typename T>
void abb_destroi(Abb<T>* a)
{
    if(a != nullptr){
        abb_destroi(a->esq);
        abb_destroi(a->dir);
        delete a;
    }
}

/* Exemplo abaixo de uma main para o código de arvore

int main(void)
{
    Abb<int>* raiz = nullptr;

    raiz = abb_insere(raiz, 41);
    raiz = abb_insere(raiz, 20);
    raiz = abb_insere(raiz, 65);
    raiz = abb_insere(raiz, 11);
    raiz = abb_insere(raiz, 50);
    raiz = abb_insere(raiz, 29);
    raiz = abb_insere(raiz, 26);
    //
    raiz = abb_insere(raiz, 23);
    raiz = abb_insere(raiz, 55);
    abb_preOrdem(raiz);
    std::cout << "\n";
    abb_destroi(raiz);

    return 0;
}


*/
