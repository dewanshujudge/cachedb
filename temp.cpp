#include <bits/stdc++.h>
using namespace std;
 
typedef long long ll;
typedef vector<int> vi;
typedef vector<ll> vll;
#define all(x) (x).begin(), (x).end()
#define inf 1000000007
#define llmax LLONG_MAX
#define pi 3.141592653589793238462643383279502884197169399
 
long long binpow(long long a, long long b) {
    long long res = 1;
    while (b > 0) {
        if (b & 1)
            res = res * a;
        a = a * a;
        b >>= 1;
    }
    return res;
}
ll ncr(int n, int r)
{
    if (n < r) return 0;
    long long p = 1, k = 1;
    if (n - r < r)
        r = n - r;
    if (r != 0) {
        while (r) {
            p *= n;
            k *= r;
            long long m = __gcd(p, k);
            p /= m;
            k /= m;
            n--;
            r--;
        }
    }
    else
        p = 1;
    return p;
}

vector <ll> vcreate(int n){
    vector <ll> v(n);
    for (int i = 0; i < n; i++)
    {
        cin>>v[i];
    }
    return v;
}

char t[32];
int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    //freopen("input.txt", "r", stdin);
    //freopen("output.txt", "w", stdout); 

    /* ll T; cin>>T;
    for(ll i=0; i<T; i++)
    {
           
    } */

    cout<<t[17]<<endl;
}