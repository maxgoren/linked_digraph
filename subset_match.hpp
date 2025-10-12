#ifndef subset_match_hpp
#define subset_match_hpp
#include <iostream>
#include "compiler.hpp"
#include "composable.hpp"
#include <set>
using namespace std;

set<NFAState*> move(NFA nfa, char ch, set<NFAState*> states) {
    cout<<"Move on: "<<ch<<endl;
    set<NFAState*> next;
    for (NFAState* state : states) {
        for (Transition t : state->transitions) {
            if (t.ch == ch || t.ch == '.') {
                if (!t.is_epsilon && next.find(t.dest) == next.end()) {
                    cout<<"\t"<<state->label<<"-("<<t.ch<<")-> "<<t.dest->label<<endl;
                    next.insert(t.dest);
                }
            }
        }
    }
    cout<<"-------------"<<endl;
    return next;
}

set<NFAState*> e_closure(NFA nfa, set<NFAState*> states) {
    set<NFAState*> next = states;
    Stack<NFAState*> st;
    cout<<"Epsilon closure from: ";
    for (auto s : states) {
        cout<<s->label<<" ";
        st.push(s);
    }
    cout<<endl;
    while (!st.empty()) {
        NFAState* curr = st.pop();
        for (auto t : curr->transitions) {
            if (t.is_epsilon && next.find(t.dest) == next.end()) {
                cout<<"\t"<<curr->label<<"-("<<t.ch<<")-> "<<t.dest->label<<endl;
                next.insert(t.dest);
                st.push(t.dest);
            }
        }
    }
    cout<<"---------------------------"<<endl;
    return next;
}

bool match(NFA& nfa, string text) {
    set<NFAState*> states;
    states.insert(nfa.start);
    states = e_closure(nfa, states);
    for (char c : text) {
        states = e_closure(nfa, move(nfa, c, states));
        if (states.find(nfa.accept) != states.end())
            return true;
    }
    return states.find(nfa.accept) != states.end();
}

#endif