#!/bin/bash

# Usage:
#   ./filter_info_tree.sh /path/to/directory
#
# 在给定目录中，对所有 skimmed_*.root：
#   - 若不存在名为 "Info" 的 TTree，则删除该文件。

DIR="$1"

if [ -z "$DIR" ] || [ ! -d "$DIR" ]; then
    echo "ERROR: Please provide a valid directory."
    echo "Usage: $0 /path/to/directory"
    exit 1
fi

for f in "$DIR"/skimmed_*.root; do
    # 如果没有匹配的文件，glob 会原样返回字符串，需要跳过
    [ -e "$f" ] || continue

    echo "Checking: $f"

    # 用 ROOT 检查：文件能否打开，且包含一个名为 Info 的 TTree
    root -l -b -q <<EOF >/dev/null 2>&1
#include <TFile.h>
#include <TTree.h>

void check_info_tree() {
    TFile *file = TFile::Open("$f");
    if (!file || file->IsZombie()) {
        gSystem->Exit(1);
    }

    TObject *obj = file->Get("Info");
    if (!obj || !obj->InheritsFrom(TTree::Class())) {
        gSystem->Exit(1);
    }

    gSystem->Exit(0);
}

check_info_tree();
EOF

    status=$?

    if [ $status -ne 0 ]; then
        echo "Removing: $f (no valid TTree \"Info\")"
        rm -f "$f"
    else
        echo "Keep: $f"
    fi
done