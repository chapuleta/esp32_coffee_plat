// Manipulação dos botões de valor
document.addEventListener('DOMContentLoaded', function() {
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
});

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
                <p><strong>Valor:</strong> R$ ${amount.toFixed(2)}</p>
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