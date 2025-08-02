const mercadopago = require('mercadopago');

// Configure o Mercado Pago com seu Access Token
mercadopago.configure({
    access_token: process.env.MERCADOPAGO_ACCESS_TOKEN,
});

module.exports = async (req, res) => {
    const { id } = req.query;

    if (!id) {
        return res.status(400).json({ error: 'Por favor, forneça um ID de pagamento na URL. Ex: /api/test-find?id=12345' });
    }

    try {
        console.log(`[test-find] Consultando pagamento ${id} no Mercado Pago...`);
        const payment = await mercadopago.payment.findById(id);
        console.log('[test-find] Resposta do Mercado Pago:', payment.body);
        
        // Se a chamada for bem-sucedida, retorna os detalhes do pagamento
        return res.status(200).json(payment.body);

    } catch (error) {
        console.error('[test-find] Erro ao consultar o pagamento:', error);
        
        // Se ocorrer um erro (como 401), retorna o erro
        return res.status(error.status || 500).json({ 
            message: 'Erro ao consultar o pagamento.',
            error_details: error.message,
            status: error.status
        });
    }
};