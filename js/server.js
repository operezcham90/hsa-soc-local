require('./config').config();
const ip = require('./ip.js').ip;
const log = require('./log.js').log;
const http = require('http');
const fs = require('fs');
http.createServer(function (req, res) {
    var stat = fs.statSync('/root/hsa-soc-local/js/index.html');
    response.writeHead(200, {
        'Content-Type': 'text/html',
        'Content-Length': stat.size
    });
    var rs = fs.createReadStream('/root/hsa-soc-local/js/index.html');
    rs.pipe(res);
}).listen(process.env.PORT, ip, function () {
    const url = `http://${ip}:${process.env.PORT}`;
    const message = `Listening on <a href=${url}>${url}</a>`;
    log(message);
});