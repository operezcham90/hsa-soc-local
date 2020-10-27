const { networkInterfaces } = require('os');
const nets = networkInterfaces();
exports.ip = nets['eth0'][0]['address'];