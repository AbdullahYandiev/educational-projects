% Яндиев Абдуллах, 424 группа
% Программа "Родственные связи"

% --------------------- факты ---------------------
% is_man(X) - "X является мужчиной" 
is_man('Ivan').
is_man('Alexander').
is_man('Petr').
is_man('Michael').
is_man('Alexey').
is_man('Matvey').
is_man('German').

% is_woman(Y) - "Y является женщиной" 
is_woman('Ekaterina').
is_woman('Elizaveta').
is_woman('Roza').
is_woman('Tamara').
is_woman('Alisa').
is_woman('Anna').
is_woman('Sophia').

% be_married(X, Y) - "X и Y являются супругами" 
be_married('Ivan', 'Ekaterina').
be_married('Elizaveta', 'Alexander').
be_married('German', 'Sophia').

% be_parent(X, Y) - "X является родителем Y" 

be_parent('Ivan', 'Roza').
be_parent('Ekaterina', 'Roza').
be_parent('Ivan', 'Tamara').
be_parent('Ekaterina', 'Tamara').
be_parent('Ivan', 'Michael').
be_parent('Ekaterina', 'Michael').
be_parent('Ivan', 'Petr').
be_parent('Ekaterina', 'Petr').
be_parent('Elizaveta', 'Alexey').
be_parent('Alexander', 'Alexey').
be_parent('Elizaveta', 'Matvey').
be_parent('Alexander', 'Matvey').
be_parent('Elizaveta', 'Alisa').
be_parent('Alexander', 'Alisa').
be_parent('Elizaveta', 'Anna').
be_parent('Alexander', 'Anna').
be_parent('German', 'Ivan').
be_parent('Sophia', 'Ivan').
be_parent('German', 'Elizaveta').
be_parent('Sophia', 'Elizaveta').
% --------------------- факты ---------------------

% ------------------- предикаты -------------------
% be_son(X, Y) - "X является сыном Y"
be_son(X, Y) :- 
    is_man(X), 
    be_parent(Y, X).

% be_daughter(X, Y) - "X является дочерью Y"
be_daughter(X, Y) :- 
    is_woman(X), 
    be_parent(Y, X).

% be_brother(X, Y) - "X является братом Y"
be_brother(X, Y) :-
    is_man(X), 
    be_parent(Z, X),
    be_parent(Z, Y),
    is_man(Z), % чтобы не дублировались пары
    dif(X, Y).

% be_sister(X, Y) - "X является сестрой Y"
be_sister(X, Y) :-
    is_woman(X), 
    be_parent(Z, X),
    be_parent(Z, Y),
    is_man(Z), % чтобы не дублировались пары
    dif(X, Y).

% be_grandmum(X, Y) - "X является бабушкой Y"
be_grandmum(X, Y) :-
    is_woman(X),
    be_parent(X, Z),
    be_parent(Z, Y).

% be_granddad(X, Y) - "X является дедушкой Y"
be_granddad(X, Y) :-
    is_man(X),
    be_parent(X, Z),
    be_parent(Z, Y).

% be_grandchild(X, Y) - "X является внуком или внучкой Y"
be_grandchild(X, Y) :-
    be_granddad(Y, X);
    be_grandmum(Y, X).

% be_grandson(X, Y) - "X является внуком Y"
be_grandson(X, Y) :-
    is_man(X),
    be_grandchild(X, Y).

% be_granddaughter(X, Y) - "X является внучкой Y"
be_granddaughter(X, Y) :-
    is_woman(X),
    be_grandchild(X, Y).

% be_uncle(X, Y) - "X является дядей Y"
be_uncle(X, Y) :-
    be_brother(X, Z),
    be_parent(Z, Y).

% be_aunt(X, Y) - "X является тетей Y"
be_aunt(X, Y) :-
    be_sister(X, Z),
    be_parent(Z, Y).
% ------------------- предикаты -------------------

% --------------------- тесты ---------------------
% ?- be_son(X, Y).
% X = 'Petr',
% Y = 'Ivan' ;
% X = 'Petr',
% Y = 'Ekaterina' ;
% и так далее...

% ?- be_daughter(X, Y).
% X = 'Roza',
% Y = 'Ivan' ;
% X = 'Roza',
% Y = 'Ekaterina' ;
% и так далее...


% ?- be_brother(X, Y).
% X = 'Petr',
% Y = 'Roza' ;
% X = 'Petr',
% Y = 'Tamara' ;
% и так далее...

% ?- be_sister(X, Y).
% X = 'Roza',
% Y = 'Tamara' ;
% X = 'Roza',
% Y = 'Michael' ;
% и так далее...

% ?- be_grandmum(X, Y).
% X = 'Sophia',
% Y = 'Roza' ;
% X = 'Sophia',
% Y = 'Tamara' ;
% и так далее...

% ?- be_granddad(X, Y).
% X = 'German',
% Y = 'Roza' ;
% X = 'German',
% Y = 'Tamara' ;
% и так далее...

% ?- be_grandson(X, Y).
% X = 'Petr',
% Y = 'German' ;
% X = 'Petr',
% Y = 'Sophia' ;
% и так далее...

% ?- be_granddaughter(X, Y).
% X = 'Roza',
% Y = 'German' ;
% X = 'Roza',
% Y = 'Sophia' ;
% и так далее...

% ?- be_aunt(X, Y).
% X = 'Elizaveta',
% Y = 'Roza' ;
% X = 'Elizaveta',
% Y = 'Tamara' ;
% и так далее...

% ?- be_uncle(X, Y).
% X = 'Ivan',
% Y = 'Alexey' ;
% X = 'Ivan',
% Y = 'Matvey' ;
% и так далее...
% --------------------- тесты ---------------------