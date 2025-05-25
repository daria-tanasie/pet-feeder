#include <Arduino.h>
#include <WiFi.h>
#include <ESP_Mail_Client.h>
#include "HardwareSerial.h"

const char* id = "hopa, nu ai net?";
const char* pass = "poiu0987";

#define SMTP_server "smtp.gmail.com"
#define SMTP_Port 465

#define sender_email "baubau.miau32@gmail.com"
#define sender_pass "movc ajcy uhse biin"

#define rec_email "tanasiedaria.dodo@gmail.com"
#define rec_name "owner"

SMTPSession smtp;

HardwareSerial espSer(2);
int cnt = 0;

void setup() {
  Serial.begin(9600);
  espSer.begin(9600, SERIAL_8N1, 16, 17);

  Serial.print("Connecting...");
  WiFi.begin(id, pass);

  while (WiFi.status() != WL_CONNECTED)
  { 
    Serial.print(".");
    delay(200);
  }

  Serial.println("WiFi connected.");
  smtp.debug(1);

}

void loop() {
  if (espSer.available()) {
    String msg = espSer.readStringUntil('\n');
    msg.trim();

    if (msg == "send_msg") {
      espSer.println("ACK");
      ESP_Mail_Session session;
      session.server.host_name = SMTP_server;
      session.server.port = SMTP_Port;
      session.login.email = sender_email;
      session.login.password = sender_pass;
      session.login.user_domain = "";

      SMTP_Message message;
      message.sender.name = "Pet feeder";
      message.sender.email = sender_email;
      message.subject = "Notification!";
      message.addRecipient(rec_name,rec_email);

      String htmlMsg = "<div style=\"color: #000000;\"><h1> Food was served!</h1><p> The pet is happy!:D</p></div>";
      message.html.content = htmlMsg.c_str();
      message.html.content = htmlMsg.c_str();
      message.text.charSet = "us-ascii";
      message.html.transfer_encoding = Content_Transfer_Encoding::enc_7bit;

      if (!smtp.connect(&session))
        return;
      if (!MailClient.sendMail(&smtp, &message))
        Serial.println("Error sending Email");
    }
  }
}