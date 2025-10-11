#include <iostream>
#include "re_parser.hpp"
#include "composable.hpp"
#include <queue>
#include <set>
using namespace std;

NFA compile(string pattern) {
    Parser parser;
    astnode* ast = parser.parse(pattern);
    print(ast, 1);
    Compiler compiler;
    return compiler.compile(ast);
}

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

void fromStdIn(string pattern) {
    NFA nfa = compile(pattern);
    char buffer[1024];
    print(nfa);
    while (fgets(buffer, 1014, stdin)) {
        if (match(nfa, buffer)) {
            cout<<buffer<<endl;
        }
    }
}

int main(int argc, char* argv[]) {
    fromStdIn(argv[1]);
    return 0;
}