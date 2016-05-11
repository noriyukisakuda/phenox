#include "SioClientWrapper.h"
//#include <iostream>
//#include <string>
#include <mutex>
SioClientWrapper::SioClientWrapper() {
}

SioClientWrapper::~SioClientWrapper() {
}

void SioClientWrapper::onOpen(void){
	//WebSocket通信が開通した時のハンドラ。
	//ここでは特に何かしなくともよいはず。
	//std::cout<<"onOpen()"<<std::endl;
}
void SioClientWrapper::onFail(void){
	//WebSocket通信の開通に失敗した時のハンドラ。
	//普通にしてればまず呼ばれないはず。
	//std::cout<<"onFail()"<<std::endl;
}
void SioClientWrapper::onClose(sio::client::close_reason const& reason){
	//WebSocket通信が終了した時のハンドラ。
	//サーバが強制終了などで落ちたら呼ばれないっぽい。
	//std::cout<<"onClose("<<reason<<")"<<std::endl;
}
void SioClientWrapper::onSocketOpen(std::string const& nsp){
	//データ送受信のイベントはclientではなくsocket単位のため、ここで登録する。
	//std::cout<<"onSocketOpen("<<nsp<<")"<<std::endl;
	sio::socket::event_listener f;
	f = std::bind(&SioClientWrapper::OnEvent, this, std::placeholders::_1);
	for (std::vector<std::string>::iterator it = eventList.begin(); it != eventList.end(); ++it){
		client.socket(nsp)->on((*it), f);
	}
	client.socket()->emit("enter_room", sio::Object().add("room", myRoom).pack());
}
void SioClientWrapper::onSocketClose(std::string const& nsp){
	//引数で与えられたnamespaceが閉じた時のハンドラ。
	//あえて対策しなくともよいのでは？
	//std::cout<<"onSocketClose("<<nsp<<")"<<std::endl;
}
void SioClientWrapper::OnEvent(sio::event &event){
	//イベント名はevent.get_name()で取得できる
	std::string name=event.get_name();
	//データはevent.get_message()で取得できる
	sio::message::ptr data=event.get_message();
	std::mutex mtx;
	{
		std::lock_guard<std::mutex> lock(mtx);
		latestData[name] = data;
		updateFlag[name] = true;
	}
}
//取得したいイベント名のリストを設定する
void SioClientWrapper::setEventList(const std::vector<std::string>& list){
	eventList.clear(); eventList.reserve(list.size());
	latestData.clear();
	updateFlag.clear();
	std::string name;
	for (std::vector<std::string>::const_iterator it = list.begin(); it != list.end();++it){
		name = *it;
		eventList.push_back(name);
		latestData[name] = nullptr;
		updateFlag[name] = false;
	}
}
//自分を表す宛先名を指定
void SioClientWrapper::setMyRoom(const std::string& name){
	myRoom = name;
}
//宛先となる部屋名を指定する
void SioClientWrapper::setDstRoom(const std::string& name){
	dstRoom = name;
}
//初期化して、接続する
void SioClientWrapper::start(const std::string& uri){
	//接続時のイベントハンドラなどは接続前に登録
	client.set_open_listener(std::bind(&SioClientWrapper::onOpen, this));
	client.set_fail_listener(std::bind(&SioClientWrapper::onFail, this));
	client.set_close_listener(std::bind(&SioClientWrapper::onClose, this, std::placeholders::_1));
	client.set_socket_open_listener(std::bind(&SioClientWrapper::onSocketOpen, this, std::placeholders::_1));
	client.set_socket_close_listener(std::bind(&SioClientWrapper::onSocketClose, this, std::placeholders::_1));
	//接続する(デフォルトでnamespace"/"のsocketが開く)
	client.connect(uri);
}
//あるイベントのデータが更新されたかどうか
bool SioClientWrapper::isUpdated(const std::string& name){
	bool ret;
	std::mutex mtx;
	{
		std::lock_guard<std::mutex> lock(mtx);
		ret=updateFlag[name];
	}
	return ret;
}
//あるイベントの最新データを得る
sio::message::ptr SioClientWrapper::getData(const std::string& name){
	sio::message::ptr ret;
	std::mutex mtx;
	{
		std::lock_guard<std::mutex> lock(mtx);
		ret = latestData[name];
		updateFlag[name] = false;
	}
	return ret;
}
void SioClientWrapper::sendData(const std::string& eventName){
	client.socket()->emit("transfer", sio::Object()
		.add("event", eventName)
		.add("room", sio::Array().add(dstRoom))
		.add("data", sio::Null()).pack());
}
