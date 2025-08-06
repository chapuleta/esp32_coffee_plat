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

    try {
        console.log(`🧹 INICIANDO LIMPEZA DE DUPLICATAS...`);
        
        // Busca todo o histórico
        const historyRef = db.ref('donations/history');
        const historySnapshot = await historyRef.once('value');
        const historyData = historySnapshot.val();
        
        if (!historyData) {
            console.log('📭 Nenhum histórico encontrado');
            return res.status(200).json({
                success: true,
                message: 'Nenhum histórico encontrado',
                removed: 0
            });
        }
        
        console.log(`🔍 Analisando ${Object.keys(historyData).length} registros...`);
        
        // Converte para array com IDs
        const donations = Object.keys(historyData).map(key => ({
            firebaseId: key,
            payment_id: historyData[key].payment_id || key,
            donor_name: historyData[key].donor_name || 'Doador Anônimo',
            amount: parseFloat(historyData[key].amount || 0),
            timestamp: historyData[key].timestamp || Date.now(),
            data: historyData[key]
        }));
        
        // Encontra duplicatas por payment_id
        const paymentIdGroups = {};
        const duplicates = [];
        const toKeep = [];
        
        for (const donation of donations) {
            if (!paymentIdGroups[donation.payment_id]) {
                paymentIdGroups[donation.payment_id] = [];
            }
            paymentIdGroups[donation.payment_id].push(donation);
        }
        
        // Para cada payment_id, mantém apenas o mais recente
        for (const [paymentId, group] of Object.entries(paymentIdGroups)) {
            if (group.length > 1) {
                console.log(`🔍 Duplicata encontrada para payment_id: ${paymentId} (${group.length} registros)`);
                
                // Ordena por timestamp (mais recente primeiro)
                group.sort((a, b) => b.timestamp - a.timestamp);
                
                // Mantém o primeiro (mais recente)
                toKeep.push(group[0]);
                console.log(`   ✅ Mantendo: ${group[0].firebaseId} (${new Date(group[0].timestamp).toLocaleString()})`);
                
                // Marca os outros para remoção
                for (let i = 1; i < group.length; i++) {
                    duplicates.push(group[i]);
                    console.log(`   ❌ Removendo: ${group[i].firebaseId} (${new Date(group[i].timestamp).toLocaleString()})`);
                }
            } else {
                toKeep.push(group[0]);
            }
        }
        
        console.log(`📊 RESULTADO DA ANÁLISE:`);
        console.log(`   - Total de registros: ${donations.length}`);
        console.log(`   - Registros únicos: ${toKeep.length}`);
        console.log(`   - Duplicatas encontradas: ${duplicates.length}`);
        
        // Remove as duplicatas
        if (duplicates.length > 0) {
            console.log(`🗑️ Removendo ${duplicates.length} duplicatas...`);
            
            const batch = {};
            for (const duplicate of duplicates) {
                batch[`donations/history/${duplicate.firebaseId}`] = null;
            }
            
            await db.ref().update(batch);
            console.log(`✅ ${duplicates.length} duplicatas removidas com sucesso!`);
        } else {
            console.log(`✅ Nenhuma duplicata encontrada!`);
        }
        
        // Recalcula o saldo total baseado nos registros únicos
        const totalAmount = toKeep.reduce((sum, donation) => sum + donation.amount, 0);
        console.log(`💰 Recalculando saldo total: R$ ${totalAmount.toFixed(2)}`);
        
        // Atualiza o saldo no Firebase
        await db.ref('donations/total_amount').set(totalAmount.toFixed(2));
        
        // Retorna resultado
        res.status(200).json({
            success: true,
            message: `Limpeza concluída! ${duplicates.length} duplicatas removidas.`,
            total_records: donations.length,
            unique_records: toKeep.length,
            duplicates_removed: duplicates.length,
            new_total_amount: totalAmount.toFixed(2),
            removed_details: duplicates.map(d => ({
                firebase_id: d.firebaseId,
                payment_id: d.payment_id,
                donor_name: d.donor_name,
                amount: d.amount,
                timestamp: new Date(d.timestamp).toLocaleString()
            }))
        });
        
    } catch (error) {
        console.error('❌ Erro durante limpeza:', error);
        res.status(500).json({ 
            success: false,
            error: 'Erro durante limpeza de duplicatas',
            message: error.message 
        });
    }
};
