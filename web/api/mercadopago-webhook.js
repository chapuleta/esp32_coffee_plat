const admin = require('firebase-admin');
const mercadopago = require('mercadopago');

// Configure o Mercado Pago com seu Access Token
mercadopago.configure({
    access_token: process.env.MERCADOPAGO_ACCESS_TOKEN,
});

// Inicializa o Firebase Admin SDK apenas uma vez
if (!admin.apps.length) {
    admin.initializeApp({
        credential: admin.credential.cert({
            projectId: process.env.FIREBASE_PROJECT_ID,
            privateKey: process.env.FIREBASE_PRIVATE_KEY.replace(/\n/g, '\n'),
            clientEmail: process.env.FIREBASE_CLIENT_EMAIL,
        }),
        databaseURL: process.env.FIREBASE_DATABASE_URL,
    });
}

const db = admin.database();

module.exports = async (req, res) => {
    if (req.method !== 'POST') {
        return res.status(405).send('Method Not Allowed');
    }

    console.log('Webhook do Mercado Pago recebido!');
    console.log('Corpo da requisição:', req.body);

    const paymentId = req.body.data?.id;
    const topic = req.body.type;

    if (topic === 'payment' && paymentId) {
        try {
            // Responde imediatamente ao Mercado Pago para confirmar o recebimento
            res.status(200).send('Webhook recebido.');

            // Busca os detalhes do pagamento no Mercado Pago
            console.log(`Consultando pagamento ${paymentId} no Mercado Pago...`);
            const payment = await mercadopago.payment.findById(paymentId);
            const statusFromMercadoPago = payment.body.status;

            console.log(`Status do pagamento ${paymentId}: ${statusFromMercadoPago}`);

            if (statusFromMercadoPago === 'approved') {
                console.log(`Pagamento ${paymentId} aprovado! Atualizando Firebase...`);

                const paymentStatusRef = db.ref('payment_status/status');
                await paymentStatusRef.set('approved');

                console.log('Firebase atualizado para "approved".');
            } else {
                console.log(`Pagamento ${paymentId} com status: ${statusFromMercadoPago}. Nenhuma ação no Firebase.`);
            }

        } catch (error) {
            console.error('Erro ao processar webhook ou atualizar Firebase:', error);
            // Não envie um status 500 aqui, pois a resposta inicial já foi enviada.
        }
    } else {
        console.log('Notificação de webhook não relevante ou inválida.');
        res.status(400).send('Requisição inválida.');
    }
};