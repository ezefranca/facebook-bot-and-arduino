'use strict'


const express = require('express')
const bodyParser = require('body-parser')
const request = require('request')
const app = express()
var lampadaLigada = new Boolean(false);

app.set('port', (process.env.PORT || 5000))

// Process application/x-www-form-urlencoded
app.use(bodyParser.urlencoded({extended: false}))

// Process application/json
app.use(bodyParser.json())

// Index route
app.get('/', function (req, res) {
    res.send('Olá amiguinho, tudo bem?')
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

app.post('/webhook/', function (req, res) {
    let messaging_events = req.body.entry[0].messaging
    for (let i = 0; i < messaging_events.length; i++) {
        let event = req.body.entry[0].messaging[i]
        let sender = event.sender.id
        if (event.message && event.message.text) {
            let text = event.message.text
            
            if(text == "ligar"){
                var fs = require('fs');
                fs.writeFile("/status/", "on", function(err) {
                    if(err) {
                        return console.log(err);
                    }

                    console.log("The file was saved!");
                }); 
                sendTextMessage(sender, "Ligando a lampada 💡💡")
                lampadaLigada = true;
                sendJsonData(req, res, lampadaLigada)
                return res.send({"status": "on"});
            }
            else if(text == "desligar"){

                var fs = require('fs');
                fs.writeFile("/status/", "off", function(err) {
                    if(err) {
                        return console.log(err);
                    }

                    console.log("The file was saved!");
                }); 

                sendTextMessage(sender, "Desligando a lampada 🔌")
                lampadaLigada = false;
                sendJsonData(req, res, lampadaLigada)
                return res.send({"status": "off"});
            }
            else {
                sendTextMessage(sender, "Você me disse " + text.substring(0, 200) + " " + "... hmm, não entendi...")
            }
        }
    }
    res.sendStatus(200)
})

const token = "EAAK38x1SRkUBAGeDEOtxnleV175oEcmSphJhbOKGGcDpZAHE0JDnXOk94mZCkG10x8C3Njps6lKhuxpCc7hkOXuCnMQo7U8r5tCoOnn7TalkrQUhKsiYKWxNMwUZBUAPEDKRNJhqLlOLqFEbIn2VIy7qoQSJFG4JJ2ME10vJQZDZD"

function sendJsonData(req, res, status) {
    app.get('/', function (req, res) {
        let data = {"status" : status}
        res.send(data)
    })
}

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
