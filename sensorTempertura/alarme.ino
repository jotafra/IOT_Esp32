
void handleAlarme(AdafruitIO_Data *data){
  String valor = data -> toString();

  Serial.print(F("Valor recebido do feed: "));
  Serial.print(valor);

  if (valor == "true"){
    alarmeAtivo = true;
    Serial.print("Alarme ARMADO pelo dash / app");
  } else {
    alarmeAtivo = false;
    Serial.println("Alarme DESARMADO pelo dash / app!");
  }
}

void ativarAlerta(){
  digitalWrite(LED_ALARMEG, LOW);
  digitalWrite(BUZZER_PIN, HIGH);
  digitalWrite(LED_ALARMER, HIGH);

}

void desligarAlerta(){
  digitalWrite(LED_ALARMEG, HIGH);
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(LED_ALARMER, LOW);
}

