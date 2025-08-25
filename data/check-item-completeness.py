import json
import sys

ICON_SET_FOR_TYPE = {
    "Cube": {str(i) for i in range(1, 485+1)},
    "Ship": {str(i) for i in range(1, 169+1)},
    "Ball": {str(i) for i in range(1, 118+1)},
    "Bird": {str(i) for i in range(1, 149+1)},
    "Dart": {str(i) for i in range(1, 96+1)},
    "Robot": {str(i) for i in range(1, 68+1)},
    "Spider": {str(i) for i in range(1, 69+1)},
    "Swing": {str(i) for i in range(1, 43+1)},
    "Jetpack": {str(i) for i in range(1, 8+1)},
}


def main() -> None:
    text = sys.stdin.read()
    content = json.loads(text);
    
    violations = []
    for item in content:
        bucket_sum = set()
        for bucket in content[item]:
            bucket_sum |= set(content[item][bucket])
        if (violation := bucket_sum - ICON_SET_FOR_TYPE[item]):
            violations.append(("Extra", item, violation))
        if (violation := ICON_SET_FOR_TYPE[item] - bucket_sum):
            violations.append(("Missing", item,  violation))
    if violations:
        raise ValueError('\n'.join([str(violation) for violation in violations]))

    print(text)




if __name__ == '__main__':
    if len(sys.argv) != 1:
        raise ValueError("takes no arguments")
    main()
