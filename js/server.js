require('./config').config();
const ip = require('./ip.js').ip;
const log = require('./log.js').log;
const http = require('http');
const fs = require('fs');
http.createServer(function (req, res) {
    let file = '/root/hsa-soc-local/html/error.html';
    let type = 'text/html';
    if (req.path === '/') {
        file = '/root/hsa-soc-local/html/index.html';
        type = 'text/html';
    } else if (req.path === '/dogo') {
        file = '/root/hsa-soc-local/img/dog.jpg';
        type = 'image/jpg';
    }
    const stat = fs.statSync(file);
    res.writeHead(200, {
        'Content-Type': type,
        'Content-Length': stat.size
    });
    const rs = fs.createReadStream(file);
    rs.pipe(res);
}).listen(process.env.PORT);
const url = `http://${ip}:${process.env.PORT}`;
const message = `Listening on <a href=${url}>${url}</a>`;
log(message);