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
    console.log(`🔧 [${new Date().toISOString()}] Endpoint fix-balance chamado`);
    
    if (req.method !== 'POST') {
        return res.status(405).json({ error: 'Method Not Allowed' });
    }
    
    try {
        console.log('🔍 Iniciando diagnóstico e correção do saldo...');
        
        // 1. Busca todo o histórico de doações
        const historyRef = db.ref('donations/history');
        const historySnapshot = await historyRef.once('value');
        const historyData = historySnapshot.val();
        
        if (!historyData) {
            console.log('📭 Nenhum histórico encontrado');
            return res.status(200).json({
                message: 'Nenhum histórico encontrado',
                current_balance: 0,
                history_count: 0
            });
        }
        
        // 2. Converte para array e analisa duplicatas
        const donations = Object.entries(historyData).map(([key, value]) => ({
            id: key,
            ...value
        }));
        
        console.log(`📊 Total de registros no histórico: ${donations.length}`);
        
        // 3. Identifica duplicatas por payment_id
        const paymentIds = {};
        const duplicates = [];
        const uniqueDonations = [];
        
        donations.forEach(donation => {
            const paymentId = donation.payment_id;
            
            if (paymentId) {
                if (paymentIds[paymentId]) {
                    // Duplicata encontrada
                    duplicates.push({
                        duplicate_id: donation.id,
                        payment_id: paymentId,
                        amount: donation.amount,
                        donor_name: donation.donor_name,
                        original_id: paymentIds[paymentId].id
                    });
                    console.log(`🔍 Duplicata encontrada: Payment ID ${paymentId}`);
                } else {
                    // Primeira ocorrência
                    paymentIds[paymentId] = donation;
                    uniqueDonations.push(donation);
                }
            } else {
                // Sem payment_id (doações antigas), manter
                uniqueDonations.push(donation);
            }
        });
        
        console.log(`✅ Doações únicas: ${uniqueDonations.length}`);
        console.log(`⚠️ Duplicatas encontradas: ${duplicates.length}`);
        
        // 4. Calcula saldo correto baseado nas doações únicas
        const correctBalance = uniqueDonations.reduce((total, donation) => {
            const amount = parseFloat(donation.amount || 0);
            return total + amount;
        }, 0);
        
        // 5. Busca saldo atual no Firebase
        const totalRef = db.ref('donations/total_amount');
        const currentTotalSnapshot = await totalRef.once('value');
        const currentBalance = parseFloat(currentTotalSnapshot.val() || 0);
        
        console.log(`💰 Saldo atual no Firebase: R$ ${currentBalance.toFixed(2)}`);
        console.log(`💰 Saldo correto calculado: R$ ${correctBalance.toFixed(2)}`);
        console.log(`💰 Diferença: R$ ${(currentBalance - correctBalance).toFixed(2)}`);
        
        // 6. Encontra último doador e maior doador válidos
        const sortedDonations = uniqueDonations
            .filter(d => d.timestamp)
            .sort((a, b) => b.timestamp - a.timestamp);
        
        const lastDonor = sortedDonations.length > 0 ? sortedDonations[0].donor_name : 'Nenhum ainda';
        
        const topDonation = uniqueDonations.reduce((max, donation) => {
            const amount = parseFloat(donation.amount || 0);
            return amount > max.amount ? { donor: donation.donor_name, amount } : max;
        }, { donor: 'Nenhum ainda', amount: 0 });
        
        // 7. Prepara resposta com diagnóstico
        const diagnosis = {
            timestamp: new Date().toISOString(),
            current_balance: currentBalance,
            correct_balance: correctBalance,
            difference: currentBalance - correctBalance,
            needs_correction: Math.abs(currentBalance - correctBalance) > 0.01,
            total_donations: donations.length,
            unique_donations: uniqueDonations.length,
            duplicates_found: duplicates.length,
            last_donor: lastDonor,
            top_donor: topDonation.donor,
            top_amount: topDonation.amount,
            duplicates: duplicates
        };
        
        // 8. Se foi solicitada correção, aplica as mudanças
        if (req.body.apply_fix === true && diagnosis.needs_correction) {
            console.log('🔧 Aplicando correção automática...');
            
            // Remove duplicatas do histórico
            for (const duplicate of duplicates) {
                await db.ref(`donations/history/${duplicate.duplicate_id}`).remove();
                console.log(`🗑️ Removida duplicata: ${duplicate.duplicate_id} (Payment ID: ${duplicate.payment_id})`);
            }
            
            // Atualiza saldo correto
            await totalRef.set(correctBalance.toFixed(1));
            
            // Atualiza último doador
            await db.ref('donations/last_donor').set(lastDonor);
            
            // Atualiza maior doador
            await db.ref('donations/top_donor').set(topDonation.donor);
            await db.ref('donations/top_amount').set(topDonation.amount.toFixed(2));
            
            console.log('✅ Correção aplicada com sucesso!');
            
            diagnosis.correction_applied = true;
            diagnosis.correction_timestamp = new Date().toISOString();
        }
        
        return res.status(200).json(diagnosis);
        
    } catch (error) {
        console.error('❌ Erro no diagnóstico/correção:', error);
        return res.status(500).json({
            error: 'Erro interno no diagnóstico',
            message: error.message,
            timestamp: new Date().toISOString()
        });
    }
};
