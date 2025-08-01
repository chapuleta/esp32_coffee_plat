const admin = require('firebase-admin');

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
      // Em um cenário real, você usaria o SDK do Mercado Pago para
      // buscar os detalhes completos do pagamento e verificar o status.
      // Por simplicidade, vamos assumir que a notificação já indica sucesso
      const statusFromMercadoPago = 'approved';

      if (statusFromMercadoPago === 'approved') {
        console.log(`Pagamento ${paymentId} aprovado! Atualizando Firebase...`);

        const paymentStatusRef = db.ref('payment_status/status');
        await paymentStatusRef.set('approved');

        console.log('Firebase atualizado para "approved".');
        res.status(200).send('Webhook processado com sucesso!');
      } else {
        console.log(`Pagamento ${paymentId} com status: ${statusFromMercadoPago}. Nenhuma ação no Firebase.`);
        res.status(200).send('Webhook processado, mas sem ação no Firebase.');
      }

    } catch (error) {
      console.error('Erro ao processar webhook ou atualizar Firebase:', error);
      res.status(500).send('Erro interno do servidor.');
    }
  } else {
    console.log('Notificação de webhook não relevante ou inválida.');
    res.status(400).send('Requisição inválida.');
  }
};