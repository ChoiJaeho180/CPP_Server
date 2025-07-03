import json
import os
import xml.etree.ElementTree as ET
import xml.dom.minidom

class CmsHeaderGenerator:
    def __init__(self, vcxproj_path, vcxproj_filter_path, vs_include_path):
        self.vcxproj_path = vcxproj_path
        self.vcxproj_tree = ET.parse(self.vcxproj_path)

        self.vcxproj_filter_path = vcxproj_filter_path
        self.vcxproj_filter_tree = ET.parse(self.vcxproj_filter_path)

        self.vs_include_path = vs_include_path
        return


    def generate_header_from_json(self, json_data, name, header_full_path):
        file = ""

        file += "#pragma once\n"
        file += "#include \"../Libraries/Include/nlohmann/json.hpp\"\n"
        if "Type" in json_data:
            file +="#include \"CommonType.h\"\n"
        file += "\n"

        # struct 
        file += f"struct {name}Desc  {{\n" 
        for _name, _type in json_data["Column"].items():
            is_optional = _name in json_data.get("Optional", [])

            if _type.endswith("[]"):
                _type = f"std::vector<{_type[:-2]}>"

            if is_optional:
                _type = f"std::optional<{_type}>"
                file += f"	{_type} {_name} = std::nullopt;\n"
            else:
                file += f"	{_type} {_name};\n"

        file += "};\n"
        file += "\n";

        # macro
        field_list = ", ".join(json_data["Column"].keys())
        file += f"NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT({name}Desc, {field_list});";

        f = open(header_full_path, 'w+', encoding='utf-8')
        f.write(file)
        f.close()

        self.add_header_to_vcxproj(name)
        self.add_filter_to_vcxproj_filter(name)
        return


    def add_header_to_vcxproj(self,name):
        file_name = name + "Desc.h"
        self._add_include_if_not_exists(self.vcxproj_tree, self.vcxproj_path, file_name)

    def add_filter_to_vcxproj_filter(self,name):
        file_name = name + "Desc.h"
        self._add_include_if_not_exists(
            self.vcxproj_filter_tree,
            self.vcxproj_filter_path,
            file_name,
            self.vs_include_path
            )

    def _add_include_if_not_exists(self, tree, path, file_name, filter_path=None):
        ns_uri = "http://schemas.microsoft.com/developer/msbuild/2003"
        ns = {"ns": ns_uri}
        ET.register_namespace('', ns_uri)
        root = tree.getroot()
        
        for cl in root.findall(".//ns:ClInclude", ns):
           if cl.attrib.get("Include") == file_name:
               return False
        
        target_item_group = None
        for item_group in root.findall("ns:ItemGroup", ns):
            if item_group.find("ns:ClInclude", ns) is not None:
                target_item_group = item_group
                break
        
        if target_item_group is None:
            # 없으면 새로 만듦
            target_item_group = ET.SubElement(root, "ItemGroup")

        cl_include = ET.Element("ClInclude")
        cl_include.set("Include", file_name)
        
        # 필터 경로가 주어진 경우에만 추가
        if filter_path:
            filter_tag = ET.SubElement(cl_include, "Filter")
            filter_tag.text = filter_path
        
        # 기존 그룹에 추가
        target_item_group.append(cl_include)

        tree.write(path, encoding="utf-8", xml_declaration=True)

        return True

   

   