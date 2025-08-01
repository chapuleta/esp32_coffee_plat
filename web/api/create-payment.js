const mercadopago = require('mercadopago');

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

    const preference = {
        items: [
            {
                title: 'Café',
                quantity: 1,
                currency_id: 'BRL',
                unit_price: amount,
            },
        ],
        payment_methods: {
            excluded_payment_types: [
                { id: "ticket" },
                { id: "digital_currency" },
                { id: "debit_card" },
                { id: "credit_card" },
            ],
            installments: 1
        },
        notification_url: `${baseUrl}/api/mercadopago-webhook`,
        binary_mode: true,
    };

    try {
        const response = await mercadopago.preferences.create(preference);
        const qr_data = response.body.point_of_interaction.transaction_data.qr_code_base64;
        
        res.status(200).json({ qr_data });
    } catch (error) {
        console.error('Erro ao criar preferência no Mercado Pago:', error);
        res.status(500).json({ error: 'Erro interno do servidor.' });
    }
};