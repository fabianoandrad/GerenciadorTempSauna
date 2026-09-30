# 🌡️ Sauna & Piscina — ESP32 IoT Dashboard

Sistema de monitoramento de **temperatura da sauna, temperatura da piscina e detecção de fogo**, desenvolvido com **ESP32** e uma interface web responsiva.

A aplicação disponibiliza um dashboard em modo **Dark**, desenvolvido para ser utilizado em **smartphones, computadores e TVs**, permitindo visualizar as informações de forma simples e com grande destaque visual.

---

## 📸 Sobre o projeto

O ESP32 funciona como um servidor web local, disponibilizando a interface através da rede Wi-Fi.

O sistema apresenta:

* 🌡️ Temperatura atual da sauna
* 🌡️ Temperatura atual da piscina
* 🔺 Temperatura máxima da sauna
* 🔺 Temperatura máxima da piscina
* 🔥 Status do sensor de detecção de fogo
* 🕐 Relógio em tempo real
* 📅 Data atual
* 📡 Status de conexão do ESP32
* 📱 Interface responsiva
* 🌙 Interface em modo Dark

---

## 🖥️ Interface

O dashboard foi projetado para funcionar em diferentes tamanhos de tela.

### TV / Monitor

A interface utiliza textos e números grandes para permitir a visualização das informações mesmo a uma certa distância.

```text
┌──────────────────────────────────────────────────────────┐
│  SAUNA & PISCINA                         21:48:32         │
│  Monitoramento                           30/09/2026       │
├──────────────────────────────────────────────────────────┤
│                                                          │
│   ┌────────────────────┐   ┌────────────────────┐       │
│   │       SAUNA        │   │      PISCINA       │       │
│   │                    │   │                    │       │
│   │      72.5 °C       │   │       28.4 °C      │       │
│   │                    │   │                    │       │
│   │ Máxima: 80.0 °C    │   │ Máxima: 32.0 °C    │       │
│   └────────────────────┘   └────────────────────┘       │
│                                                          │
│          ┌──────────────────────────────┐                │
│          │       🔥 DETECÇÃO DE FOGO    │                │
│          │                              │                │
│          │          ✓ SEM FOGO          │                │
│          └──────────────────────────────┘                │
│                                                          │
│                 ● ESP32 ONLINE                           │
└──────────────────────────────────────────────────────────┘
```

No smartphone, os cards são reorganizados verticalmente para facilitar a visualização e utilização em telas menores.

---

## ⚙️ Funcionamento

O ESP32 se conecta à rede Wi-Fi e inicia um servidor HTTP local.

```text
                 ┌─────────────────┐
                 │      ESP32       │
                 │                 │
                 │  Wi-Fi + HTTP   │
                 └────────┬────────┘
                          │
                          │ HTTP
                          ▼
                 ┌─────────────────┐
                 │   Navegador     │
                 │                 │
                 │  Dashboard Web  │
                 └─────────────────┘
```

O navegador realiza requisições à API disponibilizada pelo ESP32 para obter os dados dos sensores.

As informações recebidas são utilizadas pelo JavaScript para atualizar a interface sem a necessidade de recarregar a página.

---

## 🧩 Tecnologias utilizadas

### Hardware

* ESP32 DevKit V1
* Sensor de temperatura para sauna
* Sensor de temperatura para piscina
* Sensor de detecção de fogo
* LED integrado do ESP32

### Software

* C++
* Arduino Framework
* PlatformIO
* HTML5
* CSS3
* JavaScript
* ArduinoJson
* LittleFS
* WiFi
* WebServer

---

## 📁 Estrutura do projeto

O projeto utiliza o **LittleFS** para armazenar os arquivos da interface web separadamente do código principal.

```text
SaunaPiscina/
│
├── include/
│
├── lib/
│
├── src/
│   └── main.cpp
│
├── data/
│   ├── index.html
│   ├── style.css
│   └── script.js
│
├── platformio.ini
│
└── README.md
```

### `src/main.cpp`

Responsável pelo funcionamento do ESP32:

* Inicialização do Wi-Fi
* Inicialização do LittleFS
* Configuração do servidor HTTP
* Rotas da API
* Leitura dos sensores
* Controle das informações enviadas para o navegador

### `data/index.html`

Contém a estrutura da página web.

### `data/style.css`

Responsável pelo layout e aparência da aplicação:

* Dark Mode
* Cards
* Tipografia
* Responsividade
* Layout para TV, PC e smartphone
* Estados visuais de alerta

### `data/script.js`

Responsável pela interação da página:

* Comunicação com a API do ESP32
* Atualização das temperaturas
* Atualização do status do fogo
* Relógio
* Data
* Status da conexão

---

## 🌐 Comunicação com a API

A interface web se comunica com o ESP32 através de endpoints HTTP.

Exemplo de consulta:

```http
GET /api/status
```

Exemplo de resposta JSON:

```json
{
    "status": "online",
    "led": true,
    "tempSauna": 72.5,
    "maxTempSauna": 80.0,
    "tempPiscina": 28.4,
    "maxTempPiscina": 32.0,
    "fogo": false
}
```

O JavaScript interpreta os valores recebidos e atualiza os elementos correspondentes da página.

---

## 🔥 Detecção de fogo

O sistema possui um indicador visual específico para o sensor de detecção de fogo.

### Estado normal

```text
🔥 DETECÇÃO DE FOGO

✓ SEM FOGO
```

### Fogo detectado

Quando o sensor indicar a presença de fogo, a interface poderá alterar automaticamente o estado do card:

```text
🔥 DETECÇÃO DE FOGO

🔥 FOGO DETECTADO
```

O CSS possui um estado visual específico para o alerta, permitindo destacar a ocorrência na tela.

---

## 🌡️ Monitoramento das temperaturas

São apresentadas quatro informações principais:

| Informação             | Descrição                            |
| ---------------------- | ------------------------------------ |
| Temperatura da sauna   | Temperatura atual medida pelo sensor |
| Máxima da sauna        | Limite máximo configurado            |
| Temperatura da piscina | Temperatura atual medida pelo sensor |
| Máxima da piscina      | Limite máximo configurado            |

Os valores são apresentados em graus Celsius (`°C`).

---

## 🕐 Relógio e data

O relógio é atualizado diretamente pelo JavaScript do navegador.

Exemplo:

```text
21:48:32
30/09/2026
```

Dessa forma, o relógio não precisa gerar uma requisição ao ESP32 a cada segundo.

---

## 📡 Status do ESP32

A interface também apresenta o estado da comunicação com o ESP32.

### Online

```text
● ESP32 ONLINE
```

### Offline

```text
● ESP32 OFFLINE
```

Isso permite identificar rapidamente quando a página deixou de receber dados do dispositivo.

---

## 📱 Responsividade

O layout foi desenvolvido utilizando CSS responsivo.

### Desktop / TV

Os cards de temperatura são apresentados lado a lado:

```text
┌───────────────┐   ┌───────────────┐
│    SAUNA      │   │    PISCINA    │
│               │   │               │
│    72.5 °C    │   │    28.4 °C    │
└───────────────┘   └───────────────┘
```

### Smartphone

Os cards passam para uma coluna:

```text
┌───────────────────┐
│      SAUNA        │
│                   │
│      72.5 °C      │
│                   │
│ Máxima: 80.0 °C   │
└───────────────────┘

┌───────────────────┐
│     PISCINA       │
│                   │
│      28.4 °C      │
│                   │
│ Máxima: 32.0 °C   │
└───────────────────┘
```

---

## 🚀 Como executar o projeto

### 1. Clonar o repositório

```bash
git clone https://github.com/SEU-USUARIO/SEU-REPOSITORIO.git
```

Entre na pasta:

```bash
cd SEU-REPOSITORIO
```

---

### 2. Abrir no VS Code

Abra o projeto utilizando o **Visual Studio Code** com a extensão **PlatformIO** instalada.

---

### 3. Configurar o Wi-Fi

No código do ESP32, configure:

```cpp
const char *ssid = "SUA_REDE_WIFI";
const char *password = "SUA_SENHA";
```

---

### 4. Compilar o projeto

No PlatformIO:

```text
PlatformIO → Build
```

ou pelo terminal:

```bash
pio run
```

---

### 5. Fazer upload do firmware

Conecte o ESP32 ao computador e execute:

```bash
pio run --target upload
```

---

### 6. Enviar os arquivos do LittleFS

Como o projeto utiliza arquivos HTML, CSS e JavaScript armazenados no LittleFS, é necessário fazer o upload do filesystem:

```bash
pio run --target uploadfs
```

Os arquivos da pasta:

```text
data/
```

serão enviados para o ESP32.

---

## 🌐 Acessando o dashboard

Depois que o ESP32 estiver conectado ao Wi-Fi, o endereço IP será apresentado no monitor serial.

Exemplo:

```text
WiFi conectado!
IP: 192.168.1.24
```

No navegador, acesse:

```text
http://192.168.1.24
```

O endereço IP dependerá da configuração da rede local.

---

## 🔄 Atualização dos dados

A página consulta periodicamente o endpoint:

```http
/api/status
```

Os dados recebidos são utilizados para atualizar automaticamente o dashboard.

Exemplo:

```javascript
function getStatus() {

    fetch("/api/status")

        .then(response => response.json())

        .then(data => {

            document.getElementById("tempSauna")
                .textContent = data.tempSauna;

            document.getElementById("tempPiscina")
                .textContent = data.tempPiscina;

        });
}
```

---

## 🛠️ Possíveis melhorias futuras

Algumas funcionalidades que podem ser adicionadas futuramente:

* [ ] Histórico das temperaturas
* [ ] Gráfico de temperatura
* [ ] Configuração das temperaturas máximas pela interface
* [ ] Alerta sonoro para detecção de fogo
* [ ] Notificação de fogo
* [ ] Registro de eventos
* [ ] Controle de dispositivos da sauna
* [ ] Controle de iluminação da piscina
* [ ] Controle através do smartphone
* [ ] Armazenamento histórico das temperaturas
* [ ] MQTT para integração com sistemas IoT
* [ ] Home Assistant
* [ ] Autenticação para acesso ao dashboard

---

## 🎯 Objetivo do projeto

O objetivo deste projeto é desenvolver uma solução simples e de baixo custo para **monitoramento IoT de sauna e piscina**, utilizando o ESP32 como dispositivo de aquisição de dados e servidor web.

A interface foi projetada para priorizar:

* Visualização rápida
* Informações grandes
* Baixa quantidade de elementos
* Funcionamento em diferentes dispositivos
* Comunicação em tempo real com o ESP32
* Facilidade de expansão

---

## 👨‍💻 Autor

Desenvolvido como projeto pessoal de estudo e desenvolvimento com **ESP32, C++, Web e IoT**.

---

## 📄 Licença

Este projeto pode ser utilizado para fins de estudo e desenvolvimento pessoal.
