# Sistem automat de udare a plantelor (IoT – MQTT)

**Universitatea „POLITEHNICA” Timișoara**  
**Departamentul de ID/IFR și educație digitală (DeL) – Informatică**  
**Disciplina:** Proiect Sincretic I A – Sisteme cu microcontrolere și IoT industriale  
**Student:** Carmazan Ovidiu Dan  

---

## Descrierea proiectului
Acest proiect implementează un sistem automat de udare a plantelor bazat pe microcontrolerul **ESP32** și protocolul **MQTT**. Sistemul monitorizează în timp real umiditatea solului, temperatura și umiditatea ambientală și acționează automat pompa de apă atunci când solul este uscat.

### Funcționalități principale
* **Mod Automat (`AUTO`):** Pompa pornește automat când umiditatea solului scade sub **35%** și se oprește când depășește **50%** (histerezis de 15%).
* **Mod Manual (`ON` / `OFF`):** Utilizatorul poate porni sau opri pompa de la distanță prin comenzi MQTT.
* **Configurare prag:** Pragul de umiditate poate fi modificat de la distanță cu comanda `PRAG:<valoare>`.
* **Simulare și testare:** Realizate în **Wokwi** (`wokwi.com`) și testate cu **HiveMQ Web Client**.

---

## Componente utilizate (Wokwi)
* **ESP32 DevKit v1** – microcontroler cu Wi-Fi integrat
* **Potențiometru** (`GPIO34`) – simulează senzorul de umiditate a solului
* **Senzor DHT22** (`GPIO15`) – măsoară temperatura și umiditatea aerului
* **Modul Releu** (`GPIO26`) și **LED albastru** (`GPIO27`) – simulează pompa de apă

---

## Configurare MQTT
* **Broker:** `broker.hivemq.com` (port `1883` / `8884` WebSockets)
* **Device ID:** `ESP32_OVIDIU_01`
* **Topic senzori (Publish):** `irigare/OVIDIU/senzori`
* **Topic comenzi (Subscribe):** `irigare/OVIDIU/comanda` (`ON`, `OFF`, `AUTO`, `PRAG:40`)
* **Topic status (Publish):** `irigare/OVIDIU/status`

---

## Structura repository-ului
* `sketch.ino` – codul sursă pentru ESP32
* `diagram.json` – schema circuitului din Wokwi
* `libraries.txt` – bibliotecile Arduino necesare (`PubSubClient`, `DHT sensor library`, `ArduinoJson`)
* `README.md` – prezentarea proiectului

## Link simulare Wokwi
* [Deschide proiectul în Wokwi](Adaugă_aici_linkul_tău_de_la_SHARE_din_Wokwi)
