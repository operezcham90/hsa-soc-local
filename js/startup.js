//load config
require('./config').config();
// get ip
const { networkInterfaces } = require('os');
const nets = networkInterfaces();
const ip = nets['eth0'][0]['address'];
// execute terminal commands
const { exec } = require('child_process');
// http server
const http = require('http');
http.createServer(function (req, res) {
    // response
    res.writeHead(200, {
        'Content-Type': 'text/plain'
    });
    res.write('Hi');
    res.end();
}).listen(process.env.PORT);
// log message
const link = `http://${ip}:${process.env.PORT}`;
const message = `Listening on <a href=${link}>${link}</a>`;
// data
const data = '"' + JSON.stringify({
    message: message,
    from: 'HSA SoC Local Server',
    secret: process.env.SECRET
}).replace(/"/g, '\\"') + '"';
// headers
const headers = `"Content-Type: application/json"`;
// url
const url = process.env.REMOTE + '/log';
// full command
const command = `curl --data ${data} -H ${headers} ${url}`;
console.log(command);
// execute
exec(command, (error, stdout, stderr) => {
    console.log(error || stderr || stdout);
});