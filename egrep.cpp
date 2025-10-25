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
            cout<<"Match: "<<buffer<<endl;
        }
    }
}

void sayHello() {
    cout<<" mgc-egrep v2.0.0-pre"<<endl;
    cout<<" (c) 2025 MaxGCoding.com\n"<<endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        sayHello();
        return 0;
    }
    fromStdIn(argv[1]);
    return 0;
}