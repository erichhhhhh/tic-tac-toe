import json
import pathlib

prog_root = pathlib.Path(__file__).parents[1]
langfile_path = prog_root / "res" / "en-US.json"
output_file = prog_root / "include" / "en-US.h"

with open(langfile_path, "r") as langfile:
    enUS = json.loads(langfile.read())
    
    with open(output_file, "w") as header:
        output = json.dumps(enUS, ensure_ascii=False)
        header.write("#pragma once\n#include <string_view>\n\nconstexpr std::string_view emergency_translation = R\"json(" + output + ")json\";")

