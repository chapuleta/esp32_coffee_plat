const express = require('express');
const app = express();
app.use(express.json());

let lastDonation = { nome: '', valor: 0 };

app.post('/api/donation', (req, res) => {
  lastDonation = req.body;
  res.json({ success: true });
});

app.get('/api/donation/latest', (req, res) => {
  res.json(lastDonation);
});

app.listen(process.env.PORT || 3000);
