#ifndef compiler_hpp
#define compiler_hpp
#include <iostream>
#include <vector>
#include <stack>
#include "re_parser.hpp"
#include "composable.hpp"
using namespace std;

NFAState arena[255];
int nf = 0;

NFAState* makeState(int label) {
    NFAState* ns = &arena[nf++];
    ns->label = label;
    return ns;
}


template <class T>
struct Stack : public stack<T> {
    T pop() {
        T ret = stack<T>::top();
        stack<T>::pop();
        return ret;
    }
};

int nextLabel() {
    static int label = 0;
    return label++;
}

NFA makeAtomic(char ch) {
    NFAState* ns = makeState(nextLabel());
    NFAState* ts = makeState(nextLabel());
    ns->addTransition(Transition(ch, ts));
    return NFA(ns, ts);
}

NFA makeCharClass(string ccl) {
    NFAState* ns = makeState(nextLabel());
    NFAState* ts = makeState(nextLabel());
    int i = 0; bool negate = false;
    if (ccl[0] == '^') {
        negate = true; 
        i++;
    }
    while (i < ccl.length()) {
        if (i+2 < ccl.length() && ccl[i+1] == '-') {
            char lo = ccl[i], hi = ccl[i+2];
            if (negate) {
                for (char t = 'A'; t < lo; t++)
                    ns->addTransition(Transition(t, ts));
                for (char t = hi+1; t <= '~'; t++)
                    ns->addTransition(Transition(t, ts));
                i += 2;
            } else {
                cout<<"Eh, nah."<<endl;
                for (char t = lo; t <= hi; t++)
                    ns->addTransition(Transition(t, ts));
                i+=2;
            }
        } else {
            if (negate) {
                for (char t = 'A'; t < '~'; t++) {
                    if (ccl.find(t) == std::string::npos && !ns->hasTransition(Transition(t, ts))) {
                        ns->addTransition(Transition(t, ts));
                    }
                }
            } else {
                ns->addTransition(Transition(ccl[i], ts));
            }
            i++;
        }
        cout<<i<<"/"<<ccl.length()<<endl;
    }
    return NFA(ns, ts);
}

// "The empty string"
NFA makeEpsilonAtomic() {
    NFAState* ns = makeState(nextLabel());
    NFAState* ts = makeState(nextLabel());
    ns->addTransition(Transition(ts));
    return NFA(ns, ts);
}

NFA makeConcat(NFA a, NFA b) {
    a.accept->addTransition(Transition(b.start));
    a.accept = b.accept;
    return a;
}

NFA makeAlternate(NFA a, NFA b) {
    NFAState* ns = makeState(nextLabel());
    NFAState* ts = makeState(nextLabel());
    ns->addTransition(Transition(a.start));
    ns->addTransition(Transition(b.start));
    a.accept->addTransition(Transition(ts));
    b.accept->addTransition(Transition(ts));
    return NFA(ns, ts);
}

NFA makeKleene(NFA a, bool must) {
    NFAState* ns = makeState(nextLabel());
    NFAState* ts = makeState(nextLabel());
    ns->addTransition(Transition(a.start));
    if (!must)
        ns->addTransition(Transition(ts));
    a.accept->addTransition(Transition(ts));
    a.accept->addTransition(Transition(a.start));
    return NFA(ns, ts);
}

NFA makeZeorOrOne(NFA a) {
    return makeAlternate(a, makeEpsilonAtomic());
}

class Compiler {
    private:
        Stack<NFA> st;
        void trav(astnode* node) {
            if (node != nullptr) {
                if (node->type == LITERAL) {
                    cout<<"Character State: "<<node->c<<endl;
                    st.push(makeAtomic(node->c));
                } else if (node->type == CHCLASS) {
                    st.push(makeCharClass(node->ccl));
                } else {
                    cout<<"Building: "<<node->c<<endl;
                    switch (node->c) {
                        case '|': {
                            trav(node->left);
                            trav(node->right);
                            NFA rhs = st.pop();
                            NFA lhs = st.pop();
                            st.push(makeAlternate(lhs, rhs));
                        } break;
                        case '@': {
                            trav(node->left);
                            trav(node->right);
                            NFA rhs = st.pop();
                            NFA lhs = st.pop();
                            st.push(makeConcat(lhs, rhs));
                        } break;
                        case '*': {
                            trav(node->left);
                            NFA lhs = st.pop();
                            st.push(makeKleene(lhs, false));
                        } break;
                        case '+': {
                            trav(node->left);
                            NFA lhs = st.pop();
                            st.push(makeKleene(lhs, true));
                        } break;
                        case '?': {
                            trav(node->left);
                            NFA lhs = st.pop();
                            st.push(makeZeorOrOne(lhs));
                        } break;
                        default:
                            break;
                    }
                }
            }
        }
    public:
        Compiler() {

        }
        NFA compile(astnode* node) {
            trav(node);
            return st.pop();
        }
};

NFA compile(string pattern) {
    Parser parser;
    astnode* ast = parser.parse(pattern);
    print(ast, 1);
    Compiler compiler;
    return compiler.compile(ast);
}

#endif