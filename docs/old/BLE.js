//時計
setInterval('showClock1()', 1000);

function showClock1() {
    var nowTime = new Date();
    var nowHour = nowTime.getHours();
    var nowMin = nowTime.getMinutes();
    document.getElementById("timeH").innerHTML = ("00" + nowHour).slice(-2);
    document.getElementById("timeM").innerHTML = ("00" + nowMin).slice(-2);

}

var count = 0;
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
    speedH = speed / 10;
    speedL = speed % 10;
    if (shift == 0) shift = 'N';
    if (mode == 78) mode = "N";
    if (mode == 65) mode = "A";
    if (mode == 68) mode = "D";
    if (mode == 69) mode = "E";

    count = count + 1;


    //HTMLにデータを表示
    document.getElementById('data_rpm').innerHTML = rpm;
    document.getElementById('data_shift').innerHTML = shift;
    document.getElementById('data_temp').innerHTML = temp;
    document.getElementById('data_batt').innerHTML = batt.toFixed(1);
    document.getElementById('data_speedH').innerHTML = speedH.toFixed(0);
    document.getElementById('data_speedL').innerHTML = '.' + speedL;
    document.getElementById('data_mode').innerHTML = mode;

    document.getElementById('uuid_name').innerHTML = uuid;
    document.getElementById('status').innerHTML = "read data"

    //色の変更
    if (mode == "N")document.getElementById('data_mode').style.color = "#F00";
    if (mode == "A")document.getElementById('data_mode').style.color = "#0F0";
    if (mode == "D")document.getElementById('data_mode').style.color = "#0F0";
    if (mode == "E")document.getElementById('data_mode').style.color = "#FF0";
    if (batt <= 10) document.getElementById('data_batt').style.color = "#FF0";
    else            document.getElementById('data_batt').style.color = "#FFF";
    if (temp <= 55) document.getElementById('data_temp').style.color = "#FF0";
    else            document.getElementById('data_temp').style.color = "#FFF";

    if (shift == `N`) document.getElementById('data_shift').style.color = "#0F0";
    else              document.getElementById('data_shift').style.color = "#FFF";
    
    if (rpm < 5500) document.getElementById('data_rpm').style.color = "#FFF";
    else if (rpm < 7500) document.getElementById('data_rpm').style.color = "#0FF";
    else if (rpm < 8400) document.getElementById('data_rpm').style.color = "#FF0";
    else document.getElementById('data_rpm').style.color = "#F00";

    if (count % 10 < 5) document.getElementById('timeS').style.color = "#FFF";
    else document.getElementById('timeS').style.color = "#000";

    if (speed < 450) {
        document.getElementById('speed').style.color = "#FFF";
        document.getElementById('speed').style.backgroundColor = "#111";
    } else if (speed < 575) {
        document.getElementById('speed').style.color = "#FFF";
        document.getElementById('speed').style.backgroundColor = "#02D";
    } else if (speed < 625) {
        if (count % 8 < 4) {
            document.getElementById('speed').style.color = "#FFF";
            document.getElementById('speed').style.backgroundColor = "#0F2";
        } else {
            document.getElementById('speed').style.color = "#FFF";
            document.getElementById('speed').style.backgroundColor = "#111";
        }
    } else if (speed < 725) {
        if (count % 4 < 2) {
            document.getElementById('speed').style.color = "#FFF";
            document.getElementById('speed').style.backgroundColor = "#F80";
        } else {
            document.getElementById('speed').style.color = "#FFF";
            document.getElementById('speed').style.backgroundColor = "#111";
        }
    } else {
        if (count % 2 < 1) {
            document.getElementById('speed').style.color = "#FFF";
            document.getElementById('speed').style.backgroundColor = "#F00";
        } else {
            document.getElementById('speed').style.color = "#FFF";
            document.getElementById('speed').style.backgroundColor = "#111";
        }
    }


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

document.getElementById('stopNotifications').addEventListener('click', function () {
    ble.stopNotify('UUID1');
});


