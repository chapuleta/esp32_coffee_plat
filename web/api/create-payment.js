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

    const { amount } = req.body;

    if (!amount || typeof amount !== 'number' || amount <= 0) {
        return res.status(400).json({ error: 'Valor inválido.' });
    }

    // A URL base do seu projeto na Vercel, vinda de uma variável de ambiente
    const baseUrl = process.env.VERCEL_URL ? `https://${process.env.VERCEL_URL}` : 'http://localhost:3000';

    const paymentData = {
        transaction_amount: amount,
        description: 'Café',
        payment_method_id: 'pix',
        payer: {
            email: 'test_user@example.com', // Substitua por um email de teste válido
            first_name: 'Test',
            last_name: 'User',
            identification: {
                type: 'CPF',
                number: '11111111111', // Use um CPF de teste válido se possível
            },
        },
        notification_url: `${baseUrl}/api/mercadopago-webhook`,
    };

    try {
        const idempotencyKey = uuidv4();
        const requestOptions = {
            idempotencyKey: idempotencyKey
        };
        const response = await mercadopago.payment.create(paymentData, requestOptions);
        const qr_code_base64 = response.body.point_of_interaction.transaction_data.qr_code_base64;
        const qr_code = response.body.point_of_interaction.transaction_data.qr_code;
        
        res.status(200).json({ qr_code_base64, qr_code });
    } catch (error) {
        console.error('Erro ao criar preferência no Mercado Pago:', error);
        res.status(500).json({ error: 'Erro interno do servidor.' });
    }
};