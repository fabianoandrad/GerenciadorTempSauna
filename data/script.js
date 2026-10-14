//const btnTeste = document.getElementById("btnTeste");
//const mensagem = document.getElementById("mensagem");
const tempSauna = document.getElementById("temp-Sauna");
const tempPiscina = document.getElementById("temp-Piscina");
const tempRoom = document.getElementById("temp-Room");
const humidity = document.getElementById("humidity");
const fireCard = document.querySelector(".fire-card");
const percentageFire = document.getElementById("percentage-Fire");
const connectionStatus = document.getElementById("connectionStatus");

let ledState = false;

function initTimes(){

    getStatus();
    updateDateHour();

    setInterval(() =>{
        getStatus();
        updateDateHour();
    }, 1000);
}


// =============================
//  DATE TIME 
// =============================
function updateDateHour() {
    const now = new Date();

    // Hora
    const hour = now.toLocaleTimeString('pt-BR', {
        hour: '2-digit',
        minute: '2-digit',
        second: '2-digit'
    });

    // Data
    const data = now.toLocaleDateString('pt-BR');

    // Atualiza o HTML
    document.getElementById('clock').textContent = hour;
    document.getElementById('date').textContent = data;
}

// =============================
// GET STATUS
// =============================
async function getStatus() {
    try {
        const response = await fetch("/api/status");

        if (!response.ok) {
            throw new Error("Erro HTTP: " + response.status);
        }

        const data = await response.json();

        console.log("Resposta getStatus do ESP32:", data);

        //btnTeste.textContent = data.led === "ligado" ? "Desligar LED" : "Ligar LED";

        //mensagem.textContent = `Status: ${data.status} | LED: ${data.led}`;

        tempSauna.textContent = Number(data.tempSauna).toFixed(1);
        //tempPiscina.textContent = Number(data.tempPiscina).toFixed(1);
        tempRoom.textContent = Number(data.tempRoom).toFixed(1);
        humidity.textContent = Number(data.humidity).toFixed(1);
        percentageFire.textContent = `${data.flameLevel}%`;

        if(data.status === "online"){
            connectionStatus.textContent = "ONLINE";
            
        }else{
            connectionStatus.classList.add("disconnected")
            connectionStatus.textContent = "DESCONECTADO";
        }


        if (data.flameDetected === "true") {
            fireCard.classList.add("fire-detected");
        }else{
            fireCard.classList.remove("fire-detected");
        }
    }
    catch (error) {
        console.error(error);
        connectionStatus.classList.add("disconnected")
        connectionStatus.textContent = "DESCONECTADO";
    }
}

// =============================
// POST LED STATE
// =============================
// async function postLedState() {
//     // Inverte o estado atual
//     const newState = !ledState;

//     console.log("Enviando:", newState);

//     try {
//         const response = await fetch("/api/ledState", {
//             method: "POST",
//             headers: {
//                 "Content-Type": "application/json"
//             },

//             body: JSON.stringify({
//                 state: newState
//             })
//         });

//         if (!response.ok) {
//             throw new Error("Erro HTTP: " + response.status);
//         }

//         const data = await response.json();

//         console.log("Resposta postLedState do ESP32:", data);

//         // Só atualiza depois que o ESP32 confirmou
//         ledState = newState;

//         // Consulta novamente o estado real
//         await getStatus();

//     }
//     catch (error) {
//         console.error(error);
//         mensagem.textContent = "Erro ao alterar o estado do LED";
//     }
// }

// =============================
// EVENTO DO BOTÃO
// =============================
//btnTeste.addEventListener("click", postLedState);

// =============================
// INICIALIZAÇÃO
// =============================
initTimes();
