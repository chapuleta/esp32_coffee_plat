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
    // Allow both GET and POST requests
    if (req.method !== 'POST' && req.method !== 'GET') {
        return res.status(405).send('Method Not Allowed');
    }

    try {
        console.log('🧪 Adicionando doação de teste...');
        
        const testDonation = {
            donor_name: 'João Silva (Teste)',
            amount: 25.50,
            payment_method: 'pix',
            timestamp: Date.now(),
            payment_id: 'test_' + Date.now()
        };
        
        // Busca o valor atual do total
        const totalRef = db.ref('donations/total_amount');
        const currentTotalSnapshot = await totalRef.once('value');
        const currentTotal = parseFloat(currentTotalSnapshot.val() || '0');
        
        // Calcula o novo total
        const newTotal = currentTotal + testDonation.amount;
        
        // Atualiza os dados principais
        await totalRef.set(newTotal);
        await db.ref('donations/last_donor').set(testDonation.donor_name);
        
        // Verifica e atualiza o maior doador se necessário
        const topAmountRef = db.ref('donations/top_amount');
        const topAmountSnapshot = await topAmountRef.once('value');
        const currentTopAmount = parseFloat(topAmountSnapshot.val() || '0');
        
        if (testDonation.amount > currentTopAmount) {
            await db.ref('donations/top_donor').set(testDonation.donor_name);
            await db.ref('donations/top_amount').set(testDonation.amount);
        }
        
        // Adiciona ao histórico
        const historyRef = db.ref('donations/history').push();
        await historyRef.set(testDonation);
        
        console.log('✅ Doação de teste adicionada com sucesso');
        console.log(`💰 Novo total: R$ ${newTotal.toFixed(2)}`);
        
        res.status(200).json({
            message: 'Doação de teste adicionada com sucesso',
            donation: testDonation,
            new_total: newTotal
        });
        
    } catch (error) {
        console.error('❌ Erro ao adicionar doação de teste:', error);
        res.status(500).json({ 
            error: 'Erro ao adicionar doação de teste',
            message: error.message 
        });
    }
};
