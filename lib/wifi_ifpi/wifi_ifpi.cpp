#include "wifi_ifpi.h"
#include <ArduinoJson.h> // Adicione esta biblioteca no seu platformio.ini

// Variável global para saber se precisamos salvar
bool shouldSaveConfig = false;

// Função que o WiFiManager chama quando clicamos em "Save" no celular
void saveConfigCallback() {
    Serial.println("Novas configurações recebidas do portal!");
    shouldSaveConfig = true;
}

WifiIFPI::WifiIFPI() {
    server = new AsyncWebServer(80);
    // IP padrão caso o arquivo não exista ainda
    strcpy(mqtt_server_ip, "192.168.18.189"); 
}

void WifiIFPI::iniciar() {
    // 1. Tenta ler o IP salvo anteriormente no LittleFS
    if (LittleFS.begin(true)) {
        if (LittleFS.exists("/config.json")) {
            File configFile = LittleFS.open("/config.json", "r");
            if (configFile) {
                size_t size = configFile.size();
                std::unique_ptr<char[]> buf(new char[size]);
                configFile.readBytes(buf.get(), size);
                
                StaticJsonDocument<200> json;
                deserializeJson(json, buf.get());
                strcpy(mqtt_server_ip, json["mqtt_server_ip"]);
                Serial.print("IP carregado da memoria: ");
                Serial.println(mqtt_server_ip);
                configFile.close();
            }
        }
    }

    // 2. Configura o WiFiManager
    WiFiManager wm;
    
    // Configura o Callback de salvamento
    wm.setSaveConfigCallback(saveConfigCallback);

    WiFiManagerParameter custom_mqtt_server("server", "IP do Broker Mosquitto", mqtt_server_ip, 40);
    wm.addParameter(&custom_mqtt_server);

    Serial.println("A abrir portal Guarita_IFPI (Senha: 12345678)...");
    
    // Tenta conectar. Se falhar, abre o portal por 3 minutos (180s) e depois reinicia
    wm.setConfigPortalTimeout(180);
    if (!wm.autoConnect("Guarita_IFPI", "12345678")) {
        Serial.println("Falha na conexao. Reiniciando...");
        delay(3000);
        ESP.restart();
    }

    // 3. Se o usuário mudou o IP no portal, gravamos no arquivo agora!
    strcpy(mqtt_server_ip, custom_mqtt_server.getValue());

    if (shouldSaveConfig) {
        Serial.println("A gravar novo IP no LittleFS...");
        StaticJsonDocument<200> json;
        json["mqtt_server_ip"] = mqtt_server_ip;

        File configFile = LittleFS.open("/config.json", "w");
        serializeJson(json, configFile);
        configFile.close();
        Serial.println("IP salvo com sucesso!");
    }

    Serial.println("Wi-Fi Conectado!");
    server->serveStatic("/", LittleFS, "/").setDefaultFile("index.html");
    server->begin();
}

char* WifiIFPI::getMqttServerIp() {
    return mqtt_server_ip;
}