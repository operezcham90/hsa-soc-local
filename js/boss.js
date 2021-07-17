//pm2 start hsa-soc-local/js/boss.js
//pm2 save
//pm2 startup

const exec = require('child_process').exec;

const command_work = 'node /root/hsa-soc-local/js/work.js';

function work() {
    exec(command_work, (error, stdout, stderr) => {
    });
}

work();