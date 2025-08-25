#!/usr/bin/env python3
import sys
import re
import json

"""
i have no idea why i documented this, nobody is ever going to see it

syntax:
    the syntax consists of what are called "items", "buckets", and "elements"
    an item contains zero or more buckets; a bucket contains zero or more elements
    all items, buckets, and elements are stripped of whitespace
    the following lines are determined like this:
        1. if a line starts with a "."
            the text after that ".", up until either a "#", a ":", or a line ending, is classified as an item;
            that item is selected
            then, if the line has a "#" or ":", the line is parsed further, see 2.
        2. if a line starts with a "#"
            the text after that "#", up until either a ":" or a line ending, is classified as a bucket;
            the bucket is selected
            then, if the line has a ":", the line including the ":" is parsed further, see 3.
        3. if a line contains a ":"
            the text before that ":" is ignored, and what comes after is treated as a comma separated list of elements;
            the elements are added to the currently selected item and the currently selected bucket;
            if there are no elements, then nothing is added
        4. otherwise, a line is ignored completely
    
    this syntax is made loosely so you can leave comments for what elements are easily; examples of a valid files:
        
    
    example 1
    .Item 1 # Bucket A : Element 1, Element 2, Element 3
            # Bucket B : Element 4, Element 5, Element 6
            # Bucket C : Element 7, Element 8, Element 9
    .Item 2 # Bucket A : Element 10, Element 11, Element 12
            # Bucket B : Element 13, Element 14, Element 15
            # Bucket C : Element 16, Element 17, Element 18

    the resulting json is:

    {
        "Item 1": {
            "Bucket A": ["Element 1", "Element 2", "Element 3"],
            "Bucket B": ["Element 4", "Element 5", "Element 6"],
            "Bucket C": ["Element 7", "Element 8", "Element 9"]
        },
        "Item 2": {
            "Bucket A": ["Element 10", "Element 11", "Element 12"],
            "Bucket B": ["Element 13", "Element 14", "Element 15"],
            "Bucket C": ["Element 16", "Element 17", "Element 18"]
        }
    }

    explaination:

    select item 1, select bucket A, add the elements to item 1[bucket a]
    select bucket b (item 1 is still selected), add the elements to item 1[bucket b]
    select bucket c (item 1 is still selected), add the elements to item 1[bucket c]
    select item 2 (bucket c is still selected at this point, but that fact is unused),
    etc.



    example 2 (small excerpt from category data)
    .Cube
        #Statistics
          attempts: 341, 407, 368
          jumps: 371, 365
          stars: 23, 24, 25, 26, 28, 29, 30, 61, 82, 100, 83
          moons: 207, 158, 199, 268, 457
          diamonds: 87, 101, 159, 319, 226
          secret coins: 31, 32, 34, 38, 43, 56, 88
          user coins: 49, 53, 54, 52, 99, 93, 325, 284
          insanes: 201, 266, 174, 277, 481
          demons: 19, 20, 21, 22, 37, 446, 270, 308, 459
        #Supporter: 13, 71
    .Ship
        #Statistics
          attempts: 161, 65
          stars: 4, 5, 7, 18, 19, 30, 122
          moons: 70, 105
          diamonds: 32, 103
          secret coins: 14, 12, 16, 24, 88
          user coins: 25, 23, 21, 22, 33, 134
          insane: 119
          demons: 3, 6, 8, 35, 123

    the resulting json is:

    {
     "Cube": {
      "Statistics": ["341", "407", "368", "371", "365", "23", "24", "25", "26", "28", "29",
                     "30", "61", "82", "100", "83", "207", "158", "199", "268", "457", "87",
                     "101", "159", "319", "226", "31", "32", "34", "38", "43", "56", "88", "49",
                     "53", "54", "52", "99", "93", "325", "284", "201", "266", "174", "277", "481",
                     "19", "20", "21", "22", "37", "446", "270", "308", "459"],
      "Supporter": ["13", "71"]
     },
     "Ship": {
      "Statistics": ["161", "65", "4", "5", "7", "18", "19", "30", "122", "70", "105", "32", "103",
                     "14", "12", "16", "24", "88", "25", "23", "21", "22", "33", "134", "119", "3",
                     "6", "8", "35", "123"],
      "Supporter": []
     }
    }
    
    explaination:
    the explainations for what elements are (e.g. attempts, stars, jumps) have been ignored,
    they are treated as comments, because they come before the :

    this was made so you can easily handwrite files for how icons in the icon kit should be categorized,
    this script turns it into json, the next script after that turns that json into c++ code
    writing out massive c++ maps, or even json, by hand is tedious,
    so i just made this thing really quickly to be human readable
    (hence lots of ignoring of text to enable comments) and easy to write and transpile into c++

"""

def main() -> None:
    input_lines = [x.strip() for x in sys.stdin.read().split('\n')]
    if not input_lines:
        raise ValueError("empty file")
    
    items = {}
    currently_selected_item = None
    currently_selected_bucket = None
    for line in input_lines:
        if not line: continue
        if line.startswith('.'):
            line_split = re.split(r'([#:])', line[1:])
            currently_selected_item = line_split[0].strip()
            if not currently_selected_item:
                raise ValueError(f"item has to have a name: {line}")
            if currently_selected_item not in items:
                items[currently_selected_item] = {}
            line = ''.join(line_split[1:]).strip()
        if not line: continue
        if line.startswith('#'):
            line_split = line[1:].split(':', 1)
            currently_selected_bucket = line_split[0].strip()
            if (
                currently_selected_item in items and
                currently_selected_bucket not in items[currently_selected_item]
            ):
                items[currently_selected_item][currently_selected_bucket] = []
            if len(line_split) == 1:
                continue
            line = f":{line_split[1]}"
        line_split = line.split(':', 1)
        if len(line_split) == 1:
            continue
        if currently_selected_bucket is None or currently_selected_item is None:
            raise ValueError("a bucket or an item has not been selected")
        elements = [x.strip() for x in line_split[1].split(',') if x.strip()]
        items[currently_selected_item][currently_selected_bucket].extend(elements)
    output = json.dumps(items, indent=1)
    output = re.sub('",\n *', '", ', output)
    output = re.sub(r'\[''\n *(["0-9, ]*)\n *', r'[\1', output)
    print(output)


if __name__ == '__main__':
    if len(sys.argv) != 1:
        raise ValueError("takes no arguments")
    main()
