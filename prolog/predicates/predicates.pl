% Some predicates on Prolog

% ---------------------- 09 ----------------------
% прототипы: (i, o), (i, i)
qsort([], []).
qsort([X | T], SL) :- lr(X, T, L, R), qsort(L, LS), qsort(R, RS), append(LS, [X | RS], SL).

lr(_, [], [], []).
lr(X, [Y | T], [Y | L], R) :- Y =< X, !, lr(X, T, L, R).
lr(X, [Y | T], L, [Y | R]) :- lr(X, T, L, R).
% ---------------------- 09 ----------------------

% ---------------------- 11 ----------------------
% прототипы: (i, i, o), (i, i, i)
union(M1, M2, M3) :- un(M1, M2, M), perm(M, M3), !.

un([], M2, M2).
un([X | T], M2, Res) :- member(X, M2), !, un(T, M2, Res).
un([X | T], M2, [X | Res]) :- un(T, M2, Res).

perm([], []).
perm([X | T], Res) :- perm(T, T1), delete_one(X, Res, T1).

delete_one(X, [X | T], T).
delete_one(X, [Y | T], [Y | TY]) :- delete_one(X, T, TY).
% ---------------------- 11 ----------------------

% ---------------------- 13 ----------------------
% прототипы: (i, i) [(i, o) - в смысле T1 = T2]
sub_tree(empty, _) :- !, fail.
sub_tree(T, T) :- !.
sub_tree(T, tree(_, L, R)) :- sub_tree(T, L), !; sub_tree(T, R).
% ---------------------- 13 ----------------------

edge(a, b, 5).
edge(a, e, 5).
edge(a, d, 5).
edge(b, e, 5).
edge(e, d, 5).
edge(b, f, 5).
edge(d, f, 5).

e(X, Y, Z) :- edge(X, Y, Z); edge(Y, X, Z).

% ---------------------- 16 ----------------------
% прототипы (недетерминированные): все
path(X, Y, L) :- pth(X, Y, [X], L).

pth(X, Y, [_ | Acc], Acc) :- e(X, Y, _).
pth(X, Y, Acc, L) :- e(X, Z, _), dif(Z, Y), not(member(Z, Acc)), append(Acc, [Z], Tmp), pth(Z, Y, Tmp, L).
% ---------------------- 16 ----------------------

% ---------------------- 17 ----------------------
% прототип (недетерменированный): (i, i, o)
% прототип (детерменированный): (i, i, i)
min_path(X, Y, Res) :- findall(P, path(X, Y, P), L), cal_costs(X, Y, L, LC), list_min(LC, Min), print(L, LC, Min, Res).

cal_costs(_, _, [], []).
cal_costs(X, Y, [L | TL], [C | LC]) :- append(L, [Y], LY), cal_cost(X, LY, C), cal_costs(X, Y, TL, LC).

cal_cost(X, [Y], C) :- e(X, Y, C), !.
cal_cost(X, [Y | T], S) :- e(X, Y, C), cal_cost(Y, T, SC), S is C + SC. 

list_min([X], X) :- !.
list_min([X | T], Min) :- list_min(T, Y), Min is min(X, Y).

print([P | _], [C | _], Min, P) :- C =:= Min. 
print([_ | TP], [_ | TC], Min, P) :- print(TP, TC, Min, P).
% ---------------------- 17 ----------------------

% ---------------------- 18 ----------------------
% прототипы (недетерменированный): (i, i, o), [(i, o, o), (o, i, o) - смежные вершины]
% прототип (детерменированный): (i, i, i)
short_path(X, Y, Path) :- next([X], Y, [[]], Paths), print(Paths, Y, Path).

next(LX, Y, P, Paths) :- level(LX, Y, P, [], NP, [], N), isemp(N, Y, NP, Paths).

level([], _, _, Pacc, Pacc, Acc, Acc).
level([X | T], Y, [XP | P], Pacc, Nacc, Acc, Res) :-
    findall(Z, e(X, Z, _), L),
    diff(L, XP, R),
    make(R, [], X, XP, Pacc, NP),
    memb(Y, R, [X | T], [XP | P], NP, Nacc, Acc, Res).

make([], Acc, _, _, Pacc, Nacc) :- append(Acc, Pacc, Nacc).
make([_ | T], Acc, X, XP, Pacc, Nacc) :- make(T, [[X | XP] | Acc], X, XP, Pacc, Nacc).

memb(Y, L, LX, P, _, NP, _, []) :- member(Y, L), !, f(LX, P, NP).
memb(Y, L, [_ | T], [_ | P], Pacc, Nacc, Acc, Res) :-
    append(L, Acc, Acc1),
    level(T, Y, P, Pacc, Nacc, Acc1, Res).

f([], _, []).
f([X | T], [Path | P], [[X | Path] | NP]) :- f(T, P, NP).

diff([], _, []).
diff([X | T], L, Res) :- member(X, L), !, diff(T, L, Res).
diff([X | T], L, [X | Res]) :- diff(T, L, Res).

isemp([], _, P, P) :- !.
isemp(N, Y, P, Path) :- next(N, Y, P, Path).

print([[X | P] | _], Y, Path) :- e(X, Y, _), reverse([X | P], [_ | Path]).
print([_ | Paths], Y, Path) :- print(Paths, Y, Path).
% ---------------------- 18 ----------------------

% ---------------------- 19 ----------------------
cyclic() :- e(X, _, _), e(_, Y, _), dif(X, Y), findall(P, path(X, Y, P), L), length(L, N), N > 1, !.
% ---------------------- 19 ----------------------

% ---------------------- 20 ----------------------
is_connected() :- not(not_connected()).
not_connected() :- e(X, _, _), e(_, Y, _), dif(X, Y), not(path(X, Y, _)), !.
% ---------------------- 20 ----------------------
