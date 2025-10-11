#ifndef composable_hpp
#define composable_hpp
#include <vector>
#include <stack>
using namespace std;

typedef int State;

template <class T>
struct Stack : public stack<T> {
    T pop() {
        T ret = stack<T>::top();
        stack<T>::pop();
        return ret;
    }
};

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

int nextLabel() {
    static int label = 0;
    return label++;
}

NFA makeAtomic(char ch) {
    NFA nfa;
    NFAState* ns = new NFAState(nextLabel());
    NFAState* ts = new NFAState(nextLabel());
    ns->addTransition(Transition(ch, ts));
    nfa.start = ns;
    nfa.accept = ts;
    return nfa;
}

NFA makeCharClass(string ccl) {
    NFA nfa;
    NFAState* ns = new NFAState(nextLabel());
    NFAState* ts = new NFAState(nextLabel());
    for (int i = 0; i < ccl.length(); ) {
        if (i+2 < ccl.length() && ccl[i+1] == '-') {
            char lo = ccl[i];
            char hi = ccl[i+2];
            for (char t = lo; t <= hi; t++)
                ns->addTransition(Transition(t, ts));
            i+=2;
        } else {
            ns->addTransition(Transition(ccl[i], ts));
            i++;
        }
    }
    nfa.start = ns;
    nfa.accept = ts;
    return nfa;
}

// "The empty string"
NFA makeEpsilonAtomic() {
    NFA nfa;
    NFAState* ns = new NFAState(nextLabel());
    NFAState* ts = new NFAState(nextLabel());
    ns->addTransition(Transition(ts));
    nfa.start = ns;
    nfa.accept = ts;
    return nfa;
}

NFA makeConcat(NFA a, NFA b) {
    a.accept->addTransition(Transition(b.start));
    a.accept = b.accept;
    return a;
}

NFA makeAlternate(NFA a, NFA b) {
    NFA nfa;
    NFAState* ns = new NFAState(nextLabel());
    NFAState* ts = new NFAState(nextLabel());
    ns->addTransition(Transition(a.start));
    ns->addTransition(Transition(b.start));
    a.accept->addTransition(Transition(ts));
    b.accept->addTransition(Transition(ts));
    nfa.start = ns;
    nfa.accept = ts;
    return nfa;
}

NFA makeKleene(NFA a, bool must) {
    NFA nfa;
    NFAState* ns = new NFAState(nextLabel());
    NFAState* ts = new NFAState(nextLabel());
    ns->addTransition(Transition(a.start));
    if (!must)
        ns->addTransition(Transition(ts));
    a.accept->addTransition(Transition(ts));
    a.accept->addTransition(Transition(a.start));
    nfa.start = ns;
    nfa.accept = ts;
    return nfa;
}

NFA makeZeorOrOne(NFA a) {
    return makeAlternate(a, makeEpsilonAtomic());
}

class Compiler {
    private:
        Stack<NFA> st;
        NFA nfa;
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

#endif