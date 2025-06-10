import argparse
import os
import re
import jinja2
import CmsHeaderGenerator

def build_entries(path):
    entries = []
    for jsonFile in os.listdir(path):
        if jsonFile.endswith('.json'):
            name_only = os.path.splitext(jsonFile)[0]
            entries.append(name_only)
        # todo 최소한의 검증 스키마 체크
    return entries

def main():
    arg_parser = argparse.ArgumentParser(description='CmsInitialGenerator')

    arg_parser.add_argument('--cms_path', type=str, default='../../Cms', help='Header directory')
    arg_parser.add_argument('--template', type=str, default='CmsManager.cpp', help='Template file (in Templates/)')
    arg_parser.add_argument('--output', type=str, default='../../GameServer/CmsManager.cpp', help='Output file path')
    arg_parser.add_argument('--header_path', type=str, default='../../GameServer', help='header file path')
    arg_parser.add_argument('--vcxproj_path', type=str, default='../../GameServer/GameServer.vcxproj', help='header file path')
    arg_parser.add_argument('--vcxproj_filter_path', type=str, default='../../GameServer/GameServer.vcxproj.filters', help='header file path')
    arg_parser.add_argument('--filter', type=str, default='Cms\\Description', help='header file path')
    args = arg_parser.parse_args()

    entries = build_entries(args.cms_path)

    file_loader = jinja2.FileSystemLoader('Templates')
    env = jinja2.Environment(loader=file_loader)

    template = env.get_template(args.template)
    output = template.render(entries=entries)

    f = open(args.output, 'w+')
    f.write(output)
    f.close()

    header_gen = CmsHeaderGenerator.CmsHeaderGenerator(args.vcxproj_path, args.vcxproj_filter_path, args.filter)
    for value in entries:
        header_full_path = args.header_path + "/" + value + "Desc.h"
        if os.path.exists(header_full_path):
            continue
        cms_full_path = args.cms_path + "/" + value +".json"
        header_gen.GenCmsHeader(value, header_full_path,cms_full_path)
        

if __name__ == '__main__':
    main()