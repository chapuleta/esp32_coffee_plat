// 🚀 VERSÃO SIMPLIFICADA PARA DEBUG
console.log('🚀 SCRIPT SIMPLES CARREGADO!');

window.addEventListener('DOMContentLoaded', function() {
    console.log('🚀 DOM CARREGADO!');
    
    // Mostra loading state inicial
    showLoadingState();
    
    // Função simples para carregar saldo
    loadBalance();
    loadHistory();
    
    // Atualiza a cada 10 segundos
    setInterval(loadBalance, 10000);
    setInterval(loadHistory, 30000);
});

function showLoadingState() {
    const balanceEl = document.getElementById('current-balance');
    const lastDonorEl = document.getElementById('last-donor');
    const topDonationEl = document.getElementById('top-donation');
    
    if (balanceEl) balanceEl.textContent = 'Carregando...';
    if (lastDonorEl) lastDonorEl.textContent = 'Carregando...';
    if (topDonationEl) topDonationEl.textContent = 'Carregando...';
}

async function loadBalance() {
    console.log('💰 Buscando saldo...');
    
    try {
        const response = await fetch('/api/get-current-balance');
        console.log('📡 Status:', response.status);
        
        const data = await response.json();
        console.log('📋 Dados:', data);
        
        // Atualiza elementos
        const balanceEl = document.getElementById('current-balance');
        const lastDonorEl = document.getElementById('last-donor');
        const topDonationEl = document.getElementById('top-donation');
        
        if (balanceEl) {
            balanceEl.textContent = `R$ ${data.total_amount.toFixed(2).replace('.', ',')}`;
            console.log('✅ Saldo atualizado:', balanceEl.textContent);
        }
        
        if (lastDonorEl) {
            lastDonorEl.textContent = data.last_donor || 'Nenhum ainda';
            console.log('✅ Último doador:', lastDonorEl.textContent);
        }
        
        if (topDonationEl) {
            const topText = data.top_amount > 0 ? 
                `${data.top_donor} - R$ ${data.top_amount.toFixed(2).replace('.', ',')}` : 
                'Nenhuma ainda';
            topDonationEl.textContent = topText;
            console.log('✅ Maior doação:', topDonationEl.textContent);
        }
        
    } catch (error) {
        console.error('❌ Erro:', error);
    }
}

async function loadHistory() {
    console.log('📜 Buscando histórico...');
    
    try {
        const response = await fetch('/api/get-donation-history');
        console.log('📡 History Status:', response.status);
        
        const data = await response.json();
        console.log('📋 Dados histórico:', data);
        
        const historyEl = document.getElementById('donation-history');
        
        if (historyEl && data.success && data.history && data.history.length > 0) {
            historyEl.innerHTML = data.history.map(donation => `
                <div class="history-item">
                    <div class="history-amount">R$ ${donation.amount.toFixed(2).replace('.', ',')}</div>
                    <div class="history-donor">${donation.donor_name}</div>
                    <div class="history-time">${formatTime(donation.timestamp)}</div>
                </div>
            `).join('');
            console.log('✅ Histórico atualizado:', data.history.length, 'doações');
        } else if (historyEl) {
            historyEl.innerHTML = '<div class="empty-history"><p>📭 Nenhuma doação ainda</p></div>';
            console.log('📭 Nenhuma doação no histórico');
        }
        
    } catch (error) {
        console.error('❌ Erro histórico:', error);
    }
}

function formatTime(timestamp) {
    const date = new Date(timestamp);
    const now = new Date();
    const diffMins = Math.floor((now - date) / (1000 * 60));
    
    if (diffMins < 1) return 'Agora';
    if (diffMins < 60) return `${diffMins}min atrás`;
    if (diffMins < 1440) return `${Math.floor(diffMins/60)}h atrás`;
    return date.toLocaleDateString('pt-BR');
}
