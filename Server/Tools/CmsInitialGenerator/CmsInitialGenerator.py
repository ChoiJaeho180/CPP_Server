import argparse
import os
import jinja2
import CmsHeaderGenerator
import json
import CmsCommonTypeGenerator

def get_json_file_names(path):
    entries = []
    for jsonFile in os.listdir(path):
        if jsonFile.endswith('.json'):
            name_only = os.path.splitext(jsonFile)[0]
            entries.append(name_only)
        # todo �ּ����� ���� ��Ű�� üũ
    return entries

def main():
   

    arg_parser = argparse.ArgumentParser(description='CmsInitialGenerator')

    arg_parser.add_argument('--cms_path', type=str, default='../../Cms', help='Header directory')

    # CmsManager
    arg_parser.add_argument('--template', type=str, default='CmsManager.cpp', help='Template file (in Templates/)')
    arg_parser.add_argument('--output', type=str, default='../../GameServer/CmsManager.cpp', help='Output file path')

    arg_parser.add_argument('--header_path', type=str, default='../../GameServer', help='header file path')
    arg_parser.add_argument('--vcxproj_path', type=str, default='../../GameServer/GameServer.vcxproj', help='header file path')
    arg_parser.add_argument('--vcxproj_filter_path', type=str, default='../../GameServer/GameServer.vcxproj.filters', help='header file path')
    arg_parser.add_argument('--vs_include_path', type=str, default='Cms\\Description', help='header file path')
    arg_parser.add_argument('--common_type_name', type=str, default='CommonType', help='')

    args = arg_parser.parse_args()

    json_file_names = get_json_file_names(args.cms_path)
    
    file_loader = jinja2.FileSystemLoader('Templates')
    env = jinja2.Environment(loader=file_loader)
    template = env.get_template(args.template)
    output = template.render(entries=json_file_names)
    f = open(args.output, 'w+')
    f.write(output)
    f.close()
    
    common_type_gen = CmsCommonTypeGenerator.CmsCommonTypeGenerator(args.header_path,args.common_type_name)
    for file_name in json_file_names:

        cms_full_path = f"{args.cms_path}/{file_name}.json"
        with open(cms_full_path, "r", encoding="utf-8") as f:
            json_data = json.load(f)
        
        common_type_gen.gen_common_type(json_data)
    common_type_gen.write()

    header_gen = CmsHeaderGenerator.CmsHeaderGenerator(
        args.vcxproj_path,                                   
        args.vcxproj_filter_path,                                        
        args.vs_include_path, 
    )
    for file_name in json_file_names:
        header_full_path = f"{args.header_path}/{file_name}Desc.h"
        if os.path.exists(header_full_path):
            continue

        cms_full_path = f"{args.cms_path}/{file_name}.json"
        with open(cms_full_path, "r", encoding="utf-8") as f:
            json_data = json.load(f)

        header_gen.generate_header_from_json(json_data, file_name, header_full_path)
    return
    
    
        

if __name__ == '__main__':
    main()