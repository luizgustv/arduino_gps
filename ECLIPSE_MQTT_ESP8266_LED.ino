//Programa: NodeMCU e MQTT - Controle e Monitoramento IoT do LED da placa

#include <TinyGPS.h> //importa a bliblioteca para manipular os dados do GPS
#include <SoftwareSerial.h> //com essa biblioteca pode-se fazer uma porta atuar como uma entrada TX/RX
#include <ESP8266WiFi.h> // Importa a Biblioteca ESP8266WiFi
#include <PubSubClient.h> // Importa a Biblioteca PubSubClient responsável por:

//definição de ID e TÓPICOS para publicação e assinatura
//capitalInicial
#define TOPICO_SUBSCRIBE "assets/location"    //tópico MQTT de envio do ASSINANTE (WEBSOQUETT) p/ ESP, isto é manda LIGAR (L) ou DESLIGAR (D) o D0 (LED)
#define TOPICO_PUBLISH   "assets/location"    //tópico MQTT de envio de informações do ESP p/ Broker confirmando se D0 foi ligado ou desligado
                                       //IMPORTANTE: recomendamos fortemente alterar os nomes
                                       //            desses tópicos. Caso contrário, há grandes
                                       //            chances de você controlar e monitorar o NodeMCU
                                       //            de outra pessoa.
#define ID_MQTT  "mqttjs_b9ce4ad1"     //ID MQTT (para identificação de sessão)
                             //IMPORTANTE:  este deve ser único no broker, ou seja, 
                             //             se um client MQTT (ESP DE OUTRO GRUPO) 
                             //             tentar entrar com o mesmo ID_MQTT já conectado ao Broker,
                             //             o broker fechará a conexão de um deles ou ambos ESP não funcionarão
                                                       
                                
//defines - mapeamento de pinos do NodeMCU
#define D0    16
#define D1    5
#define D2    4
#define D3    0
#define D4    2
#define D5    14
#define D6    12
#define D7    13
#define D8    15
#define D9    3
#define D10   1
#define PINLED 2      //caso o LED não pisque, troque o "2" por "LED_BUILTIN"

//CHALLENGER
//zkdt5353
 
// WIFI
<<<<<<< HEAD
const char* SSID = "LGustavo";            // SSID é o nome da rede WI-FI que deseja se conectar
const char* PASSWORD = "GSTV18520#l";            // Senha da rede WI-FI que deseja se conectar

=======
const char* SSID = "SSID";            // SSID é o nome da rede WI-FI que deseja se conectar
const char* PASSWORD = "PASSWORD";            // Senha da rede WI-FI que deseja se conectar
  
>>>>>>> bf0e494237966fa63f8ed174a0e9daaa8de05e7e
// MQTT
const char* BROKER_MQTT = "tailor.cloudmqtt.com"; //URL padrão do broker MQTT gratuito da ECLIPSE
int BROKER_PORT = 14970;                       // Porta padrão do Broker MQTT acesse: http://mqtt.eclipse.org/
const char* MQTT_USERNAME = "likyjqkw";
const char* MQTT_PASSWORD = "Zwx1-XnEvWgV";

//Variáveis e objetos globais
WiFiClient espClient;                         // Cria o objeto espClient
PubSubClient MQTT(espClient);                 // Instancia o Cliente MQTT passando o objeto espClient
char EstadoSaida = '0';                       //variável que armazena o estado atual da saída do LED

//Funções Auxiliares
void initSerial();
void initWiFi();
void initMQTT();
void reconectWiFi(); 
void mqtt_callback(char* topic, byte* payload, unsigned int length);
void VerificaConexoesWiFIEMQTT(void);
void initOutput(void);

//GPS e SoftwareSerial
TinyGPS gps;
SoftwareSerial ss(4, 5);

void setup()                                  //essa linha é padrão em todos os programas de Arduino
{
    initOutput();                             //carrega essa função initOutput()
    initSerial();                             //carrega essa função initSerial()
    initWiFi();                               //carrega essa função initWiFi()
    initMQTT();                               //carrega essa função initMQTT()
    
    randomSeed(analogRead(0)); 
}

//========================================================
//Função: inicializa a saída do LED como "desligado"
//Parâmetros: nenhum
//Retorno: nenhum
void initOutput(void)
{
    //IMPORTANTE: o Led já contido na placa é acionado com lógica invertida (ou seja,
    //enviar HIGH para o output faz o Led apagar / enviar LOW faz o Led acender)
    pinMode(PINLED, OUTPUT);
    digitalWrite(PINLED, HIGH);          
}

//========================================================
//Função: inicializa comunicação serial com baudrate 115200 (para fins de monitorar no terminal serial 
//        o que está acontecendo.
//Parâmetros: nenhum
//Retorno: nenhum
void initSerial() 
{
    Serial.begin(115200);
    ss.begin(9600);
}

//========================================================
//Função: inicializa e conecta-se na rede WI-FI desejada
//Parâmetros: nenhum
//Retorno: nenhum
void initWiFi() 
{
    delay(10);
    Serial.println("------Conexao WI-FI------");
    Serial.print("Conectando-se na rede: ");
    Serial.println(SSID);
    Serial.println("Aguarde"); 
    reconectWiFi();
}

//========================================================
//Função: inicializa parâmetros de conexão MQTT(endereço do 
//        broker, porta e seta função de callback)
//Parâmetros: nenhum
//Retorno: nenhum
void initMQTT() 
{
    MQTT.setServer(BROKER_MQTT, BROKER_PORT);   //informa qual broker e porta deve ser conectado
    MQTT.setCallback(mqtt_callback);            //atribui função de callback (função chamada quando qualquer informação de um dos tópicos subescritos chega)   
}

//========================================================
//Função: função de callback 
//        esta função é chamada toda vez que uma informação de 
//        um dos tópicos assinados chega)
//Parâmetros: nenhum
//Retorno: nenhum
void mqtt_callback(char* topic, byte* payload, unsigned int length) 
{
    String msg; 
    //obtem a string do payload recebido
    for(int i = 0; i < length; i++) 
    {
       char c = (char)payload[i];
       msg += c;
    }
   
    //toma ação dependendo da string recebida:
    //verifica se deve LIGAR (com LOW) D0:
    //IMPORTANTE: o Led já contido na placa é acionado com lógica invertida (ou seja,
    //enviar HIGH para o output faz o Led apagar / enviar LOW faz o Led acender)
    if (msg.equals("L"))
    {
        digitalWrite(PINLED, LOW);
        EstadoSaida = '1';
    }
 
    //verifica se deve colocar nivel alto de tensão na saída D0:
    if (msg.equals("D"))
    {
        digitalWrite(PINLED, HIGH);
        EstadoSaida = '0';
    }
}

//========================================================
//programa principal
void loop() 
{   
    VerificaConexoesWiFIEMQTT();        //garante funcionamento das conexões WiFi e ao broker MQTT
    EnviaEstadoOutputMQTT();            //envia o status de todos os outputs para o Broker no protocolo esperado   
    MQTT.loop();                        //keep-alive da comunicação com broker MQTT
}

//========================================================
//Função: reconecta-se ao WiFi
//Parâmetros: nenhum
//Retorno: nenhum
void reconectWiFi() 
{
    //se já está conectado a rede WI-FI, nada é feito. 
    //Caso contrário, são efetuadas tentativas de conexão
    if (WiFi.status() == WL_CONNECTED) return;
    WiFi.begin(SSID, PASSWORD); // Conecta na rede WI-FI     
    while (WiFi.status() != WL_CONNECTED) 
    {
        delay(100);
        Serial.print(".");
    }
    Serial.println();
    Serial.print("Conectado com sucesso na rede ");
    Serial.print(SSID);
    Serial.println("IP obtido: ");
    Serial.println(WiFi.localIP());
}

//========================================================
//Função: verifica o estado das conexões WiFI e ao broker MQTT. 
//        Em caso de desconexão (qualquer uma das duas), a conexão
//        é refeita.
//Parâmetros: nenhum
//Retorno: nenhum
void VerificaConexoesWiFIEMQTT(void)
{
    if (!MQTT.connected()) reconnectMQTT(); //se não há conexão com o Broker, a conexão é refeita
    reconectWiFi(); //se não há conexão com o WiFI, a conexão é refeita
}

//========================================================
//Verifica se um novo dado está disponível a cada ms
static void smartdelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (ss.available())
      gps.encode(ss.read());
  } while (millis() - start < ms);
}

//========================================================
//Função: envia ao Broker o estado atual do D0 (led), isto é, confirma se o LED está aceso ou apagado
//enviando um TÓPICO PUBLICADOR 
//Parâmetros: nenhum
//Retorno: nenhum
void EnviaEstadoOutputMQTT(void)
{
    float flat, flon;
    unsigned long age;
    
    if (EstadoSaida == '0') 
    {
       float temperatura = float(random(160, 300)) / -10.0;
       
       gps.f_get_position(&flat, &flon, &age);

       String value = "{\"gps_serial_number\":\"9488FDJADF-00\",\"latitude\":" + 
        String(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6) + ",\"longitude\":" + 
        String(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6) + ",\"metadata\":" + 
        "{ \"tempeture\": \"" + String(temperatura) + "°C\" }" + 
       "} ";

       char valueChar[value.length()];
       value.toCharArray(valueChar, value.length());
       
       MQTT.publish(TOPICO_PUBLISH, valueChar);

       Serial.println(valueChar);
      
      //MQTT.publish(TOPICO_PUBLISH, "latitude:");
      //Serial.println("Seu TÓPICO PUBLISH - que indica estado do PINLED (L ou D) foi enviado ao broker: D");
    }
    
    if (EstadoSaida == '1') 
    {
      Serial.println("Seu TÓPICO PUBLISH - que indica estado do PINLED (L ou D) foi enviado ao broker: L");
      MQTT.publish(TOPICO_PUBLISH, "L");
    }
    //delay(12000);
   
    smartdelay(180000); //300000 msa cada 5 min vai ser feito uma leitura
}

//========================================================
//Função: reconecta-se ao broker MQTT (caso ainda não esteja conectado ou em caso da conexão cair)
//em caso de sucesso na conexão ou reconexão, a assinatura dos tópicos é refeita.
//Parâmetros: nenhum
//Retorno: nenhum
void reconnectMQTT() 
{
    while (!MQTT.connected()) 
    {
        Serial.print("* Tentando se conectar ao Broker MQTT: ");
        Serial.println(BROKER_MQTT);
        if (MQTT.connect(ID_MQTT, MQTT_USERNAME, MQTT_PASSWORD)) 
        {
            Serial.println("Conectado com sucesso ao broker MQTT!");
            MQTT.subscribe(TOPICO_SUBSCRIBE); 
        } 
        else
        {
            Serial.println("Falha ao reconectar no broker.");
            Serial.println("Havera nova tentatica de conexao em 2s");
            delay(2000);
        }
    }
}
