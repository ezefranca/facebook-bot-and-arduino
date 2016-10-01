# Facebook Messenger + Arduino Intel Galileo 2 
Comunicação entre um bot do messenger do Facebook e Arduino.

![Logo](https://raw.githubusercontent.com/ezefranca/ezefranca.github.io/master/img/facebook-chatbot-galileo.png)

# 🤖 Criando o Bot no Facebook Messenger

O Facebook recentemente abriu sua plataforma Messenger para permitir bots para conversar com os usuários através do Facebook Apps e do Facebook Pages.

Você pode ver a [documentação] completa (https://developers.facebook.com/docs/messenger-platform/quickstart).

O Arduino Intel Galileo 2 é a evolução da placa Intel Galileo. É uma placa certificada Arduino. Vamos utiliza-la para receber dados de um servidor da Web e controlar uma lampada.  

## 🙌 Preparando o Bot

Os bots do Messenger usam um servidor web para processar mensagens que recebe e para as mensagens que envia. Neste tutorial utilizares o Heroku rodando uma aplicação NodeJS. 

### * Construindo o servidor *

Para este projeto utilizaremos o Heroku.  

1. Instale o toolbelt Heroku daqui https://toolbelt.heroku.com para comitar, parar e monitorar os apps que você tiver. Registre-se gratuitamente no https://www.heroku.com se você não tiver uma conta ainda.

2. Instale o NodeJS a partir do site oficial https://nodejs.org, este será o ambiente de servidor. Em seguida, abra o Terminal ou linha de comando e verifique se você tem a versão mais recente do NPM, instalando ele novamente:

    ```
    sudo npm install npm -g
    ```

3. Crie uma nova pasta em algum lugar e vamos criar um novo projeto Node. Pressione Enter para aceitar os padrões, ou preencha como quiser. 

    ```
    npm init
    ```

4. Instale as dependências adicionais do node. O Express é para o servidor, o Request é para o envio de mensagens e body-parser é para processar as mensagens.

    ```
    npm install express request body-parser --save
    ```

5. Crie um arquivo index.js na pasta e utilize este esqueleto para ele. Vamos começar autenticando o bot.

    ```javascript
    'use strict'
    
    const express = require('express')
    const bodyParser = require('body-parser')
    const request = require('request')
    const app = express()

    app.set('port', (process.env.PORT || 5000))

    // Process application/x-www-form-urlencoded
    app.use(bodyParser.urlencoded({extended: false}))

    // Process application/json
    app.use(bodyParser.json())

    // Index Route
    app.get('/', function (req, res) {
    	res.send('Olá, Eu sou um bot')
    })

    // para verificacao do Facebook
    app.get('/webhook/', function (req, res) {
    	if (req.query['hub.verify_token'] === 'my_voice_is_my_password_verify_me') {
    		res.send(req.query['hub.challenge'])
    	}
    	res.send('Error, wrong token')
    })

    // Spin up the server
    app.listen(app.get('port'), function() {
    	console.log('running on port', app.get('port'))
    })
    ```

6. Faça um arquivo chamado Procfile e copie isto. O Procfile é assim para o Heroku poder saber que arquivo executar.

    ```
    web: node index.js
    ```

7. Vamos criar um repositório com Git, adicionar nosso coódigo e em seguida, criar uma nova instância no Heroku e fazer o deploy do nosso código para a nuvem.

    ```
    git init
    git add .
    git commit --message 'primeiro commit'
    heroku create
    git push heroku master
    ```

### * Setup do Facebook App para o Bot *

1. Criar ou configurar uma Página ou App do Facebook aqui https://developers.facebook.com/apps/

 ![Alt text](https://github.com/TechThings/messenger-bot-tutorial/raw/master/demo/shot1.jpg)

2. No aplicativo vá para a aba Messenger, clique em Configurar webhook. Aqui você vai colocar no URL do seu servidor Heroku (que foi gerada anteriormente no commit) e o token. Certifique-se de verificar todos os campos.

![Alt text](https://github.com/TechThings/messenger-bot-tutorial/raw/master/demo/shot3.jpg)

3. Você vai precisar agora de um token de página do Facebook. Obtenha um e salve em algum lugar.

 ![Alt text](https://github.com/TechThings/messenger-bot-tutorial/raw/master/demo/shot2.jpg)

4. Volte para o Terminal e digite este comando para ativar o aplicativo Facebbook para enviar mensagens. Lembre-se de usar o token de pagina solicitado no passo anterior.

    ```bash
    curl -X POST "https://graph.facebook.com/v2.6/me/subscribed_apps?access_token=<PAGE_ACCESS_TOKEN>"
    ```

### *Setup do bot*

Agora que o Facebook e o Heroku podem falar um com o outro podemos escrever o código básico do bot.

1. Adicione um ponto final API para index.js para processar mensagens. Lembre-se de incluir também o token de página que tinhamos pegado antes.

    ```javascript
    app.post('/webhook/', function (req, res) {
	    let messaging_events = req.body.entry[0].messaging
	    for (let i = 0; i < messaging_events.length; i++) {
		    let event = req.body.entry[0].messaging[i]
		    let sender = event.sender.id
		    if (event.message && event.message.text) {
			    let text = event.message.text
			    sendTextMessage(sender, "Texto recebido foi: " + text.substring(0, 200))
		    }
	    }
	    res.sendStatus(200)
    })

    const token = "<PAGE_ACCESS_TOKEN>"
    ```
2. Adicione a função para replicar as mensagens

    ```javascript
    function sendTextMessage(sender, text) {
	    let messageData = { text:text }
	    request({
		    url: 'https://graph.facebook.com/v2.6/me/messages',
		    qs: {access_token:token},
		    method: 'POST',
    		json: {
			    recipient: {id:sender},
    			message: messageData,
    		}
    	}, function(error, response, body) {
    		if (error) {
			    console.log('Error sending messages: ', error)
    		} else if (response.body.error) {
			    console.log('Error: ', response.body.error)
		    }
	    })
    }
    ```

4. Comitar o código novamente e dar push para Heroku

    ```
    git add .
    git commit -m 'updated the bot to speak'
    git push heroku master
    ```

5. Vá para a página do Facebook e clique na mensagem para começar a conversar!

![Alt text](https://github.com/TechThings/messenger-bot-tutorial/raw/master/demo/shot4.jpg)

## Receptando algumas palavras e interagindo

### *Filtrando comandos*

Como o sistema se comunida através do webhook, vamos alterar nossas função para lidar com duas entradas: *ligar* e *desligar*

(bem porco, aqui poderiamos utilizar IA, um processador de linguagem natural ou qualquer tecnologia mais robusta para interpretar as mensagens).

```javascript
app.post('/webhook/', function (req, res) {
    let messaging_events = req.body.entry[0].messaging
    for (let i = 0; i < messaging_events.length; i++) {
        let event = req.body.entry[0].messaging[i]
        let sender = event.sender.id
        if (event.message && event.message.text) {
            let text = event.message.text
            
            if(text == "ligar"){
                sendStatus("on")
                sendTextMessage(sender, "Ligando a lampada 💡💡")
                lampadaLigada = true;
                sendJsonData(req, res, lampadaLigada)
                return res.send({"status": "on"});
            }
            else if(text == "desligar"){
                sendStatus("off")
                sendTextMessage(sender, "Desligando a lampada 🔌")
                lampadaLigada = false;
                sendJsonData(req, res, lampadaLigada)
                return res.send({"status": "off"});
            }
            else if(text == "status") {
                sendTextMessage(sender, "Estado da lampada:" + lampadaLigada)
            }
            else {
                sendTextMessage(sender, "Você me disse " + text.substring(0, 200) + " " + "... hmm, não entendi...")
            }
        }
    }
    res.sendStatus(200)
})
 ```
 
## Configurando nosso sistema de controle da lampada (Intel Galieo) 

### *Esquema elétrico* 

*Atenção: este circuito vai ser alimentado por uma tensão elétrica de 127v ou 220v.
![alerta](http://www.seton.com.br/media/catalog/product/cache/1/small_image/50x50/9df78eab33525d08d6e5fb8d27136e95/m/6/m6515-ba.jpg)

 *Pino 9* - Sinal do Rele
 *GND*
 *VCC*
 
![circuito](https://raw.githubusercontent.com/ezefranca/facebook-bot-and-arduino/master/images/Lampada.png)
 
 
 ### *Pulling do Arduino no serviço Web* 
 
 
 ```arduino
 byte mac[] = { 0x98, 0x4F, 0xEE, 0x05, 0x44, 0x9B };
 
 void setup() {
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
  }
  Ethernet.begin(mac, ip);
  Serial.begin(9600);
  system("telnetd -l /bin/sh"); //Start the telnet server on Galileo
  system("ifconfig eth0 10.1.102.33 netmask 255.255.255.0 up");
  system("route add default gw 10.1.102.254 eth0");
  system("echo 'nameserver 192.168.60.81' > /etc/resolv.conf");
  while (!Serial) {
    ; 
  }
  delay(3000);
  Serial.println("connecting");
  Serial.println(client);
  delay(3000);
  
  if (client.connect(server, 80)) {
    Serial.println("connected");
    // Make a HTTP request:
    client.println("GET / HTTP/1.0");
    //client.println();
    httpRequest();
  }
  else {
    // if you didn’t get a connection to the server:
    Serial.println("connection failed");
  }
}
 
 
 void httpRequest() {
  // Encerra todas conexoes para um novo request
  client.stop();

  // Se conectou com sucesso
  if (client.connect(server, 80)) {
    //Serial.println("conectandoooo...");
    // mandando aquele GET 
    client.println("GET / HTTP/1.1");
    client.println("Host: lampada-fiap.herokuapp.com");
    client.println("User-Agent: arduino-ethernet");
    client.println("Connection: close");
    client.println();

    // Pra saber quanto tempo levou
    lastConnectionTime = millis();
  } else {
    // Se der merda
    Serial.println("connection failed");
  }
}
 ```
 
 
