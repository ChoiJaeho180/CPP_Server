import json

class CmsCommonTypeGenerator:
    def __init__(self, header_path, file_name):
        self.header_path = header_path
        self.file_name = file_name
        self.should_write = False

        with open(f"{header_path}/{file_name}.h", "r", encoding="utf-8") as header:
            self.header_data = header.read()

        self.existing_structs = set()
        for line in self.header_data.splitlines():
            line = line.strip()
            if line.startswith("struct "):
                name = line.split()[1]
                if name.endswith("{"):
                    name = name[:-1]
                self.existing_structs.add(name)

    def gen_common_type(self, json_data):
        if not json_data.get("Type"):
            return
        
        for type_name, fields in json_data["Type"].items():
            if type_name in self.existing_structs:
                continue 

            self.should_write = True  

            self.header_data += f"\nstruct {type_name} {{\n"
            for _name, _type in fields.items():
                if _type.endswith("[]"):
                    _type = f"std::vector<{_type[:-2]}>"
                self.header_data += f"    {_type} {_name};\n"
            self.header_data += "};\n"
            
            field_list = ", ".join(fields.keys())
            self.header_data += f"NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE({type_name}, {field_list});\n"
            

    def write(self):
        if not self.should_write:
            return
         
        with open(f"{self.header_path}/{self.file_name}.h", "w", encoding="utf-8") as header:
            header.write(self.header_data)
