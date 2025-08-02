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

    const { amount, fname, lname, email, cpf } = req.body;

    if (!amount || typeof amount !== 'number' || amount <= 0) {
        return res.status(400).json({ error: 'Valor inválido.' });
    }

    if (!fname || !lname || !email || !cpf) {
        return res.status(400).json({ error: 'Dados do pagador incompletos.' });
    }

    // A URL base do seu projeto na Vercel, vinda de uma variável de ambiente
    const baseUrl = process.env.VERCEL_URL ? `https://${process.env.VERCEL_URL}` : 'http://localhost:3000';

    const paymentData = {
        transaction_amount: amount,
        description: 'Café',
        payment_method_id: 'pix',
        payer: {
            email: email,
            first_name: fname,
            last_name: lname,
            identification: {
                type: 'CPF',
                number: cpf,
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