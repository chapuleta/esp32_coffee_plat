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
        const { limit = 50 } = req.query; // Mudado para 50 como solicitado
        
        console.log(`🔍 Buscando histórico de doações (últimas ${limit})...`);
        
        // Busca o histórico ordenado por timestamp (mais recentes primeiro)
        const historyRef = db.ref('donations/history')
            .orderByChild('timestamp')
            .limitToLast(parseInt(limit));
        
        const historySnapshot = await historyRef.once('value');
        const historyData = historySnapshot.val();
        
        if (!historyData) {
            console.log('📭 Nenhuma doação encontrada no histórico');
            return res.status(200).json({
                success: true,
                history: []
            });
        }
        
        // Converte para array e ordena por timestamp descendente (mais recente primeiro)
        const donations = Object.keys(historyData)
            .map(key => ({
                id: key,
                donor_name: historyData[key].donor_name || 'Doador Anônimo',
                amount: parseFloat(historyData[key].amount || 0),
                timestamp: historyData[key].timestamp || Date.now(),
                payment_method: historyData[key].payment_method || 'pix'
            }))
            .filter(donation => donation.amount > 0) // Remove doações com valor 0
            .sort((a, b) => b.timestamp - a.timestamp);
        
        console.log(`✅ Encontradas ${donations.length} doações no histórico`);
        console.log('📜 Primeiras 3 doações:', donations.slice(0, 3));
        
        res.status(200).json({
            success: true,
            history: donations,
            total_count: donations.length
        });
        
    } catch (error) {
        console.error('❌ Erro ao buscar histórico:', error);
        res.status(500).json({ 
            success: false,
            error: 'Erro ao buscar histórico de doações',
            message: error.message 
        });
    }
};
