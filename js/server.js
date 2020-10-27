require('./config').config();
const ip = require('./ip.js').ip;
const log = require('./log.js').log;
const http = require('http');
const fs = require('fs');
http.createServer(function (req, res) {
    const file = '/root/hsa-soc-local/js/index.html';
    const stat = fs.statSync(file);
    res.writeHead(200, {
        'Content-Type': 'text/html',
        'Content-Length': stat.size
    });
    const rs = fs.createReadStream(file);
    rs.pipe(res);
}).listen(process.env.PORT);
const url = `http://${ip}:${process.env.PORT}`;
const message = `Listening on <a href=${url}>${url}</a>`;
log(message);