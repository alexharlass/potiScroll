var serialport = require("serialport");
var $ = require("NodObjC");
$.framework('Cocoa');
var  _ = require('lodash');

var SerialPort = serialport.SerialPort; 

//Alexander: /dev/tty.usbserial-A702NY6D
console.log("Starting...");
var sp = new SerialPort("/dev/tty.usbserial-A702NY6D", {
  baudrate: 9600,
  parser: serialport.parsers.readline("\n")
}, true);

var potiRing = [];
var proxiRing = [];
var bufferPos = 0;
for (var i = 0; i < 5; i++) {
	potiRing.push(i);
	proxiRing.push(i);
}

function getValidValue(buffer, thres) {
	var sum = 0, i, avg;
	for (i = 0; i < buffer.length; i++) {
		sum += buffer[i];
	}
	avg = sum / buffer.length;
	return avg;
	for (i = 0; i < buffer.length; i++) {
		if (Math.abs(buffer[i]-sum) > thres) {
			return undefined;
		}
	}
	return avg;
}

sp.on("open", function () {
  console.log("test");
  sp.on('data', function(data) {

  	data = data.split(',');
  	if (data.length != 3) {
  		return;
  	}
  	var poti = parseInt(data[2]);
  	var proxi = parseInt(data[0]);

  	if (isNaN(poti) || isNaN(proxi)) {
  		return;
  	}

  	proxiRing[bufferPos] = proxi;
  	potiRing[bufferPos] = poti;
  	bufferPos = (bufferPos + 1) % 5;

  	var proxiValid = getValidValue(proxiRing, 20);
  	var potiValid = getValidValue(potiRing, 100);

  	console.log(proxiValid, potiValid);

  	if (proxiValid !== undefined) {
  		console.log('valid prox:' , proxiValid);
  	}
  	if (potiValid !== undefined) {
		//handleData(data, false);
	  	potiValid = Math.round((potiValid - 512) / 51.2);
  		scroll(potiValid);
  	}

  });
});

var scroll = _.throttle(function(key){
    key = key || 0;
    //var mouseEvent = $.CGEventCreateMouseEvent(null, 1, {x: 100, y: 100}, 0);
    var mouseEvent = $.CGEventCreateScrollWheelEvent(null, 0, 1, key);
    // Fire event
    $.CGEventPost($.kCGHIDEventTap, mouseEvent);

    //console.log("zoomIn");
}, 10);

var zoom = _.throttle(function(key){
    key = key || 0;
    //var mouseEvent = $.CGEventCreateMouseEvent(null, 1, {x: 100, y: 100}, 0);
    var mouseEvent = $.CGEventCreateScrollWheelEvent(null, 0, 1, key);
    // Fire event
    $.CGEventPost($.kCGHIDEventTap, mouseEvent);

    //console.log("zoomIn");
}, 10);

