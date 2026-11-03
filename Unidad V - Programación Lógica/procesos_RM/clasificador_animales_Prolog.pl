:- encoding(utf8).

/*
  Clasificador de animales basado en el árbol de decisión generado en RapidMiner.
  Objetivo: predador (true = depredador, false = no depredador).

  El archivo reproduce las 13 hojas del árbol proporcionado.
  Los códigos idalimentacion (1, 2, 3) se mantienen como en el CSV.
  Las características binarias se representan con números 0 y 1.

  Ejemplos en SWI-Prolog:
    ?- clasificar(_{idalimentacion:1, venenoso:1}, Clase).
    Clase = true.

    ?- clasificar(_{idalimentacion:2}, Clase).
    Clase = false.

    ?- clasificar(_{idalimentacion:3, pulmones:0, escamas:0, cola:1}, Clase).
    Clase = true.

  Nota: para cada consulta deben especificarse los atributos de la rama
  recorrida. Si faltan datos necesarios o aparece un código no contemplado,
  clasificar/2 falla en vez de suponer una clase.
*/

valor(Animal, Atributo, Valor) :-
    get_dict(Atributo, Animal, Valor).

% ======= HOJAS CON RESULTADO true (DEPREDADOR) =======

% Hoja 1: idalimentacion=1, venenoso=1. {false=0, true=7}
clasificar(A, true) :-
    valor(A, idalimentacion, 1),
    valor(A, venenoso, 1).

% Hoja 2: idalimentacion=1, venenoso=0, gran tamaño=1. {false=1, true=17}
clasificar(A, true) :-
    valor(A, idalimentacion, 1),
    valor(A, venenoso, 0),
    valor(A, 'gran tamaño', 1).

% Hoja 4: idalimentacion=1, venenoso=0, gran tamaño=0,
%          dientes=1, domestico=1, escamas=0. {false=0, true=2}
clasificar(A, true) :-
    valor(A, idalimentacion, 1),
    valor(A, venenoso, 0),
    valor(A, 'gran tamaño', 0),
    valor(A, dientes, 1),
    valor(A, domestico, 1),
    valor(A, escamas, 0).

% Hoja 7: idalimentacion=1, venenoso=0, gran tamaño=0,
%          dientes=1, domestico=0, comestible=0, pulmones=0. {false=0, true=2}
clasificar(A, true) :-
    valor(A, idalimentacion, 1),
    valor(A, venenoso, 0),
    valor(A, 'gran tamaño', 0),
    valor(A, dientes, 1),
    valor(A, domestico, 0),
    valor(A, comestible, 0),
    valor(A, pulmones, 0).

% Hoja 8: idalimentacion=1, venenoso=0, gran tamaño=0, dientes=0.
%          {false=0, true=5}
clasificar(A, true) :-
    valor(A, idalimentacion, 1),
    valor(A, venenoso, 0),
    valor(A, 'gran tamaño', 0),
    valor(A, dientes, 0).

% Hoja 12: idalimentacion=3, pulmones=0, escamas=0, cola=1.
%           {false=0, true=2}
clasificar(A, true) :-
    valor(A, idalimentacion, 3),
    valor(A, pulmones, 0),
    valor(A, escamas, 0),
    valor(A, cola, 1).

% ======= HOJAS CON RESULTADO false (NO DEPREDADOR) =======

% Hoja 3: empate false=1, true=1; el árbol decide false.
clasificar(A, false) :-
    valor(A, idalimentacion, 1),
    valor(A, venenoso, 0),
    valor(A, 'gran tamaño', 0),
    valor(A, dientes, 1),
    valor(A, domestico, 1),
    valor(A, escamas, 1).

% Hoja 5: {false=2, true=0}
clasificar(A, false) :-
    valor(A, idalimentacion, 1),
    valor(A, venenoso, 0),
    valor(A, 'gran tamaño', 0),
    valor(A, dientes, 1),
    valor(A, domestico, 0),
    valor(A, comestible, 1).

% Hoja 6: {false=4, true=1}
clasificar(A, false) :-
    valor(A, idalimentacion, 1),
    valor(A, venenoso, 0),
    valor(A, 'gran tamaño', 0),
    valor(A, dientes, 1),
    valor(A, domestico, 0),
    valor(A, comestible, 0),
    valor(A, pulmones, 1).

% Hoja 9: idalimentacion=2. {false=5, true=0}
clasificar(A, false) :-
    valor(A, idalimentacion, 2).

% Hoja 10: idalimentacion=3, pulmones=1. {false=53, true=1}
clasificar(A, false) :-
    valor(A, idalimentacion, 3),
    valor(A, pulmones, 1).

% Hoja 11: idalimentacion=3, pulmones=0, escamas=1. {false=5, true=0}
clasificar(A, false) :-
    valor(A, idalimentacion, 3),
    valor(A, pulmones, 0),
    valor(A, escamas, 1).

% Hoja 13: idalimentacion=3, pulmones=0, escamas=0, cola=0.
%           Empate false=1, true=1; el árbol decide false.
clasificar(A, false) :-
    valor(A, idalimentacion, 3),
    valor(A, pulmones, 0),
    valor(A, escamas, 0),
    valor(A, cola, 0).

% Predicados auxiliares opcionales:
depredador(A) :- clasificar(A, true).
no_depredador(A) :- clasificar(A, false).
