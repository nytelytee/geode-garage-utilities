import json
import sys

VALID_AUTHORS = [
    "0", "332", "1055", "2102", "2829", "2838", "24316", "26994",
    "92744", "101834", "119501", "127035", "135919", "137794",
    "141781", "155254", "252302", "272294", "328441", "538396",
    "617383", "756232", "820393", "893112", "992231", "1093804",
    "1111063", "1275405", "1290979", "1301742", "1449955", "1502678",
    "1541587", "1584303", "1632845", "1678677", "1788388", "2117373",
    "2168397", "2317322", "2565154", "2626257", "2678947", "2816040",
    "3006276", "3061610", "3407871", "3676830", "3768957", "3828686",
    "4119239", "4622865", "4669292", "4861064", "4956388", "4964707",
    "5185839", "5363076", "5434390", "5458141", "5477818", "5568086",
    "5737229", "5794524", "5935068", "6046266", "6051650", "6109127",
    "6615856", "6667073", "6950679", "7395563", "7725202", "8385005",
    "8840229", "9238889", "9431154", "10470991", "11334262", "12285856",
    "13296775", "14023980", "-1", "-2", "-3"
]



def main() -> None:
    text = sys.stdin.read()
    content = json.loads(text);
    new_content = {}
    
    violations = []
    for item in content:
        new_content[item] = {}
        for bucket in content[item]:
            if bucket not in VALID_AUTHORS:
                violations.append((item, bucket))
        for bucket in VALID_AUTHORS:
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
