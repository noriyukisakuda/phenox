#pragma once
//受信データのsio::message::ptrから値を抽出するためのサンプル
#include <../include/sio_client.h>
#include <../include/Object.h>


/*抽出サンプルでの格納先の変数*/
// int64_t intVal;
// double doubleVal;
// double vec3[3];
bool landing;
double direction[2];
// std::string stringVal;
/*抽出サンプルでの格納先の変数*/
//取得したデータをsio::message::ptrから抽出するサンプル
// void parseSampleData(sio::message::ptr data);
//3次元ベクトルが送信されてくる場合の例
// void parseVec3(sio::message::ptr data);

void parseLanding(sio::message::ptr data);
void parseDirection(sio::message::ptr data);

// void parseSampleData(sio::message::ptr data){
// 	sio::message::ptr ptr;
// 	//sio::message::ptr::get_XXX()でXXX型の値を得られるので、
// 	//各型に合わせて以下のように順次値を得ていく。
// 	/*bool b = ptr->get_bool();
// 	int64_t i = ptr->get_int();
// 	double d = ptr->get_double();
// 	std::string s = ptr->get_string();
// 	std::vector<sio::message::ptr> vector = ptr->get_vector();
// 	std::map<std::string, sio::message::ptr> map = ptr->get_map();*/
//
// 	//例えば、{"Int":1,"Double":3.0,"Array":[5,6,7,"end"]}というObjectが送られてくる場合は、以下のように値を抽出していく。
// 	//値の格納先は各自で定義しておくこと。
// 	std::map<std::string, sio::message::ptr> map = data->get_map();//最外側のObjectからmapを抽出
// 	intVal = map["Int"]->get_int();//キー"Int"の値を抽出
// 	doubleVal = map["Double"]->get_double();//キー"Double"の値を抽出
// 	std::vector<sio::message::ptr> vector = map["Array"]->get_vector();//キー"Array"のArrayからvectorを抽出
// 	for (int i = 0; i < 3; ++i){
// 		vec3[i] = vector[i]->get_double();//vectorの先頭3つがdoubleなのでdouble型配列に代入
// 	}
// 	stringVal = vector[3]->get_string();//vectorの4つめはstringなのでstd::stringとして抽出
// }

// //3次元ベクトルが送信されてくる場合の例
// void parseVec3(sio::message::ptr data){
// 	//[0.2,-2.1,3.4]のように要素数3の実数配列としてくる場合
// 	std::vector<sio::message::ptr> vector = data->get_vector();//外側のArrayからvectorを抽出
// 	for (int i = 0; i < 3; ++i){
// 		vec3[i] = vector[i]->get_double();//vectorの要素を順に抽出してdouble型配列に代入
// 	}
// }
//
//着陸命令が送信されてくる場合
void parseDirection(sio::message::ptr data){
	std::vector<sio::message::ptr> vector = data->get_vector();//外側のArrayからvectorを抽出
        landing = vector[0]->get_bool();//vectorの要素を順に抽出してdouble型配列に代入
}

//方向ベクトルが送信されてくる場合
void parseDirection(sio::message::ptr data){
	//[0.2,-2.1,3.4]のように要素数2の実数配列としてくる場合
	std::vector<sio::message::ptr> vector = data->get_vector();//外側のArrayからvectorを抽出
	for (int i = 0; i < 2; ++i){
		direction[i] = vector[i]->get_double();//vectorの要素を順に抽出してdouble型配列に代入
	}
}
