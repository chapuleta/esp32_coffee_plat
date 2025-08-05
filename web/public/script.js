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
    
    // 💰 Carrega saldo inicial e histórico
    console.log('🚀 CHAMANDO loadCurrentBalance()');
    loadCurrentBalance();
    console.log('🚀 CHAMANDO loadDonationHistory()');
    loadDonationHistory();
    
    // 🚀 Atualização inteligente e mais rápida
    startSmartBalanceUpdates();
});

// 🚀 Sistema de atualização inteligente
let updateInterval;
let historyUpdateInterval;
let isPageVisible = true;
let consecutiveErrors = 0;

function startSmartBalanceUpdates() {
    // Atualização rápida inicial (a cada 5 segundos)
    updateInterval = setInterval(loadCurrentBalance, 5000);
    historyUpdateInterval = setInterval(loadDonationHistory, 30000); // Histórico a cada 30s
    
    // Detecta quando a página fica visível/invisível
    document.addEventListener('visibilitychange', function() {
        if (document.hidden) {
            isPageVisible = false;
            // Reduz frequência quando página não está visível (30s)
            clearInterval(updateInterval);
            clearInterval(historyUpdateInterval);
            updateInterval = setInterval(loadCurrentBalance, 30000);
            historyUpdateInterval = setInterval(loadDonationHistory, 120000); // 2min
            console.log('📱 Página em background - atualizações reduzidas');
        } else {
            isPageVisible = true;
            // Volta para atualização rápida quando página fica visível
            clearInterval(updateInterval);
            clearInterval(historyUpdateInterval);
            updateInterval = setInterval(loadCurrentBalance, 5000);
            historyUpdateInterval = setInterval(loadDonationHistory, 30000);
            loadCurrentBalance(); // Atualiza imediatamente
            loadDonationHistory(); // Atualiza histórico também
            console.log('📱 Página ativa - atualizações normais');
        }
    });
    
    // Detecta quando usuário volta para a aba (atualiza imediatamente)
    window.addEventListener('focus', function() {
        console.log('👁️ Usuário focou na página - atualizando dados');
        loadCurrentBalance();
        loadDonationHistory();
    });
}

// 💰 Função para carregar o saldo atual do Firebase (otimizada)
async function loadCurrentBalance(retryCount = 0) {
    const balanceElement = document.getElementById('current-balance');
    const lastDonorElement = document.getElementById('last-donor');
    const topDonationElement = document.getElementById('top-donation');
    
    try {
        console.log('🔍 Buscando saldo atual...');
        
        // 🚀 Requisição otimizada com timeout curto
        const controller = new AbortController();
        const timeoutId = setTimeout(() => controller.abort(), 8000); // 8s timeout
        
        const response = await fetch('/api/get-current-balance', {
            signal: controller.signal,
            headers: {
                'Cache-Control': 'no-cache', // Força busca atualizada
                'Pragma': 'no-cache'
            }
        });
        
        clearTimeout(timeoutId);
        
        console.log(`📡 Response status: ${response.status}`);
        console.log('📡 Response headers:', Object.fromEntries(response.headers.entries()));
        
        if (!response.ok) {
            const errorText = await response.text();
            console.error('❌ Error response body:', errorText);
            throw new Error(`HTTP ${response.status}: ${response.statusText}`);
        }
        
        const responseText = await response.text();
        console.log('📋 Raw response:', responseText);
        
        let data;
        try {
            data = JSON.parse(responseText);
        } catch (parseError) {
            console.error('❌ JSON parse error:', parseError);
            throw new Error('Invalid JSON in response');
        }
        
        console.log('📋 Dados recebidos da API:', data);
        
        // Verifica se os dados estão corretos
        if (typeof data.total_amount === 'undefined') {
            throw new Error('total_amount não encontrado na resposta');
        }
        
        // 🎯 Verifica se os dados realmente mudaram
        const currentBalance = balanceElement.textContent;
        const newBalance = `R$ ${data.total_amount.toFixed(2).replace('.', ',')}`;
        
        if (currentBalance !== newBalance) {
            // ✨ Animação de mudança de valor
            balanceElement.style.transform = 'scale(1.1)';
            balanceElement.style.color = '#27ae60';
            setTimeout(() => {
                balanceElement.style.transform = 'scale(1)';
                balanceElement.style.color = '';
            }, 300);
            
            console.log(`💰 Saldo alterado: ${currentBalance} → ${newBalance}`);
            
            // Atualiza histórico quando saldo muda
            loadDonationHistory();
        }
        
        // Atualiza saldo principal
        balanceElement.textContent = newBalance;
        
        // Atualiza último doador
        const lastDonor = data.last_donor && data.last_donor !== 'Doador Anônimo' ? 
            (data.last_donor.length > 20 ? data.last_donor.substring(0, 20) + '...' : data.last_donor) : 
            'Nenhum ainda';
        lastDonorElement.textContent = lastDonor;
        console.log('👤 Último doador:', lastDonor);
        
        // Atualiza maior doação
        const topDonation = data.top_amount > 0 ? 
            `${data.top_donor.length > 15 ? data.top_donor.substring(0, 15) + '...' : data.top_donor} - R$ ${data.top_amount.toFixed(2).replace('.', ',')}` : 
            'Nenhuma ainda';
        topDonationElement.textContent = topDonation;
        console.log('🏆 Maior doação:', topDonation);
        
        // Reset contador de erros
        consecutiveErrors = 0;
        
        console.log('✅ Saldo atualizado com sucesso:', data);
        
    } catch (error) {
        consecutiveErrors++;
        console.error(`❌ Erro ao carregar saldo (tentativa ${retryCount + 1}):`, error.message);
        console.error('📊 Stack trace:', error.stack);
        
        // 🔄 Retry automático para erros de rede
        if (retryCount < 2 && (error.name === 'AbortError' || error.message.includes('Failed to fetch'))) {
            console.log(`🔄 Tentando novamente em 2s... (${retryCount + 1}/3)`);
            setTimeout(() => loadCurrentBalance(retryCount + 1), 2000);
            return;
        }
        
        // Valores padrão em caso de erro persistente
        if (balanceElement.textContent === 'R$ 0,00' || consecutiveErrors > 5) {
            balanceElement.textContent = 'R$ --,--';
            lastDonorElement.textContent = 'Erro ao carregar';
            topDonationElement.textContent = 'Erro ao carregar';
        }
        
        // 🚀 Ajusta frequência de atualização baseado nos erros
        if (consecutiveErrors > 3) {
            clearInterval(updateInterval);
            updateInterval = setInterval(loadCurrentBalance, 15000); // Reduz para 15s
            console.log('⚠️ Muitos erros - reduzindo frequência para 15s');
        }
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
        console.log('📡 History response headers:', Object.fromEntries(response.headers.entries()));
        
        if (!response.ok) {
            const errorText = await response.text();
            console.error('❌ History error response body:', errorText);
            throw new Error(`HTTP ${response.status}`);
        }
        
        const responseText = await response.text();
        console.log('📋 Raw history response:', responseText);
        
        let data;
        try {
            data = JSON.parse(responseText);
        } catch (parseError) {
            console.error('❌ History JSON parse error:', parseError);
            throw new Error('Invalid JSON in history response');
        }
        
        console.log('📋 Dados de histórico recebidos:', data);
        
        if (data.success && data.history && data.history.length > 0) {
            console.log(`📜 Encontradas ${data.history.length} doações no histórico`);
            
            historyContainer.innerHTML = data.history.map(donation => `
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
        console.error('📊 Stack trace:', error.stack);
        historyContainer.innerHTML = `
            <div class="empty-history">
                <div class="empty-history-icon">❌</div>
                <p>Erro ao carregar histórico</p>
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
                
                // 💰 Atualização super rápida após confirmação
                console.log('💰 Pagamento aprovado - iniciando atualizações rápidas...');
                
                // Primeira atualização imediata
                setTimeout(() => {
                    loadCurrentBalance();
                    loadDonationHistory();
                }, 1000);
                
                // Segunda atualização em 3s (caso a primeira não tenha pego)
                setTimeout(() => {
                    loadCurrentBalance();
                    loadDonationHistory();
                }, 3000);
                
                // Terceira atualização em 6s (garantia)
                setTimeout(() => {
                    loadCurrentBalance();
                    loadDonationHistory();
                    showStatus(`💰 Saldo atualizado! Obrigado pela sua doação, ${donorName}!`, 'success');
                }, 6000);
                
                // 🔥 Temporariamente acelera as atualizações por 2 minutos
                clearInterval(updateInterval);
                clearInterval(historyUpdateInterval);
                updateInterval = setInterval(loadCurrentBalance, 3000); // A cada 3s
                historyUpdateInterval = setInterval(loadDonationHistory, 10000); // A cada 10s
                
                setTimeout(() => {
                    clearInterval(updateInterval);
                    clearInterval(historyUpdateInterval);
                    updateInterval = setInterval(loadCurrentBalance, 5000); // Volta para 5s
                    historyUpdateInterval = setInterval(loadDonationHistory, 30000); // Volta para 30s
                    console.log('💰 Voltando para atualização normal');
                }, 120000); // 2 minutos
                
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