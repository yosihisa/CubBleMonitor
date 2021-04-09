var connected = 0;

//--------------------------------------------------
//Global変数
//--------------------------------------------------
//BlueJellyのインスタンス生成
const ble = new BlueJelly();

//--------------------------------------------------
//ロード時の処理
//--------------------------------------------------
window.onload = function () {
    //UUIDの設定
    ble.setUUID("UUID1", "90af54ef-d367-41ec-ac04-b3f9f42cdcb2", "a018d2c9-5838-4196-bd97-d00547de2c57");
}


//--------------------------------------------------
//Scan後の処理
//--------------------------------------------------
ble.onScan = function (deviceName) {
    document.getElementById('device_name').innerHTML = deviceName;
    document.getElementById('status').innerHTML = "found device!";
}


//--------------------------------------------------
//ConnectGATT後の処理
//--------------------------------------------------
ble.onConnectGATT = function (uuid) {
    console.log('> connected GATT!');

    document.getElementById('uuid_name').innerHTML = uuid;
    document.getElementById('status').innerHTML = "connected GATT!";
}


//--------------------------------------------------
//Read後の処理：得られたデータの表示など行う
//--------------------------------------------------
ble.onRead = function (data, uuid) {
    //フォーマットに従って値を取得
    rpm   = data.getInt16(0);
    shift = data.getInt8(2);
    temp =  data.getUint8(3);
    batt  = data.getUint8(4);
    speed = data.getInt16(5);
    mode  = data.getInt8(7);

    rpm = rpm ;
    batt = batt / 10;
    temp = temp - 20;
    if (shift == 0) shift = 'N';
    if (mode == 78) mode = "N";
    if (mode == 65) mode = "A";
    if (mode == 68) mode = "D";
    if (mode == 69) mode = "E";

    document.getElementById('uuid_name').innerHTML = uuid;

    updateParam(rpm, shift, temp, batt, speed, mode);
}


//--------------------------------------------------
//Start Notify後の処理
//--------------------------------------------------
ble.onStartNotify = function (uuid) {
    console.log('> Start Notify!');

    document.getElementById('uuid_name').innerHTML = uuid;
    document.getElementById('status').innerHTML = "started Notify";

    setTimeOut()
    deviceConnected = 1;
}


//--------------------------------------------------
//Stop Notify後の処理
//--------------------------------------------------
ble.onStopNotify = function (uuid) {
    console.log('> Stop Notify!');

    document.getElementById('uuid_name').innerHTML = uuid;
    document.getElementById('status').innerHTML = "stopped Notify";

    deviceConnected = 0;

}


//--------------------------------------------------
//Disconnect状態時の処理
//--------------------------------------------------
ble.onDisconnect = function () {
    console.log('> Disconnect');
    document.getElementById('uuid_name').innerHTML = "Not Connected";
    document.getElementById('status').innerHTML = "disconnected";
    connected = 0;
}

//--------------------------------------------------
//Error後の処理
//--------------------------------------------------
ble.onError = function (error) {
    document.getElementById('status').innerHTML = "ERROR : " + error;
}

//-------------------------------------------------
//ボタンが押された時のイベント登録
//--------------------------------------------------

function connectBLE() {
    if (connected == 0) {
        ble.startNotify('UUID1');
        connected = 1;
    }
}

document.getElementById('startNotifications').addEventListener('click', function () {

    if (connected == 0) {
        setInterval(connectBLE, 500);
    }
    //ble.startNotify('UUID1');
});

/*
document.getElementById('stopNotifications').addEventListener('click', function () {
    ble.stopNotify('UUID1');
});
*/