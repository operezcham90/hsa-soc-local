require('./config').config();
const ip = require('./ip.js').ip;
const log = require('./log.js').log;
const http = require('http');
const fs = require('fs');
http.createServer(function (req, res) {
    var stat = fs.statSync('./index.html');
    response.writeHead(200, {
        'Content-Type': 'text/html',
        'Content-Length': stat.size
    });
    var rs = fs.createReadStream('./index.html');
    rs.pipe(res);
}).listen(process.env.PORT, 'localhost', function () {
    const url = `http://${ip}:${process.env.PORT}`;
    const message = `Listening on <a href=${url}>${url}</a>`;
    log(message);
});