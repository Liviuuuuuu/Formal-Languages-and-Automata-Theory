#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <set>
#include <queue>

using namespace std;

ifstream fin("date.in");

set <char> alf;

class NFA
{
    int nrStari, si;
    vector <map <char, set<int> > > tranzitii;
    vector <bool> f;
public:
    void removeLambda()
    {
        bool ok = 1;
        while(ok)
        {
            ok = 0;
            for(int i = 1; i <= nrStari; ++i)
            {
                if(!tranzitii[i].count('$')) continue;
                for(auto it : tranzitii[i]['$'])
                {
                    for(auto it2 : tranzitii[it])
                        for(auto to : it2.second)
                        {
                            tranzitii[i][it2.first].insert(to);
                            ok = 1;
                        }
                    if(f[it]) f[i] = 1, ok = 1;
                }
                tranzitii[i]['$'].clear();
            }
        }
    }
    int getNrStari() const { return nrStari; }
    int getSI() const { return si; }
    vector <map <char, set<int> > > getTranz() const { return tranzitii; }
    vector <bool> getF() const { return f; }
    friend istream& operator>>(istream&, NFA&);
    friend ostream& operator<<(ostream&, const NFA&);
};
istream& operator >>(istream& in, NFA& a)
{
    int aux;
    in >> a.nrStari >> aux;
    a.tranzitii.resize(a.nrStari+1);
    a.f.resize(a.nrStari+1);
    for(int i = 1; i <= aux; ++i)
    {
        int x, y;
        char c;
        in >> x >> y >> c;
        a.tranzitii[x][c].insert(y);
        if(c != '$') alf.insert(c);
    }
    in >> a.si >> aux;
    for(int i = 1, x; i <= aux; ++i)
    {
        in >> x;
        a.f[x] = 1;
    }
    return in;
}
ostream& operator <<(ostream& out, const NFA& a)
{
    out << a.nrStari << " stari\n";
    out << "Starea initiala: " << a.si << '\n';
    out << "Stari finale: ";
    for(int i = 1; i <= a.nrStari; ++i)
        if(a.f[i]) out << i << " ";
    out << '\n';
    for(int i = 1; i <= a.nrStari; ++i)
        for(auto it : a.tranzitii[i])
            for(auto j : it.second) out << i << " " << j << " " << it.first << '\n';

    return out;
}

class DFA
{
    int nrStari, si;
    vector <map <char, int> > tranzitii;
    vector <bool> f;

    void removeStates(vector <bool> &unreachable)
    {
        // starile in care nu putem ajunge
        int nod;
        queue <int> Q;
        vector <bool> viz(nrStari+1, 0);
        Q.push(si);
        viz[si] = 1;
        while(!Q.empty())
        {
            nod = Q.front();
            Q.pop();
            for(auto it : tranzitii[nod])
                if(!viz[it.second])
                {
                    viz[it.second] = 1;
                    Q.push(it.second);
                }
        }
        int out = 0;
        for(int i = 1; i <= nrStari; ++i)
            if(!viz[i])
            {
                unreachable[i] = 1;
                out++;
            }
        nrStari -= out;


        // stari din care nu ajungem intr-o stare finala
        viz.clear();
        viz.resize(nrStari+1, 0);
        vector <vector <int> > inv(nrStari+1);
        for(int i = 1; i <= nrStari; ++i)
            for(auto it : tranzitii[i]) inv[it.second].push_back(i);

        for(int i = 1; i <= nrStari; ++i)
            if(f[i])
            {
                Q.push(i);
                viz[i] = 1;
            }
        while(!Q.empty())
        {
            nod = Q.front();
            Q.pop();
            for(auto it : inv[nod])
                if(!viz[it])
                {
                    viz[it] = 1;
                    Q.push(it);
                }
        }
        out = 0;
        for(int i = 1; i <= nrStari; ++i)
            if(!viz[i])
            {
                unreachable[i] = 1;
                out++;
            }
        nrStari -= out;
    }
    int equiv(int x, vector <vector <int> > sets)
    {
        for(int i = 0; i < sets.size(); ++i)
            for(int j = 0; j < sets[i].size(); ++j)
                if(sets[i][j] == x) return i;
    }
public:
    DFA(const NFA& anfa)
    {
        nrStari = 0;
        map <set <int>, int> myMap;
        vector <map <char, set<int> > > nfaTranz = anfa.getTranz();
        vector <bool> nfaF = anfa.getF();
        f.push_back(0); // incepem de pe 1
        queue <set <int> > Q;
        set <int> aux;

        si = anfa.getSI();
        aux.insert(si);
        f.push_back(nfaF[si]);
        myMap[aux] = ++nrStari;
        tranzitii.push_back(map <char, int>());
        tranzitii.push_back(map <char, int>());

        Q.push(aux);
        while(!Q.empty())
        {
            set <int> nodes = Q.front();
            Q.pop();

            for(char c : alf)
            {
                aux.clear();
                for(auto it : nodes)
                {
                    if(!nfaTranz[it].count(c)) continue;
                    for(auto to : nfaTranz[it][c]) aux.insert(to);
                }
                if(!aux.size()) continue;
                if(!myMap.count(aux))
                {
                    myMap[aux] = ++nrStari;
                    bool Final = 0;
                    for(auto it : aux)
                        if(nfaF[it]) Final = 1;
                    f.push_back(Final);
                    tranzitii.push_back(map <char, int>());
                    Q.push(aux);
                }
                tranzitii[myMap[nodes]][c] = myMap[aux];
            }
        }
    }
    void minimisation()
    {
        vector <bool> unreachable(nrStari+1, 0);
        removeStates(unreachable);

        vector <int> aux1[2];
        for(int i = 1; i <= nrStari; ++i)
            if(!unreachable[i]) aux1[f[i]].push_back(i);
        vector <vector <int> > sets[2];
        sets[0].push_back(aux1[0]); sets[0].push_back(aux1[1]);

        sets[1] = sets[0];
        do
        {
            sets[0] = sets[1];

            for(int i = 0; i < sets[1].size(); ++i)
                for(int j = 1; j < sets[1][i].size(); ++j)
                {
                    bool ePusOK = 0;
                    for(int k = 0; k < j; ++k)
                    {
                        bool echiv = 1;
                        for(char c : alf)
                            if(tranzitii[sets[1][i][j]].count(c) && tranzitii[sets[1][i][k]].count(c) && !unreachable[tranzitii[sets[1][i][j]][c]] && !unreachable[tranzitii[sets[1][i][k]][c]])
                                if(equiv(tranzitii[sets[1][i][j]][c], sets[0]) != equiv(tranzitii[sets[1][i][k]][c], sets[0]))
                                {
                                    echiv = 0;
                                    break;
                                }
                        if(echiv)
                        {
                            ePusOK = 1;
                            break;
                        }
                    }
                    if(ePusOK) continue;

                    for(int I = sets[0].size(); I < sets[1].size() && !ePusOK; ++I)
                        for(int J = 0; J < sets[1][I].size(); ++J)
                        {
                            bool echiv = 1;
                            for(char c : alf)
                                if(tranzitii[sets[1][i][j]].count(c) && tranzitii[sets[1][I][J]].count(c) && !unreachable[tranzitii[sets[1][i][j]][c]] && !unreachable[tranzitii[sets[1][I][J]][c]])
                                    if(equiv(tranzitii[sets[1][i][j]][c], sets[0]) != equiv(tranzitii[sets[1][I][J]][c], sets[0]))
                                    {
                                        echiv = 0;
                                        break;
                                    }
                            if(echiv)
                            {
                                sets[1][I].push_back(sets[1][i][j]);
                                sets[1][i].erase(sets[1][i].begin()+j);
                                ePusOK = 1;
                                break;
                            }
                        }
                    if(ePusOK) continue;
                    sets[1].push_back(vector <int> ({sets[1][i][j]}));
                    sets[1][i].erase(sets[1][i].begin()+j);
                }
        } while(sets[0] != sets[1]);

        vector <int> newNodes(nrStari+1, 0);
        int n = 0;
        for(int i = 0; i < sets[1].size(); ++i)
        {
            n++;
            for(int j = 0; j < sets[1][i].size(); ++j) newNodes[sets[1][i][j]] = n;
        }
        vector <map <char, int> > newTranz(n+1, map <char, int>());
        for(int i = 0; i < sets[1].size(); ++i)
        {
            int first = sets[1][i][0];
            for(char c : alf)
                if(tranzitii[first].count(c) && !unreachable[tranzitii[first][c]]) newTranz[newNodes[first]][c] = newNodes[ tranzitii[first][c] ];
        }
        vector <bool> newF(n+1, 0);
        for(int i = 0; i < sets[1].size(); ++i)
            if(f[ sets[1][i][0] ]) newF[newNodes[sets[1][i][0]]] = 1;


        for(int i = 0; i < sets[1].size(); ++i)
        {
            cout << "{ ";
            for(int j = 0; j < sets[1][i].size(); ++j)
                cout << sets[1][i][j] << " ";
            cout << "}\n";
        }
        si = newNodes[equiv(si, sets[1])+1];
        nrStari = n;
        f = newF;
        tranzitii = newTranz;
    }
    friend ostream& operator <<(ostream&, const DFA&);
};
ostream& operator <<(ostream& out, const DFA& a)
{
    out << a.nrStari << " stari\n";
    out << "Starea initiala: " << a.si << '\n';
    out << "Stari finale: ";
    for(int i = 1; i <= a.nrStari; ++i)
        if(a.f[i]) out << i << " ";
    out << '\n';
    for(int i = 1; i <= a.nrStari; ++i)
    {
        if(a.tranzitii[i].empty()) continue;
        for(auto it : a.tranzitii[i]) out << i << " " << it.second << " " << it.first << '\n';
    }
    return out;
}

int main()
{
    NFA anfa;
    fin >> anfa;
    cout << "Automatul initial:\n";
    cout << anfa;

    anfa.removeLambda();
    cout << "\nAm transformat automatul din Lambda-NFA in NFA:\n";
    cout << anfa;

    DFA adfa(anfa);
    cout << "\nAm transformat NFA-ul in DFA:\n";
    cout << adfa;

    adfa.minimisation();
    cout << "\nAm minimizat DFA-ul:\n";
    cout << adfa;
    return 0;
}
