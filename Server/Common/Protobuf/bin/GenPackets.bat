pushd %~dp0

protoc -I=./ --cpp_out=./ ./Enum.proto
protoc -I=./ --cpp_out=./ ./Struct.proto
protoc -I=./ --cpp_out=./ ./Protocol.proto
protoc -I=./ --cpp_out=./ ./DBStruct.proto
protoc -I=./ --cpp_out=./ ./DBProtocol.proto

GenPackets.exe --path=./Protocol.proto --output=ClientPacketHandler --recv=C_ --send=S_ --template=PacketHandler.h --start_packet_id=1000
GenPackets.exe --path=./Protocol.proto --output=ServerPacketHandler --recv=S_ --send=C_ --template=PacketHandler.h --start_packet_id=1000
GenPackets.exe --path=./DBProtocol.proto --output=DBClientPacketHandler --recv=C_ --send=S_ --template=DBClientPacketHandler.h --start_packet_id=10000
GenPackets.exe --path=./DBProtocol.proto --output=DBServerPacketHandler --recv=S_ --send=C_ --template=DBServerPacketHandler.h --start_packet_id=10000

IF ERRORLEVEL 1 PAUSE

XCOPY /Y Enum.pb.h "../../../GameServer"
XCOPY /Y Enum.pb.cc "../../../GameServer"
XCOPY /Y Struct.pb.h "../../../GameServer"
XCOPY /Y Struct.pb.cc "../../../GameServer"
XCOPY /Y Protocol.pb.h "../../../GameServer"
XCOPY /Y Protocol.pb.cc "../../../GameServer"
XCOPY /Y ClientPacketHandler.h "../../../GameServer"

XCOPY /Y Enum.pb.h "../../../DummyClient"
XCOPY /Y Enum.pb.cc "../../../DummyClient"
XCOPY /Y Struct.pb.h "../../../DummyClient"
XCOPY /Y Struct.pb.cc "../../../DummyClient"
XCOPY /Y Protocol.pb.h "../../../DummyClient"
XCOPY /Y Protocol.pb.cc "../../../DummyClient"
XCOPY /Y ServerPacketHandler.h "../../../DummyClient"

XCOPY /Y Enum.pb.h "../../../../../M1/Source/M1/Network"
XCOPY /Y Enum.pb.cc "../../../../../M1/Source/M1/Network"
XCOPY /Y Struct.pb.h "../../../../../M1/Source/M1/Network"
XCOPY /Y Struct.pb.cc "../../../../../M1/Source/M1/Network"
XCOPY /Y Protocol.pb.h "../../../../../M1/Source/M1/Network"
XCOPY /Y Protocol.pb.cc "../../../../../M1/Source/M1/Network"
XCOPY /Y ServerPacketHandler.h "../../../../../M1/Source/M1/Network"


XCOPY /Y Enum.pb.h "../../../DBServer"
XCOPY /Y Enum.pb.cc "../../../DBServer"
XCOPY /Y Struct.pb.h "../../../DBServer"
XCOPY /Y Struct.pb.cc "../../../DBServer"

XCOPY /Y DBStruct.pb.h "../../../GameServer"
XCOPY /Y DBStruct.pb.cc "../../../GameServer"
XCOPY /Y DBProtocol.pb.h "../../../GameServer"
XCOPY /Y DBProtocol.pb.cc "../../../GameServer"
XCOPY /Y DBServerPacketHandler.h "../../../GameServer"

XCOPY /Y DBStruct.pb.h "../../../DBServer"
XCOPY /Y DBStruct.pb.cc "../../../DBServer"
XCOPY /Y DBProtocol.pb.h "../../../DBServer"
XCOPY /Y DBProtocol.pb.cc "../../../DBServer"
XCOPY /Y DBClientPacketHandler.h "../../../DBServer"

DEL /Q /F *.pb.h
DEL /Q /F *.pb.cc
DEL /Q /F *.h

PAUSE