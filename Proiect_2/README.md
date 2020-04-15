# Proiectul 2: Lambda-NFA to minimal DFA

Se da un Lambda-NFA. Transformați NFA-ul într-un DFA minimal. Proces în 3 pași:
* Lambda-NFA to NFA
* NFA to DFA
* Eliminăm stările neaccesibile (nu se poate ajunge la ele), stările necoaccesibile (stările din care nu se poate ajunge la o stare finală) apoi aplicăm algoritmul de minimizare
