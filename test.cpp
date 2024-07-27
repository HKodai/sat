#include <algorithm>  // std::sort, std::max_element
#include <cmath>      // std::abs
#include <iostream>
#include <vector>

int main() {
    // 初期化
    std::vector<std::vector<int>> vec = {
        {3, -5, 2}, {1, 9, -4}, {6, -3, 8}, {7, 2, 0}};

    // 要素の絶対値の最大値をキーとしてソートする
    std::sort(
        vec.begin(), vec.end(),
        [](const std::vector<int>& a, const std::vector<int>& b) {
            int max_a = *std::max_element(a.begin(), a.end(), [](int x, int y) {
                return std::abs(x) < std::abs(y);
            });
            int max_b = *std::max_element(b.begin(), b.end(), [](int x, int y) {
                return std::abs(x) < std::abs(y);
            });
            return std::abs(max_a) < std::abs(max_b);
        });

    // 結果を表示
    for (const auto& v : vec) {
        for (int n : v) {
            std::cout << n << " ";
        }
        std::cout << std::endl;
    }

    return 0;
}
