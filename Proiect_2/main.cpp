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
istream& operator>>(istream& in, NFA& a)
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
ostream& operator<<(ostream& out, const NFA& a)
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
    friend ostream& operator<<(ostream&, const DFA&);
};
ostream& operator<<(ostream& out, const DFA& a)
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

    //adfa.minimisation();
    cout << "\nAm minimizat DFA-ul:\n";
    //cout << adfa;
    return 0;
}
