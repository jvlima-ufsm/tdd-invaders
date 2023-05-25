// principal.cpp
// Programa principal do jogo Invaders.
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

#include <chrono>
#include <list>
#include <random>
#include <iostream>
#include <functional>
#include <cstdlib>

#include "abb.hpp"

#include "tela.hpp"
#include "geom.hpp"

using namespace tela;
using namespace geom;

// programa de teste das funcoes geometricas
//
// o programa desenha algumas figuras
// se o mouse passar sobre alguma figura, ela muda de cor
// (se as funcoes ptemret e ptemcirc estiverem corretas)
// pode-se arrastar as figuras com o mouse (botao esquerdo pressionado)
//
// com o botao esquerdo pressionado, as figuras com intersecao mudam de cor
// (se as funcoes de intersecao estiverem funcionando)
//
// apertando a tecla q o programa termina

struct laser_t {
  float v;       /* velocidade */
  Retangulo ret; /* figura */
};

struct tiro_t {
  bool ativo;
  float v;   /* velocidade */
  Circulo  c; /* figura */
};

/* estados para o jogo */
enum Estado { nada, fim };

/* Direção na tela */
enum Direcao { ESQ = -1, DIR = 1 };

/* Estrutura de um invader */
struct Invader {
  Retangulo r;  // descreve o desenho
  int valor;  // valor na árvore

  // operador de comparação na árvore
  bool operator< (const Invader& i) const {
    return (  valor < i.valor );
  }

  bool operator> (const Invader& i) const {
    return (  valor > i.valor );
  }

  bool operator== (const Invader& i) const {
    return (  valor == i.valor );
  }

};

// Estrutura para controlar todos os objetos e estados do Jogo Centipede
struct Jogo {
  Estado estado;             // estado do jogo
  laser_t laser;             // laser
  std::list<tiro_t> tiros;   // tiros ativos

  Abb<Invader>* invaders;     // árvore de invaders
  Ponto p0;                   // ponto de referência da árvore na tela
  int velocidade;             // velocidade de movimento 
  Direcao direcao;            // direção da tela
  bool sinalNovoInvader;      // sinaliza quando adicionar um novo invader aleatório

  Tela tela;                    // estrutura que controla a tela
  int tecla;                 // ultima tecla apertada pelo usuario
  Tamanho tamanhoTela;        // otimiza a questão do tamanho da tela

  // inicia estruturas principais do jogo
  void inicia(void) {
    tela.inicia(600, 400, "AVL Invaders");
    estado = Estado::nada;
    tamanhoTela = tela.tamanho();

    invaders = nullptr;
    p0.x = 0;
    p0.y = 10;
    velocidade = 2;
    sinalNovoInvader = false;
    direcao = Direcao::DIR;
    inicia_arvore();
    laser_inicia();
    
    // cria gerador aleatório de numeros de 0 a 100
    auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    srand(seed);
  }

  void inicia_arvore(void){
    std::vector<int> valores {25, 75, 15, 35, 85, 65  };
    Invader i1;

    // raiz
    i1.r = {{290, 0}, {20, 20}};
    i1.valor = 50;
    invaders = abb_insere( invaders, i1 );
    
    // nodos, posicionamento definido mais tarde
    for(int& v: valores) {
      i1.r = {{0, 0}, {20, 20}};
      i1.valor = v;
      invaders = abb_insere( invaders, i1 );
    }
  }

  // verifica se o jogo terminou ou não
  // - retorna TRUE quando  termina
  // - retorna FALSE caso contrário
  bool verifica_fim(void) {
    if (estado == Estado::nada)
      return false;
    else
      return true;
  }

  void finaliza(void) {
    // fecha a tela
    tela.finaliza();
    abb_destroi( invaders );
  }

  // move o tiro (se existir) em certa velocidade
  void tiro_movimenta(void) {
    for(auto t = tiros.begin(); t != tiros.end(); /* nao precisa aqui */){
      (*t).c.centro.y -= (*t).v;
      /* saiu da tela */
      if ((*t).c.centro.y < 0.0) {
        t = tiros.erase(t);
      } else
        t++;
    }
  }

  // lanca tiro do laser
  void tiro_fogo(void) {
    tiro_t tiro;
    tiro.ativo = true;
    tiro.v = 20;
    tiro.c.centro.x = laser.ret.pos.x;
    tiro.c.centro.y = laser.ret.pos.y - 10;
    tiro.c.raio = 5;
    tiros.push_back(tiro);
  }

 // desenha o tiro, sendo que a cada desenha sua cor muda.
  void tiro_desenha(void) {
    if (tiros.empty() == false) {
      for(auto t = tiros.begin(); t != tiros.end(); t++){
        Cor outra = {1.0, 0.0, 0.0};
        tela.cor(outra);
        tela.circulo((*t).c);
      }
    }
  }

 // inicia o laser na posição inicial
  void laser_inicia(void) {
    laser.v = 0.0;
    laser.ret.pos.x = tamanhoTela.larg / 2;
    laser.ret.pos.y = tamanhoTela.alt - 20;
    laser.ret.tam.larg = 10;
    laser.ret.tam.alt = 20;
  }

  // laser lança um tiro
  void laser_atira() {
    if (tecla == ALLEGRO_KEY_F) {
      tiro_fogo();
    }
  }

  // altera velocidade do laser com as teclas A ou D
  void laser_altera_velocidade() {
    if (tecla == ALLEGRO_KEY_A) {
      /* altera velocidade mais a esquerda */
      laser.v -= 1;
    } else if (tecla == ALLEGRO_KEY_D) {
      /* altera velocidade mais a direita */
      laser.v += 1;
    }
  }

  // apenas movimenta o laser na tela se necessário
  void laser_move(void) {
    if (((laser.ret.pos.x + laser.v) < 0.0) ||
        ((laser.ret.pos.x + laser.v) > (tamanhoTela.larg - 10)))
      laser.v = 0.0;
    laser.ret.pos.x += laser.v;
  }

  // desenha o laser
  void laser_desenha(void) {
    Cor vermelho = {1.0, 0.0, 0.0};
    tela.cor(vermelho);
    tela.retangulo(laser.ret);
  }

  // Desenha a arvore baseado em divisão geométrica.
  void desenha_arvore(Abb<Invader>* a, Ponto p) {
    const Cor azul = {0.2, 0.3, 0.8};
    const Cor preto = {0, 0, 0};
    const Cor vermelho = {1, 0.2, 0};
    static char valor[10];

    if(a == nullptr)
      return;
    tela.cor(vermelho);            
    tela.retangulo(a->dado.r);
    // desenha valor do nó
    tela.cor(preto);
    sprintf(valor, "%d", a->dado.valor);
    tela.texto(a->dado.r.pos, valor);
    // ajusta a linha para ficar no meio do retangulo
    tela.cor(azul); 
    tela.linha(
      Ponto{p.x+a->dado.r.tam.larg/2, p.y+a->dado.r.tam.alt/2}, 
      Ponto{a->dado.r.pos.x+a->dado.r.tam.larg/2, a->dado.r.pos.y+a->dado.r.tam.alt/2}
      );
    desenha_arvore(a->esq, a->dado.r.pos);
    desenha_arvore(a->dir, a->dado.r.pos);
  }

  // Desenha a arvore baseado em divisão geométrica.
  void desenha_arvore(Abb<Invader>* a) {
    char valor[10];
    //const Cor azul = {0.2, 0.3, 0.8};
    const Cor preto = {0, 0, 0};
    const Cor vermelho = {1, 0.2, 0};

    if(a == nullptr)
      return;
      // desenha figura
    tela.cor(vermelho);      
    tela.retangulo(a->dado.r);
    // desenha o valor 
    tela.cor(preto);
    sprintf(valor, "%d", a->dado.valor);
    tela.texto(a->dado.r.pos, valor);
    // faz desenho recursivamente
    desenha_arvore(a->esq, a->dado.r.pos);
    desenha_arvore(a->dir, a->dado.r.pos);
  }

  // desenha todas as figuras e objetos na tela
  void desenha_figuras() {
    //Cor azul = {0.2, 0.3, 0.8};
    // Cor vermelho = {1, 0.2, 0};
    // Cor verde = {0.2, 0.9, 0.6};
    // Cor preto = {0, 0, 0};

    // mudar cor da tela
    //tela.cor(azul);

    desenha_arvore( invaders );

    // desenha laser e tiro
    laser_altera_velocidade();
    laser_atira();
    laser_desenha();
    tiro_desenha();
  }

  // realiza um percurso em largura
  Invader* verifica_intercep_abb(Abb<Invader>* a, tiro_t t)
  {
    Invader* ret = nullptr;
    if(a == nullptr)
      return nullptr;

    if( intercr( t.c, a->dado.r ) )
      return &a->dado;
    else {
      ret = verifica_intercep_abb(a->esq, t);
      if(ret != nullptr)
        return ret;
      ret = verifica_intercep_abb(a->dir, t);
    }
      
    return ret;
  }

  void tiro_verifica_interceptacao(void){
    // testa por uma colisão entre objetos e o tiro
    if (tiros.empty() == false) {
      for( auto t = tiros.begin(); t != tiros.end(); t++ ) {
        auto v = verifica_intercep_abb(invaders, *t);
        if( v != nullptr )
          invaders = abb_remove(invaders, *v );
      } // for tiros
    } // if tiros
  }

  // Move/posiciona a arvore baseado em divisão geométrica.
  // - A raiz é dividida em 2 partes, uma para cada sub-árvore
  // - Recursivamente, divide espaços da tela no eixo X em 2
  void move_arvore(Abb<Invader>* a, int x0, int x1, int y0) {
    if(a == nullptr)
      return;
    a->dado.r.pos.x = p0.x +  x0 + (x1-x0)/2 - 10;
    a->dado.r.pos.y = p0.y + y0;

    if( (a->dado.r.pos.x+a->dado.r.tam.larg+velocidade*direcao) >= tamanhoTela.larg )
    {
      // bate na direita
      direcao = Direcao::ESQ;
      p0.y = p0.y + velocidade;
      // avisa flag de criar novo invaders na próxima vez
      sinalNovoInvader = true;
    } else if ( (a->dado.r.pos.x-a->dado.r.tam.larg+velocidade*direcao) <=  0 ) 
    {
      // bate na esquerda
      direcao = Direcao::DIR;
      // avisa flag de criar novo invaders na próxima vez
      sinalNovoInvader = true;
    }

    move_arvore(a->esq, x0, x0+(x1-x0)/2, y0+30);
    move_arvore(a->dir, x0+(x1-x0)/2, x1, y0+30);
  }

  void move_arvore(Abb<Invader>* a) 
  {
    p0.x = p0.x + velocidade * direcao;
    move_arvore( invaders, 0, 600, 0 );
  }

  // Cria um novo invader e insere na árvore. O valor aleatório
  // fica entre [0,100]. 
  // - Se a direção é direita, insere na sub-árvore da direita 
  // - Se a direção é esquerda, insere na sub-árvore da esquerda
  void cria_novo_invader(void)
  {
    Invader i1;
    i1.r = {{0, 0}, {20, 20}};
    i1.valor = rand() % 100;
    if( direcao ==  Direcao::DIR )
      invaders->dir = abb_insere( invaders->dir, i1 );
    else
      invaders->esq = abb_insere( invaders->esq, i1 );
    sinalNovoInvader = false;
  }

  // Esta função tem os seguintes passos:
  // 1 - movimenta todos os objetos do centipede
  // 2 - movimenta o laser e tiro
  // 3 - testa por colisões entre objetos e tiro do laser
  void move_figuras(void) {
    if( sinalNovoInvader )
      cria_novo_invader();
    // movimenta, ou posiciona, a arvore na tela
    move_arvore( invaders );
    // move o laser
    laser_move();
    // movimenta tiros
    tiro_movimenta();
    // verifica se algum tiro pegou um bloco
    tiro_verifica_interceptacao();
  }

  void legenda(void){
    std::cout << "Pressione: " << std::endl;
    std::cout << " - 'a' ou 'd' para mover " << std::endl;
    std::cout << " - 'f' para atirar " << std::endl;
    std::cout << " - 'q' sair" << std::endl;
  }

  // atualiza o jogo, como se fosse um passo na execução
  void atualiza(void) {
    // le ultima tecla
    tecla = tela.tecla();
    // tecla Q termina
    if (tecla != ALLEGRO_KEY_Q) {
      // faz o resto
      move_figuras();
      tela.limpa();
      desenha_figuras();
      tela.mostra();
      // espera 60 ms antes de atualizar a tela
      tela.espera(20);
    } else {
      // troca estado do jogo para terminado
      estado = Estado::fim;
    }
  }  
};

int main(int argc, char **argv) {
  Jogo jogo;

  jogo.inicia();
  jogo.legenda();
  while (jogo.verifica_fim() == false) {
    jogo.atualiza();
  }
  jogo.finaliza();
  return 0;
}