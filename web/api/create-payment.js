const mercadopago = require('mercadopago');
const { v4: uuidv4 } = require('uuid');

// Configure o Mercado Pago com seu Access Token
mercadopago.configure({
    access_token: process.env.MERCADOPAGO_ACCESS_TOKEN,
});

module.exports = async (req, res) => {
    if (req.method !== 'POST') {
        return res.status(405).send('Method Not Allowed');
    }

    const { name, email, phone, amount } = req.body;

    console.log('📋 Dados recebidos para criação do PIX:', { name, email, phone, amount });

    // Validações
    if (!name || !name.trim()) {
        return res.status(400).json({ error: 'Nome do doador é obrigatório' });
    }

    if (!amount || isNaN(amount) || amount <= 0) {
        return res.status(400).json({ error: 'Valor inválido' });
    }

    if (amount < 0.50) {
        return res.status(400).json({ error: 'Valor mínimo: R$ 0,50' });
    }

    // A URL base do seu projeto na Vercel
    const baseUrl = process.env.VERCEL_URL ? `https://${process.env.VERCEL_URL}` : 'http://localhost:3000';

    // Preparar dados do doador para external_reference
    const donorInfo = {
        name: name.trim(),
        email: email ? email.trim() : '',
        phone: phone ? phone.trim() : '',
        timestamp: new Date().toISOString()
    };

    const paymentData = {
        transaction_amount: parseFloat(amount),
        description: `Doação - ${name.trim()}`,
        payment_method_id: 'pix',
        external_reference: JSON.stringify(donorInfo), // 🎯 Dados do doador aqui!
        payer: {
            email: email || 'doador@anonimo.com',
            first_name: name.trim().split(' ')[0],
            last_name: name.trim().split(' ').slice(1).join(' ') || 'Doador'
        },
        notification_url: `${baseUrl}/api/mercadopago-webhook`,
    };

    console.log('💳 Criando pagamento PIX:', {
        amount: paymentData.transaction_amount,
        description: paymentData.description,
        external_reference: paymentData.external_reference,
        notification_url: paymentData.notification_url
    });

    try {
        const idempotencyKey = uuidv4();
        const requestOptions = {
            idempotencyKey: idempotencyKey
        };
        
        const response = await mercadopago.payment.create(paymentData, requestOptions);
        
        console.log('✅ Pagamento criado com sucesso:', {
            id: response.body.id,
            status: response.body.status,
            qr_code_available: !!response.body.point_of_interaction?.transaction_data?.qr_code
        });

        // Verificar se o QR code foi gerado
        if (!response.body.point_of_interaction?.transaction_data?.qr_code) {
            console.error('❌ QR Code não foi gerado na resposta do MercadoPago');
            return res.status(500).json({ error: 'QR Code não foi gerado. Tente novamente.' });
        }

        const qr_code_base64 = response.body.point_of_interaction.transaction_data.qr_code_base64;
        const qr_code = response.body.point_of_interaction.transaction_data.qr_code;
        
        res.status(200).json({ 
            payment_id: response.body.id,
            qr_code_base64, 
            qr_code,
            amount: amount,
            donor_name: name.trim(),
            message: `PIX gerado para ${name.trim()} - R$ ${parseFloat(amount).toFixed(2)}`
        });
        
    } catch (error) {
        console.error('❌ Erro ao criar pagamento PIX:', error);
        
        if (error.message && error.message.includes('Invalid payment_method_id')) {
            return res.status(400).json({ error: 'Método de pagamento PIX não disponível' });
        }
        
        if (error.message && error.message.includes('Invalid amount')) {
            return res.status(400).json({ error: 'Valor inválido para PIX' });
        }

        res.status(500).json({ 
            error: 'Erro interno do servidor ao criar pagamento',
            details: process.env.NODE_ENV === 'development' ? error.message : undefined
        });
    }
};