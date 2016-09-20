# facebook-bot-and-arduino
Comunicação entre um bot do messenger do Facebook e Arduino apresentado no MBA da Fiap

# 🤖 Crindo o Bot no Facebook Messenger

! [Texto Alt] (/ demo / Demo.gif)

O Facebook recentemente abriu sua plataforma Messenger para permitir bots para conversar com os usuários através do Facebook Apps e do Facebook Pages.

Você pode ler a [documentação] completa (https://developers.facebook.com/docs/messenger-platform/quickstart).

O Arduino Intel Galileo 2 é a evolução da placa Intel Galileo. É uma placa certificada Arduino. Vamos utiliza-la para receber dados de um servidor da Web e controlar uma lampada.  

## 🙌 Preparando o Bot

Os bots do Messenger usam um servidor web para processar mensagens que recebe e para as mensagens que envia. Neste tutorial utilizares o Heroku rodando uma aplicação NodeJS. 

### * Construindo o servidor *

1. Instale o toolbelt Heroku daqui https://toolbelt.heroku.com para lançar, parar e monitorar instâncias. Registre-se gratuitamente no https://www.heroku.com se você não tiver uma conta ainda.

2. Instale Nó a partir daqui https://nodejs.org, este será o ambiente de servidor. Em seguida, abra o Terminal ou linha de comando Prompt e certifique-se que você tem a versão muito mais recente do NPM, instalando-lo novamente:

    ```
    sudo npm install npm -g
    ```

3. Crie uma nova pasta em algum lugar e vamos criar um novo projeto Node. Pressione Enter para aceitar os padrões.

    ```
    npm init
    ```

4. Instale as dependências nó adicional. Express é para o servidor, o pedido é para o envio de mensagens e corpo-parser é processar mensagens.

    ```
    npm install express request body-parser --save
    ```

5. Crie um arquivo index.js na pasta e copiar este para ele. Vamos começar por autenticar o bot.

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

    // Index route
    app.get('/', function (req, res) {
    	res.send('Hello world, I am a chat bot')
    })

    // for Facebook verification
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

6. Faça um arquivo chamado Procfile e copiar este. Isto é assim Heroku pode saber o arquivo para executar.

    ```
    web: node index.js
    ```

7. Confirmar tudo o código com Git, em seguida, criar uma nova instância Heroku e empurre o código para a nuvem.

    ```
    git init
    git add .
    git commit --message 'hello world'
    heroku create
    git push heroku master
    ```

### * Setup o Facebook App *

1. Criar ou configurar um Facebook App ou página aqui https://developers.facebook.com/apps/

 ![Alt text](/demo/shot1.jpg)

2. No aplicativo vá para a guia Messenger, clique em Configurar webhook. Aqui você vai colocar no URL do seu servidor Heroku e um token. Certifique-se de verificar todos os campos de assinatura.

![Alt text](/demo/shot3.jpg)

3. Obter uma página de acesso token e salvar isto em algum lugar.

 ![Alt text](/demo/shot2.jpg)

4. Volte para o Terminal e digite este comando para acionar o aplicativo Facebbook para enviar mensagens. Lembre-se de usar o token solicitado anteriormente.

    ```bash
    curl -X POST "https://graph.facebook.com/v2.6/me/subscribed_apps?access_token=<PAGE_ACCESS_TOKEN>"
    ```

### * Setup o bot *

Agora que o Facebook e Heroku pode falar um com o outro que pode codificar o bot.

1. Adicione um ponto final API para index.js para processar mensagens. Lembre-se de incluir também o token chegamos mais cedo.

    ```javascript
    app.post('/webhook/', function (req, res) {
	    let messaging_events = req.body.entry[0].messaging
	    for (let i = 0; i < messaging_events.length; i++) {
		    let event = req.body.entry[0].messaging[i]
		    let sender = event.sender.id
		    if (event.message && event.message.text) {
			    let text = event.message.text
			    sendTextMessage(sender, "Text received, echo: " + text.substring(0, 200))
		    }
	    }
	    res.sendStatus(200)
    })

    const token = "<PAGE_ACCESS_TOKEN>"
    ```
    
    **Optional, but recommended**: keep your app secrets out of version control!
    - On Heroku, its easy to create dynamic runtime variables (known as [config vars](https://devcenter.heroku.com/articles/config-vars)). This can be done in the Heroku dashboard UI for your app **or** from the command line:
    ![Alt text](/demo/config_vars.jpg)
    ```bash
    heroku config:set FB_PAGE_ACCESS_TOKEN=fake-access-token-dhsa09uji4mlkasdfsd
    
    # view
    heroku config
    ```

    - For local development: create an [environmental variable](https://en.wikipedia.org/wiki/Environment_variable) in your current session or add to your shell config file.
    ```bash
    # create env variable for current shell session
    export FB_PAGE_ACCESS_TOKEN=fake-access-token-dhsa09uji4mlkasdfsd
    
    # alternatively, you can add this line to your shell config
    # export FB_PAGE_ACCESS_TOKEN=fake-access-token-dhsa09uji4mlkasdfsd
    
    echo $FB_PAGE_ACCESS_TOKEN
    ```
    
    - `config var` access at runtime
    ``` javascript
    const token = process.env.FB_PAGE_ACCESS_TOKEN
    ```
    
    
    
3. Adicione uma função para ecoar mensagens

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

4. Cometer o código novamente e empurrar para Heroku

    ```
    git add .
    git commit -m 'updated the bot to speak'
    git push heroku master
    ```

5. Vá para a página do Facebook e clique na mensagem para começar a conversar!

![Alt text](/demo/shot4.jpg)

## ⚙ Customize what the bot says

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

You can also connect an AI brain to your bot [here](https://wit.ai)

Read about all things chat bots with the ChatBots Magazine [here](https://medium.com/chat-bots)

You can also design Messenger bots in Sketch with the [Bots UI Kit](https://bots.mockuuups.com)!

## How I can help

I build and design bots all day. Email me for help!
