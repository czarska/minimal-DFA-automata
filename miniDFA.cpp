//
//  main.cpp
//  minautomat
//
//  Created by Gabrysia Czarska on 27.04.2017.
//  Copyright © 2017 Gabrysia Czarska. All rights reserved.
//

#include <iostream>
#include <string>
#include <stack>
#include <vector>
#include <set>
#include <map>
#include <queue>
#include <algorithm>
using namespace std;

int n;
int startState;
int finishState;
int statesCounter;
int DFAStatesCounter;
stack<int> S;
vector<pair<int, int>*> delta[10000];
int deltaDFA[10000][27];
bool fin[10000];

/*------------------------------------------ FUNCTIONS -------------------------------------------------*/
void preclean();
void clean();

void parseString(string s);
void concatenation();
void KleeneStar();
void sum();
void letter(int x);
void newState();
void printNFA();

void epsilonClosure(set<int> *inSet, set<int> *outSet);
string makeDFAVerticleName(set<int> *sfd);
void addNewDFAState(set<int> *sfd, int letter, int from, map <set<int>, int> *mapForDFA, queue<pair<int, set<int>>> *statesToMake);
void goByAllLetters(int from, set<int> *sfd, map <set<int>, int> *mapForDFA, queue<pair<int, set<int>>> *statesToMake);
void NFAtoDFA();
void printDFA();

void minimalization();
/*------------------------------------------ IMPLEMENTATION -------------------------------------------------*/
int main(int argc, const char * argv[]) {
    int m;
    cin>>n>>m;
    for (int i=0; i<m; i++) {
        string s;
        cin>>s;
        parseString(s);
        NFAtoDFA();
        preclean();
        minimalization();
        clean();
    }
    return 0;
}
void preclean() {
    for (int j=0; j<statesCounter; j++)
        for (int jj=0; jj<delta[j].size(); jj++)
            delete delta[j][jj];
}
void clean() {
    for (int j=0; j<DFAStatesCounter; j++)
        for (int jj=0; jj<27; jj++)
            deltaDFA[j][jj] = -1;
    for (int j=0; j<statesCounter; j++)
        delta[j].clear();
    while(!S.empty()) S.pop();
    statesCounter = 0;
    DFAStatesCounter = 0;
    
}

/*------------------------------------------ BUILD NFA -------------------------------------------------*/
void parseString(string s) {
    startState = 0;
    statesCounter = 0;
    for (int j=0; j<s.size(); j++) {
        int val = s[j] - 97;
        if (val == -48)
            val = -1;
        else if (val == -97)
            val = -2;
        switch (s[j])
        {
            case '.':
                concatenation();
                break;
            case '*':
                KleeneStar();
                break;
            case '+':
                sum();
                break;
            default:
                letter(val);
                break;
        }
    }
}
void concatenation() {
    int s4 = S.top(); S.pop();
    int s3 = S.top(); S.pop();
    int s2 = S.top(); S.pop();
    int s1 = S.top(); S.pop();
    pair<int, int> *p = new pair<int, int> (-1, s3);
    delta[s2].push_back(p);
    S.push(s1); S.push(s4);
}
void KleeneStar() {
    int s2 = S.top(); S.pop();
    int s1 = S.top(); S.pop();
    newState();
    pair<int, int> *p = new pair<int, int> (-1, statesCounter-1);
    delta[s2].push_back(p);
    pair<int, int> *p1 = new pair<int, int> (-1, s1);
    delta[statesCounter-1].push_back(p1);
    S.push(statesCounter-1);
    if (s1 == startState)
        startState = statesCounter-1;
}
void sum() {
    int s4 = S.top(); S.pop();
    int s3 = S.top(); S.pop();
    int s2 = S.top(); S.pop();
    int s1 = S.top(); S.pop();
    newState();
    newState();
    pair<int, int> *p = new pair<int, int> (-1, statesCounter-1);
    delta[s2].push_back(p);
    pair<int, int> *p1 = new pair<int, int> (-1, statesCounter-1);
    delta[s4].push_back(p1);
    pair<int, int> *p2 = new pair<int, int> (-1, s1);
    delta[statesCounter-2].push_back(p2);
    pair<int, int> *p3 = new pair<int, int> (-1, s3);
    delta[statesCounter-2].push_back(p3);
    if (s1 == startState || s3 == startState)
        startState = statesCounter-2;
}
void letter(int x) {
    newState();
    pair<int, int> *p = new pair<int, int> (x, statesCounter);
    delta[statesCounter-1].push_back(p);
    newState();
}
void newState() {
    finishState = statesCounter;
    S.push(statesCounter);
    statesCounter++;
}

/*------------------------------------------ NFA TO DFA -------------------------------------------------*/
void epsilonClosure(set<int> *inSet, set<int> *outSet) {
    queue<int> q;
    for(set<int>::iterator it=(*inSet).begin(); it!=(*inSet).end(); ++it) {
        q.push(*it);
        (*outSet).insert(*it);
    }
    while(!q.empty()) {
        int x = q.front(); q.pop();
        if ((*outSet).find(x) == (*outSet).end())
            (*outSet).insert(x);
        for (int j=0; j<delta[x].size(); j++) {
            int y = (*delta[x][j]).second;
            int letter = (*delta[x][j]).first;
            if (letter == -1 && (*outSet).find(y) == (*outSet).end())
                q.push(y);
        }
    }
    
}
void addNewDFAState(set<int> *sfd, int letter, int from, map <set<int>, int> *mapForDFA, queue<pair<int, set<int>*>> *statesToMake) {
    if ((*mapForDFA).find(*sfd) == (*mapForDFA).end()) {
        (*mapForDFA)[*sfd] = DFAStatesCounter;
        fin[DFAStatesCounter] = false;
        if (letter != -1)
            deltaDFA[from][letter] = DFAStatesCounter;
        if ((*sfd).find(finishState) != (*sfd).end())
            fin[DFAStatesCounter] = true;
        for (int j=0; j<n; j++)
            deltaDFA[DFAStatesCounter][j] = -1;
        pair<int, set<int>*> p(DFAStatesCounter, sfd);
        (*statesToMake).push(p);
        DFAStatesCounter++;
    } else {
        if (letter != -1) {
            deltaDFA[from][letter] = (*mapForDFA)[*sfd];
        }
        delete sfd;
    }
}
void goByAllLetters(int from, set<int> *sfd, map <set<int>, int> *mapForDFA, queue<pair<int, set<int>*>> *statesToMake){
    for (int jj=0; jj<n; jj++) {
        int letter = jj;
        set<int> *helpSet = new set<int>();
        set<int> *helpSet2 = new set<int>();
        for(set<int>::iterator it=(*sfd).begin(); it!=(*sfd).end(); ++it) {
            for (int j=0; j<delta[*it].size(); j++) {
                if ((*delta[*it][j]).first == letter) {
                    (*helpSet).insert((*delta[*it][j]).second);
                }
            }
        }
        epsilonClosure(helpSet, helpSet2);
        addNewDFAState(helpSet2, letter, from, mapForDFA, statesToMake);
        delete helpSet;
    }
}
void NFAtoDFA() {
    DFAStatesCounter = 0;
    set<int> *startSet = new set<int>;
    map <set<int>, int> *mapForDFA = new map <set<int>, int>();
    queue<pair<int, set<int>*>> *statesToMake = new queue<pair<int, set<int>*>>();
    set<int> *startSet0 = new set<int>;
    (*startSet0).insert(startState);
    epsilonClosure(startSet0, startSet);
    delete startSet0;
    addNewDFAState(startSet, -1, -1, mapForDFA, statesToMake);
    while(!(*statesToMake).empty()) {
        pair<int, set<int>*> p = (*statesToMake).front(); (*statesToMake).pop();
        goByAllLetters(p.first, p.second, mapForDFA, statesToMake);
        delete p.second;
    }
    delete statesToMake;
    delete mapForDFA;
}
/*------------------------------------------ MINIMALIZATION -------------------------------------------------*/
void minimalization() {
    queue<pair<int,int>> Q;
    bool helpTable[DFAStatesCounter][DFAStatesCounter];
    int classes[DFAStatesCounter];
    vector<int> table[26];
    
    for (int j=0; j<DFAStatesCounter; j++) {
        for (int jj=0; jj<n; jj++) {
            table[jj].push_back(deltaDFA[j][jj]);
        }
    }
    for (int j=0; j<DFAStatesCounter; j++) {
        classes[j] = -1;
        for (int jj=0; jj<DFAStatesCounter; jj++)
            helpTable[j][jj] = 0;
    }
    for (int j=0; j<DFAStatesCounter; j++) {
        for (int jj=j; jj<DFAStatesCounter; jj++) {
            if(fin[j] != fin[jj]){
                helpTable[j][jj] = 1;
                pair<int, int> p(j, jj);
                Q.push(p);
            }
        }
    }
    while (!Q.empty()) {
        pair<int, int> p = Q.front(); Q.pop();
        for (int letter=0; letter<n; letter++) {
            for (int jj=0; jj<table[letter].size(); jj++) {
                if (table[letter][jj] == p.first) {
                    for (int jjj=0; jjj<table[letter].size(); jjj++) {
                        if (table[letter][jjj] == p.second) {
                            if (jj < jjj) {
                                pair<int, int> p1 (jj, jjj);
                                if (helpTable[jj][jjj] == 0)
                                    Q.push(p1);
                                helpTable[jj][jjj] = 1;
                            } else {
                                pair<int, int> p1 (jjj, jj);
                                if (helpTable[jjj][jj] == 0)
                                    Q.push(p1);
                                helpTable[jjj][jj] = 1;
                            }
                            
                        }
                    }
                }
            }
        }
    }
    int classCounter = 0;
    for (int j=0; j<DFAStatesCounter; j++) {
        if (classes[j] == -1) {
            classes[j] = classCounter;
            for (int jj=j; jj<DFAStatesCounter; jj++){
                if (helpTable[j][jj] == 0) {
                    classes[jj] = classCounter;
                }
            }
            classCounter++;
        }
    }
    int deltaMin[n][classCounter];
    int finitee[classCounter];
    for (int j=0; j<DFAStatesCounter; j++) {
        for (int jj=0; jj<n; jj++) {
            deltaMin[jj][classes[j]] = classes[table[jj][j]];
            finitee[classes[j]] = fin[j];
        }
    }
    queue<int> q;
    int odw[classCounter];
    for (int j=0; j<classCounter; j++)
        odw[j] = 0;
    q.push(0);
    odw[0] = 1;
    while(!q.empty()) {
        int a = q.front(); q.pop();
        cout<<a;
        if (finitee[a] == 1)
            cout<<"*";
        else
            cout<<":";
        for (int j=0; j<n; j++) {
            int y = deltaMin[j][a];
            cout<<y<<";";
            if (odw[y] == 0) {
                odw[y] = 1;
                q.push(y);
            }
            
        }
    }
    cout<<endl;
}
