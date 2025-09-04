window.addEventListener('DOMContentLoaded', function() {
    console.log('🚀 SCRIPT TV CARREGADO (v2)!');

    // Gera o QR Code no novo container
    new QRCode(document.getElementById('qrcode-container'), {
        text: 'https://webhook-coffee.vercel.app/',
        width: 256,
        height: 256,
        colorDark : '#000000',
        colorLight : '#ffffff',
        correctLevel : QRCode.CorrectLevel.H
    });

    // Carregamento inicial e agendado
    loadData();
    loadHistory();
    setInterval(loadData, 10000);
    setInterval(loadHistory, 60000); // Histórico pode atualizar mais devagar
});

// Função para buscar dados de saldo e doadores
async function loadData() {
    try {
        const response = await fetch('/api/get-current-balance');
        if (!response.ok) throw new Error(`Erro HTTP: ${response.status}`);
        const data = await response.json();

        // Saldo
        document.getElementById('current-balance').textContent = `R$ ${data.total_amount.toFixed(2).replace('.', ',')}`;

        // Último doador
        const lastName = data.last_donor || 'Aguardando...';
        document.getElementById('last-donor-name').textContent = lastName;
        document.getElementById('last-donor-initial').textContent = lastName.charAt(0).toUpperCase();

        // Maior doador
        const topName = data.top_donor || 'Aguardando...';
        document.getElementById('top-donor-name').textContent = topName;
        document.getElementById('top-donor-initial').textContent = topName.charAt(0).toUpperCase();
        document.getElementById('top-donor-amount').textContent = data.top_amount ? `+ R$${data.top_amount.toFixed(2).replace('.', ',')}` : '';

    } catch (error) {
        console.error('❌ Erro ao buscar dados de saldo:', error);
    }
}

// Função para buscar e construir o histórico de rolagem
async function loadHistory() {
    try {
        const response = await fetch('/api/get-donation-history');
        if (!response.ok) throw new Error(`Erro HTTP: ${response.status}`);
        const data = await response.json();

        if (!data.success || !data.history || data.history.length === 0) {
            console.log('📜 Histórico vazio ou com erro.');
            return;
        }

        const historyWrapper = document.getElementById('history-wrapper');
        historyWrapper.innerHTML = ''; // Limpa o conteúdo anterior

        // Array de cores para os ícones, para dar variedade
        const colors = [
            'bg-pink-400/20 text-pink-300',
            'bg-blue-400/20 text-blue-300',
            'bg-yellow-400/20 text-yellow-300',
            'bg-indigo-400/20 text-indigo-300',
            'bg-green-400/20 text-green-300',
            'bg-red-400/20 text-red-300',
            'bg-orange-400/20 text-orange-300',
            'bg-sky-400/20 text-sky-300',
        ];

        let historyContent = '';
        data.history.forEach((donation, index) => {
            const colorClass = colors[index % colors.length]; // Seleciona uma cor do array
            const initial = (donation.donor_name || '?').charAt(0).toUpperCase();
            const amount = `+ R$${(donation.amount || 0).toFixed(2).replace('.', ',')}`;

            historyContent += `
                <div class="flex items-center gap-3 mx-4">
                    <span class="${colorClass} text-sm font-semibold w-8 h-8 flex items-center justify-center rounded-full flex-shrink-0">${initial}</span>
                    <div>
                        <p class="text-sm font-semibold text-white">${donation.donor_name || 'Anônimo'}</p>
                        <p class="text-xs text-green-400 font-bold">${amount}</p>
                    </div>
                </div>
            `;
        });

        // Duplica o conteúdo para o efeito de rolagem contínua
        historyWrapper.innerHTML = historyContent + historyContent;

    } catch (error) {
        console.error('❌ Erro ao buscar histórico:', error);
    }
}