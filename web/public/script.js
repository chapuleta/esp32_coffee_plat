document.getElementById('generate-btn').addEventListener('click', async () => {
    const amount = parseFloat(document.getElementById('amount').value);
    const statusEl = document.getElementById('status');
    const qrcodeEl = document.getElementById('qrcode');

    if (isNaN(amount) || amount <= 0) {
        statusEl.textContent = 'Por favor, insira um valor válido.';
        return;
    }

    statusEl.textContent = 'Gerando QR Code...';
    qrcodeEl.innerHTML = '';

    try {
        const response = await fetch('/api/create-payment', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify({ amount: amount }),
        });

        if (!response.ok) {
            throw new Error('Falha ao criar o pagamento.');
        }

        const data = await response.json();

        if (data.qr_data) {
            const img = document.createElement('img');
            img.src = `data:image/png;base64,${data.qr_data}`;
            qrcodeEl.appendChild(img);
            statusEl.textContent = 'Escaneie o QR Code para pagar.';
        } else {
            throw new Error('Resposta inválida do servidor.');
        }
    } catch (error) {
        console.error('Erro:', error);
        statusEl.textContent = `Erro: ${error.message}`;
    }
});