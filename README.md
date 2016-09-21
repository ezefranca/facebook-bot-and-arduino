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

### *Send a Structured Message*

Facebook Messenger can send messages structured as cards or buttons. 

![Alt text](/demo/shot5.jpg)

1. Copy the code below to index.js to send an test message back as two cards.

    ```javascript
    function sendGenericMessage(sender) {
	    let messageData = {
		    "attachment": {
			    "type": "template",
			    "payload": {
    				"template_type": "generic",
				    "elements": [{
    					"title": "First card",
					    "subtitle": "Element #1 of an hscroll",
					    "image_url": "http://messengerdemo.parseapp.com/img/rift.png",
					    "buttons": [{
						    "type": "web_url",
						    "url": "https://www.messenger.com",
						    "title": "web url"
					    }, {
						    "type": "postback",
						    "title": "Postback",
						    "payload": "Payload for first element in a generic bubble",
					    }],
				    }, {
					    "title": "Second card",
					    "subtitle": "Element #2 of an hscroll",
					    "image_url": "http://messengerdemo.parseapp.com/img/gearvr.png",
					    "buttons": [{
						    "type": "postback",
						    "title": "Postback",
						    "payload": "Payload for second element in a generic bubble",
					    }],
				    }]
			    }
		    }
	    }
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

### *Act on what the user messages*

What happens when the user clicks on a message button or card though? Let's update the webhook API one more time to send back a postback function.

```javascript  
  app.post('/webhook/', function (req, res) {
    let messaging_events = req.body.entry[0].messaging
    for (let i = 0; i < messaging_events.length; i++) {
      let event = req.body.entry[0].messaging[i]
      let sender = event.sender.id
      if (event.message && event.message.text) {
  	    let text = event.message.text
  	    if (text === 'Generic') {
  		    sendGenericMessage(sender)
  		    continue
  	    }
  	    sendTextMessage(sender, "Text received, echo: " + text.substring(0, 200))
      }
      if (event.postback) {
  	    let text = JSON.stringify(event.postback)
  	    sendTextMessage(sender, "Postback received: "+text.substring(0, 200), token)
  	    continue
      }
    }
    res.sendStatus(200)
  })
```

Git add, commit, and push to Heroku again.

Now when you chat with the bot and type 'Generic' you can see this.

   ![Alt text](/demo/shot6.jpg)

## 📡 How to share your bot

### *Add a chat button to your webpage*

Go [here](https://developers.facebook.com/docs/messenger-platform/plugin-reference) to learn how to add a chat button your page.

### *Create a shortlink*

You can use https://m.me/<PAGE_USERNAME> to have someone start a chat.

## 💡 What's next?

You can learn how to get your bot approved for public use [here](https://developers.facebook.com/docs/messenger-platform/app-review).

## How I can help

I build and design bots all day. Email me for help!
