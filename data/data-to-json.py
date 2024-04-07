#!/usr/bin/env python3
import sys
import re
import json


def main() -> None:
    input_lines = [x.strip() for x in sys.stdin.read().split('\n')]
    if not input_lines:
        raise ValueError("empty file")
    categories_line = None
    for start_at, line in enumerate(input_lines, 1):
        if line.startswith('@'):
            categories_line = line[1:]
            break
    if categories_line is None:
        raise ValueError("a comma-separated category list must be provided, starting with @")
    categories = [x.strip().lstrip('#') for x in categories_line.split(',') if x.strip()]
    
    items = {}
    currently_selected_item = None
    currently_selected_category = None
    for line in input_lines[start_at:]:
        if not line: continue
        if line.startswith('.'):
            line_split = re.split(r'([#:])', line[1:])
            currently_selected_item = line_split[0].strip()
            if not currently_selected_item:
                raise ValueError(f"item has to have a name: {line}")
            if currently_selected_item not in items:
                items[currently_selected_item] = {x: [] for x in categories}
            line = ''.join(line_split[1:]).strip()
        if not line: continue
        if line.startswith('#'):
            line_split = line[1:].split(':', 1)
            currently_selected_category = line_split[0].strip()
            if currently_selected_category not in categories:
                raise ValueError(f"invalid category name: {line}")
            if len(line_split) == 1:
                continue
            line = f":{line_split[1]}"
        line_split = line.split(':', 1)
        if len(line_split) == 1:
            continue
        if currently_selected_category is None or currently_selected_item is None:
            raise ValueError("a category or an item has not been selected")
        elements = [x.strip() for x in line_split[1].split(',') if x.strip()]
        items[currently_selected_item][currently_selected_category].extend(elements)
    output = json.dumps(items, indent=1)
    output = re.sub('",\n *', '", ', output)
    output = re.sub(r'\[''\n *(["0-9, ]*)\n *', r'[\1', output)
    print(output)


if __name__ == '__main__':
    if len(sys.argv) != 1:
        raise ValueError("takes no arguments")
    main()
