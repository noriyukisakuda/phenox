#pragma once
//送信データを生成するためのサンプル
#include <sio_client.h>
#include <Object.h>

//送信データを生成するサンプル関数一覧(おそらく大抵は配列か辞書型で送るはず)
// sio::Null makeSampleNull();//Nullの場合
// sio::Boolean makeSampleBoolean();//真偽値の場合
bool makePxStart();//真偽値の場合
bool makePxBounce();//真偽値の場合
bool makePxReady();//真偽値の場合
// sio::Integer makeSampleInteger();//整数の場合
// sio::Float makeSampleFloat();//浮動小数の場合
// sio::String makeSampleString();//文字列の場合
// sio::Array makeSampleArray();//配列の場合
sio::Array makePxPosition();//配列の場合
sio::Array makePxVelocity();//配列の場合
// sio::Object makeSampleObject();//辞書型の場合

// //Nullデータの生成
// sio::Null makeSampleNull(){
// 	//コンストラクタでセットしてそのまま返せばよい(＝SendDataの第2引数に直接入れても同じ)
// 	return sio::Null();
// }
// //真偽値型データの生成
// sio::Boolean makeSampleBoolean(){
// 	//コンストラクタでセットしてそのまま返せばよい(＝SendDataの第2引数に直接入れても同じ)
// 	return sio::Boolean(true);
// }
// //整数型データの生成
// sio::Integer makeSampleInteger(){
// 	//コンストラクタでセットしてそのまま返せばよい(＝SendDataの第2引数に直接入れても同じ)
// 	return sio::Integer(555);
// }
// //浮動小数型データの生成
// sio::Float makeSampleFloat(){
// 	//コンストラクタでセットしてそのまま返せばよい(＝SendDataの第2引数に直接入れても同じ)
// 	return sio::Float(1e8);
// }
// //文字列型データの生成
// sio::String makeSampleString(){
// 	//コンストラクタでセットしてそのまま返せばよい(＝SendDataの第2引数に直接入れても同じ)
// 	return sio::String("string data");
// }

//反射データの生成
bool makePxBounce(){
	return true; 
}

//スタートの生成
bool makePxStart(){
	return true; 
}

bool makePxReady(){
	return true; 
}


//位置データの生成
sio::Array makePxPosition(double px_position_x, double px_position_y){
	sio::Array ret;//配列型の場合はsio::Array
	//Arrayへの追加はadd(value)で行う。
	ret.add(px_position_x)
		.add(px_position_y);
	return ret;
}

//位置データの生成
sio::Array makePxVelocity(double px_velocity_x, double px_velocity_y){
	sio::Array ret;//配列型の場合はsio::Array
	//Arrayへの追加はadd(value)で行う。
	ret.add(px_velocity_x)
		.add(px_velocity_y);
	return ret;
}
