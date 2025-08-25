import json
import sys

def main() -> None:
    content = json.load(sys.stdin);

    print("std::unordered_map<UnlockType, std::unordered_map<int, std::vector<int>>> AUTHORS = {")
    for i, unlock_type in enumerate(content):
        comma1 = (i != len(content) - 1) * ','
        comma1 = ','
        print("  { UnlockType::" + unlock_type + ", {")
        for j, author in enumerate(content[unlock_type]):
            comma2 = (j != len(content[unlock_type]) - 1) * ','
            comma2 = ','
            print(("    { " + author + ", { " + ', '.join(content[unlock_type][author]) + " } }" + comma2).replace('{  }', '{}'))
        print("  }}" + comma1)
    print("};\n")

    print("std::unordered_map<int, size_t> AUTHOR_TO_ORDER = {")
    for i, author in enumerate(content["Cube"]):
        #comma = (i != len(content["Cube"]) - 1) * ','
        #comma = ','
        print("{ " + f"{author}, {i}" + " },") #+ comma)
    print("};\n")
    
    print("std::vector<int> AUTHORS_IN_ORDER = {")
    for i, author in enumerate(content["Cube"]):
        #comma = (i != len(content["Cube"]) - 1) * ','
        #comma = ','
        print(f"{author},")
    print("};\n")
    
    print("std::unordered_map<UnlockType, std::unordered_map<int, int>> AUTHOR_FOR_ICON = {")
    for unlock_type in content:
        print("  { UnlockType::" + unlock_type + ", {")
        for author in content[unlock_type]:
            for element in content[unlock_type][author]:
                print("    { " + f"{element}, {author}" + " },")
        print("  }},")
    print("};\n")

if __name__ == '__main__':
    if len(sys.argv) != 1:
        raise ValueError("takes no arguments")
    main()
