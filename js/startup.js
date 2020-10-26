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
    res.write('Hi from HSA-SoC');
    res.end();
}).listen(process.env.PORT);
// log message
const message = `Listening on http://${ip}:${process.env.PORT}`;
// data
const data = '"' + JSON.stringify({
    message: message,
    from: 'HSA SoC Client',
    secret: process.env.SECRET
}).replace(/"/g, '\\"') + '"';
// headers
const headers = `"Content-Type: application/json"`;
// url
const url = process.env.REMOTE + '/log';
// full command
const command = `curl --data ${data} -H ${headers} ${url} > /root/log`;
console.log(command);
// execute
exec(command, (error, stdout, stderr) => {
    console.log(error || stderr || stdout);
});