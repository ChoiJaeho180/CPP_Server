import json
import logging
import os
import xml.etree.ElementTree as ET
import xml.dom.minidom

class CmsHeaderGenerator:
    def __init__(self, vcxproj_path, vcxproj_filter_path, virtual_path):
        self.vcxproj_path = vcxproj_path
        self.vcxproj_tree = ET.parse(self.vcxproj_path)

        self.vcxproj_filter_path = vcxproj_filter_path
        self.vcxproj_filter_tree = ET.parse(self.vcxproj_filter_path)

        self.virtual_path = virtual_path
        return

    def GenCmsHeader(self, name, header_full_path, cms_full_path):
        with open(cms_full_path, "r", encoding="utf-8") as f:
            data = json.load(f)

        hasVector = False;

        file =""
        file += "#pragma once\n"
        file += "#include \"../Libraries/Include/nlohmann/json.hpp\"\n"

        struct = ""
        struct += f"struct {name}Desc  {{\n" 
        
        define_parameter = ""
        for k, v in data.items():
            if k == name:
                continue
            struct += f"	{v} {k};\n"
            define_parameter += f", {k}"
            
            if k =="std::vector":
                hasVector = True
            
        struct += "};\n"
        
        
        if hasVector:
            file += "#include \"<vector>\"\n"

        file += struct;
        file += f"NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE({name}Desc{define_parameter}";
        file +=");"

        f = open(header_full_path, 'w+', encoding='utf-8')
        f.write(file)
        f.close()

        self.add_header_to_vcxproj(name)
        self.add_filter_to_vcxproj_filter(name)

    def add_header_to_vcxproj(self,name):
        file_name = name + "Desc.h"
        self._add_include_if_not_exists(self.vcxproj_tree, self.vcxproj_path, file_name)

    def add_filter_to_vcxproj_filter(self,name):
        file_name = name + "Desc.h"
        self._add_include_if_not_exists(
            self.vcxproj_filter_tree,
            self.vcxproj_filter_path,
            file_name,
            self.virtual_path
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

        xml_str = ET.tostring(tree.getroot(), encoding='utf-8')
        parsed = xml.dom.minidom.parseString(xml_str)
        with open(path, 'w', encoding='utf-8') as f:
            f.write(parsed.toprettyxml(indent="  "))
        return True

   

   