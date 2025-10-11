#include <iostream>
#include "compiler.hpp"
#include <queue>
#include <set>
#include "subset_match.hpp"
using namespace std;


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