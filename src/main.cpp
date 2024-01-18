#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <../lib/CRC16_ModbusRTU.h>

// Replace with your network credentials
const char* ssid = "levshx";
const char* password = "123456781";

bool ledState = 0;
const int ledPin = 2;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

const char index_html[] PROGMEM = R"rawliteral(
<html><head>
    <title>ESP Web Server</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
    html {
      font-family: Arial, Helvetica, sans-serif;
      text-align: center;
    }
    h1 {
      font-size: 1.8rem;
      color: white;
    }
    h2{
      font-size: 1.5rem;
      font-weight: bold;
      color: #143642;
    }
    .topnav {
      overflow: hidden;
      background-color: #143642;
    }
    body {
      margin: 0;
    }
    .content {
      padding: 30px;
      max-width: 600px;
      margin: 0 auto;
    }
    .card {
      background-color: #F8F7F9;;
      box-shadow: 2px 2px 12px 1px rgba(140,140,140,.5);
      padding-top:10px;
      padding-bottom:20px;
    }
    .button {
      padding: 2px 15px;
      font-size: 24px;
      text-align: center;
      outline: none;
      color: #fff;
      background-color: #0f8b8d;
      border: none;
      border-radius: 5px;
      -webkit-touch-callout: none;
      -webkit-user-select: none;
      -khtml-user-select: none;
      -moz-user-select: none;
      -ms-user-select: none;
      user-select: none;
      -webkit-tap-highlight-color: rgba(0,0,0,0);
     }
     /*.button:hover {background-color: #0f8b8d}*/
     .button:active {
       background-color: #0f8b8d;
       box-shadow: 2 2px #CDCDCD;
       transform: translateY(2px);
     }
     .state {
       font-size: 1.5rem;
       color:#8c8c8c;
       font-weight: bold;
     }
    </style>
  <title>ESP Web Server</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="icon" href="data:,">
  </head>
  <body>
    <div class="topnav">
      <h1>ESP WebSocket Server</h1>
    </div>
    <div class="content">
      
  <div class="card">
      
        <h2>SERIAL MONITOR</h2>
        <p class="state">STATE: <span id="state">DISCONNECTED</span></p>
        <textarea rows="10" cols="70" readonly="" id="serial_monitor_area" style="resize: none;"></textarea>
        <p>
            <input type="text" id="send_data" /> 
            <button id="button" class="button">Send</button>
        </p>
        
        
        
        
      </div>
    </div>
  <script>
    var gateway = `ws://${window.location.hostname}/ws`;
    var websocket;
    window.addEventListener('load', onLoad);
    function initWebSocket() {
      console.log('Trying to open a WebSocket connection...');
      websocket = new WebSocket(gateway);
      websocket.onopen    = onOpen;
      websocket.onclose   = onClose;
      websocket.onmessage = onMessage; // <-- add this line
    }
    function onOpen(event) {
      console.log('Connection opened');
      document.getElementById('state').innerHTML = "CONNECTED";
    }
    function onClose(event) {
      console.log('Connection closed');
      document.getElementById('state').innerHTML = "DISCONNECTED";
      setTimeout(initWebSocket, 2000);
    }
    function onMessage(event) {      
        document.getElementById('serial_monitor_area').value = document.getElementById('serial_monitor_area').value + event.data + "\n";
    }
    function onLoad(event) {
      initWebSocket();
      initButton();
    }
    function initButton() {
      document.getElementById('button').addEventListener('click', send);
      document.getElementById('send_data').addEventListener("keyup", function(event) {
        if (event.key === "Enter") {
            send();
        }
      })
    }
    function send(){
      if (document.getElementById('state').innerHTML == "CONNECTED") {
        var data = document.getElementById('send_data').value.replaceAll(' ','').toUpperCase();
        console.log('send data: ' + data);
        websocket.send(data);
        document.getElementById('serial_monitor_area').value = document.getElementById('serial_monitor_area').value + data + "\n";
        document.getElementById('serial_monitor_area').scrollTop = document.getElementById('serial_monitor_area').scrollHeight;;
        document.getElementById('send_data').value = ""; 
      }
      
    }
  </script>
  </body>
  </html>
)rawliteral";

void notifyClients() {
  ws.textAll(String(ledState));
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    if (strcmp((char*)data, "TOGGLE") == 0) {
      ledState = !ledState;
      notifyClients();
    }
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len) {
    switch (type) {
      case WS_EVT_CONNECT:
        Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
        break;
      case WS_EVT_DISCONNECT:
        Serial.printf("WebSocket client #%u disconnected\n", client->id());
        break;
      case WS_EVT_DATA:
        handleWebSocketMessage(arg, data, len);
        break;
      case WS_EVT_PONG:
      case WS_EVT_ERROR:
        break;
  }
}

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

String processor(const String& var){
  Serial.println(var);
  if(var == "STATE"){
    if (ledState){
      return "ON";
    }
    else{
      return "OFF";
    }
  }
  return String();
}

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  wifi_set_sleep_type(NONE_SLEEP_T);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP Local IP Address
  Serial.println(WiFi.localIP());

  initWebSocket();

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  // Start server
  server.begin();
}

void loop() {
  ws.cleanupClients();
  digitalWrite(ledPin, ledState);
}