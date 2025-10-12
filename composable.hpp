#ifndef composable_hpp
#define composable_hpp
#include <vector>
#include <queue>
#include <set>
using namespace std;

typedef int State;

struct NFAState;

struct Transition {
    char ch;
    bool is_epsilon;
    NFAState* dest;
    Transition(NFAState* d) : ch('&'), dest(d), is_epsilon(true) { }
    Transition(char c, NFAState* d) : ch(c), dest(d), is_epsilon(false) { }
    Transition() {
        is_epsilon = false;
        dest = nullptr;
    }
};

struct NFAState {
    State label;
    vector<Transition> transitions;
    NFAState(State st = -1) : label(st) { }
    ~NFAState() {    }
    void addTransition(Transition t) {
        transitions.push_back(t);
    }
};

struct NFA {
    NFAState* start;
    NFAState* accept;
    NFA() {

    }
};

void printR(NFA& nfa, NFAState* curr, set<pair<char, NFAState*>>& seen, int d) {
    if (curr == nfa.accept) {
        for (int i = 0; i < d; i++) cout<<" ";
        cout<<"(accept)"<<endl;
    }
    for (auto it : curr->transitions) {
        for (int i = 0; i < d; i++) cout<<" ";
        cout<<curr->label<<" -("<<it.ch<<")-> "<<it.dest->label<<endl;
        if (seen.find(make_pair(it.ch,it.dest)) == seen.end()) {
            seen.insert(make_pair(it.ch,it.dest));
            printR(nfa, it.dest, seen, d+1);
        }
    }
} 

void print(NFA& nfa) {
    set<pair<char,NFAState*>> seen;
    printR(nfa, nfa.start, seen, 1);
}

void BFS(NFA& nfa) {
    queue<pair<int,NFAState*>> fq;
    set<pair<char,NFAState*>> seen;
    seen.clear();
    fq.push(make_pair(0,nfa.start));
    seen.insert(make_pair('&',nfa.start));
    while (!fq.empty()) {
        NFAState* curr = fq.front().second;
        int depth = fq.front().first;
        fq.pop();
        for (auto it : curr->transitions) {
            for (int i = 0; i < depth; i++) cout<<" ";
            cout<<"      "<<curr->label<<" -("<<it.ch<<")-> "<<it.dest->label<<endl;
            if (seen.find(make_pair(it.ch,it.dest)) == seen.end()) {
                fq.push(make_pair(depth+1,it.dest));
                seen.insert(make_pair(it.ch,it.dest));
            }
        }
    }
}

#endif