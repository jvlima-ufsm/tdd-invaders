
# TDD- Árvore Binária de Busca

Este é o código exemplo do trabalho usando TDD (*Test Driven Development*) com o framework C++ [Catch2](https://github.com/catchorg/Catch2/tree/v2.x).

O framework Catch2 consegue ser utilizado apenas com o arquivo header [catch.hpp](catch.hpp) sem necessidade de instalação.

**NÃO MODIFIQUE OS TESTES** 

Todos os testes estão prontos no arquivo [arvore.cpp](arvore.cpp). A compilação e teste pode ser feita com os comandos:
```
$ g++ -Wall -std=c++11 -o arvore arvore.cpp 
```

A saída resumida do teste (código exemplo):
```
$ ./arvore --reporter compact --success
arvore.cpp:52: passed: a.vazio() == true for: true == true
arvore.cpp:60: failed: a.vazio() == false for: true == false
arvore.cpp:71: failed: saida == resultado for: {  } == { 3, 7, 5 }
arvore.cpp:82: failed: saida == resultado for: {  } == { 4, 5, 2, 3, 1 }
arvore.cpp:93: failed: saida == resultado for: {  }
==
{ 'A', 'D', 'C', 'I', 'H', 'K', 'J', 'F' }
arvore.cpp:104: failed: saida == resultado for: {  }
==
{ 5, 28, 24, 45, 30, 60, 52, 98, 50 }
Failed 5 test cases, failed 5 assertions.
```

