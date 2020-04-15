#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <set>

using namespace std;

ifstream fin("date.in");

class NFA
{
    int nrStari, si;
    vector <map <char, set<int> > > tranzitii;
    vector <bool> f;
public:
    void removeLambda()
    {
        bool ok = 0;
        while(ok)
        {
            ok = 1;
            for(int i = 1; i <= nrStari; ++i)
            {
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
}

int main()
{
    NFA automat;
    fin >> automat;
    cout << "Automatul initial:\n";
    cout << automat;
    automat.removeLambda();
    cout << "\nAm transformat automatul din Lambda-NFA in NFA:\n";
    cout << automat;
    return 0;
}
