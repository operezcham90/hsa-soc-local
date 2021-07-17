const exec = require('child_process').exec;

const command_work = 'node work.js';

function work() {
    exec(command_work, (error, stdout, stderr) => {
    });
}

work();