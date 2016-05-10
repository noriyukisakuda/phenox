#pragma once
#include <sio_client.h>
#include <Object.h>
class SioClientWrapper{
	//Socket.IO client object
	sio::client client;
	std::string myRoom, dstRoom;
	std::vector<std::string> eventList;
	std::map<std::string, sio::message::ptr> latestData;
	std::map<std::string, bool> updateFlag;

	//WebSocket Connection Listener
	void onOpen(void);
	void onFail(void);
	void onClose(sio::client::close_reason const& reason);
	//Socket Namespace Connection Listener
	void onSocketOpen(std::string const& nsp);
	void onSocketClose(std::string const& nsp);
	//On event
	void OnEvent(sio::event &event);
public:
	SioClientWrapper();
	~SioClientWrapper();
	//取得したいイベント名のリストを設定する
	void setEventList(const std::vector<std::string>& list);
	//自分を表す宛先名を指定
	void setMyRoom(const std::string& name);
	//宛先となる部屋名を指定する
	void setDstRoom(const std::string& name);
	//初期化して、接続する
	void start(const std::string& uri);
	//あるイベントのデータが更新されたかどうか
	bool isUpdated(const std::string& name);
	//あるイベントの最新データを得る
	sio::message::ptr getData(const std::string& name);
	//データを送信する
	template<typename T>
	void sendData(const std::string& eventName, const T& data){
		client.socket()->emit("transfer", sio::Object()
			.add("event", eventName)
			.add("room", sio::Array().add(dstRoom))
			.add("data", data).pack());
	}
	//空データ(イベント名のみ)を送信する
	void sendData(const std::string& eventName);
};
