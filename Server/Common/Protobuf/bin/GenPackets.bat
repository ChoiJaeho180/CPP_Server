pushd %~dp0

protoc -I=./ --cpp_out=./ ./Enum.proto
protoc -I=./ --cpp_out=./ ./Struct.proto
protoc -I=./ --cpp_out=./ ./Protocol.proto

GenPackets.exe --path=./Protocol.proto --output=ClientPacketHandler --recv=C_ --send=S_
GenPackets.exe --path=./Protocol.proto --output=ServerPacketHandler --recv=S_ --send=C_

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

DEL /Q /F *.pb.h
DEL /Q /F *.pb.cc
DEL /Q /F *.h

PAUSE