#!/bin/bash

# コマンドライン引数から文字列を取得
prefix="$1"

# 開始時間を記録
start_time=$(date +%s%3N)

for i in {1..10}
do
    formatted="${prefix}${i}.cnf"
    ./vo "$formatted"
done

# 終了時間を記録
end_time=$(date +%s%3N)

# 実行時間を計算
execution_time=$((end_time - start_time))

# 実行時間を表示
echo "Total execution time: ${execution_time} milliseconds"
