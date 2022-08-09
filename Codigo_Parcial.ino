#include <Keypad.h>
#include <string.h>
#include <SPI.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_Fingerprint.h>
#define linha  4
#define coluna  3
#define tamanho 123

byte mac[] = {0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED};
IPAddress ip(192,168,0,9);
IPAddress server(10,8,37,154);

WiFiClient WifiClient;
PubSubClient client(WifiClient);

struct estrutura_id 
{
  int id;
  int senha;
};

struct estrutura_id usuario[tamanho];

void iniciar()
{
  int i = 0;
  for(i = 0; i < tamanho; i++)
  {
      usuario[i].id = -1;
      usuario[i].senha = -1;    
  }
}

int busca()
{
  int i = 0;
  
  for(i = 0; i < tamanho; i++)
  {
    if(usuario[i].id == -1 && usuario[i].senha == -1)
    {
      return i;
    }
  }
  return -1;
}

void sobreescrever(int indice, int id, int senha)
{
  Serial.print("Em sobreescrever");
  
  usuario[indice].id = id;
  usuario[indice].senha = senha;

  
}

void imprimir()
{
  int i;
  for(i = 0; i < tamanho; i++)
  {
    Serial.print(i);
    Serial.print(": ");
    Serial.print( usuario[i].id);
    Serial.print(", Senha: ");
    Serial.print( usuario[i].senha);
    Serial.println();
  }
}

void callback(char *topic, byte *payload, unsigned int length) 
{
  char pl[length];

/*
   for(int i = 0; i < length; i++)
    {
      Serial.println(payload[i], HEX);
    }
*/
  
  memcpy(pl, payload, length);
  pl[length] = '\0'; 
  Serial.println();
  Serial.println(pl);
  Serial.println();
  
  
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  
  if(strcmp(topic,"adicionar") == 0) 
  {

    char *idrecebido = (char *) malloc(tamanho * sizeof(idrecebido));
    char *senharecebido =  (char *) malloc(tamanho * sizeof(senharecebido));

    idrecebido = strtok(pl, ",");
    senharecebido = strtok(NULL, ",");
    int id = atoi(idrecebido);
    int senha = atoi(senharecebido);
    
    if(busca() != -1)
    {
      sobreescrever(busca(), id, senha);
      imprimir();
      Serial.println("Entrei no outro IF KOROI");
      
    }
    else
    {
      Serial.println("Nenhum espaco para armazenar novo usuario");
    }

    Serial.println("\nEsse é o id recebido: ");
    Serial.println(id);
    
    Serial.println("\nEssa é a senha recebida:  ");
    Serial.println(senha);
    Serial.println("\nEssa é a senha recebida:  ");
    Serial.println(senharecebido);
    
  }
  Serial.println();
}

void reconnect() {
  int i = 0;
  
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Tentando se conectar ao MQTT ...");
    // Attempt to connect
    if (client.connect("ESP32_Cliente")) {
      Serial.println("conectado!");
      // Once connected, publish an announcement...
      client.publish("work","Olá Sistema");
      // ... and resubscribe
      client.subscribe("adicionar");
    } else {
      Serial.print("\nFalha ao conectar a rede, tentando novamente...\n");
      Serial.print(client.state());
      i++;
      }
      
      if(i > 3)
      {
      Serial.println("\nTentando Novamente em 5 segundos: \n");
      Serial.println("Tentativa:");
      Serial.println(i);
      Serial.println("\n");
      
      }
      
      // Wait 5 seconds before retrying
      delay(1000);
    
      if(i == 10)
      {
        Serial.print("\nServidor fora do ar, tentando novamente.\n");
        break;
    
    }
  }
}

String senha[5] = {"23568", "54321", "51234", "52134", "97412"};
int i = 0;
String entrada; 
char teclas[linha][coluna] =
{
      {'1', '2', '3'},
      {'4', '5', '6'},
      {'7', '8', '9'},
      {'*', '0', '#'} 
};

  byte pinosLinha[linha] = {23, 22, 4, 21};
  byte pinosColuna[coluna] = {19, 18, 5};

  Keypad teclado = Keypad(makeKeymap(teclas), pinosLinha, pinosColuna, linha, coluna); 


int teclado_senha(bool permi)
{
 char enteclado = teclado.getKey();
   
    if(enteclado)
    {
      Serial.println(enteclado);

      if(enteclado == '*')
      {
        entrada = "";
      }
      else if(enteclado == '#')
      {
        for(i = 0; i < 5; i++)
        {
           if(senha[i].equals(entrada))
           {
               Serial.print("\nSenha encontrada!\n");
               client.publish("outTopic","Acesso concedido por senha!!!!!!");
               permi = true;
               break;
           }
           else
           {
            Serial.print("\nSenha nao encontrada\n");
            permi = false;
            
           }
         }
        
        entrada = "";
        if(permi == false)
        {
          client.publish("outTopic","Acesso Negado por senha!!!!!");
        }
      
      }
        else
        {
          entrada += enteclado;
        }

     }
    
    return permi;
}
const char *ssid = "ifmaker";
const char *password = "Engenh@ri@s";

//const char *ssid = "Marcius Vinicius";
//const char *password = "35214241";

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
const uint32_t passwordf = 0x0;
Adafruit_Fingerprint fingerprintSensor = Adafruit_Fingerprint(&Serial2, passwordf);

void setupFingerprintSensor()
{
  //Inicializa o sensor
  fingerprintSensor.begin(57600);

  //Verifica se a senha está correta
  if(!fingerprintSensor.verifyPassword())
  {
    //Se chegou aqui significa que a senha está errada ou o sensor está problemas de conexão
    Serial.println(F("Não foi possível conectar ao sensor. Verifique a senha ou a conexão"));
    while(true);
  }
}

void printMenu()
{
  Serial.println();
  Serial.println(F("Digite um dos números do menu abaixo"));
  Serial.println(F("1 - Cadastrar digital"));
  Serial.println(F("2 - Verificar digital"));
  Serial.println(F("3 - Mostrar quantidade de digitais cadastradas"));
  Serial.println(F("4 - Apagar digital em uma posição"));
  Serial.println(F("5 - Apagar banco de digitais"));
}

String getCommand()
{
  while(!Serial.available()) delay(100);
  return Serial.readStringUntil('\n');
}

//Cadastro da digital
void storeFingerprint()
{
  Serial.println(F("Qual a posição para guardar a digital? (1 a 149)"));
  String strLocation = getCommand();
  int location = strLocation.toInt();
  if(location < 1 || location > 149)
  {
    
    Serial.println(F("Posição inválida"));
    return;
  }

  Serial.println(F("Encoste o dedo no sensor"));

  
  while (fingerprintSensor.getImage() != FINGERPRINT_OK);
  
  
  if (fingerprintSensor.image2Tz(1) != FINGERPRINT_OK)
  {
    Serial.println(F("Erro image2Tz 1"));
    return;
  }
  
  Serial.println(F("Tire o dedo do sensor"));

  delay(2000);

  
  while(fingerprintSensor.getImage() != FINGERPRINT_NOFINGER);

  
  Serial.println(F("Encoste o mesmo dedo no sensor"));
  
  while(fingerprintSensor.getImage() != FINGERPRINT_OK);
  
  if(fingerprintSensor.image2Tz(2) != FINGERPRINT_OK)
  {
    
    Serial.println(F("Erro image2Tz 2"));
    return;
  }

  if(fingerprintSensor.createModel() != FINGERPRINT_OK)
  {
    
    Serial.println(F("Erro createModel"));
    return;
  }

  if(fingerprintSensor.storeModel(location) != FINGERPRINT_OK)
  {
    //Se chegou aqui deu erro, então abortamos os próximos passos
    Serial.println(F("Erro storeModel"));
    return;
  }
  Serial.println(F("Sucesso!!!"));
}

//Verifica se a digital está cadastrada
void checkFingerprint()
{
  Serial.println(F("Encoste o dedo no sensor"));

  
  while (fingerprintSensor.getImage() != FINGERPRINT_OK);

  
  if (fingerprintSensor.image2Tz() != FINGERPRINT_OK)
  {
    
    Serial.println(F("Erro image2Tz"));
    return;
  }
  
  
  if (fingerprintSensor.fingerFastSearch() != FINGERPRINT_OK)
  {
    
    Serial.println(F("Digital não encontrada"));
    return;
  }
  Serial.print(F("Digital encontrada com confiança de "));
  Serial.print(fingerprintSensor.confidence);
  Serial.print(F(" na posição "));
  Serial.println(fingerprintSensor.fingerID);
}

//Exibe a quantidade salva
void printStoredFingerprintsCount()
{
  fingerprintSensor.getTemplateCount();
  Serial.print(F("Digitais cadastradas: "));
  Serial.println(fingerprintSensor.templateCount);
}

void deleteFingerprint()
{
  Serial.println(F("Qual a posição para apagar a digital? (1 a 149)"));

  //Lê o que foi digitado no monitor serial
  String strLocation = getCommand();

  //Transforma em inteiro
  int location = strLocation.toInt();

  //Verifica se a posição é válida ou não
  if(location < 1 || location > 149)
  {
    //Se chegou aqui a posição digitada é inválida, então abortamos os próximos passos
    Serial.println(F("Posição inválida"));
    return;
  }

  //Apaga a digital nesta posição
  if(fingerprintSensor.deleteModel(location) != FINGERPRINT_OK)
  {
    Serial.println(F("Erro ao apagar digital"));
  }
  else
  {
    Serial.println(F("Digital apagada com sucesso!!!"));
  }
}

// Deletar base de dados
void emptyDatabase()
{
  Serial.println(F("Tem certeza? (s/N)"));

  //Lê o que foi digitado no monitor serial
  String command = getCommand();

  //Coloca tudo em maiúsculo para facilitar a comparação
  command.toUpperCase();

  //Verifica se foi digitado "S" ou "SIM"
  if(command == "S" || command == "SIM")
  {
    Serial.println(F("Apagando banco de digitais..."));

    //Apaga todas as digitais
    if(fingerprintSensor.emptyDatabase() != FINGERPRINT_OK)
    {
      Serial.println(F("Erro ao apagar banco de digitais"));
    }
    else
    {
      Serial.println(F("Banco de digitais apagado com sucesso!!!"));
    }
  }
  else
  {
    Serial.println(F("Cancelado"));
  }
}

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void setup() 
{
   Serial.begin(9600);
   WiFi.begin(ssid, password);
   client.setServer(server, 1883);
   client.setCallback(callback);
   
 while (WiFi.status() != WL_CONNECTED) 
 {
  delay(1000);
  Serial.println("Conectando a rede wifi... ");
 }
 Serial.print("Conectado a rede WIFI: ");
 Serial.println(ssid);
 Serial.print("\n");
 Serial.print("Endereço MAC:  ");
 Serial.println(WiFi.macAddress());
 Serial.print("\n");
 Serial.print("O IP é:  ");
 Serial.println(WiFi.localIP());
 iniciar();

 setupFingerprintSensor();
}

void loop() 
{

    bool permi;
    teclado_senha(permi); 

    if (!client.connected()) 
    {
      reconnect();
    }
  client.loop();


  printMenu();
  //char command = teclado.getKey();
  char i = teclado.getKey();
  
  switch(i)
  {
    case 1:
      storeFingerprint();
      break;
    case 2:
      checkFingerprint();
      break;
    case 3:
      printStoredFingerprintsCount();
      break;
    case 4:
      deleteFingerprint();
      break;
    case 5:
      emptyDatabase();
      break;
    default:
      Serial.println(F("Opção inválida"));
      break;
  }

}
