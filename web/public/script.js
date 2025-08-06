console.log('🚀 SCRIPT CARREGADO - script.js iniciado');

// 🔒 Sistema de Cache Inteligente
const CacheManager = {
    keys: {
        balance: 'coffee_donation_balance',
        history: 'coffee_donation_history',
        lastUpdate: 'coffee_donation_last_update'
    },
    
    // Salva dados no cache
    save(key, data) {
        try {
            const cacheData = {
                data: data,
                timestamp: Date.now(),
                version: '1.0'
            };
            localStorage.setItem(this.keys[key], JSON.stringify(cacheData));
            console.log(`💾 Cache salvo: ${key}`, data);
        } catch (error) {
            console.warn('⚠️ Erro ao salvar cache:', error);
        }
    },
    
    // Recupera dados do cache
    load(key) {
        try {
            const cached = localStorage.getItem(this.keys[key]);
            if (!cached) return null;
            
            const cacheData = JSON.parse(cached);
            const age = Date.now() - cacheData.timestamp;
            const maxAge = 5 * 60 * 1000; // 5 minutos
            
            if (age > maxAge) {
                console.log(`🗑️ Cache expirado: ${key} (${Math.round(age/1000)}s)`);
                this.clear(key);
                return null;
            }
            
            console.log(`📦 Cache carregado: ${key} (${Math.round(age/1000)}s atrás)`);
            return cacheData.data;
        } catch (error) {
            console.warn('⚠️ Erro ao carregar cache:', error);
            return null;
        }
    },
    
    // Limpa cache específico
    clear(key) {
        localStorage.removeItem(this.keys[key]);
    },
    
    // Limpa todo o cache
    clearAll() {
        Object.values(this.keys).forEach(key => localStorage.removeItem(key));
    }
};

// 🔄 Sistema de Gerenciamento de Estado da Aplicação
const AppState = {
    isVisible: true,
    lastActiveTime: Date.now(),
    intervals: {
        balance: null,
        history: null
    },
    
    // Detecta quando a página fica visível/invisível
    init() {
        document.addEventListener('visibilitychange', () => {
            const wasVisible = this.isVisible;
            this.isVisible = !document.hidden;
            
            if (!wasVisible && this.isVisible) {
                console.log('👁️ Aplicação reativada - sincronizando dados...');
                this.onAppReactivated();
            } else if (wasVisible && !this.isVisible) {
                console.log('😴 Aplicação em background');
                this.lastActiveTime = Date.now();
            }
        });
        
        // Detecta foco/desfoco da janela
        window.addEventListener('focus', () => {
            if (Date.now() - this.lastActiveTime > 30000) { // 30s
                console.log('🔄 Janela focada após inatividade - atualizando...');
                this.onAppReactivated();
            }
        });
    },
    
    // Quando a app é reativada
    async onAppReactivated() {
        try {
            // Busca dados atualizados em background
            await Promise.all([
                loadCurrentBalance(true), // true = silencioso
                loadDonationHistory(true)
            ]);
            console.log('✅ Sincronização completa');
        } catch (error) {
            console.error('❌ Erro na sincronização:', error);
        }
    },
    
    // Configura intervalos inteligentes
    setupIntervals() {
        // Limpa intervalos existentes
        if (this.intervals.balance) clearInterval(this.intervals.balance);
        if (this.intervals.history) clearInterval(this.intervals.history);
        
        // Configura novos intervalos
        this.intervals.balance = setInterval(() => loadCurrentBalance(true), 10000); // 10s
        this.intervals.history = setInterval(() => loadDonationHistory(true), 30000); // 30s
        
        console.log('⏰ Intervalos de atualização configurados');
    }
};

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
    
    // Inicializa sistema de estado
    AppState.init();
    
    // 🚀 CARREGAMENTO INTELIGENTE
    loadAppWithCache();
    
    // Configura intervalos
    AppState.setupIntervals();
});

// � Carregamento inicial inteligente com cache
async function loadAppWithCache() {
    console.log('🚀 Iniciando carregamento inteligente...');
    
    // 1. Tenta carregar do cache primeiro
    const cachedBalance = CacheManager.load('balance');
    const cachedHistory = CacheManager.load('history');
    
    if (cachedBalance) {
        console.log('⚡ Exibindo saldo do cache');
        displayBalanceData(cachedBalance);
    } else {
        showLoadingState();
    }
    
    if (cachedHistory) {
        console.log('⚡ Exibindo histórico do cache');
        displayHistoryData(cachedHistory);
    } else if (!cachedBalance) {
        showLoadingState();
    }
    
    // 2. Busca dados atualizados em paralelo
    try {
        console.log('🔄 Buscando dados atualizados...');
        await Promise.all([
            loadCurrentBalance(cachedBalance ? true : false), // silencioso se tem cache
            loadDonationHistory(cachedHistory ? true : false)
        ]);
        console.log('✅ Carregamento inicial completo');
    } catch (error) {
        console.error('❌ Erro no carregamento inicial:', error);
        if (!cachedBalance && !cachedHistory) {
            showErrorState('Erro ao conectar com o servidor. Tentando novamente...');
        }
    }
}

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

function showErrorState(message = 'Erro de conexão') {
    const balanceEl = document.getElementById('current-balance');
    const lastDonorEl = document.getElementById('last-donor');
    const topDonationEl = document.getElementById('top-donation');
    const historyEl = document.getElementById('donation-history');
    
    if (balanceEl && balanceEl.textContent === 'Carregando...') balanceEl.textContent = message;
    if (lastDonorEl && lastDonorEl.textContent === 'Carregando...') lastDonorEl.textContent = message;
    if (topDonationEl && topDonationEl.textContent === 'Carregando...') topDonationEl.textContent = message;
    if (historyEl && historyEl.innerHTML.includes('Carregando')) {
        historyEl.innerHTML = `<div class="empty-history"><div class="empty-history-icon">⚠️</div><p>${message}</p></div>`;
    }
}

// 💰 Função para carregar o saldo atual do Firebase (otimizada)
async function loadCurrentBalance(silent = false) {
    const balanceElement = document.getElementById('current-balance');
    const lastDonorElement = document.getElementById('last-donor');
    const topDonationElement = document.getElementById('top-donation');
    
    try {
        if (!silent) console.log('🔍 Buscando saldo atual...');
        
        const controller = new AbortController();
        const timeoutId = setTimeout(() => controller.abort(), 8000); // 8s timeout
        
        const response = await fetch('/api/get-current-balance', {
            headers: {
                'Cache-Control': 'no-cache',
                'Pragma': 'no-cache'
            },
            signal: controller.signal
        });
        
        clearTimeout(timeoutId);
        
        if (!silent) console.log(`📡 Response status: ${response.status}`);
        
        if (!response.ok) {
            throw new Error(`HTTP ${response.status}`);
        }
        
        const data = await response.json();
        if (!silent) console.log('📋 Dados recebidos da API:', data);
        
        // Salva no cache
        CacheManager.save('balance', data);
        
        // Exibe os dados
        displayBalanceData(data);
        
        if (!silent) console.log('✅ Saldo atualizado com sucesso');
        
    } catch (error) {
        console.error('❌ Erro ao carregar saldo:', error);
        
        // Se não é um reload silencioso, tenta carregar do cache
        if (!silent) {
            const cached = CacheManager.load('balance');
            if (cached) {
                console.log('🔄 Usando dados do cache devido ao erro');
                displayBalanceData(cached);
                return;
            }
        }
        
        // Só mostra erro se não tem cache e não é silencioso
        if (!silent) {
            const isTimeout = error.name === 'AbortError';
            const errorMsg = isTimeout ? 'Conexão lenta...' : 'Erro de conexão';
            
            if (balanceElement && balanceElement.textContent === 'Carregando...') {
                balanceElement.textContent = errorMsg;
            }
            if (lastDonorElement && lastDonorElement.textContent === 'Carregando...') {
                lastDonorElement.textContent = errorMsg;
            }
            if (topDonationElement && topDonationElement.textContent === 'Carregando...') {
                topDonationElement.textContent = errorMsg;
            }
            
            // Retry automático em caso de timeout
            if (isTimeout) {
                setTimeout(() => loadCurrentBalance(false), 3000);
            }
        }
    }
}

// 📊 Função para exibir dados do saldo
function displayBalanceData(data) {
    const balanceElement = document.getElementById('current-balance');
    const lastDonorElement = document.getElementById('last-donor');
    const topDonationElement = document.getElementById('top-donation');
    
    // Atualiza saldo principal
    if (balanceElement) {
        const newBalance = `R$ ${data.total_amount.toFixed(2).replace('.', ',')}`;
        balanceElement.textContent = newBalance;
    }
    
    // Atualiza último doador
    if (lastDonorElement) {
        const lastDonor = data.last_donor && data.last_donor !== 'Doador Anônimo' ? 
            (data.last_donor.length > 20 ? data.last_donor.substring(0, 20) + '...' : data.last_donor) : 
            'Nenhum ainda';
        lastDonorElement.textContent = lastDonor;
    }
    
    // Atualiza maior doação
    if (topDonationElement) {
        const topDonation = data.top_amount > 0 ? 
            `${data.top_donor.length > 15 ? data.top_donor.substring(0, 15) + '...' : data.top_donor} - R$ ${data.top_amount.toFixed(2).replace('.', ',')}` : 
            'Nenhuma ainda';
        topDonationElement.textContent = topDonation;
    }
}

// 📜 Função para carregar histórico de doações (otimizada)
async function loadDonationHistory(silent = false) {
    const historyContainer = document.getElementById('donation-history');
    
    try {
        if (!silent) console.log('📜 Buscando histórico de doações...');
        
        const controller = new AbortController();
        const timeoutId = setTimeout(() => controller.abort(), 8000); // 8s timeout
        
        const response = await fetch('/api/get-donation-history', {
            headers: {
                'Cache-Control': 'no-cache',
                'Pragma': 'no-cache'
            },
            signal: controller.signal
        });
        
        clearTimeout(timeoutId);
        
        if (!silent) console.log(`📡 History response status: ${response.status}`);
        
        if (!response.ok) {
            throw new Error(`HTTP ${response.status}`);
        }
        
        const data = await response.json();
        if (!silent) console.log('📋 Dados de histórico recebidos:', data);
        
        if (data.success && data.history && Array.isArray(data.history)) {
            // Salva no cache
            CacheManager.save('history', data.history);
            
            // Exibe os dados
            displayHistoryData(data.history);
            
            if (!silent) console.log(`✅ Histórico atualizado: ${data.history.length} doações`);
        } else {
            throw new Error('Dados de histórico inválidos');
        }
        
    } catch (error) {
        console.error('❌ Erro ao carregar histórico:', error);
        
        // Se não é um reload silencioso, tenta carregar do cache
        if (!silent) {
            const cached = CacheManager.load('history');
            if (cached) {
                console.log('🔄 Usando histórico do cache devido ao erro');
                displayHistoryData(cached);
                return;
            }
        }
        
        // Só mostra erro se não tem cache e não é silencioso
        if (!silent) {
            const isTimeout = error.name === 'AbortError';
            const errorMsg = isTimeout ? 'Carregando histórico...' : 'Erro ao carregar histórico';
            
            historyContainer.innerHTML = `
                <div class="empty-history">
                    <div class="empty-history-icon">${isTimeout ? '⏳' : '❌'}</div>
                    <p>${errorMsg}</p>
                </div>
            `;
            
            // Retry automático em caso de timeout
            if (isTimeout) {
                setTimeout(() => loadDonationHistory(false), 3000);
            }
        }
    }
}

// 📊 Função para exibir dados do histórico
function displayHistoryData(historyData) {
    const historyContainer = document.getElementById('donation-history');
    
    if (!historyData || !Array.isArray(historyData) || historyData.length === 0) {
        historyContainer.innerHTML = `
            <div class="empty-history">
                <div class="empty-history-icon">📭</div>
                <p>Nenhuma doação ainda</p>
            </div>
        `;
        return;
    }
    
    // Verifica se cada item do histórico tem as propriedades necessárias
    const validDonations = historyData.filter(donation => {
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
    
    historyContainer.innerHTML = validDonations.map(donation => `
        <div class="history-item">
            <div class="history-amount">R$ ${donation.amount.toFixed(2).replace('.', ',')}</div>
            <div class="history-donor">${donation.donor_name}</div>
            <div class="history-time">${formatTimestamp(donation.timestamp)}</div>
        </div>
    `).join('');
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
            // Detecta se é dispositivo móvel
            const isMobile = /Android|webOS|iPhone|iPad|iPod|BlackBerry|IEMobile|Opera Mini/i.test(navigator.userAgent);
            
            // Gera ID único para o textarea
            const textareaId = `pix-code-${Date.now()}`;
            
            // Exibe QR Code
            qrcodeEl.innerHTML = `
                <h3>✅ PIX Gerado com Sucesso!</h3>
                <p style="color: #ffffff; font-weight: 500;"><strong>Doador:</strong> ${donorName}</p>
                <p style="color: #ffffff; font-weight: 500;"><strong>Valor:</strong> R$ ${amount.toFixed(2).replace('.', ',')}</p>
                <img src="data:image/png;base64,${data.qr_code_base64}" style="max-width: 250px; margin: 10px 0;">
                <div class="pix-container">
                    <div class="pix-header">
                        <small><strong>📋 PIX Copia e Cola:</strong></small>
                        <button id="copy-pix-btn" onclick="copyPixCode('${textareaId}')">
                            📋 Copiar
                        </button>
                    </div>
                    <textarea id="${textareaId}" readonly class="pix-textarea" style="color: #000000 !important;">${data.qr_code}</textarea>
                    <div id="copy-status" class="pix-status"></div>
                </div>
                <p style="color: #ffffff; font-size: 14px; margin-top: 15px; font-weight: 500;">
                    💡 Após o pagamento, seu nome aparecerá automaticamente no display do ESP32!
                </p>
                ${isMobile ? '<p style="color: #ffffff; font-size: 13px; margin-top: 10px; font-weight: 500;">📱 <strong>PIX copiado automaticamente!</strong> Cole no seu app de pagamentos.</p>' : ''}
            `;
            qrcodeEl.style.display = 'block';
            
            // Cópia automática em dispositivos móveis
            if (isMobile) {
                setTimeout(() => {
                    copyPixCode(textareaId, true);
                }, 500);
            }
            
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
                
                // Limpa cache para forçar atualização imediata
                CacheManager.clear('balance');
                CacheManager.clear('history');
                
                // Atualiza dados imediatamente (modo não-silencioso)
                setTimeout(() => {
                    loadCurrentBalance(false);
                    loadDonationHistory(false);
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

// 📋 Função para copiar PIX Copia e Cola
function copyPixCode(textareaId, isAutomatic = false) {
    const textarea = document.getElementById(textareaId);
    const copyBtn = document.getElementById('copy-pix-btn');
    const statusEl = document.getElementById('copy-status');
    
    if (!textarea) {
        console.error('❌ Textarea do PIX não encontrado');
        return;
    }
    
    try {
        // Método 1: Clipboard API (moderno)
        if (navigator.clipboard && window.isSecureContext) {
            navigator.clipboard.writeText(textarea.value).then(() => {
                showCopySuccess(copyBtn, statusEl, isAutomatic);
            }).catch(() => {
                // Fallback para método tradicional
                fallbackCopy(textarea, copyBtn, statusEl, isAutomatic);
            });
        } else {
            // Método 2: Fallback para navegadores antigos/inseguros
            fallbackCopy(textarea, copyBtn, statusEl, isAutomatic);
        }
        
    } catch (error) {
        console.error('❌ Erro ao copiar PIX:', error);
        showCopyError(copyBtn, statusEl);
    }
}

// 📋 Método fallback para copiar texto
function fallbackCopy(textarea, copyBtn, statusEl, isAutomatic) {
    try {
        textarea.select();
        textarea.setSelectionRange(0, 99999); // Para dispositivos móveis
        
        const successful = document.execCommand('copy');
        
        if (successful) {
            showCopySuccess(copyBtn, statusEl, isAutomatic);
        } else {
            showCopyError(copyBtn, statusEl);
        }
        
        // Remove seleção
        if (window.getSelection) {
            window.getSelection().removeAllRanges();
        }
        
    } catch (error) {
        console.error('❌ Erro no fallback copy:', error);
        showCopyError(copyBtn, statusEl);
    }
}

// ✅ Mostra sucesso na cópia
function showCopySuccess(copyBtn, statusEl, isAutomatic) {
    if (copyBtn) {
        copyBtn.innerHTML = '✅ Copiado!';
        copyBtn.style.background = '#28a745';
        
        setTimeout(() => {
            copyBtn.innerHTML = '📋 Copiar';
            copyBtn.style.background = '#007bff';
        }, 2000);
    }
    
    if (statusEl) {
        statusEl.textContent = isAutomatic ? '✅ PIX copiado automaticamente!' : '✅ PIX copiado para área de transferência!';
        statusEl.style.opacity = '1';
        
        setTimeout(() => {
            statusEl.style.opacity = '0';
        }, 3000);
    }
    
    console.log('✅ PIX Copia e Cola copiado com sucesso');
}

// ❌ Mostra erro na cópia
function showCopyError(copyBtn, statusEl) {
    if (copyBtn) {
        copyBtn.innerHTML = '❌ Erro';
        copyBtn.style.background = '#dc3545';
        
        setTimeout(() => {
            copyBtn.innerHTML = '📋 Copiar';
            copyBtn.style.background = '#007bff';
        }, 2000);
    }
    
    if (statusEl) {
        statusEl.textContent = '❌ Erro ao copiar. Tente selecionar e copiar manualmente.';
        statusEl.style.color = '#dc3545';
        statusEl.style.opacity = '1';
        
        setTimeout(() => {
            statusEl.style.opacity = '0';
            statusEl.style.color = '#28a745';
        }, 4000);
    }
    
    console.error('❌ Erro ao copiar PIX Copia e Cola');
}