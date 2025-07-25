import argparse
import jinja2
import ProtoParser

def main():
    arg_parser = argparse.ArgumentParser(description='PacketGenerator')

    arg_parser.add_argument('--path', type=str, default='../../Common/Protobuf/bin/Protocol.proto', help='proto path')
    arg_parser.add_argument('--output', type=str, default='TestPacketHandler', help='output file')
    arg_parser.add_argument('--recv', type=str, default='C_', help='recv convention')
    arg_parser.add_argument('--send', type=str, default='S_', help='send convention')
    arg_parser.add_argument('--template', type=str, default='PacketHandler.h', help='template')
    arg_parser.add_argument('--start_packet_id', type=int, default=1000, help='template')
    args = arg_parser.parse_args()
    
    
    parser = ProtoParser.ProtoParser(args.start_packet_id, args.recv, args.send)
    parser.parse_proto(args.path)
    
    #jinja2
    file_loader = jinja2.FileSystemLoader('Templates')
    env = jinja2.Environment(loader=file_loader)
    
    template = env.get_template(args.template)
    output = template.render(parser = parser, output = args.output)

    f = open(args.output + '.h', 'w+', encoding='utf-8')
    f.write(output)
    f.close()
    
    print(output);
    
    return


if __name__ == '__main__':
    main()