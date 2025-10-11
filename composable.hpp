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
    NFAState(State st) : label(st) { }
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

void print(NFA& nfa) {
    queue<NFAState*> fq;
    set<pair<char,NFAState*>> seen;
    fq.push(nfa.start);
    seen.insert(make_pair('&',nfa.start));
    while (!fq.empty()) {
        NFAState* curr = fq.front();
        fq.pop();
        for (auto it : curr->transitions) {
            if (seen.find(make_pair(it.ch,it.dest)) == seen.end()) {
                cout<<"      "<<curr->label<<" -("<<it.ch<<")-> "<<it.dest->label<<endl;
                fq.push(it.dest);
                seen.insert(make_pair(it.ch,it.dest));
            }
        }
    }
}

#endif