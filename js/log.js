const { exec } = require('child_process');
exports.log = function (message) {
    const data = '"' + JSON.stringify({
        message: message,
        from: 'HSA SoC Local Server',
        secret: process.env.SECRET
    }).replace(/"/g, '\\"') + '"';
    const headers = `"Content-Type: application/json"`;
    const url = process.env.REMOTE + '/log';
    const command = `curl --data ${data} -H ${headers} ${url} > /root/log`;
    exec(command, (error, stdout, stderr) => {
        console.log(error || stderr || stdout);
    });
}