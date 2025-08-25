import json
import sys


def main() -> None:
    text = sys.stdin.read()
    content = json.loads(text);
    
    violations = []
    for item in content:
        for bucket in content[item]:
            content[item][bucket] = set(content[item][bucket])
        for bucket1 in content[item]:
            for bucket2 in content[item]:
                if bucket1 == bucket2:
                    continue
                if (violation := content[item][bucket1] & content[item][bucket2]):
                    violations.append((item, bucket1, bucket2, violation))
    if violations:
        raise ValueError('\n'.join([str(violation) for violation in violations]))

    print(text)




if __name__ == '__main__':
    if len(sys.argv) != 1:
        raise ValueError("takes no arguments")
    main()
