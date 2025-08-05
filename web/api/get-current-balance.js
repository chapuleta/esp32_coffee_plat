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
    if (req.method !== 'GET') {
        return res.status(405).send('Method Not Allowed');
    }

    try {
        // 🚀 Headers para otimizar cache e performance
        res.setHeader('Cache-Control', 'no-cache, no-store, must-revalidate');
        res.setHeader('Pragma', 'no-cache');
        res.setHeader('Expires', '0');
        
        console.log('🔍 Buscando dados atuais do Firebase...');
        
        // 🚀 Busca paralela otimizada com timeout
        const timeout = new Promise((_, reject) => 
            setTimeout(() => reject(new Error('Firebase timeout')), 5000)
        );
        
        const dataPromise = Promise.all([
            db.ref('donations/total_amount').once('value'),
            db.ref('donations/last_donor').once('value'),
            db.ref('donations/top_donor').once('value'),
            db.ref('donations/top_amount').once('value')
        ]);
        
        const [totalSnapshot, lastDonorSnapshot, topDonorSnapshot, topAmountSnapshot] = 
            await Promise.race([dataPromise, timeout]);
        
        const currentData = {
            total_amount: parseFloat(totalSnapshot.val() || '0'),
            last_donor: lastDonorSnapshot.val() || 'Nenhum doador ainda',
            top_donor: topDonorSnapshot.val() || 'Nenhum doador ainda',
            top_amount: parseFloat(topAmountSnapshot.val() || '0'),
            updated_at: new Date().toISOString()
        };
        
        console.log('✅ Dados atuais recuperados em', process.hrtime.bigint(), 'ns:', currentData);
        
        res.status(200).json(currentData);
        
    } catch (error) {
        console.error('❌ Erro ao buscar dados do Firebase:', error);
        res.status(500).json({ 
            error: 'Erro ao buscar dados atuais',
            message: error.message,
            timestamp: new Date().toISOString()
        });
    }
};
