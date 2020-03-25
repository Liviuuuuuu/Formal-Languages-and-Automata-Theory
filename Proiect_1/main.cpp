#include <fstream>
#include <vector>
#include <cstring>
#include <queue>
#include <map>

using namespace std;

ifstream fin ("nfa.in");
ofstream fout("nfa.out");

int n, m, q0;
vector <pair <int, char> > G[1001];
bool viz[1001][1001], f[1001], ok;
char s[1001];
queue <pair <string, int> > Q;
vector <string> sol;
map <pair <int, string>, char> myMap;

void DFS(int nod, int poz)
{
    if(ok) return;
    if(poz == strlen(s))
    {
        ok = f[nod];
        return;
    }
    for(int i = 0; i < G[nod].size(); ++i)
        if(G[nod][i].second == s[poz] && !viz[G[nod][i].first][poz])
        {
            viz[G[nod][i].first][poz] = 1;
            DFS(G[nod][i].first, poz+1);
        }
}

void subp_b()
{
    int q;
    fin >> q;
    while(q--)
    {
        fin >> s;
        ok = 0;
        memset(viz, 0, sizeof(viz));
        DFS(q0, 0);
        fout << ok << '\n';
    }
}

void subp_c()
{
    Q.push(make_pair("", q0));
    while(!Q.empty())
    {
        int nod = Q.front().second;
        string sir = Q.front().first;
        Q.pop();
        if(f[nod])
        {
            sol.push_back(sir);
            if(sol.size() == 100)
            {
                for(int i = 0; i < sol.size(); ++i) fout << sol[i] << '\n';
                return;
            }
        }
        for(int i = 0; i < G[nod].size(); ++i)
        {
            string aux = sir + G[nod][i].second;
            if(myMap[make_pair(G[nod][i].first, aux)] == 0)
            {
                myMap[make_pair(G[nod][i].first, aux)] = 1;
                Q.push(make_pair(aux, G[nod][i].first));
            }

        }
    }
}

int main()
{
    int k;
    fin >> n >> m >> k;
    fin >> q0;
    for(int i = 1, qf; i <= k; ++i)
    {
        fin >> qf;
        f[qf] = 1;
    }
    for(int i = 1; i <= m; ++i)
    {
        int q1, q2;
        char c;
        fin >> q1 >> q2 >> c;
        G[q1].push_back(make_pair(q2, c));
    }

    subp_b();
    subp_c();
    return 0;
}
