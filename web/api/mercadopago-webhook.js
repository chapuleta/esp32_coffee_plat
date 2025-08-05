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
    // Log de entrada sempre ativo
    console.log(`\n🔔 [${new Date().toISOString()}] Webhook do Mercado Pago recebido!`);
    console.log(`📋 Método: ${req.method}`);
    console.log(`� Headers:`, JSON.stringify(req.headers, null, 2));
    console.log(`📋 Corpo da requisição:`, JSON.stringify(req.body, null, 2));
    
    if (req.method !== 'POST') {
        console.log('❌ Método não permitido:', req.method);
        return res.status(405).send('Method Not Allowed');
    }

    const paymentId = req.body.data?.id;
    const topic = req.body.type;
    
    console.log(`🔍 Extraído da requisição:`);
    console.log(`   - Topic: ${topic}`);
    console.log(`   - Payment ID: ${paymentId}`);

    if (topic === 'payment' && paymentId) {
        try {
            // Busca os detalhes do pagamento no Mercado Pago
            console.log(`🔍 Consultando pagamento ${paymentId} no Mercado Pago...`);
            
            // Faz requisição manual para garantir que pega todos os dados
            const response = await fetch(`https://api.mercadopago.com/v1/payments/${paymentId}?expand=payer`, {
                method: 'GET',
                headers: {
                    'Authorization': `Bearer ${process.env.MERCADOPAGO_ACCESS_TOKEN}`,
                    'Content-Type': 'application/json'
                }
            });
            
            if (!response.ok) {
                throw new Error(`API Error: ${response.status} - ${response.statusText}`);
            }
            
            const paymentData = await response.json();
            console.log(`✅ Dados recebidos da API do Mercado Pago`);
            
            // Log completo dos dados para análise
            console.log(`📋 Dados completos do pagamento:`, JSON.stringify(paymentData, null, 2));
            
            const status = paymentData.status;
            const amount = parseFloat(paymentData.transaction_amount || 0);
            const paymentMethod = paymentData.payment_method_id || 'N/A';
            
            // Extração avançada de dados do pagador com logs detalhados
            let donorName = 'Doador Anônimo';
            let donorEmail = '';
            let donorPhone = '';
            let donorDocument = '';
            
            console.log(`🔍 Analisando dados do pagador...`);
            
            // 🎯 PRIMEIRO: Tenta extrair do external_reference (dados do formulário)
            if (paymentData.external_reference) {
                try {
                    console.log(`📋 External reference encontrado: ${paymentData.external_reference}`);
                    const donorInfo = JSON.parse(paymentData.external_reference);
                    
                    if (donorInfo.name && donorInfo.name.trim()) {
                        donorName = donorInfo.name.trim();
                        console.log(`✅ Nome extraído do formulário: ${donorName}`);
                    }
                    
                    if (donorInfo.email && donorInfo.email.trim()) {
                        donorEmail = donorInfo.email.trim();
                        console.log(`✅ Email extraído do formulário: ${donorEmail}`);
                    }
                    
                    if (donorInfo.phone && donorInfo.phone.trim()) {
                        donorPhone = donorInfo.phone.trim();
                        console.log(`✅ Telefone extraído do formulário: ${donorPhone}`);
                    }
                    
                    console.log(`🎯 Dados do doador extraídos do formulário com sucesso!`);
                    
                } catch (parseError) {
                    console.log(`⚠️ Erro ao fazer parse do external_reference:`, parseError.message);
                }
            }
            
            // Se não conseguiu extrair do external_reference, tenta do payer object
            if (donorName === 'Doador Anônimo') {
                console.log(`   - Payer object existe:`, !!paymentData.payer);
                
                if (paymentData.payer) {
                    console.log(`📋 Dados do payer disponíveis:`, JSON.stringify(paymentData.payer, null, 2));
                    
                    // Nome completo ou primeiro nome
                    if (paymentData.payer.first_name && paymentData.payer.last_name) {
                        donorName = `${paymentData.payer.first_name} ${paymentData.payer.last_name}`;
                        console.log(`✅ Nome completo encontrado: ${donorName}`);
                    } else if (paymentData.payer.first_name) {
                        donorName = paymentData.payer.first_name;
                        console.log(`✅ Primeiro nome encontrado: ${donorName}`);
                    } else if (paymentData.payer.name) {
                        donorName = paymentData.payer.name;
                        console.log(`✅ Nome direto encontrado: ${donorName}`);
                    } else {
                        console.log(`⚠️ Nenhum nome encontrado nos dados do pagador`);
                        // Tenta extrair de outros campos possíveis
                        if (paymentData.additional_info?.payer?.first_name) {
                            donorName = paymentData.additional_info.payer.first_name;
                            console.log(`✅ Nome encontrado em additional_info: ${donorName}`);
                        }
                    }
                    
                    // Email do pagador (se não veio do formulário)
                    if (!donorEmail) {
                        donorEmail = paymentData.payer.email || '';
                        if (!donorEmail && paymentData.additional_info?.payer?.email) {
                            donorEmail = paymentData.additional_info.payer.email;
                        }
                        console.log(`📧 Email encontrado: ${donorEmail || 'Não informado'}`);
                    }
                    
                    // Telefone do pagador (se não veio do formulário)
                    if (!donorPhone) {
                        if (paymentData.payer.phone) {
                            donorPhone = paymentData.payer.phone.number || '';
                            console.log(`📱 Telefone encontrado: ${donorPhone || 'Não informado'}`);
                        } else if (paymentData.additional_info?.payer?.phone?.number) {
                            donorPhone = paymentData.additional_info.payer.phone.number;
                            console.log(`📱 Telefone encontrado em additional_info: ${donorPhone}`);
                        } else {
                            console.log(`📱 Telefone: Não informado`);
                        }
                    }
                    
                    // Documento do pagador (CPF)
                    if (paymentData.payer.identification) {
                        donorDocument = paymentData.payer.identification.number || '';
                        console.log(`🆔 Documento encontrado: ${donorDocument ? donorDocument.replace(/(\d{3})(\d{3})(\d{3})(\d{2})/, '$1.***.**$4') : 'Não informado'}`);
                    } else if (paymentData.additional_info?.payer?.identification?.number) {
                        donorDocument = paymentData.additional_info.payer.identification.number;
                        console.log(`🆔 Documento encontrado em additional_info: ${donorDocument.replace(/(\d{3})(\d{3})(\d{3})(\d{2})/, '$1.***.**$4')}`);
                    } else {
                        console.log(`🆔 Documento: Não informado`);
                    }
                } else {
                    console.log(`❌ Objeto 'payer' não encontrado nos dados do pagamento!`);
                    console.log(`🔍 Tentando buscar em additional_info...`);
                    
                    if (paymentData.additional_info?.payer) {
                        console.log(`📋 Dados em additional_info.payer:`, JSON.stringify(paymentData.additional_info.payer, null, 2));
                        
                        const additionalPayer = paymentData.additional_info.payer;
                        if (additionalPayer.first_name) {
                            donorName = additionalPayer.last_name ? 
                                `${additionalPayer.first_name} ${additionalPayer.last_name}` : 
                                additionalPayer.first_name;
                            console.log(`✅ Nome encontrado em additional_info: ${donorName}`);
                        }
                        
                        if (!donorEmail) donorEmail = additionalPayer.email || '';
                        if (!donorPhone) donorPhone = additionalPayer.phone?.number || '';
                        donorDocument = additionalPayer.identification?.number || '';
                    }
                }
            }
            
            // Informações adicionais para PIX
            let pixInfo = '';
            if (paymentMethod === 'pix') {
                if (paymentData.point_of_interaction?.transaction_data?.pix_qr_code) {
                    pixInfo = 'QR Code PIX';
                } else {
                    pixInfo = 'PIX Copia e Cola';
                }
            }
            
            console.log(`💳 Pagamento ${paymentId}:`);
            console.log(`   - Status: ${status}`);
            console.log(`   - Valor: R$ ${amount.toFixed(2)}`);
            console.log(`   - Método: ${paymentMethod} ${pixInfo ? `(${pixInfo})` : ''}`);
            console.log(`   - Doador: ${donorName}`);
            console.log(`   - Email: ${donorEmail}`);
            console.log(`   - Telefone: ${donorPhone}`);
            console.log(`   - Documento: ${donorDocument ? donorDocument.replace(/(\d{3})(\d{3})(\d{3})(\d{2})/, '$1.***.**$4') : 'N/A'}`);

            if (status === 'approved') {
                console.log(`✅ Pagamento aprovado! Processando automaticamente...`);
                
                // Atualiza o status do pagamento para o ESP32 detectar
                const paymentStatusRef = db.ref('payment_status/status');
                await paymentStatusRef.set('approved');
                
                // AUTOMÁTICO: Atualiza o saldo total e informações dos doadores
                await updateTotalAmount(amount, donorName);
                
                // Registra o histórico completo da doação
                const historyRef = db.ref('donations/history').push();
                await historyRef.set({
                    payment_id: paymentId,
                    amount: amount.toFixed(2),
                    donor_name: donorName,
                    donor_email: donorEmail,
                    donor_phone: donorPhone,
                    donor_document: donorDocument ? donorDocument.replace(/(\d{3})(\d{3})(\d{3})(\d{2})/, '$1.***.**$4') : '',
                    payment_method: paymentMethod,
                    pix_info: pixInfo,
                    timestamp: admin.database.ServerValue.TIMESTAMP,
                    status: 'approved',
                    processed_at: new Date().toISOString()
                });
                
                console.log(`📝 Doação registrada no histórico com dados completos:`);
                console.log(`   - ID: ${paymentId}`);
                console.log(`   - Doador: ${donorName}`);
                console.log(`   - Email: ${donorEmail}`);
                console.log(`   - Valor: R$ ${amount.toFixed(2)}`);
                console.log(`   - Método: ${paymentMethod} ${pixInfo ? `(${pixInfo})` : ''}`);
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