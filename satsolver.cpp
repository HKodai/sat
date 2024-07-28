#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
using namespace std;

bool upvo(vector<vector<int>> &formula, vector<int> &assignment,
          vector<int> &new_assignment, int &assigned, int &next_variable,
          int variables) {
    // unit propagation and variable ordering

    // formula: 式全体
    // assignment: 真偽値の割り当て. i番目の要素はi番目の変数が真ならi,
    // 偽なら-i, 未割り当てなら0
    // new_assignment: 単位伝搬による新しい割り当て
    // assigned: 割り当て済みの変数の個数
    // next_variable: 次に選択する変数
    // variables: 変数の個数

    // 単位伝搬をできる限り続ける
    while (true) {
        bool updated = false;  // 単位伝搬が発生したか否か
        vector<int> unassigned_count(variables + 1,
                                     0);  // 未割り当ての変数の出現回数
        // 全てのclauseを調べる
        for (vector<int> &c : formula) {
            int conflict_count = 0;  // 矛盾している変数の個数
            int unit = 0;  // 単位伝搬により割り当てる変数
            // 全ての変数を調べる
            for (int v : c) {
                if (assignment[abs(v)] == -v || new_assignment[abs(v)] == -v) {
                    // 矛盾している
                    conflict_count++;
                } else if (!(assignment[abs(v)] == v ||
                             new_assignment[abs(v)] == v)) {
                    // 未割り当て
                    unit = v;
                    unassigned_count[abs(v)]++;
                }
            }
            if (conflict_count == c.size())
                return false;  // clauseが偽なので式全体も偽
            if (conflict_count == c.size() - 1 && unit != 0) {
                // unitの割り当てが確定する
                new_assignment[abs(unit)] = unit;
                updated = true;
            }
        }
        if (!updated) {
            // 単位伝搬後、最も多くのclauseに含まれる未割り当ての変数を次に選ぶ
            auto it =
                max_element(unassigned_count.begin(), unassigned_count.end());
            next_variable = distance(unassigned_count.begin(), it);
            break;
        }
    }
    for (int i = 1; i <= variables; i++) {
        if (new_assignment[i] != 0) {
            assignment[i] = new_assignment[i];
            assigned++;
        }
    }
    return true;
}

bool solve(vector<vector<int>> &formula, vector<int> &assignment, int &assigned,
           int variables) {
    vector<int> new_assignment(variables + 1, 0);
    // unit propagationと変数選択を行う
    // この時点で矛盾していればバックトラック
    int variable;  // 次に割り当てる変数
    if (!upvo(formula, assignment, new_assignment, assigned, variable,
              variables))
        return false;
    // 矛盾せず全ての変数が割り当てられていれば充足可能
    if (assigned == variables) {
        cout << "SAT" << endl;
        for (int i = 1; i <= variables; i++) {
            cout << assignment[i] << ' ';
        }
        cout << endl;
        return true;
    }

    // 真偽値を割り当て、再帰的に探索
    assigned++;
    assignment[variable] = variable;
    if (solve(formula, assignment, assigned, variables)) return true;
    assignment[variable] = -variable;
    if (solve(formula, assignment, assigned, variables)) return true;
    // どう割り当てても矛盾するならば、選択した変数を未割り当てに戻してバックトラック
    assignment[variable] = 0;
    assigned--;
    // unit propagationによる割り当ても取り消す
    for (int i = 1; i <= variables; i++) {
        if (new_assignment[i] != 0) {
            assignment[i] = 0;
            assigned--;
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
    vector<int> assignment(variables + 1, 0);
    int assigned = 0;
    if (!solve(formula, assignment, assigned, variables)) {
        cout << "UNSAT" << endl;
    }
    auto end = chrono::system_clock::now();
    auto ellapsed = chrono::duration_cast<chrono::microseconds>(end - start);
    cout << "ellapsed time: " << ellapsed.count() << " microsec." << endl;
}