//
//  main.cpp
//  Compiler Design
//
//  Created by Akshat Gupta on 22/04/19.
//  Copyright © 2019 coded. All rights reserved.
//

//#include <iostream>
//
//int main(int argc, const char * argv[]) {
//    // insert code here...
//    std::cout << "Hello, World!\n";
//    return 0;
//}
#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <map>
#include <stack>
using namespace std;
//for IC generator
int codeGenerator(string);
string input,op,arg1,arg2;
string r="01234";

int l,j=0;

void rep(int);
void checkUnaryMinus(int);
int getPrecedence(char);

void printCode();
string getCode(char);
//

//lexical analyser
set<char> first(vector<set<char> > &firstprod, vector<string> prod, char ch, map<char,int> nontermap, map<char,int> termap) {
    int ind=0;
    if(nontermap.find(ch)==nontermap.end())
        ind+=nontermap.size();
    else
        ind+=nontermap[ch];
    if(termap.find(ch)!=termap.end())
        ind+=termap[ch];
    if(firstprod[ind].size()!=0)
        return firstprod[ind];
    if(ind>=nontermap.size()) {
        firstprod[ind].insert(ch);
        return firstprod[ind];
    }
    int p=prod.size();
    int i;
    for(i=0;i<p;++i) {
        if(prod[i][0]==ch) {
            int j;
            for(j=3;j<prod[i].length();++j) {
                set<char> temp=first(firstprod,prod,prod[i][j],nontermap,termap);
                if(temp.find('^')==temp.end()) {
                    firstprod[ind].insert(temp.begin(),temp.end());
                    break;
                }
                else {
                    temp.erase('^');
                    firstprod[ind].insert(temp.begin(),temp.end());
                    continue;
                }
            }
            if(j==prod[i].length())
                firstprod[ind].insert('^');
        }
    }
    return firstprod[ind];
}
set<char> follow(vector<set<char> > &followprod, vector<set<char> > &firstprod, vector<string> prod, char ch, map<char,int> nontermap, map<char,int> termap) {
    int ind=nontermap[ch];
    if(followprod[ind].size()!=0)
        return followprod[ind];
    if(ind==0)
        followprod[0].insert('$');
    int p=prod.size();
    int i;
    for(i=0;i<p;++i) {
        int j;
        for(j=3;j<prod[i].length();++j) {
            if(prod[i][j]==ch) {
                int k;
                for(k=j+1;k<prod[i].length();++k) {
                    int ind2=0;
                    if(nontermap.find(prod[i][k])==nontermap.end())
                        ind2+=nontermap.size();
                    else
                        ind2+=nontermap[prod[i][k]];
                    if(termap.find(prod[i][k])!=termap.end())
                        ind2+=termap[prod[i][k]];
                    set<char> temp=firstprod[ind2];
                    if(temp.find('^')==temp.end()) {
                        followprod[ind].insert(temp.begin(),temp.end());
                        break;
                    }
                    else {
                        temp.erase('^');
                        followprod[ind].insert(temp.begin(),temp.end());
                        continue;
                    }
                }
                if(k==prod[i].length()) {
                    set<char> temp=follow(followprod,firstprod,prod,prod[i][0],nontermap,termap);
                    followprod[ind].insert(temp.begin(),temp.end());
                }
            }
        }
    }
    return followprod[ind];
}
void ll1tab(vector<vector<int> > &ll1table, vector<set<char> > &followprod, vector<set<char> > &firstprod, vector<string> prod, map<char,int> nontermap, map<char,int> termap) {
    int p=prod.size();
    int i;
    int t=termap.size();
    for(i=0;i<p;++i) {
        set<char> tempfirst;
        int j;
        for(j=3;j<prod[i].length();++j) {
            int ind2=0;
            if(nontermap.find(prod[i][j])==nontermap.end())
                ind2+=nontermap.size();
            else
                ind2+=nontermap[prod[i][j]];
            if(termap.find(prod[i][j])!=termap.end())
                ind2+=termap[prod[i][j]];
            set<char> temp=firstprod[ind2];
            if(temp.find('^')==temp.end()) {
                tempfirst.insert(temp.begin(),temp.end());
                break;
            }
            else {
                temp.erase('^');
                tempfirst.insert(temp.begin(),temp.end());
                continue;
            }
        }
        if(j==prod[i].length())
            tempfirst.insert('^');
        set<char>::iterator it;
        int k=0;
        for(it=tempfirst.begin();it!=tempfirst.end();it++) {
            char ch = (*it);
            if(ch=='^') {
                k=1;
                continue;
            }
            int y=termap[ch];
            int x=nontermap[prod[i][0]];
            ll1table[x][y]=i;
        }
        if(k==1) {
            int ind2=nontermap[prod[i][0]];
            set<char> tempfollow=followprod[ind2];
            for(it=tempfollow.begin();it!=tempfollow.end();it++) {
                char ch = (*it);
                if(ch=='$') {
                    int y=t-1;
                    int x=nontermap[prod[i][0]];
                    ll1table[x][y]=i;
                    continue;
                }
                else {
                    int y=termap[ch];
                    int x=nontermap[prod[i][0]];
                    ll1table[x][y]=i;
                }
            }
        }
    }
}
char error_handler(int inonter, int iter, vector<int> tableentry, vector<char> ter) {
    int i;
    cout<<"\nUnexpected symbol: "<<ter[iter]<<"\nExpected: ";
    char x='1';
    for(i=0;i<tableentry.size();++i) {
        if(tableentry[i]!=-1) {
            if(x=='1') {
                x='2';
                cout<<ter[i];
            }
            else {
                cout<<", "<<ter[i];
            }
        }
    }
    cout<<"\nPlease select a token to continue: ";
    cin>>x;
    cout<<"\nAssuming "<<x<<" and continuing...";
    return x;
}
int main()
{
    int n;
    cout<<"\nEnter number of non terminals: ";
    cin>>n;
    vector<char> nonter(n);
    map<char,int> nontermap;
    int i;
    cout<<"\nEnter non terminals one by one: ";
    for(i=0;i<n;++i) {
        cin>>nonter[i];
        nontermap[nonter[i]]=i;
    }
    int t;
    cout<<"\nEnter number of terminals: ";
    cin>>t;
    vector<char> ter(t);
    map<char,int> termap;
    cout<<"\nEnter terminals one by one: ";
    for(i=0;i<t;++i) {
        cin>>ter[i];
        termap[ter[i]]=i;
    }
    ter.push_back('^');
    ++t;
    termap['^']=t-1;
    cout<<"\nEnter number of productions: ";
    int p;
    cin>>p;
    vector<string> prod(p);
    cout<<"\nEnter productions one by one: ";
    for(i=0;i<p;++i) {
        cout<<"\nProduction "<<i+1<<": ";
        cin>>prod[i];
    }
    vector<set<char> > firstprod(n+t);
    for(i=0;i<t;++i)
        first(firstprod,prod,ter[i],nontermap,termap);
    for(i=0;i<n;++i)
        first(firstprod,prod,nonter[i],nontermap,termap);
    cout<<"\n\n\n";
    for(i=0;i<n;++i) {
        int k=0;
        cout<<"\nFirst of "<<nonter[i]<<" is {";
        set<char>::iterator it;
        for(it=firstprod[i].begin();it!=firstprod[i].end();it++) {
            if(k==0) {
                cout<<(*it);
                k=1;
            }
            else {
                cout<<", "<<(*it);
            }
        }
        cout<<"}";
    }
    vector<set<char> > followprod(n);
    for(i=0;i<n;++i)
        follow(followprod,firstprod,prod,nonter[i],nontermap,termap);
    cout<<"\n\n\n";
    for(i=0;i<n;++i) {
        int k=0;
        cout<<"\nFollow of "<<nonter[i]<<" is {";
        set<char>::iterator it;
        for(it=followprod[i].begin();it!=followprod[i].end();it++) {
            if(k==0) {
                cout<<(*it);
                k=1;
            }
            else {
                cout<<", "<<(*it);
            }
        }
        cout<<"}";
    }
    cout<<"\n\n\n";
    vector<vector<int> > ll1table(n,vector<int>(t,-1));
    ll1tab(ll1table,followprod,firstprod,prod,nontermap,termap);
    int j;
    for(i=0;i<n;++i) {
        for(j=0;j<t;++j) {
            cout<<"Production for ("<<nonter[i]<<", ";
            if(j==t-1)
                cout<<"$): ";
            else
                cout<<ter[j]<<"): ";
            if(ll1table[i][j]!=-1)
                cout<<prod[ll1table[i][j]];
            else
                cout<<"Error";
            cout<<"\n";
        }
    }
    cout<<"\n\n\n";
    string inp;
    cout<<"Enter input string: ";
    cin>>inp;
    inp+="$";
    int l=inp.length();
    stack<char> parsestack;
    parsestack.push('$');
    parsestack.push(nonter[0]);
    i=0;
    vector<string> prodseq;
    while(1) {
        char x=parsestack.top();
        char y=inp[i];
        if(x==y&&x=='$')
            break;
        if(x==y) {
            parsestack.pop();
            ++i;
            continue;
        }
        if(nontermap.find(x)==nontermap.end()) {
            cout<<"\nUnexpected symbol: "<<y<<"\nExpected: "<<x<<"\nAssuming "<<x<<" and continuing...";
            inp[i]=x;
            parsestack.pop();
            ++i;
            continue;
        }
        int xi=nontermap[x];
        int yi;
        if(y=='$')
            yi=t-1;
        else
            yi=termap[y];
        if(ll1table[xi][yi]==-1) {
            y=error_handler(xi,yi,ll1table[xi],ter);
            if(y=='$')
                yi=t-1;
            else
                yi=termap[y];
            inp[i]=y;
        }
        string q=prod[ll1table[xi][yi]];
        q=q.substr(3,q.length()-3);
        parsestack.pop();
        if(q!="^") {
            for(j=q.length()-1;j>=0;--j)
                parsestack.push(q[j]);
        }
        if(prodseq.size()==0)
            prodseq.push_back(q);
        else {
            string w=prodseq[prodseq.size()-1];
            for(j=0;j<w.size();++j) {
                if(nontermap.find(w[j])!=nontermap.end())
                    break;
            }
            w.erase(w.begin()+j);
            if(q!="^")
                w.insert(j,q);
            prodseq.push_back(w);
        }
    }
    cout<<"\nLeft most derivation of the string is: ";
    for(i=0;i<prodseq.size();++i) {
        if(i==0)
            cout<<"\n"<<nonter[0]<<"->"<<prodseq[i];
        else
            cout<<"\n ->"<<prodseq[i];
    }
    cout<<endl<<"Intermediate Code :"<<endl;
    codeGenerator(inp);
    return 0;
}

int codeGenerator(string inp) {
    int i,p;
    //label :
//    cout<<"Enter the input Expression\n";
//    cin>>input;
    input = inp;
    l=input.length();
    for(i=0;i<l;i++){
        if(input[i]=='-')
            checkUnaryMinus(i);
    }
    
    for(i=0;i<l;i++)
    {
        p=getPrecedence(input[i]);
        if(p==6)
        {
            rep(i);
            i=0;
        }
    }
    for(i=0;i<l;i++)
    {
        p=getPrecedence(input[i]);
        if(p==5)
        {
            rep(i);
            i=0;
        }
    }
    for(i=0;i<l;i++)
    {
        p=getPrecedence(input[i]);
        if(p==4)
        {
            rep(i);
            i=0;
        }
    }
    
    cout<<"The triplet 3-address code notation is \n";
    cout<<"No:\top\targ1\targ2\n";
    for(i=0;i<j;i++)
        cout<<i<<"\t"<<op[i]<<"\t"<<arg1[i]<<"\t\t"<<arg2[i]<<endl;
    cout<<"\nGenerated code\n";
    printCode();
    return 0;
}

string getCode(char c)
{
    switch(c)
    {
        case '*': return "MUL";
        case '/': return "DIV";
        case '+': return "ADD";
        case '-': return "SUB";
    }
    return "STR";
}
void printCode()
{
    for(int i=0;i<j;i++)
    {
        if(op[i]!='=')
        {
            cout<<"MOV "<<arg1[i]<<",R0\n";
            if(op[i]=='m') cout<<"NEG "<<arg1[i]<<endl;
            else    cout<<getCode(op[i])<<" "<<arg2[i]<<" , R0\n";
        }
        else cout<<"MOV "<<arg2[i]<<" , "<<arg1[i]<<endl;
    }
}
int getPrecedence(char c)
{
    switch(c)
    {
        case '*':
        case '/': return 6;
        case '+':
        case '-': return 5;
        case '=': return 4;
        default : return -1;
    }
}
void rep(int i)
{
    op[j]=input[i];
    arg1[j]=input[i-1];
    arg2[j]=input[i+1];
    
    //input[i-1] = (char)j;
    input.replace(i-1,3,to_string(j));
    l=l-2;
    j++;
}

void checkUnaryMinus(int i)
{
    if(!isalpha(input[i-1]))
    {
        op[j]='m';
        arg1[j]=input[i+1];
        arg2[j]=' ';
        
        //input[i] = (char)j;
        input.replace(i,2,to_string(j));
        l--;
        j++;
    }
}
