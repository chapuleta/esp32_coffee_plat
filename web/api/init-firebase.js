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
        console.log('🔧 Inicializando dados de teste no Firebase...');
        
        // Verifica se já existem dados
        const totalRef = db.ref('donations/total_amount');
        const currentTotalSnapshot = await totalRef.once('value');
        const currentTotal = currentTotalSnapshot.val();
        
        if (currentTotal !== null && currentTotal !== undefined) {
            console.log(`💰 Dados já existem: R$ ${currentTotal}`);
            return res.status(200).json({
                message: 'Dados já existem no Firebase',
                current_total: currentTotal
            });
        }
        
        // Cria dados iniciais
        const initialData = {
            'donations/total_amount': 0,
            'donations/last_donor': 'Nenhum doador ainda',
            'donations/top_donor': 'Nenhum doador ainda',
            'donations/top_amount': 0,
            'payment_status/status': 'none'
        };
        
        // Aplica os dados iniciais
        await db.ref().update(initialData);
        
        console.log('✅ Dados iniciais criados no Firebase');
        
        res.status(200).json({
            message: 'Firebase inicializado com sucesso',
            data: initialData
        });
        
    } catch (error) {
        console.error('❌ Erro ao inicializar Firebase:', error);
        res.status(500).json({ 
            error: 'Erro ao inicializar dados',
            message: error.message 
        });
    }
};
