#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <queue>
using namespace std;

// ��������� ��� �������� ���������� � ����������������
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
    // ����� ��� ��������� ������� �������� � ��������� ���������
    bool processInput(char input);
    // ������� ��� ���������� ������ � ������
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
        return false; // ������ ��� �������� �����
    }

    string line;
    bool insideTransitionFunction = false; // ���� ��� �����������, ��������� �� ������ TransitionFunction
    stringstream transitionBuffer; // ����� ��� ����������� ����� ������ TransitionFunction

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
                transitionBuffer.str(""); // ������� �����
            }
            else if (insideTransitionFunction && file.peek() == EOF) {
                insideTransitionFunction = false;
                string transitionText = transitionBuffer.str();
                istringstream transitionLine(transitionText);

                int fromState;
                char symbol;
                int toState;

                while (transitionLine >> fromState >> symbol >> toState) {
                    this->transitionFunction[fromState][symbol] = toState;
                }
            }
            else if (insideTransitionFunction) {
                // ���� �� ������ TransitionFunction, ��������� ������ � �����
                transitionBuffer << line << " ";
            }
#pragma endregion
        }
    }

    file.close();
    return true;
}

bool FiniteAutomata::processInput(char input) {
    // �������� ������� �������� ������� � ��������
    if (!contains(this->inputSymbols, input)) /*find(this->inputSymbols.begin(), this->inputSymbols.end(), input) == this->inputSymbols.end()*/
    {
        cout << "There are no '" << input << "' symbols in the input symbol set." << endl;
        return false;
    }
    // ��������� ������� ������ ����� ����������� �� ������� ��������
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
    unordered_map<int, PredecessorInfo> predecessors; // ������ ���������� � ����������������
    int finaleState = this->currentState, startState = this->currentState;

    statesQueue.push(this->currentState);

    while (!statesQueue.empty()) {
        this->currentState = statesQueue.front();
        statesQueue.pop();

        if (contains(this->finalStates, this->currentState)) /*find(this->finalStates.begin(), this->finalStates.end(), this->currentState) != this->finalStates.end()*/
        {
            // ���������� ��������� ���������
            finaleState = this->currentState;
            break;
        }

        visitedStates.push_back(currentState);

        // ������� ���� �������� � ������� ��������� �� �������� ���������
        for (const auto& transition : this->transitionFunction[currentState]) {
            char inputSymbol = transition.first;
            int nextState = transition.second;

            if (!contains(visitedStates, nextState)) {
                // ��������� ��������� �� ���� ��������
                statesQueue.push(nextState);
                // ��������� ���������� � ���������������
                predecessors[nextState] = { currentState, inputSymbol };
            }
        }
    }

    // ��������������� ���� �� endState � startState
    vector<PredecessorInfo> path;
    int currentState = finaleState;
    while (currentState != startState) {
        path.push_back(predecessors[currentState]);
        currentState = predecessors[currentState].prevState;
    }

    reverse(path.begin(), path.end()); // �������������� ����, ����� �� ��������� � startState

    return path;
}



int main() {
    FiniteAutomata automata;
    string inputStr;
    cin >> inputStr;
    const vector<char> w0(inputStr.begin(), inputStr.end());

    if (automata.readAutomataFromFile("test.txt")) {
        for (char symbol : w0)
        {
            if (!automata.processInput(symbol)) { return 0; }
        }
        cout << "--------------------------------------------" << endl;

        vector<PredecessorInfo> path = automata.bfsSearch();

        if (!path.empty()) {
            for (const auto& info : path) {
                cout << info.prevState << endl << info.prevState << " " << info.inputSymbol << " ";
            }
        }
        else {
            std::cout << "���� �� ��������� " << automata.getCurrentState() << " �� ��������� ��������� �� ������." << std::endl;
        }
    }
    else {
        cout << "Error reading the automata file." << endl;
    }

    return 0;
}