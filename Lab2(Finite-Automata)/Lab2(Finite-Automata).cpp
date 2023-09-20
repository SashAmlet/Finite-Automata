#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <queue>
#include <string>
using namespace std;

#define NONE -999

// Structure for storing information about predecessors
struct PredecessorInfo {
    int prevState;
    char inputSymbol;
};

class FiniteAutomata {
private:
    vector<char> inputSymbols;
    vector<int> states;
    int initialState;
    int currentState;
    vector<int> finalStates;
    unordered_map<int, unordered_map<char, int>> transitionFunction;
public:
    bool readAutomataFromFile(const string& filename);
    // Method for processing input characters and changing state
    bool processInput(char input);
    // Function to perform breadth first search
    vector<PredecessorInfo> bfsSearch();

    int getCurrentState();
};

#pragma region Helper

template<typename T>
bool contains(const vector<T>& vec, const T& value) {
    return find(vec.begin(), vec.end(), value) != vec.end();
}

#pragma endregion



int FiniteAutomata::getCurrentState() { return this->currentState; }

bool FiniteAutomata::readAutomataFromFile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        return false; // Error opening file
    }

    string line;
    bool insideTransitionFunction = false; // Flag to determine if we are inside a TransitionFunction
    stringstream transitionBuffer; // Buffer for concatenating strings inside TransitionFunction

    while (getline(file, line)) {
        istringstream iss(line);
        string key;

        if (iss >> key) {
 #pragma region reading: InputSymbols, StatesOfAutomata, InitialState, FinalStates
            if (key == "InputSymbols=") {
                char symbol;
                while (iss >> symbol) {
                    if (isalpha(symbol)) {
                        this->inputSymbols.push_back(symbol);
                    }
                }
            }
            else if (key == "StatesOfAutomata=") {
                char state;
                while (iss >> state) {
                    if (isalnum(state)) {
                        this->states.push_back(state - '0');
                    }
                }
            }
            else if (key == "InitialState=") {
                char initState;
                while (iss >> initState) {
                    if (isalnum(initState)) {
                        this->initialState = initState - '0';
                        this->currentState = this->initialState;
                    }
                }
            }
            else if (key == "FinalStates=") {
                char finalState;
                while (iss >> finalState) {
                    if (isalnum(finalState)) {
                        this->finalStates.push_back(finalState - '0');
                    }
                }
            }
#pragma endregion
#pragma region reading: TransitionFunction
            else if (key == "TransitionFunction=") {
                insideTransitionFunction = true;
                transitionBuffer.str(""); // Очищаем буфер
            }
            else if (insideTransitionFunction && file.peek() == EOF) {
                insideTransitionFunction = false;

                int fromState;
                char symbol;
                int toState;

                while (transitionBuffer >> fromState >> symbol >> toState) {
                    this->transitionFunction[fromState][symbol] = toState;
                }
            }
            else if (insideTransitionFunction) {
                // Если мы внутри TransitionFunction, добавляем строку в буфер
                transitionBuffer << line << " ";
            }
#pragma endregion
        }
    }

    file.close();
    return true;
}

bool FiniteAutomata::processInput(char input) {
    // Checking the presence of an input character in the alphabet
    if (!contains(this->inputSymbols, input)) /*find(this->inputSymbols.begin(), this->inputSymbols.end(), input) == this->inputSymbols.end()*/
    {
        cout << "There are no '" << input << "' symbols in the input symbol set." << endl;
        return false;
    }
    // Checking for the presence of keys before extracting from the switching function
    if (this->transitionFunction.find(this->currentState) == this->transitionFunction.end() ||
        this->transitionFunction[this->currentState].find(input) == this->transitionFunction[this->currentState].end()) {
        cout << "The transition for the current state '" << this->currentState <<"' with input symbol '" << input << "' is not defined." << endl;
        return false;
    }
    int prevState = this->currentState;
    this->currentState = this->transitionFunction[currentState][input];

    cout << prevState << ' ' << input << ' ' << this->currentState << endl;
    return true;
}

vector<PredecessorInfo> FiniteAutomata::bfsSearch() {
    queue<int> statesQueue;
    vector<int> visitedStates;
    unordered_map<int, PredecessorInfo> predecessors; // Stores information about predecessors
    int finaleState = NONE, startState = this->currentState;

    statesQueue.push(this->currentState);

    while (!statesQueue.empty()) {
        this->currentState = statesQueue.front();
        statesQueue.pop();

        if (contains(this->finalStates, this->currentState)) /*find(this->finalStates.begin(), this->finalStates.end(), this->currentState) != this->finalStates.end()*/
        {
            // Final state reached
            finaleState = this->currentState;
            break;
        }

        visitedStates.push_back(currentState);

        // Enumerating all symbols in the function of transitions from the current state
        for (const auto& transition : this->transitionFunction[currentState]) {
            char inputSymbol = transition.first;
            int nextState = transition.second;

            if (!contains(visitedStates, nextState)) {
                // The following state has not been visited
                statesQueue.push(nextState);
                // Saving information about the predecessor
                predecessors[nextState] = { currentState, inputSymbol };
            }
        }
    }

    // Restoring the path from endState to startState
    vector<PredecessorInfo> path;
    if (finaleState != NONE)
    {
        PredecessorInfo finale = { finaleState, '.' };
        path.push_back(finale);
        while (finaleState != startState) {
            path.push_back(predecessors[finaleState]);
            finaleState = predecessors[finaleState].prevState;
        }

        reverse(path.begin(), path.end()); // Reverse the path so that it starts with startState
    }

    return path;
}

int main() {
    FiniteAutomata automata;
    string inputStr;
    cin >> inputStr;
    const vector<char> w0(inputStr.begin(), inputStr.end());

    if (automata.readAutomataFromFile("test2.txt")) {
        for (char symbol : w0)
        {
            if (!automata.processInput(symbol)) { return 0; }
        }
        cout << "------------------------------------------" << endl;

        vector<PredecessorInfo> path = automata.bfsSearch();

        if (!path.empty()) {
            for (const auto& info : path) {
                cout << info.prevState << endl << info.prevState << " " << info.inputSymbol << " ";
            }
        }
        else {
            std::cout << "The path from state " << automata.getCurrentState() << " to the final state was not found." << std::endl;
        }
    }
    else {
        cout << "Error reading the automata file." << endl;
    }

    return 0;
}
