window.addEventListener('DOMContentLoaded', function() {
    console.log('🚀 SCRIPT TV CARREGADO!');

    // Gera o QR Code
    new QRCode(document.getElementById('qrcode'), {
        text: 'https://webhook-coffee.vercel.app/',
        width: 256,
        height: 256,
        colorDark : '#000000',
        colorLight : '#ffffff',
        correctLevel : QRCode.CorrectLevel.H
    });

    // Funções de carregamento inicial
    loadData();
    loadHistory();

    // Atualiza os dados a cada 10 segundos
    setInterval(loadData, 10000);
    // Atualiza o histórico a cada 30 segundos
    setInterval(loadHistory, 30000);
});

async function loadData() {
    try {
        console.log('💰 Buscando saldo e doadores...');
        const response = await fetch('/api/get-current-balance');
        if (!response.ok) {
            throw new Error(`HTTP error! status: ${response.status}`);
        }
        const data = await response.json();
        console.log('📋 Dados recebidos:', data);

        // Formata o valor para BRL
        const formattedBalance = `R$ ${data.total_amount.toFixed(2).replace('.', ',')}`;

        // Atualiza o DOM
        document.getElementById('current-balance').textContent = formattedBalance;
        document.getElementById('last-donor').textContent = data.last_donor || 'Ninguém ainda';
        document.getElementById('top-donor').textContent = data.top_donor || 'Ninguém ainda';

        console.log('✅ Dados de saldo e doadores atualizados!');

    } catch (error) {
        console.error('❌ Erro ao buscar dados de saldo:', error);
    }
}

async function loadHistory() {
    try {
        console.log('📜 Buscando histórico de doações...');
        const response = await fetch('/api/get-donation-history');
        if (!response.ok) {
            throw new Error(`HTTP error! status: ${response.status}`);
        }
        const history = await response.json();
        console.log('📋 Histórico recebido:', history);

        const historyContainer = document.getElementById('donation-history');
        
        if (history.length === 0) {
            historyContainer.innerHTML = '<p style="text-align: center; font-size: 1.5vw;">Nenhuma doação ainda.</p>';
            return;
        }

        // Cria a tabela
        let tableHTML = '<table><thead><tr><th>Doador</th><th>Valor</th><th>Data</th></tr></thead><tbody>';
        
        history.forEach(donation => {
            const donationDate = new Date(donation.timestamp.seconds * 1000);
            const formattedDate = donationDate.toLocaleDateString('pt-BR', { day: '2-digit', month: '2-digit' }) + ' ' + donationDate.toLocaleTimeString('pt-BR', { hour: '2-digit', minute: '2-digit' });
            const formattedValue = `R$ ${donation.amount.toFixed(2).replace('.', ',')}`;

            tableHTML += `
                <tr>
                    <td>${donation.name}</td>
                    <td>${formattedValue}</td>
                    <td>${formattedDate}</td>
                </tr>
            `;
        });

        tableHTML += '</tbody></table>';
        historyContainer.innerHTML = tableHTML;

        console.log('✅ Histórico de doações atualizado!');

    } catch (error) {
        console.error('❌ Erro ao buscar histórico:', error);
    }
}
