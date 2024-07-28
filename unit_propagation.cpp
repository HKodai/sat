#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <vector>
using namespace std;

bool unit_propagation(vector<vector<int>> &formula, set<int> &unassigned,
                      vector<int> &assigned, vector<int> &new_assigned,
                      int variables) {
    bool updated;
    do {
        updated = false;
        for (vector<int> &c : formula) {
            int conflict_count = 0;
            int unit = 0;
            for (int v : c) {
                if (assigned[abs(v)] == -v || new_assigned[abs(v)] == -v) {
                    conflict_count++;
                } else if (!(assigned[abs(v)] == v ||
                             new_assigned[abs(v)] == v)) {
                    unit = v;
                }
            }
            if (conflict_count == c.size()) return false;
            if (conflict_count == c.size() - 1 && unit != 0) {
                new_assigned[abs(unit)] = unit;
                updated = true;
            }
        }
    } while (updated);
    for (int i = 1; i <= variables; i++) {
        if (new_assigned[i] != 0) {
            unassigned.erase(i);
            assigned[i] = new_assigned[i];
        }
    }
    return true;
}

bool solve(vector<vector<int>> &formula, set<int> &unassigned,
           vector<int> &assigned, int variables) {
    vector<int> new_assigned(variables + 1, 0);
    // unit propagationを行う
    // この時点で矛盾していればバックトラック
    if (!unit_propagation(formula, unassigned, assigned, new_assigned,
                          variables))
        return false;
    // 矛盾せず全ての変数が割り当てられていれば充足可能
    if (unassigned.size() == 0) {
        cout << "SAT" << endl;
        for (int i = 1; i <= variables; i++) {
            cout << assigned[i] << ' ';
        }
        cout << endl;
        return true;
    }
    // 変数番号の昇順に変数を選択
    int variable = *unassigned.begin();
    unassigned.erase(variable);
    // 真偽値を割り当て、再帰的に探索
    assigned[variable] = variable;
    if (solve(formula, unassigned, assigned, variables)) return true;
    assigned[variable] = -variable;
    if (solve(formula, unassigned, assigned, variables)) return true;
    // どう割り当てても矛盾するならば、選択した変数を未割り当てに戻してバックトラック
    assigned[variable] = 0;
    unassigned.insert(variable);
    // unit_propagationによる割り当ても取り消す
    for (int i = 1; i <= variables; i++) {
        if (new_assigned[i] != 0) {
            assigned[i] = 0;
            unassigned.insert(i);
        }
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
    vector<int> assigned(variables + 1, 0);
    if (!solve(formula, unassigned, assigned, variables)) {
        cout << "UNSAT" << endl;
    }
    auto end = chrono::system_clock::now();
    auto ellapsed = chrono::duration_cast<chrono::microseconds>(end - start);
    cout << "ellapsed time: " << ellapsed.count() << " microsec." << endl;
}