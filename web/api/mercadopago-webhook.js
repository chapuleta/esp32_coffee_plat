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

// Função para atualizar o saldo total automaticamente
async function updateTotalAmount(newPaymentAmount, donorName) {
    try {
        console.log(`🧮 Atualizando saldo total com nova doação de R$ ${newPaymentAmount}`);
        
        // Busca o valor atual do total
        const totalRef = db.ref('donations/total_amount');
        const currentTotalSnapshot = await totalRef.once('value');
        const currentTotal = parseFloat(currentTotalSnapshot.val() || '0');
        
        // Calcula o novo total
        const newTotal = currentTotal + newPaymentAmount;
        
        console.log(`💰 Saldo anterior: R$ ${currentTotal.toFixed(2)}`);
        console.log(`💰 Nova doação: R$ ${newPaymentAmount.toFixed(2)}`);
        console.log(`💰 Novo saldo total: R$ ${newTotal.toFixed(2)}`);
        
        // Atualiza o total no Firebase
        await totalRef.set(newTotal.toFixed(1));
        
        // Atualiza o último doador
        const lastDonorRef = db.ref('donations/last_donor');
        await lastDonorRef.set(donorName);
        
        // Verifica e atualiza o maior doador se necessário
        await updateTopDonorIfNeeded(newPaymentAmount, donorName);
        
        console.log(`✅ Firebase atualizado automaticamente!`);
        console.log(`   - Total: R$ ${newTotal.toFixed(2)}`);
        console.log(`   - Último doador: ${donorName}`);
        
        return newTotal;
        
    } catch (error) {
        console.error('❌ Erro ao atualizar saldo total:', error);
        throw error;
    }
}

// Função para verificar e atualizar o maior doador
async function updateTopDonorIfNeeded(newAmount, donorName) {
    try {
        // Busca informações do maior doador atual
        const topDonorRef = db.ref('donations/top_donor');
        const topAmountRef = db.ref('donations/top_amount');
        
        const [topDonorSnapshot, topAmountSnapshot] = await Promise.all([
            topDonorRef.once('value'),
            topAmountRef.once('value')
        ]);
        
        const currentTopDonor = topDonorSnapshot.val() || '';
        const currentTopAmount = parseFloat(topAmountSnapshot.val() || '0');
        
        console.log(`🏆 Maior doação atual: R$ ${currentTopAmount.toFixed(2)} por ${currentTopDonor}`);
        console.log(`🔍 Verificando se R$ ${newAmount.toFixed(2)} de ${donorName} é maior...`);
        
        if (newAmount > currentTopAmount) {
            console.log(`🎉 Nova maior doação! Atualizando recordes...`);
            await Promise.all([
                topDonorRef.set(donorName),
                topAmountRef.set(newAmount.toFixed(2))
            ]);
            console.log(`✅ Novo maior doador: ${donorName} com R$ ${newAmount.toFixed(2)}`);
        } else {
            console.log(`📊 Doação não supera o recorde atual de R$ ${currentTopAmount.toFixed(2)}`);
        }
        
    } catch (error) {
        console.error('❌ Erro ao verificar maior doador:', error);
    }
}

module.exports = async (req, res) => {
    if (req.method !== 'POST') {
        return res.status(405).send('Method Not Allowed');
    }

    console.log('🔔 Webhook do Mercado Pago recebido!');
    console.log('📋 Corpo da requisição:', req.body);

    const paymentId = req.body.data?.id;
    const topic = req.body.type;

    if (topic === 'payment' && paymentId) {
        try {
            // Busca os detalhes do pagamento no Mercado Pago
            console.log(`🔍 Consultando pagamento ${paymentId} no Mercado Pago...`);
            const payment = await mercadopago.payment.findById(paymentId);
            const paymentData = payment.body;
            
            const status = paymentData.status;
            const amount = parseFloat(paymentData.transaction_amount || 0);
            const donorName = paymentData.payer?.first_name || 'Doador Anônimo';
            const paymentMethod = paymentData.payment_method_id || 'N/A';
            
            console.log(`💳 Pagamento ${paymentId}:`);
            console.log(`   - Status: ${status}`);
            console.log(`   - Valor: R$ ${amount.toFixed(2)}`);
            console.log(`   - Doador: ${donorName}`);
            console.log(`   - Método: ${paymentMethod}`);

            if (status === 'approved') {
                console.log(`✅ Pagamento aprovado! Processando automaticamente...`);
                
                // Atualiza o status do pagamento para o ESP32 detectar
                const paymentStatusRef = db.ref('payment_status/status');
                await paymentStatusRef.set('approved');
                
                // AUTOMÁTICO: Atualiza o saldo total e informações dos doadores
                await updateTotalAmount(amount, donorName);
                
                // Registra o histórico da doação
                const historyRef = db.ref('donations/history').push();
                await historyRef.set({
                    payment_id: paymentId,
                    amount: amount.toFixed(2),
                    donor: donorName,
                    method: paymentMethod,
                    timestamp: admin.database.ServerValue.TIMESTAMP,
                    status: 'approved'
                });
                
                console.log(`📝 Doação registrada no histórico`);
                console.log(`🎉 Sistema atualizado automaticamente!`);
                
            } else {
                console.log(`⏳ Pagamento com status "${status}" - aguardando aprovação`);
                
                // Atualiza status para outros estados (pending, rejected, etc.)
                const paymentStatusRef = db.ref('payment_status/status');
                await paymentStatusRef.set(status);
            }

            // Responde ao Mercado Pago que o webhook foi processado
            return res.status(200).json({
                message: 'Webhook processado com sucesso',
                payment_id: paymentId,
                status: status,
                processed_at: new Date().toISOString()
            });

        } catch (error) {
            console.error('❌ Erro ao processar webhook:', error);
            
            // Registra o erro no Firebase para debugging
            const errorRef = db.ref('webhook_errors').push();
            await errorRef.set({
                payment_id: paymentId,
                error: error.message,
                timestamp: admin.database.ServerValue.TIMESTAMP,
                request_body: req.body
            });
            
            return res.status(500).json({
                error: 'Erro interno ao processar webhook',
                payment_id: paymentId
            });
        }
    } else {
        console.log('⚠️ Notificação de webhook não relevante ou inválida');
        console.log(`   - Topic: ${topic}`);
        console.log(`   - Payment ID: ${paymentId}`);
        
        return res.status(400).json({
            error: 'Requisição inválida',
            received_topic: topic,
            received_payment_id: paymentId
        });
    }
};