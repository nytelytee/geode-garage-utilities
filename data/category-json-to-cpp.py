import json
import sys

def main() -> None:
    content = json.load(sys.stdin);

    print("std::unordered_map<UnlockType, std::unordered_map<std::string, std::vector<int>>> CATEGORIES = {")
    for unlock_type in content:
        print("  { UnlockType::" + unlock_type + ", {")
        for category in content[unlock_type]:
            print(("    { \"" + category + "\", { " + ', '.join(content[unlock_type][category]) + " } },").replace('{  }', '{}'))
        print("  }},")
    print("};\n")

    print("std::unordered_map<std::string, size_t> CATEGORY_TO_ORDER = {")
    for i, category in enumerate(content["Cube"]):
        print("  { " + f"\"{category}\", {i}" + " },")
    print("};\n")
    
    print("std::vector<std::string> CATEGORIES_IN_ORDER = {")
    for category in content["Cube"]:
        print(f"  \"{category}\",")
    print("};\n")

    print("std::unordered_map<UnlockType, std::unordered_map<int, std::string>> CATEGORY_FOR_ICON = {")
    for unlock_type in content:
        print("  { UnlockType::" + unlock_type + ", {")
        for category in content[unlock_type]:
            for element in content[unlock_type][category]:
                print("    { " + f"{element}, \"{category}\"" + " },")
        print("  }},")
    print("};\n")
    
    print("std::unordered_map<UnlockType, std::unordered_map<int, size_t>> CATEGORY_INDEX_FOR_ICON = {")
    for unlock_type in content:
        print("  { UnlockType::" + unlock_type + ", {")
        for category in content[unlock_type]:
            for i, element in enumerate(content[unlock_type][category]):
                print("    { " + f"{element}, {i}" + " },")
        print("  }},")
    print("};\n")


if __name__ == '__main__':
    if len(sys.argv) != 1:
        raise ValueError("takes no arguments")
    main()
