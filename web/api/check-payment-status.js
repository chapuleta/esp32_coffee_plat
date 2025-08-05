const mercadopago = require('mercadopago');

// Configure o Mercado Pago com seu Access Token
mercadopago.configure({
    access_token: process.env.MERCADOPAGO_ACCESS_TOKEN,
});

module.exports = async (req, res) => {
    if (req.method !== 'GET') {
        return res.status(405).send('Method Not Allowed');
    }

    const { payment_id } = req.query;

    if (!payment_id) {
        return res.status(400).json({ error: 'Payment ID é obrigatório' });
    }

    try {
        console.log(`🔍 Verificando status do pagamento ${payment_id}...`);
        
        const response = await mercadopago.payment.get(payment_id);
        const paymentData = response.body;
        
        console.log(`📋 Status do pagamento ${payment_id}: ${paymentData.status}`);
        
        res.status(200).json({
            payment_id: payment_id,
            status: paymentData.status,
            amount: paymentData.transaction_amount,
            checked_at: new Date().toISOString()
        });
        
    } catch (error) {
        console.error('❌ Erro ao verificar status do pagamento:', error);
        res.status(500).json({ 
            error: 'Erro ao verificar status do pagamento',
            payment_id: payment_id 
        });
    }
};
