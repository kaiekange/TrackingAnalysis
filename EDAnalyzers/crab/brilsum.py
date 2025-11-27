import sys

# 1. 先把每个 era 对应的 (L, R) 记在一个字典里
ERA_LR = {
    "2022B": (355100, 355769),
    "2022C": (355862, 357482),
    "2022D": (357538, 357900),

    "2022preEE": (355100, 357900),

    "2022E": (359356, 360327),

    "2023C": (367095, 368823),
    "2023D": (369869, 370790),

    "2024C": (379415, 380238),
    "2024D": (380306, 380947),
    "2024E": (380963, 381594),
    "2024F": (382209, 383779),
    "2024G": (383811, 385801),
    "2024H": (385836, 386478),
    "2024I": (386505, 387721),

    "2024": (379415, 387721)
}

def load_rows(txt_path):
    rows = []
    with open(txt_path) as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith("#"):
                continue

            parts = line.split(",")
            run = int(parts[0].split(":")[0])
            recorded = float(parts[-1])

            rows.append((run, recorded))

    rows.sort(key=lambda x: x[0])
    return rows


def effective_sum(rows, L, R):
    left_run = None
    for run, _ in rows:
        if run >= L:
            left_run = run
            break

    right_run = None
    for run, _ in reversed(rows):
        if run <= R:
            right_run = run
            break

    if left_run is None or right_run is None or left_run > right_run:
        return None, None, None

    total = sum(rec for run, rec in rows if left_run <= run <= right_run)
    return total, left_run, right_run


def lumi_for_era(rows, era):
    if era not in ERA_LR:
        raise ValueError(f"Unknown era: {era}")

    L, R = ERA_LR[era]
    total, left_run, right_run = effective_sum(rows, L, R)

    if total is None:
        print(f"[{era}] no runs found between {L} and {R}")
        return None

    # print(
    #     f"[{era}] requested {L}-{R}, "
    #     f"effective {left_run}-{right_run}, total recorded = {total}"
    # )
    print(total)

    return total

def main():
    # 用法：python script.py lumi.txt 2022B
    # trig = sys.argv[1]
    # era = sys.argv[2]

    # trigs = ["180", "250", "370", "430", "510", "590", "680", "780", "890", "1050"]
    trigs = ["1050", "890", "780", "680", "590", "510", "430", "370", "250", "180"]

    eras = ["2022preEE", "2022E", "2023C", "2023D", "2024"]

    for era in eras:
        print(f"PFHT{era}:")
        for trig in trigs:
        
            year = int(era[:4])

            txt_path = f"./LUMI/lumi_{year}_PFHT{trig}.txt"
            rows = load_rows(txt_path)
            lumi_for_era(rows, era)



if __name__ == "__main__":
    main()