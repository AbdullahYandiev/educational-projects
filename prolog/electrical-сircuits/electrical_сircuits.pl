:- use_module(library(pce)).

:- dynamic mode/1.
:- dynamic data/2.
:- dynamic node/3.
:- dynamic connection/3.

% -------------- Преобразование графа в неориентированный ---------------
edge(X, Y, Z) :- connection(X, Y, Z); connection(Y, X, Z).
% -------------- Преобразование графа в неориентированный ---------------

% --------------------- Предикаты из первого задания --------------------
path(X, Y, L) :- path(X, Y, [X], L).
path(X, Y, A, A) :- edge(X, Y, _).
path(X, Z, A, L) :- edge(X, Y, _), dif(Y, Z), not(member(Y, A)), path(Y, Z, [Y | A], L).

notConnected() :- edge(X, _, _), edge(_, Y, _), dif(X, Y), not(path(X, Y, _)), !.
isConnected() :- not(notConnected()).
% --------------------- Предикаты из первого задания --------------------

% ------------------ Доопределение встроенных функций -------------------
foldl(F, [X | T], R) :- foldl(F, T, X, R).
member(X, Y, L) :- member(X, L); member(Y, L).
dif(X, Y, Z) :- dif(X, Y), dif(X, Z), dif(Y, Z).
retractall  :-  retractall(mode(_)),
                retractall(data(_, _)),
                retractall(node(_, _, _)), 
                retractall(connection(_, _, _)).
% ------------------ Доопределение встроенных функций -------------------

% -------------------------- Визуализация цепи --------------------------
printGraph(N1, N2) :-
        new(F, frame('Electrical circuit')),
        new(P, picture),
        new(D, dialog),
        send(F, append(P)),
        send(D, below(P)),
        printNodesAndEdges(P, []),
        send(D, append, button('Next', message(@prolog, applyRule, P, N1, N2))),
        send(D, open).

printNodesAndEdges(P, L) :-
        send(P, clear),
        forall(node(Name, X, Y), printNode(P, Name, X, Y)),
        forall(edge(From, To, R), printEdge(P, From, To, R, L)).

printNode(P, Name, X, Y) :-
        new(Device, device),
        send(Device, name(Name)),
        new(Node, box(40, 20)),
        send(Device, display(Node)),
        new(Identifier, text(Name)),
        send(Device, display(Identifier, point(18, 5))),
        send(P, display(Device, point(X, Y))),
        new(M, move_gesture(left)),
        send(Device, recogniser(M)),
        send(Device, handle(handle(w/2, 0, link, link))),
        send(Device, handle(handle(w/2, h, link, link))),
        send(Device, handle(handle(0, h/2, link, link))),
        send(Device, handle(handle(w, h/2, link, link))).

printEdge(P, From, To, R, L) :-
        node(From, X1, Y1), 
        node(To, X2, Y2),
        X is (X1 + X2) / 2, 
        Y is (Y1 + Y2) / 2,
        get(P, member(From), Node1),
        get(P, member(To), Node2),
        (From == To, S = R;
        % round(V, N) -> format(atom(N), '~2f', V): поддержка типа float
        findall(N, (edge(From, To, V), format(atom(N), '~4f', V)), LR), foldl(scat, LR, S)),
        new(Link, link(link,link,line(0, 0, 0, 0))),
        new(Text, text(S, center, bold)),
        (member([From, To], [To, From], L), send(Link, colour, red),
        send(Text, colour, red); true),
        send(Node1, connect, Node2, Link),
        send(P, display(Text, point(X, Y))).
scat(S1, S2, S) :- format(atom(S), '~w | ~w', [S1, S2]).
% -------------------------- Визуализация цепи --------------------------

% -------------------------- Применение правил --------------------------
applyRule(P, N1, N2) :-
        rule1(P, N1, N2);
        rule2(P);
        rule3(P, N1, N2);
        rule4(P);
        rule5(P, N1, N2);
        checkEnd(N1, N2), printNodesAndEdges(P, []);
        restart(P, N1, N2).

restart(P, N1, N2) :-
        changeMode('append'),
        lookData(Data, _),
        assertFromList(Data),
        (rule5(P, N1, N2);
        getData, restart(P, N1, N2)).

changeMode(Mode) :-
        retract(mode(_)),
        assert(mode(Mode)).

putData(NewData, NewV) :-
        data(Data, V), 
        retract(data(_, _)),
        assert(data([NewData | Data], [NewV | V])).

lookData(Data, V) :- data([Data | _], [V | _]).

getData :-
        data([_ | Data], [_ | V]),
        retract(data(_, _)),
        assert(data(Data, V)).
% -------------------------- Применение правил --------------------------

% --------------------- Последовательное соединение ---------------------
rule1(P, N1, N2) :-
        edge(X, Y, R1),
        edge(Y, Z, R2),
        dif(X, Z),
        dif(Y, N1, N2),
        findall(_, edge(Y, _, _), L),
        length(L, N),
        N =:= 2,
        R is R1 + R2,
        printNodesAndEdges(P, [[X, Y], [Y, Z]]),
        retract(node(Y, _, _)),
        retractall(connection(Y, _, _)),
        retractall(connection(_, Y, _)),
        assert(connection(X, Z, R)).
% --------------------- Последовательное соединение ---------------------

% ----------------------- Параллельное соединение -----------------------
rule2(P) :-
        edge(X, Y, _),
        dif(X, Y),
        findall(PR, edge(X, Y, PR), L),
        length(L, N),
        N > 1,
        foldl(rule2, L, R),
        printNodesAndEdges(P, [[X, Y]]),
        retractall(connection(X, Y, _)),
        retractall(connection(Y, X, _)),
        assert(connection(X, Y, R)).
rule2(R1, R2, R) :- R is R1 * R2 / (R1 + R2).
% ----------------------- Параллельное соединение -----------------------

% ------------------------- Изолированная ветвь -------------------------
rule3(P, N1, N2) :-
        edge(X, _, _),
        dif(X, N1, N2),
        findall([X, Y], edge(X, Y, _), L),
        length(L, N),
        N =:= 1,
        printNodesAndEdges(P, L),
        retract(node(X, _, _)),
        (retract(connection(X, _, _));
        retract(connection(_, X, _))).
% ------------------------- Изолированная ветвь -------------------------

% -------------------------------- Петля --------------------------------
rule4(P) :-
        connection(X, X, _),
        printNodesAndEdges(P, [[X, X]]),
        retractall(connection(X, X, _)).
% -------------------------------- Петля --------------------------------

% -------------------------- Трехлучевая звезда --------------------------
rule5(P, N1, N2) :-
        edge(X, V, R1),
        edge(Y, V, R2),
        edge(Z, V, R3),
        dif(X, Y, Z), 
        dif(V, N1, N2),
        findall(_, edge(V, _, _), L), 
        length(L, N), 
        N =:= 3,
        (mode('new'), makeLists(Lists), putData(Lists, [V]);
        mode('append'), lookData(Data, LV), not(member(V, LV)), getData,
        putData(Data, [V | LV]), changeMode('new')),
        R is R1*R2 + R1*R3 + R2*R3,
        R11 is R / R1,
        R22 is R / R2,
        R33 is R / R3,
        printNodesAndEdges(P, [[X, V], [Y, V], [Z, V]]),
        retract(node(V, _, _)),
        retractall(connection(V, _, _)),
        retractall(connection(_, V, _)),
        assert(connection(X, Y, R33)),
        assert(connection(X, Z, R22)),
        assert(connection(Y, Z, R11)).
% -------------------------- Трехлучевая звезда --------------------------

% ------------------------ Проверка на завершение ------------------------
checkEnd(N1, N2) :-
        edge(N1, N2, _),
        findall(_, connection(_, _, _), L),
        length(L, N),
        N =:= 1.
% ------------------------ Проверка на завершение ------------------------

% -------------------- Преобразование фактов в список --------------------
makeLists([Nodes, Edges]) :- 
        findall(node(Name, X, Y), node(Name, X, Y), Nodes),
        findall(connection(X, Y, R), connection(X, Y, R), Edges).
% -------------------- Преобразование фактов в списки --------------------

% -------------------- Преобразование списков в факты --------------------
assertFromList([Nodes, Edges]) :-
        retractall(node(_, _, _)), 
        retractall(connection(_, _, _)),
        maplist(assert, Nodes),
        maplist(assert, Edges).
% -------------------- Преобразование списков в факты --------------------

% ----------------------- Проверка входных данных ------------------------
checkInput(N1, N2) :-
        (edge(N1, _, _); !, format(atom(S), 'Узел ~w отсутствует в сети', N1), error(S)),
        (edge(N2, _, _); !, format(atom(S), 'Узел ~w отсутствует в сети', N2), error(S)),
        (dif(N1, N2); !, error('Источники тока не должны совпадать')),
        (isConnected(); !, error('Цепь не является связной')).
error(Msg) :- format('ОШИБКА: ~w', Msg), retractall, fail.
% ----------------------- Проверка входных данных ------------------------

% ----------------------- Загрузка фактов из файла -----------------------
loadFile(FileName) :-
        retractall,
        assert(mode('new')),
        assert(data([], [])),
        see(FileName),
        repeat,
        read(F),
        (F = end_of_file, !, seen;
        assert(F), fail).
% ----------------------- Загрузка фактов из файла -----------------------

% --------------------------- Основная функция ---------------------------
main(N1, N2, FileName) :-
        loadFile(FileName),
        checkInput(N1, N2),
        printGraph(N1, N2),
        !.
% --------------------------- Основная функция ---------------------------