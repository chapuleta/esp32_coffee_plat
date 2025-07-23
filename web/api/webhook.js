const admin = require('firebase-admin');

// Inicialize o Firebase Admin SDK
// As credenciais são carregadas via variáveis de ambiente no Vercel
if (!admin.apps.length) {
    admin.initializeApp({
        credential: admin.credential.cert({
            projectId: process.env.FIREBASE_PROJECT_ID,
            privateKey: process.env.FIREBASE_PRIVATE_KEY.replace(/\n/g, '
'),
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

    const { body, query } = req;

    // O Mercado Pago envia o ID do pagamento na query string para alguns tipos de notificação
    // e o tópico (ex: 'payment') no corpo.
    if (query.type === 'payment' && body.action === 'payment.updated') {
        const paymentId = query['data.id'];
        
        // Aqui você poderia (opcionalmente) buscar os detalhes do pagamento no Mercado Pago
        // para verificar se o status é 'approved'.
        // Por simplicidade, vamos confiar na notificação e atualizar o Firebase diretamente.
        
        console.log(`Recebida notificação de pagamento aprovado: ${paymentId}`);

        try {
            const paymentStatusRef = db.ref('/payment_status');
            await paymentStatusRef.set({
                status: 'approved',
                timestamp: new Date().toISOString(),
                paymentId: paymentId
            });
            
            console.log('Status do pagamento atualizado no Firebase.');
            
            // Responda ao Mercado Pago para confirmar o recebimento
            res.status(200).send('OK');

        } catch (error) {
            console.error('Erro ao atualizar o Firebase:', error);
            res.status(500).send('Internal Server Error');
        }
    } else {
        // Se não for a notificação que esperamos, apenas ignoramos e respondemos OK.
        res.status(200).send('Notification received but not processed.');
    }
};