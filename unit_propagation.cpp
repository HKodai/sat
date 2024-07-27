#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <unordered_set>
#include <vector>
using namespace std;

bool unit_propagation(vector<vector<int>> &formula, set<int> &unassigned,
                      unordered_set<int> &assigned,
                      unordered_set<int> &new_assigned) {
    bool updated;
    do {
        updated = false;
        for (vector<int> &c : formula) {
            int conflict_count = 0;
            int unit = 0;
            for (int v : c) {
                if (assigned.count(-v) || new_assigned.count(-v)) {
                    conflict_count++;
                } else if (!(assigned.count(v) || new_assigned.count(v))) {
                    unit = v;
                }
            }
            if (conflict_count == c.size()) return false;
            if (conflict_count == c.size() - 1 && unit != 0) {
                new_assigned.insert(unit);
                updated = true;
            }
        }
    } while (updated);
    for (int u : new_assigned) {
        unassigned.erase(abs(u));
        assigned.insert(u);
    }
    return true;
}

bool solve(vector<vector<int>> &formula, set<int> &unassigned,
           unordered_set<int> &assigned, int variables) {
    unordered_set<int> new_assigned;
    // unit propagationを行う
    // この時点で矛盾していればバックトラック
    if (!unit_propagation(formula, unassigned, assigned, new_assigned))
        return false;
    // 矛盾せず全ての変数が割り当てられていれば充足可能
    if (assigned.size() == variables) {
        cout << "SAT" << endl;
        vector<int> solution;
        for (auto v : assigned) {
            solution.push_back(v);
        }
        sort(solution.begin(), solution.end(),
             [](int a, int b) { return abs(a) < abs(b); });
        for (auto v : solution) {
            cout << v << ' ';
        }
        cout << endl;
        return true;
    }
    // 変数番号の昇順に変数を選択
    int variable = *unassigned.begin();
    unassigned.erase(variable);
    // 真偽値を割り当て、再帰的に探索
    assigned.insert(variable);
    if (solve(formula, unassigned, assigned, variables)) return true;
    assigned.erase(variable);
    assigned.insert(-variable);
    if (solve(formula, unassigned, assigned, variables)) return true;
    // どう割り当てても矛盾するならば、選択した変数を未割り当てに戻してバックトラック
    assigned.erase(-variable);
    unassigned.insert(variable);
    // unit_propagationによる割り当ても取り消す
    for (int v : new_assigned) {
        assigned.erase(v);
        unassigned.insert(abs(v));
    }
    return false;
}

int main(int argc, char *argv[]) {
    // DIMACS CNF file formatの読み込み
    ifstream ifs(argv[1]);
    char str[BUFSIZ];
    do {
        ifs.getline(str, BUFSIZ);
    } while (str[0] == 'c');
    stringstream ss(str);
    int variables, clauses;
    ss.ignore(6);
    ss >> variables >> clauses;
    vector<vector<int>> formula(clauses);
    for (int i = 0; i < clauses; i++) {
        ifs.getline(str, BUFSIZ);
        stringstream ss(str);
        vector<int> clause;
        int variable;
        while (ss >> variable) {
            if (variable) clause.push_back(variable);
        }
        formula[i] = clause;
    }

    // 実行時間を計測
    auto start = chrono::system_clock::now();
    // 矛盾の判定を効率化するため、clauseの要素の絶対値の最大値をキーとしてformulaをソートする
    sort(formula.begin(), formula.end(),
         [](const vector<int> &a, const vector<int> &b) {
             int max_a = *max_element(a.begin(), a.end(), [](int x, int y) {
                 return abs(x) < abs(y);
             });
             int max_b = *max_element(b.begin(), b.end(), [](int x, int y) {
                 return abs(x) < abs(y);
             });
             return abs(max_a) < abs(max_b);
         });
    set<int> unassigned;
    for (int i = 1; i <= variables; i++) {
        unassigned.insert(i);
    }
    unordered_set<int> assigned;
    if (!solve(formula, unassigned, assigned, variables)) {
        cout << "UNSAT" << endl;
    }
    auto end = chrono::system_clock::now();
    auto ellapsed = chrono::duration_cast<chrono::microseconds>(end - start);
    cout << "ellapsed time: " << ellapsed.count() << " microsec." << endl;
}