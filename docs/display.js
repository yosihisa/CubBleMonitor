var Rpm ;
var Shift;
var Temp;
var Batt = 0;
var Speed = 0;
var Mode;

var count = 0;

var cv;
var id;
var canvas

var kb = 0.10;
var ks = 0.40;

updateParam = function (rpm, shift, temp, batt, speed, mode) {
    Rpm = parseInt(rpm,10);
    Shift = shift;
    Temp = temp;
    Batt = kb * batt + (1 - kb) * Batt;
    Speed = ks * speed + (1 - ks) * Speed;
    Mode = mode;

    count = count + 1;
}

function Start_drawing() {
    console.log('draw');
    canvas = document.getElementById('canvas');
    if (canvas.getContext) {
        cv = canvas.getContext('2d');
        cv.fillStyle = "rgb(255,255,255)";
        cv.fillRect(0, 0, 380, 250);
        cv.fill();
        id = setInterval(ndraw, 25);
    }

    Rpm = 0;
}

function ndraw() {
    var MAX_RPM = 10000;
    //背景
    cv.fillStyle = "rgb(0,0,0)";
    cv.fillRect(0, 0, 380, 250);

    //タコメーター
    cv.fillStyle = "#222";
    cv.fillRect(0, 0, 380, 200);
    cv.fillStyle = "#080";
    cv.beginPath();
    cv.arc(190, 200, 300, -Math.PI, (-1+(Rpm / MAX_RPM)) * (3.1415), false);
    cv.lineTo(190, 200);
    cv.closePath();
    cv.fill();

    if (Rpm > 2300) {
        cv.fillStyle = "#0F0";
        cv.beginPath();
        cv.arc(190, 200, 300, (-1 + (2300 / MAX_RPM)) * (3.1415), (-1 + (Rpm / MAX_RPM)) * (3.1415), false);
        cv.lineTo(190, 200);
        cv.closePath();
        cv.fill();
    }

    if (Rpm > 5300) {
        cv.fillStyle = "#FF0";
        cv.beginPath();
        cv.arc(190, 200, 300, (-1 + (5300 / MAX_RPM)) * (3.1415), (-1 + (Rpm / MAX_RPM)) * (3.1415), false);
        cv.lineTo(190, 200);
        cv.closePath();
        cv.fill();
    }

    if (Rpm > 8400) {
        cv.fillStyle = "#F00";
        cv.beginPath();
        cv.arc(190, 200, 300, (-1 + (8400 / MAX_RPM)) * (3.1415), (-1 + (Rpm / MAX_RPM)) * (3.1415), false);
        cv.lineTo(190, 200);
        cv.closePath();
        cv.fill();
    }

    //タコメーターカバー
    cv.fillStyle = "#111"
    cv.beginPath();
    cv.fillRect(20, 20, 340, 180);
    cv.closePath();


    //速度
    speedH = (Speed / 10).toFixed(0);
    speedL = (Speed.toFixed(0) % 10);
    cv.fillStyle = "#FFF"
    cv.font = 'bold 120pt Oswald';
    cv.textAlign ='end'
    cv.fillText(speedH, 298, 150,180);

    cv.font = 'bold 60pt Oswald';
    cv.textAlign = 'start'
    cv.fillText(speedL, 305, 150, 50);

    cv.font = '30pt Oswald';
    cv.textAlign = 'start'
    cv.fillText('km/h', 305, 70, 50);

    //シフトインジケータ
    cv.fillStyle = "#0FF"
    if (Shift == `N`) {
        cv.fillStyle = "#0F0"
    }
    cv.font = 'bold 70pt Oswald';
    cv.textAlign = 'start'
    cv.fillText(Shift, 40, 100, 50);

    //時計
    var nowTime = new Date();
    var nowHour = nowTime.getHours();
    var nowMin = nowTime.getMinutes();
    cv.fillStyle = "#FFF"
    cv.font = 'bold 40pt Oswald';
    cv.textAlign = 'start'    
    cv.fillText(("00" + nowHour).slice(-2) + ':' + ("00" + nowMin).slice(-2), 30, 150, 80);

    //温度
    cv.fillStyle = "#FFF"
    cv.font = '20pt Oswald';
    cv.textAlign = 'start'
    cv.fillText(Temp+'℃', 40, 190, 60);

    //バッテリ電圧
    cv.fillStyle = "#FFF"
    cv.font = '20pt Oswald';
    cv.textAlign = 'end'
    cv.fillText(Batt.toFixed(1) + 'V', 190, 190, 80);

    cv.fillStyle = "#333";
    cv.beginPath();
    cv.fillRect(200, 170, 50, 20);
    cv.closePath();
    cv.fillStyle = "#080";
    if (Batt < 12) {
        cv.fillStyle = "#FF0";
    }
    cv.beginPath();
    cv.fillRect(200, 170, 50*(Batt/16), 20);
    cv.closePath();

    //GNSS
    cv.fillStyle = "#FFF"
    cv.font = '20pt Oswald';
    cv.textAlign = 'start'
    cv.fillText('GNSS', 275, 190, 50);

    cv.textAlign = 'start'
    if (Mode == 'N') {
        cv.fillStyle = "#F00"
        cv.fillText('×', 330, 190);
    } else if (Mode == 'E') {
        cv.fillStyle = "#FF0"
        cv.fillText('△', 330, 190);
    } else if (Mode == 'A') {
        cv.fillStyle = "#0F0"
        cv.fillText('〇', 330, 190);
    } else if (Mode == 'D') {
        cv.fillStyle = "#0F0"
        cv.fillText('◎', 330, 190);
    } else {
        cv.fillStyle = "#FFF"
        cv.fillText(Mode, 330, 190, 30);
    }
    
    //速度計小数点
    cv.fillStyle = "#FFF"
    cv.beginPath();
    cv.arc(300, 147, 4, 0, 3.142 * 2, true);
    cv.closePath();
    cv.fill();
}