#pragma once


class DBServerSession : public PacketSession {
public:
	~DBServerSession() {
		cout << "~ServerSession" << endl;
	}

	static void SetInstance(DBServerSessionRef session) { _instance = session; }
	static DBServerSessionRef GetInstance() { return _instance; }

	virtual void OnConnected() override;

	virtual void OnDisconnected() override;
	
	virtual void OnRecvPacket(BYTE* buffer, int32 len) override;
	virtual void OnSend(int32 len) override;

private:
	static DBServerSessionRef _instance;
};