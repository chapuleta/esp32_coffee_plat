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
    console.log('🧪 Endpoint de teste do webhook ativado!');
    
    try {
        // Simula dados de um pagamento PIX aprovado
        const simulatedPayment = {
            id: `TEST_${Date.now()}`,
            status: 'approved',
            transaction_amount: 0.01,
            payment_method_id: 'pix',
            payer: {
                first_name: 'João',
                last_name: 'Victor',
                email: 'joao.victor@email.com',
                phone: {
                    number: '11999999999'
                },
                identification: {
                    type: 'CPF',
                    number: '12345678901'
                }
            },
            point_of_interaction: {
                transaction_data: {
                    pix_qr_code: 'test_qr_code'
                }
            }
        };

        console.log('📋 Simulando pagamento:', JSON.stringify(simulatedPayment, null, 2));

        // Extrai dados do pagador
        const donorName = `${simulatedPayment.payer.first_name} ${simulatedPayment.payer.last_name}`;
        const donorEmail = simulatedPayment.payer.email;
        const donorPhone = simulatedPayment.payer.phone.number;
        const donorDocument = simulatedPayment.payer.identification.number;
        const amount = simulatedPayment.transaction_amount;

        console.log(`💳 Pagamento de teste:`);
        console.log(`   - ID: ${simulatedPayment.id}`);
        console.log(`   - Status: ${simulatedPayment.status}`);
        console.log(`   - Valor: R$ ${amount.toFixed(2)}`);
        console.log(`   - Doador: ${donorName}`);
        console.log(`   - Email: ${donorEmail}`);
        console.log(`   - Telefone: ${donorPhone}`);
        console.log(`   - CPF: ${donorDocument.replace(/(\d{3})(\d{3})(\d{3})(\d{2})/, '$1.***.**$4')}`);

        // Atualiza o Firebase com dados do teste
        
        // 1. Atualiza status do pagamento
        const paymentStatusRef = db.ref('payment_status/status');
        await paymentStatusRef.set('approved');
        
        // 2. Busca total atual e calcula novo total
        const totalRef = db.ref('donations/total_amount');
        const currentTotalSnapshot = await totalRef.once('value');
        const currentTotal = parseFloat(currentTotalSnapshot.val() || '0');
        const newTotal = currentTotal + amount;
        
        console.log(`💰 Saldo anterior: R$ ${currentTotal.toFixed(2)}`);
        console.log(`💰 Nova doação: R$ ${amount.toFixed(2)}`);
        console.log(`💰 Novo saldo total: R$ ${newTotal.toFixed(2)}`);
        
        // 3. Atualiza total e último doador
        await totalRef.set(newTotal.toFixed(1));
        
        const lastDonorRef = db.ref('donations/last_donor');
        await lastDonorRef.set(donorName);
        
        // 4. Verifica maior doador
        const topAmountRef = db.ref('donations/top_amount');
        const topDonorRef = db.ref('donations/top_donor');
        
        const topAmountSnapshot = await topAmountRef.once('value');
        const currentTopAmount = parseFloat(topAmountSnapshot.val() || '0');
        
        if (amount > currentTopAmount) {
            await topAmountRef.set(amount.toFixed(2));
            await topDonorRef.set(donorName);
            console.log(`🏆 Novo recorde! ${donorName} com R$ ${amount.toFixed(2)}`);
        }
        
        // 5. Registra no histórico
        const historyRef = db.ref('donations/history').push();
        await historyRef.set({
            payment_id: simulatedPayment.id,
            amount: amount.toFixed(2),
            donor_name: donorName,
            donor_email: donorEmail,
            donor_phone: donorPhone,
            donor_document: donorDocument.replace(/(\d{3})(\d{3})(\d{3})(\d{2})/, '$1.***.**$4'),
            payment_method: 'pix',
            pix_info: 'QR Code PIX (TESTE)',
            timestamp: admin.database.ServerValue.TIMESTAMP,
            status: 'approved',
            processed_at: new Date().toISOString(),
            is_test: true
        });

        console.log(`✅ Teste concluído! Firebase atualizado com:`);
        console.log(`   - Total: R$ ${newTotal.toFixed(2)}`);
        console.log(`   - Último doador: ${donorName}`);
        console.log(`   - Dados completos registrados no histórico`);
        console.log(`🎉 O ESP32 deve detectar as mudanças em ~10 segundos!`);

        return res.status(200).json({
            message: 'Teste do webhook concluído com sucesso!',
            test_data: {
                payment_id: simulatedPayment.id,
                donor_name: donorName,
                donor_email: donorEmail,
                donor_phone: donorPhone,
                donor_document_masked: donorDocument.replace(/(\d{3})(\d{3})(\d{3})(\d{2})/, '$1.***.**$4'),
                amount: amount,
                new_total: newTotal,
                timestamp: new Date().toISOString()
            },
            instructions: [
                'Verifique o Firebase Console para confirmar as atualizações',
                'Monitore o ESP32 serial para ver se detecta as mudanças',
                'O display deve atualizar em ~10 segundos'
            ]
        });

    } catch (error) {
        console.error('❌ Erro no teste do webhook:', error);
        
        return res.status(500).json({
            error: 'Erro ao executar teste do webhook',
            details: error.message,
            timestamp: new Date().toISOString()
        });
    }
};
