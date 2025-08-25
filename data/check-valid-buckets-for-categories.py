import json
import sys

VALID_CATEGORIES = [
    "Default Icons", "RobTop Levels", "User Levels", "Statistics",
    "Shards", "Paths", "Map Packs", "Gauntlets", "List Rewards",
    "Daily Levels", "Creator", "Social", "Main Menu", "Vaults",
    "Shop", "Secret Shop", "Community Shop", "Diamond Shop",
    "Mechanic", "1 Key Chest", "5 Key Chest", "10 Key Chest",
    "25 Key Chest", "50 Key Chest", "100 Key Chest",
    "Gold Key Chest", "Wraith Chest", "Special Chest",
    "Supporter", "2.21"
]


def main() -> None:
    text = sys.stdin.read()
    content = json.loads(text);
    new_content = {}
    
    violations = []
    for item in content:
        new_content[item] = {}
        for bucket in content[item]:
            if bucket not in VALID_CATEGORIES:
                violations.append((item, bucket))
        for bucket in VALID_CATEGORIES:
            if bucket not in content[item]:
                new_content[item][bucket] = []
            else:
                new_content[item][bucket] = content[item][bucket]
    if violations:
        raise ValueError('\n'.join([str(violation) for violation in violations]))

    print(json.dumps(new_content))




if __name__ == '__main__':
    if len(sys.argv) != 1:
        raise ValueError("takes no arguments")
    main()
