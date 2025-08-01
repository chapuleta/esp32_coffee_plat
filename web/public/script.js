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

        if (data.qr_code_base64) {
            const img = document.createElement('img');
            img.src = `data:image/png;base64,${data.qr_code_base64}`;
            qrcodeEl.appendChild(img);

            if (data.qr_code) {
                const pixCopyPaste = document.createElement('p');
                pixCopyPaste.textContent = `Pix Copia e Cola: ${data.qr_code}`;
                qrcodeEl.appendChild(pixCopyPaste);
            }

            statusEl.textContent = 'Escaneie o QR Code ou use o Pix Copia e Cola para pagar.';
        } else {
            throw new Error('Resposta inválida do servidor: QR Code não recebido.');
        }
    } catch (error) {
        console.error('Erro:', error);
        statusEl.textContent = `Erro: ${error.message}`;
    }
});