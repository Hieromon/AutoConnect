
#include <ThingsBoard.h>
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>     
#include <AutoConnect.h>
#include <ArduinoNvs.h>


WebServer Server;
AutoConnect Portal(Server);
AutoConnectConfig Config;

WiFiClient  wifiClient; 
ThingsBoard tb(wifiClient);
ArduinoNvs nvs;

ACText(MqttTitle, "MQTT broker settings", "", "", AC_Tag_BR);
ACText(MqttSubTitle, "Publishing the WiFi...", "", "", AC_Tag_BR);
ACInput(MqttServer, "localhost", "Server (IP/FQDN):", "", "", AC_Tag_BR);
ACInput(MqttAccessToken, "", "AccessToken:", "", "", AC_Tag_BR);
ACInput(MqttPort, "1883", "Port:", "", "", AC_Tag_BR);
ACInput(MqttDeviceId, "", "DeviceId:", "", "", AC_Tag_BR);
ACInput(MqttPassword, "", "Password:", "", "", AC_Tag_BR);
ACSubmit(MqttSubmit, "SAVE", "/_mqtt_save");
ACSubmit(MqttResetSettingsSubmit, "RESET SETTINGS", "/_mqtt_reset_settings");
AutoConnectAux  MqttPage("/_mqtt_setting", "MQTT Setting", true, { MqttTitle, MqttSubTitle, MqttServer, MqttAccessToken, MqttPort, MqttDeviceId, MqttPassword, MqttSubmit, MqttResetSettingsSubmit});

ACText(MqttSaveTitle, "Save parameters");
ACSubmit(MqttSaveSubmit, "START", "/_mqtt_start"); 
AutoConnectAux  MqttSavePage("/_mqtt_save", "MQTT Setting", false, { MqttSaveTitle, MqttSaveSubmit });

ACText(MqttStartTitle, "Trying to connect to broker");
AutoConnectAux  MqttStartPage("/_mqtt_start", "MQTT Start", false, { MqttStartTitle });

ACText(MqttResetTitle, "Reset settings");
AutoConnectAux MqttResetPage("/_mqtt_reset_settings","MQTT Setting",false,{MqttResetTitle},false);

String mqttServer;
String mqttAccessToken;
String mqttPort;
String mqttDeviceId;
String mqttPassword;

String nvsServer = "";
String nvsAccessToken = "";
String nvsPort = "";
String nvsDeviceId = "";
String nvsPassword = "";

bool tbConnectOnLoad = true;

void rootPage() {
  const char content[] = R""""(
  <h1>Menu</h1>
  <a href="/_ac">Network Config</a>
  )"""";
  Server.send(200, "text/html", content);
}

String onMqttSavePage(AutoConnectAux& aux, PageArgument& args) {

  String mqttServer = args.arg("MqttServer");
  String mqttAccessToken = args.arg("MqttAccessToken");
  String mqttPort = args.arg("MqttPort");
  String mqttDeviceId = args.arg("MqttDeviceId");
  String mqttPassword = args.arg("MqttPassword");

  Serial.println(mqttServer);
  
  if(!tb.connect(mqttServer.c_str(),mqttAccessToken.c_str(),mqttPort.toInt(),mqttDeviceId.c_str(),mqttPassword.c_str())){
    Serial.println("failed");
    return String();
  }

  Serial.println("connected");

  nvs.setString("MqServer", mqttServer);
  nvs.setString("MqAccessToken", mqttAccessToken);
  nvs.setString("MqPort", mqttPort);
  nvs.setString("MqDeviceId", mqttDeviceId);
  nvs.setString("MqPassword", mqttPassword);

  nvsServer = mqttServer;
  nvsAccessToken = mqttAccessToken;
  nvsPort = mqttPort;
  nvsDeviceId = mqttDeviceId;
  nvsPassword = mqttPassword;

  tbConnectOnLoad = true;

  return String();
}

String onMqttLoadSetting(AutoConnectAux& aux, PageArgument& args){
  if (nvsServer != ""){aux.getElement("MqttServer")->value = nvsServer;}
  if (nvsAccessToken != ""){aux.getElement("MqttAccessToken")->value = nvsAccessToken;}
  if (nvsPort != ""){aux.getElement("MqttPort")->value = nvsPort;}
  if (nvsDeviceId != ""){aux.getElement("MqttDeviceId")->value = nvsDeviceId;}
  if (nvsPassword != ""){aux.getElement("MqttPassword")->value = nvsPassword;}

  return String();
}

String onMqttResetPage(AutoConnectAux& aux, PageArgument& args){
  if(nvsServer != ""){nvs.erase("MqServer");}
  if(nvsAccessToken != ""){nvs.erase("MqAccessToken");}
  if(nvsPort != ""){nvs.erase("MqPort");}
  if(nvsDeviceId != ""){nvs.erase("MqDeviceId");}
  if(nvsPassword != ""){nvs.erase("MqPassword");}

  nvsServer = mqttServer;
  nvsAccessToken = mqttAccessToken;
  nvsPort = mqttPort;
  nvsDeviceId = mqttDeviceId;
  nvsPassword = mqttPassword;

  
  MqttPage.setElementValue("MqttServer",nvsServer);
  MqttPage.setElementValue("MqttAccessToken",nvsAccessToken);
  MqttPage.setElementValue("MqttPort",nvsPort);
  MqttPage.setElementValue("MqttDeviceId",nvsDeviceId);
  MqttPage.setElementValue("MqttPassword",nvsPassword);
  
  tbConnectOnLoad = false;

  aux.redirect("/_mqtt_setting");
  return String();
}

void handleThingsboard(){
  if (tb.connected()){ 
    return;
  }

  if(!WiFi.isConnected()){
    return;
  }

  if( nvsServer == "" && nvsAccessToken == "" && nvsDeviceId == "" ){
    return;
  }
    
  if (tbConnectOnLoad){
    Serial.println("Trying to automaticaly connect to mqtt");
    if(!tb.connect(nvsServer.c_str(),nvsAccessToken.c_str(),nvsPort.toInt(),nvsDeviceId.c_str(),nvsPassword.c_str())){
      Serial.println("Failed");
      tbConnectOnLoad = false;
      return;
    }else{
      Serial.println("Success");
    }
  }
  return;
}

void setup() {
  delay(1000);
  Serial.begin(9600);
  Serial.println();

  Portal.join({MqttPage, MqttSavePage, MqttStartPage, MqttResetPage});

  Config.autoReconnect=1;

  Server.on("/", rootPage);
  Portal.on("/_mqtt_save", onMqttSavePage);
  Portal.on("/_mqtt_reset_settings", onMqttResetPage);
  MqttPage.on(onMqttLoadSetting);

  if (Portal.begin()) {
    Serial.println("WiFi connected: " + WiFi.localIP().toString());
  }

  nvs.begin();
  nvsServer = nvs.getString("MqServer");
  nvsAccessToken = nvs.getString("MqAccessToken");
  nvsPort = nvs.getString("MqPort");
  nvsDeviceId = nvs.getString("MqDeviceId");
  nvsPassword = nvs.getString("MqPassword");
}

void loop() {
  delay(1000);
  Portal.handleClient();
  handleThingsboard();

  tb.sendTelemetryInt("temperature", random(120));
  tb.loop();
}