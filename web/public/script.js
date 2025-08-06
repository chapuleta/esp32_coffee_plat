console.log('🚀 SCRIPT CARREGADO - script.js iniciado');

// Manipulação dos botões de valor
document.addEventListener('DOMContentLoaded', function() {
    console.log('🚀 DOM CARREGADO - executando inicialização');
    
    const amountBtns = document.querySelectorAll('.amount-btn');
    const customAmountInput = document.getElementById('custom_amount');
    
    amountBtns.forEach(btn => {
        btn.addEventListener('click', function() {
            // Remove active de todos os botões
            amountBtns.forEach(b => b.classList.remove('active'));
            // Adiciona active no clicado
            this.classList.add('active');
            // Limpa o campo customizado
            customAmountInput.value = '';
        });
    });
    
    // Se digitar valor customizado, remove active dos botões
    customAmountInput.addEventListener('input', function() {
        amountBtns.forEach(b => b.classList.remove('active'));
    });
    
    // Mostra loading state inicial
    showLoadingState();
    
    // 💰 Carrega saldo inicial e histórico
    console.log('🚀 CHAMANDO loadCurrentBalance()');
    loadCurrentBalance();
    console.log('🚀 CHAMANDO loadDonationHistory()');
    loadDonationHistory();
    
    // 🚀 Atualização regular
    setInterval(loadCurrentBalance, 10000); // 10s
    setInterval(loadDonationHistory, 30000); // 30s
});

function showLoadingState() {
    const balanceEl = document.getElementById('current-balance');
    const lastDonorEl = document.getElementById('last-donor');
    const topDonationEl = document.getElementById('top-donation');
    const historyEl = document.getElementById('donation-history');
    
    if (balanceEl) balanceEl.textContent = 'Carregando...';
    if (lastDonorEl) lastDonorEl.textContent = 'Carregando...';
    if (topDonationEl) topDonationEl.textContent = 'Carregando...';
    if (historyEl) historyEl.innerHTML = '<div class="empty-history"><p>📜 Carregando histórico...</p></div>';
}

// 💰 Função para carregar o saldo atual do Firebase (simplificada)
async function loadCurrentBalance() {
    const balanceElement = document.getElementById('current-balance');
    const lastDonorElement = document.getElementById('last-donor');
    const topDonationElement = document.getElementById('top-donation');
    
    try {
        console.log('🔍 Buscando saldo atual...');
        
        const response = await fetch('/api/get-current-balance', {
            headers: {
                'Cache-Control': 'no-cache',
                'Pragma': 'no-cache'
            }
        });
        
        console.log(`📡 Response status: ${response.status}`);
        
        if (!response.ok) {
            throw new Error(`HTTP ${response.status}`);
        }
        
        const data = await response.json();
        console.log('📋 Dados recebidos da API:', data);
        
        // Atualiza saldo principal
        if (balanceElement) {
            const newBalance = `R$ ${data.total_amount.toFixed(2).replace('.', ',')}`;
            balanceElement.textContent = newBalance;
            console.log('✅ Saldo atualizado:', newBalance);
        }
        
        // Atualiza último doador
        if (lastDonorElement) {
            const lastDonor = data.last_donor && data.last_donor !== 'Doador Anônimo' ? 
                (data.last_donor.length > 20 ? data.last_donor.substring(0, 20) + '...' : data.last_donor) : 
                'Nenhum ainda';
            lastDonorElement.textContent = lastDonor;
            console.log('👤 Último doador:', lastDonor);
        }
        
        // Atualiza maior doação
        if (topDonationElement) {
            const topDonation = data.top_amount > 0 ? 
                `${data.top_donor.length > 15 ? data.top_donor.substring(0, 15) + '...' : data.top_donor} - R$ ${data.top_amount.toFixed(2).replace('.', ',')}` : 
                'Nenhuma ainda';
            topDonationElement.textContent = topDonation;
            console.log('🏆 Maior doação:', topDonation);
        }
        
        console.log('✅ Saldo atualizado com sucesso');
        
    } catch (error) {
        console.error('❌ Erro ao carregar saldo:', error);
        
        // Valores padrão em caso de erro
        if (balanceElement) balanceElement.textContent = 'Erro ao carregar';
        if (lastDonorElement) lastDonorElement.textContent = 'Erro ao carregar';
        if (topDonationElement) topDonationElement.textContent = 'Erro ao carregar';
    }
}

// 📜 Função para carregar histórico de doações
async function loadDonationHistory() {
    const historyContainer = document.getElementById('donation-history');
    
    try {
        console.log('📜 Buscando histórico de doações...');
        
        const response = await fetch('/api/get-donation-history', {
            headers: {
                'Cache-Control': 'no-cache',
                'Pragma': 'no-cache'
            }
        });
        
        console.log(`📡 History response status: ${response.status}`);
        
        if (!response.ok) {
            throw new Error(`HTTP ${response.status}`);
        }
        
        const data = await response.json();
        console.log('📋 Dados de histórico recebidos:', data);
        
        if (data.success && data.history && Array.isArray(data.history) && data.history.length > 0) {
            console.log(`📜 Encontradas ${data.history.length} doações no histórico`);
            
            // Verifica se cada item do histórico tem as propriedades necessárias
            const validDonations = data.history.filter(donation => {
                if (!donation || typeof donation !== 'object') {
                    console.warn('⚠️ Item inválido no histórico:', donation);
                    return false;
                }
                if (typeof donation.amount !== 'number' || donation.amount <= 0) {
                    console.warn('⚠️ Item com valor inválido:', donation);
                    return false;
                }
                if (!donation.donor_name) {
                    console.warn('⚠️ Item sem nome do doador:', donation);
                    return false;
                }
                if (!donation.timestamp) {
                    console.warn('⚠️ Item sem timestamp:', donation);
                    return false;
                }
                return true;
            });
            
            console.log(`✅ ${validDonations.length} doações válidas após filtro`);
            
            historyContainer.innerHTML = validDonations.map(donation => `
                <div class="history-item">
                    <div class="history-amount">R$ ${donation.amount.toFixed(2).replace('.', ',')}</div>
                    <div class="history-donor">${donation.donor_name}</div>
                    <div class="history-time">${formatTimestamp(donation.timestamp)}</div>
                </div>
            `).join('');
        } else {
            console.log('📭 Nenhuma doação encontrada no histórico');
            historyContainer.innerHTML = `
                <div class="empty-history">
                    <div class="empty-history-icon">📭</div>
                    <p>Nenhuma doação ainda</p>
                </div>
            `;
        }
    } catch (error) {
        console.error('❌ Erro ao carregar histórico:', error);
        historyContainer.innerHTML = `
            <div class="empty-history">
                <div class="empty-history-icon">❌</div>
                <p>Erro ao carregar histórico: ${error.message}</p>
            </div>
        `;
    }
}

// 🕐 Função para formatar timestamp
function formatTimestamp(timestamp) {
    const date = new Date(timestamp);
    const now = new Date();
    const diffMs = now - date;
    const diffMins = Math.floor(diffMs / (1000 * 60));
    const diffHours = Math.floor(diffMs / (1000 * 60 * 60));
    const diffDays = Math.floor(diffMs / (1000 * 60 * 60 * 24));
    
    if (diffMins < 1) {
        return 'Agora';
    } else if (diffMins < 60) {
        return `${diffMins}min atrás`;
    } else if (diffHours < 24) {
        return `${diffHours}h atrás`;
    } else if (diffDays < 7) {
        return `${diffDays}d atrás`;
    } else {
        return date.toLocaleDateString('pt-BR', { 
            day: '2-digit', 
            month: '2-digit',
            year: '2-digit'
        });
    }
}

// Formulário de doação
document.getElementById('donation-form').addEventListener('submit', async (e) => {
    e.preventDefault();
    
    const donorName = document.getElementById('donor_name').value.trim();
    const donorEmail = document.getElementById('donor_email').value.trim();
    const donorPhone = document.getElementById('donor_phone').value.trim();
    const customAmount = document.getElementById('custom_amount').value;
    const statusEl = document.getElementById('status');
    const qrcodeEl = document.getElementById('qrcode');
    const submitBtn = document.getElementById('generate-btn');
    
    // Determina o valor selecionado
    let amount = 0;
    const selectedBtn = document.querySelector('.amount-btn.active');
    
    if (selectedBtn) {
        amount = parseFloat(selectedBtn.dataset.amount);
    } else if (customAmount) {
        amount = parseFloat(customAmount);
    }
    
    // Validações
    if (!donorName) {
        showStatus('Por favor, informe seu nome completo.', 'error');
        return;
    }
    
    if (isNaN(amount) || amount <= 0) {
        showStatus('Por favor, selecione ou digite um valor válido.', 'error');
        return;
    }
    
    if (amount < 0.50) {
        showStatus('Valor mínimo: R$ 0,50', 'error');
        return;
    }
    
    // Preparar dados do doador
    const donorData = {
        name: donorName,
        email: donorEmail || '',
        phone: donorPhone || '',
        amount: amount
    };
    
    showStatus('🔄 Gerando PIX personalizado...', 'info');
    submitBtn.disabled = true;
    qrcodeEl.style.display = 'none';
    
    try {
        const response = await fetch('/api/create-payment', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify(donorData),
        });
        
        if (!response.ok) {
            throw new Error('Falha ao criar o pagamento PIX.');
        }
        
        const data = await response.json();
        
        if (data.qr_code_base64) {
            // Exibe QR Code
            qrcodeEl.innerHTML = `
                <h3>✅ PIX Gerado com Sucesso!</h3>
                <p><strong>Doador:</strong> ${donorName}</p>
                <p><strong>Valor:</strong> R$ ${amount.toFixed(2).replace('.', ',')}</p>
                <img src="data:image/png;base64,${data.qr_code_base64}" style="max-width: 250px; margin: 10px 0;">
                <div style="background: #f8f9fa; padding: 10px; border-radius: 6px; margin-top: 10px;">
                    <small><strong>PIX Copia e Cola:</strong></small><br>
                    <textarea readonly style="width: 100%; height: 60px; font-size: 12px; margin-top: 5px;">${data.qr_code}</textarea>
                </div>
                <p style="color: #666; font-size: 14px; margin-top: 10px;">
                    💡 Após o pagamento, seu nome aparecerá automaticamente no display do ESP32!
                </p>
            `;
            qrcodeEl.style.display = 'block';
            
            showStatus('🎯 Escaneie o QR Code ou use o PIX Copia e Cola para doar!', 'success');
            
            // Monitora o pagamento
            monitorPayment(data.payment_id, donorName);
            
        } else {
            throw new Error('QR Code não foi gerado. Tente novamente.');
        }
        
    } catch (error) {
        console.error('Erro:', error);
        showStatus(`❌ Erro: ${error.message}`, 'error');
    } finally {
        submitBtn.disabled = false;
    }
});

function showStatus(message, type) {
    const statusEl = document.getElementById('status');
    statusEl.textContent = message;
    statusEl.className = type;
}

function monitorPayment(paymentId, donorName) {
    const statusEl = document.getElementById('status');
    let attempts = 0;
    const maxAttempts = 60; // 5 minutos
    
    const checkPayment = async () => {
        try {
            const response = await fetch(`/api/check-payment-status?payment_id=${paymentId}`);
            const data = await response.json();
            
            if (data.status === 'approved') {
                showStatus(`🎉 Pagamento confirmado! Obrigado, ${donorName}! Seu nome já aparece no ESP32!`, 'success');
                
                // Atualiza dados imediatamente
                setTimeout(() => {
                    loadCurrentBalance();
                    loadDonationHistory();
                }, 2000);
                
                return;
            } else if (data.status === 'rejected' || data.status === 'cancelled') {
                showStatus('❌ Pagamento não foi aprovado. Tente novamente.', 'error');
                return;
            }
            
            attempts++;
            if (attempts < maxAttempts) {
                setTimeout(checkPayment, 5000); // Verifica a cada 5 segundos
            } else {
                showStatus('⏰ Tempo de monitoramento esgotado. Verifique seu PIX.', 'info');
            }
            
        } catch (error) {
            console.error('Erro ao verificar pagamento:', error);
        }
    };
    
    setTimeout(checkPayment, 5000); // Primeira verificação em 5 segundos
}