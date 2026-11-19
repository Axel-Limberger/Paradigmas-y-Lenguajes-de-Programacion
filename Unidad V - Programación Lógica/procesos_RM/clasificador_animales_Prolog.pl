
% ==================================================
% CLASIFICACION DE ANIMALES DEPREDADORES
% Arbol de decision generado en RapidMiner
% ==================================================

% Obtener el valor de un atributo del animal.
valor(Animal, Atributo, Valor) :-
    get_dict(Atributo, Animal, Valor).


% ==================================================
% ALIMENTACION = 1
% ==================================================

% Regla 1
clasificar(A, true) :-
    valor(A, idalimentacion, 1),
    valor(A, venenoso, 0),
    valor(A, gran_tamano, 0),
    valor(A, dientes, 0).

% Regla 2
clasificar(A, true) :-
    valor(A, idalimentacion, 1),
    valor(A, venenoso, 0),
    valor(A, gran_tamano, 0),
    valor(A, dientes, 1),
    valor(A, domestico, 0),
    valor(A, comestible, 0),
    valor(A, pulmones, 0).

% Regla 3
clasificar(A, false) :-
    valor(A, idalimentacion, 1),
    valor(A, venenoso, 0),
    valor(A, gran_tamano, 0),
    valor(A, dientes, 1),
    valor(A, domestico, 0),
    valor(A, comestible, 0),
    valor(A, pulmones, 1).

% Regla 4
clasificar(A, false) :-
    valor(A, idalimentacion, 1),
    valor(A, venenoso, 0),
    valor(A, gran_tamano, 0),
    valor(A, dientes, 1),
    valor(A, domestico, 0),
    valor(A, comestible, 1).

% Regla 5
clasificar(A, true) :-
    valor(A, idalimentacion, 1),
    valor(A, venenoso, 0),
    valor(A, gran_tamano, 0),
    valor(A, dientes, 1),
    valor(A, domestico, 1),
    valor(A, escamas, 0).

% Regla 6
clasificar(A, false) :-
    valor(A, idalimentacion, 1),
    valor(A, venenoso, 0),
    valor(A, gran_tamano, 0),
    valor(A, dientes, 1),
    valor(A, domestico, 1),
    valor(A, escamas, 1).

% Regla 7
clasificar(A, true) :-
    valor(A, idalimentacion, 1),
    valor(A, venenoso, 0),
    valor(A, gran_tamano, 1).

% Regla 8
clasificar(A, true) :-
    valor(A, idalimentacion, 1),
    valor(A, venenoso, 1).


% ==================================================
% ALIMENTACION = 2
% ==================================================

% Regla 9
clasificar(A, false) :-
    valor(A, idalimentacion, 2).


% ==================================================
% ALIMENTACION = 3
% ==================================================

% Regla 10
clasificar(A, false) :-
    valor(A, idalimentacion, 3),
    valor(A, pulmones, 0),
    valor(A, escamas, 0),
    valor(A, cola, 0).

% Regla 11
clasificar(A, true) :-
    valor(A, idalimentacion, 3),
    valor(A, pulmones, 0),
    valor(A, escamas, 0),
    valor(A, cola, 1).

% Regla 12
clasificar(A, false) :-
    valor(A, idalimentacion, 3),
    valor(A, pulmones, 0),
    valor(A, escamas, 1).

% Regla 13
clasificar(A, false) :-
    valor(A, idalimentacion, 3),
    valor(A, pulmones, 1).

