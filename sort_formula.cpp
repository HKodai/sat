#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
using namespace std;

bool conflict_clause(vector<int> &clause, vector<int> &allocation) {
    // clauseが矛盾していればtrue, していなければfalseを返す
    // allocation: 各変数について、真なら1, 偽なら-1, 未割り当てなら0
    for (int v : clause) {
        // いずれかの変数が矛盾していないならばclauseも矛盾していない
        if (v * allocation[abs(v)] >= 0) return false;
    }
    return true;
}

bool conflict_formula(vector<vector<int>> &formula, vector<int> &allocation) {
    // formulaが矛盾していればtrue, していなければfalseを返す
    for (vector<int> &c : formula) {
        // いずれかのclauseが矛盾していればformulaも矛盾している
        if (conflict_clause(c, allocation)) return true;
    }
    return false;
}

bool solve(vector<vector<int>> &formula, vector<int> &allocation, int depth,
           int variables) {
    // 矛盾していればバックトラック
    if (conflict_formula(formula, allocation)) return false;
    // 矛盾せず葉まで到達していれば充足可能
    if (depth == variables) {
        cout << "SAT" << endl;
        for (int i = 1; i <= variables; i++) {
            cout << i * allocation[i] << ' ';
        }
        cout << endl;
        return true;
    }
    // 変数番号の昇順に変数を選択
    int variable = depth + 1;
    // 真偽値を割り当て、再帰的に探索
    allocation[variable] = 1;
    if (solve(formula, allocation, depth + 1, variables)) return true;
    allocation[variable] = -1;
    if (solve(formula, allocation, depth + 1, variables)) return true;
    // どう割り当てても矛盾するならば、選択した変数を未割り当てに戻してバックトラック
    allocation[variable] = 0;
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
    vector<int> allocation(variables + 1, 0);
    if (!solve(formula, allocation, 0, variables)) {
        cout << "UNSAT" << endl;
    }
    auto end = chrono::system_clock::now();
    auto ellapsed = chrono::duration_cast<chrono::microseconds>(end - start);
    cout << "ellapsed time: " << ellapsed.count() << " microsec." << endl;
}