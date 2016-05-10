#include <iostream>
#include "SioClientWrapper.h"
#include "Parser.h"//受信データのsio::message::ptrから値を抽出するためのサンプル
#include "DataMaker.h"//送信データを生成するためのサンプル

int main()
{
	SioClientWrapper client;
	sio::message::ptr data;
	//発生するイベント名一覧をstd::vector<std::string>としてclientに渡す
	std::vector<std::string> eventList(0);
	eventList.push_back("landing");
	eventList.push_back("direction");
	eventList.push_back("px_bounce");
	eventList.push_back("px_start");
	eventList.push_back("px_position");
	eventList.push_back("px_velocity");
	client.setEventList(eventList);
	//自身を表す部屋名を設定する(Phenoxなら例えば"Phenox"と決める)
	client.setMyRoom("Phenox");
	//データの送信宛先となる部屋名を設定する(Gameサーバなら例えば"Game")
	client.setDstRoom("Game");
	//URLを指定して接続開始
	client.start("http://localhost:8000");
	while (true){
		//"landing"に対応するデータが来ているかチェック
		if (client.isUpdated("landing")){
			data = client.getData("landing");//データをsio::message::ptrとして取得
			parseLanding(data);//データ抽出用関数に渡す
			std::cout << "landing=" << landing << std::endl;
		}
		//"direction"に対応するデータが来ているかチェック
		if (client.isUpdated("direction")){
			data = client.getData("direction");//データをsio::message::ptrとして取得
			parseDirection(data);//データ抽出用関数に渡す
                        std::cout << "direction = [" << direction[0] << ", " << direction[1] << "]" << std::endl;
		}
                client.sendData("Phenox Data", makeSampleObject());//
		//sleep(100);
	}
	return 0;
}



